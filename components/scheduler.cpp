//
// Created by Jianlang Huang on 1/4/21.
//

/*
 * 调度器作用在于调度协程，将协程放置到空闲的线程中调度
 */
#include "scheduler.h"
#include "log.h"
#include "macro.h"

//自己库用system
static Logger::pointer g_logger = LOG_NAME("system");

static thread_local Scheduler* t_scheduler = nullptr;
//主协程
static thread_local Fiber* t_fiber = nullptr;

//use_caller 含义 -> 是否使用主调线程作为其中一个线程
Scheduler::Scheduler(size_t thread_num, bool use_caller, const std::string& name)
    :m_name(name) {
    MY_ASSERT(thread_num > 0);

    if (use_caller){
        //如果线程没有协程，就初始化一个主协程
        Fiber::GetThis();
        --thread_num;

        //这里需要理解一下 -> 一个线程只能有一个调度器
        MY_ASSERT(GetThis() == nullptr);
        //设置主调度器
        t_scheduler = this;

        //bind 用于类成员函数，将object 自身作为参数传入类成员函数的this中
        //为什么要使用& -> 因为run方法不是静态方法
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));

        Thread::SetName(m_name);

        t_fiber = m_rootFiber.get();
        m_rootThread = GetThreadId();
        m_threadIds.push_back(m_rootThread);
    } else {
        m_rootThread = -1;
    }
    m_threadCount = thread_num;
}

Scheduler::~Scheduler(){
    //在停止状态才能stop
    MY_ASSERT(m_stopping);
    if(GetThis() == this){
        t_scheduler = nullptr;
    }
}

Scheduler* Scheduler::GetThis(){
    return t_scheduler;
}
//住协程来负责任务

Fiber* Scheduler::GetMainFiber(){
    return t_fiber;
}

void Scheduler::start(){
    MutexType::Lock lock(m_mutex);
    if (!m_stopping) {
        return;;
    }
    m_stopping = false;
    MY_ASSERT(m_threads.empty());

    m_threads.resize(m_threadCount);
    for (size_t i=0; i< m_threadCount; i++){
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }
    lock.unlock();
//
//    if(m_rootFiber){
//        //m_rootFiber->swapIn();
//        m_rootFiber->call();
//    }
}

void Scheduler::stop(){
    m_autoStop = true;
    if (m_rootFiber && m_threadCount == 0
    && (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT)){
        LOG_INFO(g_logger) << this << " stopped";
        m_stopping = true;

        if (stopping()){
            return;
        }
    }

    //bool exit_on_this_fiber = false;
    if (m_rootThread != -1) {
        MY_ASSERT(GetThis() == this);
    } else {
        MY_ASSERT(GetThis() != this);
    }

    m_stopping = true;
    for (size_t i=0; i < m_threadCount; i++){
        tickle();
    }

    if(m_rootFiber){
        tickle();
    }

    if(m_rootFiber){
//        while(!stopping()){
//            if (m_rootFiber->getState() == Fiber::TERM
//             || m_rootFiber->getState() == Fiber::EXCEPT) {
//                m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
//                LOG_INFO(g_logger) << "root fiber is term, reset";
//                t_fiber = m_rootFiber.get();
//            }
//            m_rootFiber->call();
//        }
        if(!stopping()){
            m_rootFiber->call();
        }
    }

    std::vector<Thread::pointer> thrs;
    {
        MutexType::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }

    for(auto& i : thrs){
        i->join();
    }
}

void Scheduler::setThis(){
    t_scheduler = this;
}

void Scheduler::run() {
    LOG_INFO(g_logger) << "Scheduler::run";
    setThis();
    if(GetThreadId() != m_rootThread){
        //设置为主协程
        t_fiber = Fiber::GetThis().get();
    }

    //创建一个idle协程 当所有的任务都完成的时候，调用这个协程
    Fiber::pointer idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::pointer callback_fiber;

    FiberAndThread fiberAndThread;
    while(true){
        fiberAndThread.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            MutexType::Lock lock(m_mutex);
            auto it = m_fibers.begin();
            while(it != m_fibers.end()){
                //当前fiber的线程已经分配并且不属于此线程
                if(it->thread != -1 && it->thread != GetThreadId()){
                    ++it;
                    tickle_me = true;
                    continue;
                }

                MY_ASSERT(it->fiber || it->callback);
                //如果fiber正在执行就跳过寻找下一个
                if (it->fiber && it->fiber->getState() == Fiber::EXEC){
                    ++it;
                    continue;
                }

                fiberAndThread = *it;
                m_fibers.erase(it);
                ++m_activeThreadCount;
                is_active = true;
                break;
            }
        }

        if(tickle_me){
            tickle();
        }

        if(fiberAndThread.fiber
            && (fiberAndThread.fiber->getState() != Fiber::TERM || fiberAndThread.fiber->getState() != Fiber::EXCEPT)){
            //符合条件，可以运行
            fiberAndThread.fiber->swapIn();
            --m_activeThreadCount;

            if (fiberAndThread.fiber->getState() == Fiber::READY){
                schedule(fiberAndThread.fiber);
            } else if(fiberAndThread.fiber->getState() != Fiber::TERM
                    && fiberAndThread.fiber->getState() != Fiber::EXCEPT){
                fiberAndThread.fiber->setState(Fiber::HOLD);
            }
            fiberAndThread.reset();

        } else if (fiberAndThread.callback){
            if (callback_fiber) {
                callback_fiber->reset(fiberAndThread.callback);
            } else {
                callback_fiber.reset(new Fiber(fiberAndThread.callback));
            }
            fiberAndThread.reset();
            callback_fiber->swapIn();
            --m_activeThreadCount;
            if (callback_fiber->getState() == Fiber::READY){
                schedule(callback_fiber);
                callback_fiber.reset();
            } else if (callback_fiber->getState() == Fiber::EXCEPT
                    || callback_fiber->getState() == Fiber::TERM){
                callback_fiber->reset(nullptr);
            } else {
                callback_fiber->setState(Fiber::HOLD);
                callback_fiber.reset();
            }
        } else {
            if (is_active){
                --m_activeThreadCount;
                continue;
            }
            if (idle_fiber->getState() == Fiber::TERM){
                LOG_INFO(g_logger) << "idle fiber term";
                break;
            }

            ++m_idleThreadCount;
            idle_fiber->swapIn();
            if (idle_fiber->getState() != Fiber::TERM && idle_fiber->getState() != Fiber::EXCEPT){
                idle_fiber->setState(Fiber::HOLD);
            }
            --m_idleThreadCount;
        }
    }
}

void Scheduler::tickle(){
    LOG_INFO(g_logger) << "tickle";
}

void Scheduler::idle(){
    LOG_INFO(g_logger) << "idle";
    while (!stopping()){
        Fiber::YieldToHold();
    }
}

bool Scheduler::stopping() {
    MutexType::Lock lock(m_mutex);
    return m_autoStop && m_stopping && m_fibers.empty() && m_activeThreadCount == 0;
}

