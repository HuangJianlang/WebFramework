#ifndef WEBFRAMEWORK_FIBER_H
#define WEBFRAMEWORK_FIBER_H

#ifdef __APPLE__
#define _XOPEN_SOURCE 600
#endif

#include <memory>
#include <functional>
#include <ucontext.h>
#include "thread.h"

class Scheduler;

class Fiber : public std::enable_shared_from_this<Fiber> {
    friend class Scheduler;
public:
    using pointer = std::shared_ptr<Fiber>;

    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };

private:
    Fiber();

public:
    Fiber(std::function<void()> callback, size_t stacksize = 0, bool use_caller = false);
    ~Fiber();

    //重置协程函数 重置状态
    //INIT TERM
    void reset(std::function<void()> callback);
    //切换到当前协程执行
    void swapIn();
    //切换到后台执行
    void swapOut();

    void call();
    void back();

    uint64_t getId() const {
        return m_id;
    }

    State getState() const {
        return m_state;
    }

    void setState(State state) {
        m_state = state;
    }

public:
    static void SetThis(Fiber* f);
    //返回当前协程
    static Fiber::pointer GetThis();
    //协程切换到后台 状态为Ready
    static void YieldToReady();
    //协程切换到后台 状态为Hold
    static void YieldToHold();
    //协程总数
    static uint64_t TotalFibers();

    static void MainFunc();
    static void CallerMainFunc();

    static uint64_t GetFiberId();
private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;

    ucontext_t m_ctx;
    void* m_stack = nullptr;

    //协程的callback方法
    std::function<void()> m_callback;

};


#endif //WEBFRAMEWORK_FIBER_H
