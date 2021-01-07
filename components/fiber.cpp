//
// Created by Jianlang Huang on 1/3/21.
//

#include <stdlib.h>
#include "fiber.h"
#include "config.h"
#include "macro.h"
#include "log.h"
#include "scheduler.h"
#include <atomic>

static Logger::pointer g_logger = LOG_NAME("system");

//atomic 实现不需要加锁就可以对数据进行修改
static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

//thread_local 每一个线程独有的变量空间
static thread_local Fiber* t_fiber = nullptr;
//::pointer -> shared_ptr<Fiber>
static thread_local Fiber::pointer t_threadFiber = nullptr;

static ConfigVar<uint32_t>::pointer g_fiber_stack_size =
        Config::Lookup<uint32_t>("fiber.stack_size", 128*1024, "fiber stack size");

class MallocStackAllocator {
public:
    static void* Alloc(size_t size){
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
        free(vp);
    }
};

using StackAlloc = MallocStackAllocator;

//协程的构造函数
Fiber::Fiber(){
    //协程创建就要设置为EXEC状态
    m_state = EXEC;
    //设置主协程
    SetThis(this);

    //失败会返回-1 成功返回0
    if(getcontext(&m_ctx)){
        MY_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;

    LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

Fiber::Fiber(std::function<void()> callback, size_t stacksize, bool use_caller)
    :m_id(++s_fiber_id), m_callback(callback){
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    //协程所需要的栈空间
    m_stack = StackAlloc::Alloc(m_stacksize);
    if(getcontext(&m_ctx)) {
        MY_ASSERT2(false, "getcontext");
    }
    //该上下文
    m_ctx.uc_link = nullptr;
    //context所在的栈内存
    m_ctx.uc_stack.ss_sp = m_stack;
    //context所在栈内存的大小
    m_ctx.uc_stack.ss_size = m_stacksize;

    if (! use_caller){
        //创建一个context, 起点为MainFunc, argc为0 -> 没有参数
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }


    LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
}

Fiber::~Fiber(){
    --s_fiber_count;
    if (m_stack) {
        MY_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
        StackAlloc::Dealloc(m_stack, m_stacksize);
    } else {
        MY_ASSERT(!m_callback);
        MY_ASSERT(m_state == EXEC);
        //手动回收
        Fiber* cur = t_fiber;
        if (cur == this) {
            SetThis(nullptr);
        }
    }

    LOG_DEBUG(LOG_NAME("system")) << "~Fiber(): fiber id: " << m_id;
}

//重置协程函数 重置状态
//INIT TERM
void Fiber::reset(std::function<void()> callback){
    //先判断栈是不是存在
    MY_ASSERT(m_stack);
    MY_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
    m_callback = callback;
    if (getcontext(&m_ctx)){
        MY_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

//切换到当前协程执行
void Fiber::swapIn(){
    SetThis(this);
    //在后台的协程不能是在执行的状态
    MY_ASSERT(m_state != EXEC);
    m_state = EXEC;
    //保存当前context到m_ctx 并设置新的context 为m_ctx
    if (swapcontext(&(Scheduler::GetMainFiber()->m_ctx), &m_ctx)){
        MY_ASSERT2(false, "swapcontext");
    }
}

//切换到后台执行
void Fiber::swapOut(){
    SetThis(Scheduler::GetMainFiber());
    if (swapcontext(&m_ctx, &(Scheduler::GetMainFiber()->m_ctx))){
        MY_ASSERT2(false, "swapcontext");
    }
}

void Fiber::SetThis(Fiber* f){
    t_fiber = f;
}

//返回当前协程
Fiber::pointer Fiber::GetThis(){
    //若主协程为空，则为该线程创建一个主协程
    if(t_fiber){
        return t_fiber->shared_from_this();
    }

    Fiber::pointer main_fiber(new Fiber());
    MY_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}
//协程切换到后台 状态为Ready
void Fiber::YieldToReady(){
    Fiber::pointer cur = GetThis();
    cur->m_state = READY;
    cur->swapOut();
}
//协程切换到后台 状态为Hold
void Fiber::YieldToHold(){
    Fiber::pointer cur = GetThis();
    cur->m_state = HOLD;
    cur->swapOut();
}
//协程总数
uint64_t Fiber::TotalFibers(){
    return s_fiber_count;
}

void Fiber::MainFunc(){
    Fiber::pointer cur = GetThis();
    MY_ASSERT(cur);
    try{
        cur->m_callback();
        cur->m_callback = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& exception) {
        cur->m_state = EXCEPT;
        LOG_ERROR(g_logger) << "Fiber Except: " << exception.what()
            << std::endl
            << BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        LOG_ERROR(g_logger) << "Fiber Except"
            << std::endl
            << BacktraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();

    MY_ASSERT2(false, "never reach");
}

void Fiber::CallerMainFunc() {
    Fiber::pointer cur = GetThis();
    MY_ASSERT(cur);
    try{
        cur->m_callback();
        cur->m_callback = nullptr;
        cur->m_state = TERM;
    } catch (std::exception& exception) {
        cur->m_state = EXCEPT;
        LOG_ERROR(g_logger) << "Fiber Except: " << exception.what()
                            << std::endl
                            << BacktraceToString();
    } catch (...) {
        cur->m_state = EXCEPT;
        LOG_ERROR(g_logger) << "Fiber Except"
                            << std::endl
                            << BacktraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();

    MY_ASSERT2(false, "never reach");
}

uint64_t Fiber::GetFiberId(){
    if(t_fiber){
        return t_fiber->getId();
    }
    return 0;
}

void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        MY_ASSERT2(false, "swapcontext");
    }
}

void Fiber::back() {
    SetThis(t_threadFiber.get());
    if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)){
        MY_ASSERT2(false, "swapcontext");
    }
}