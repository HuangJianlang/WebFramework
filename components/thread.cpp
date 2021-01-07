//
// Created by Jianlang Huang on 12/17/20.
//

#include "thread.h"
#include "log.h"
#include "utils.h"

Logger::pointer g_logger = LOG_NAME("system");
//=============================Semaphore=====================================
Semaphore::Semaphore(uint32_t count){
#ifdef __APPLE__
    m_semaphore = dispatch_semaphore_create(count);
#else
    if(sem_init(&m_semaphore, 0, count)) {
        std::cout << errno << std::endl;
        throw std::logic_error("sem_init error");
    }
#endif
}

Semaphore::~Semaphore() {
#ifdef __APPLE__

#else
    sem_destroy(&m_semaphore);
#endif
}

void Semaphore::wait() {
#ifdef __APPLE__
    dispatch_semaphore_wait(m_semaphore, DISPATCH_TIME_FOREVER);
#else
    if(sem_wait(&m_semaphore)){
        throw std::logic_error("sem_wait error");
    }
#endif
}

void Semaphore::notify() {
#ifdef __APPLE__
    dispatch_semaphore_signal(m_semaphore);
#else
    if (sem_post(&m_semaphore)){
        throw std::logic_error("sem_post error");
    }
#endif
}
//=============================Thread=====================================
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOWN";

Thread::Thread(std::function<void()> callback, const std::string& name) : m_callback(callback) {
    if (name.empty()){
        m_name = "UNKNOWN";
    }
    m_name = name;
    int pid = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (pid) {
        LOG_ERROR(g_logger) << "pthread_create thread fails, pid = " << pid << " name = " << name;
        throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait();
}


Thread::~Thread(){
    if (m_thread) {
        pthread_detach(m_thread);
    }
}


void Thread::join(){
    if (m_thread){
        int ret = pthread_join(m_thread, nullptr);
        if (ret) {
            LOG_ERROR(g_logger) << "pthread_join thread fails, pid = " << ret << " name = " << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

Thread* Thread::GetThis(){
    return t_thread;
}

const std::string& Thread::GetName(){
    return t_thread_name;
}

void Thread::SetName(const std::string& name){
    if(t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

void* Thread::run(void* arg){
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    thread->m_id = GetThreadId();
    SetName(thread->m_name);

#ifdef __linux__
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
#elif __APPLE__
    pthread_setname_np(thread->m_name.substr(0, 15).c_str());
#endif

    std::function<void()> callback;
    callback.swap(thread->m_callback);

    thread->m_semaphore.notify();

    callback();
    return 0;
}