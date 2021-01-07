//
// Created by Jianlang Huang on 12/17/20.
//

#ifndef WEBFRAMEWORK_THREAD_H
#define WEBFRAMEWORK_THREAD_H

#include <pthread.h>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#include <libkern/OSAtomic.h>
#else
#include <semaphore.h>
#endif

class Semaphore {
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();

private:
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

#ifdef __APPLE__
    dispatch_semaphore_t    m_semaphore;
#else
    sem_t m_semaphore;
#endif
};

//类似 lock_guard
template <class T>
struct ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.lock();
        m_locked = true;
    }

    ~ScopedLockImpl(){
        m_mutex.unlock();
    }

    void lock(){
        if(!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

//lock_guard for read write lock
template <class T>
struct ReadScopedLockImpl {
public:
    ReadScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImpl(){
        m_mutex.unlock();
    }

    void lock(){
        if(!m_locked) {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

template <class T>
struct WriteScopedLockImpl {
public:
    WriteScopedLockImpl(T& mutex) : m_mutex(mutex) {
        m_mutex.wrlock();
        m_locked = true;
    }

    ~WriteScopedLockImpl(){
        m_mutex.unlock();
    }

    void lock(){
        if(!m_locked) {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T& m_mutex;
    bool m_locked;
};

class Mutex {
public:
    using Lock = ScopedLockImpl<Mutex>;

    Mutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~Mutex() {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock(){
        pthread_mutex_lock(&m_mutex);
    }

    void unlock(){
        pthread_mutex_unlock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;
};

//空的mutex 用于验证线程安全
class NullMutex {
public:
    using Lock = ScopedLockImpl<NullMutex>;
    NullMutex(){

    }

    ~NullMutex(){

    }

    void lock(){

    }

    void unlock(){

    }
};

//读写锁
class RWMutex {
public:
    using ReadLock = ReadScopedLockImpl<RWMutex>;
    using WriteLock = WriteScopedLockImpl<RWMutex>;

    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock() {
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t  m_lock;
};

class NullRWMutex {
public:
    using ReadLock = ReadScopedLockImpl<RWMutex>;
    using WriteLock = WriteScopedLockImpl<RWMutex>;

    NullRWMutex() {

    }

    ~NullRWMutex() {

    }

    void rdlock() {}
    void wrlock() {}
    void unlock() {}
};

//用于提高性能
//todo: Spinlock 能提高性能吗
//Spinlock 属于busy-waiting -> 适用于阻塞时间很短的场景
//普通mutex 属于sleep-waiting -> 适用于阻塞时间很长的场景
class Spinlock {
public:
    using Lock = ScopedLockImpl<Spinlock>;

#ifdef __APPLE__
public:
    Spinlock() : m_mutex(0) {
    }

    void lock(){
        OSSpinLockLock(&m_mutex);
    }

    void unlock(){
        OSSpinLockUnlock(&m_mutex);
    }
private:
    Spinlock(const Spinlock&) = delete;
    Spinlock& operator=(const Spinlock&) = delete;

    OSSpinLock m_mutex;

#else
public:
    Spinlock() {
        pthread_spin_init(&m_mutex, 0);
    }

    void lock(){
        pthread_spin_lock(&m_mutex);
    }

    void unlock(){
        pthread_spin_unlock(&m_mutex);
    }

    ~Spinlock(){
        pthread_spin_destroy(&m_mutex);
    }

private:
    Spinlock(const Spinlock&) = delete;
    Spinlock& operator=(const Spinlock&) = delete;

    pthread_spinlock_t m_mutex;

#endif

};

//对系统的pthread进行封装
class Thread {
public:
    using pointer = std::shared_ptr<Thread>;

    Thread(std::function<void()> callback, const std::string& name);
    ~Thread();


    pid_t getId() const {
        return m_id;
    }

    const std::string& getName() const {
        return m_name;
    }

    void join();

    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& name);
    static void* run(void* arg);

private:

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_callback;
    std::string m_name;

    Semaphore m_semaphore;

};


#endif //WEBFRAMEWORK_THREAD_H
