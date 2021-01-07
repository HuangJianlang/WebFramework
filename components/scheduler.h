//
// Created by Jianlang Huang on 1/4/21.
//

#ifndef WEBFRAMEWORK_SCHEDULER_H
#define WEBFRAMEWORK_SCHEDULER_H

#include <memory>
#include "thread.h"
#include "fiber.h"
#include <vector>
#include <list>
#include <atomic>
#include <functional>

class Scheduler {
public:
    using pointer = std::shared_ptr<Scheduler>;
    using MutexType = Mutex;

    Scheduler(size_t thread_num = 1, bool use_caller = true, const std::string& name="");

    virtual ~Scheduler();

    const std::string& getName(){
        return m_name;
    }

    static Scheduler* GetThis();
    //住协程来负责任务
    static Fiber* GetMainFiber();

    void start();
    void stop();

    //执行的模版函数
    template<class FiberOrCallback>
    void schedule(FiberOrCallback caller, int thread = -1){
        bool need_tickle = false;
        {
            MutexType::Lock  lock(m_mutex);
            need_tickle = scheduleNoLock(caller, thread);
        }
        //含义？
        if (need_tickle){
            tickle();
        }
    }

    template <class InputIterator>
    void schedule(InputIterator begin, InputIterator end){
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while(begin != end){
                need_tickle = scheduleNoLock(&*begin) || need_tickle;
            }
        }

        if (need_tickle){
            tickle();
        }
    }
protected:
    //schedule 的执行方式
    virtual void tickle();
    virtual bool stopping();
    virtual void idle();
    void run();

    void setThis();

private:
    //无锁的schedule函数
    template <class T>
    bool scheduleNoLock(T caller, int thread) {
        //以前是没有任务的
        bool need_tickle = m_fibers.empty();
        FiberAndThread fiberAndThread(caller, thread);
        if (fiberAndThread.fiber || fiberAndThread.callback){
            m_fibers.push_back(fiberAndThread);
        }
        return need_tickle;
    }
private:
    struct FiberAndThread {
        Fiber::pointer fiber;
        std::function<void()> callback;
        int thread;

        FiberAndThread(Fiber::pointer f, int thr)
            : fiber(f), thread(thr){

        }

        //这里用move可以吗？
        FiberAndThread(Fiber::pointer* f, int thr)
            : thread(thr) {
            fiber.swap(*f);
        }

        FiberAndThread(std::function<void()> func, int thr)
            : callback(func), thread(thr) {

        }

        FiberAndThread(std::function<void()>* func, int thr)
            : thread(thr) {
            callback.swap(*func);
        }

        FiberAndThread()
            : thread(-1) {

        }

        void reset() {
            fiber = nullptr;
            callback = nullptr;
            thread = -1;
        }
    };

private:
    MutexType m_mutex;
    std::vector<Thread::pointer> m_threads;
    std::list<FiberAndThread> m_fibers;
    std::string m_name;

    //主协程
    Fiber::pointer m_rootFiber;

protected:
    std::vector<int> m_threadIds;
    size_t m_threadCount = 0;
    std::atomic<size_t> m_activeThreadCount = {0};
    std::atomic<size_t> m_idleThreadCount = {0};
    bool m_stopping = true;
    bool m_autoStop = false;
    int m_rootThread = 0;
};


#endif //WEBFRAMEWORK_SCHEDULER_H
