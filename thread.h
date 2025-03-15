#ifndef __MYITEM_THREAD_H__
#define __MYITEM_THREAD_H__

#include <iostream>
#include <pthread.h>
#include <string>
#include <functional>
#include <memory>
#include <semaphore.h>

namespace myitem{

template <class T>
class ReadScopedLock{
public:
    ReadScopedLock(T& mutex)
    :m_mutex(mutex){
        //std::cout << "rdlock lock" << std::endl;
        m_mutex.rdLock();
        m_locked = true;
    }

    ~ReadScopedLock()
    {
        if(m_locked == true)
        {
            //std::cout << "rdlock unlock" << std::endl;
            m_mutex.unlock();
            m_locked = false;
        }
    }

    void lock()
    {
        if(m_locked == false)
        {
            m_mutex.rdLock();
            m_locked = true;
        }
    }
    void unlock()
    {
        if(m_locked = true)
        {
            //std::cout << "rdlock unlock" << std::endl;
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    bool m_locked;
    T& m_mutex;
};

template <class T>
class WriteScopedLock{
public:
    WriteScopedLock(T& mutex)
    :m_mutex(mutex){
        //std::cout << "wrlock lock" << std::endl;
        m_mutex.wrLock();
        m_locked = true;
    }

    ~WriteScopedLock()
    {
        if(m_locked == true)
        {
            //std::cout << "wrlock unlock" << std::endl;
            m_mutex.unlock();
            m_locked = false;
        }
    }

    void lock()
    {
        if(m_locked == false)
        {
            m_mutex.wrLock();
            m_locked = true;
        }
    }
    void unlock()
    {
        if(m_locked = true)
        {
            //std::cout << "wrlock unlock" << std::endl;
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    bool m_locked;
    T& m_mutex;
};

template <class T>
class ScopedLock{
public:
    ScopedLock(T& mutex)
    :m_mutex(mutex){
        //std::cout << "mulock lock" << std::endl;
        m_mutex.lock();
        m_locked = true;
    }

    ~ScopedLock()
    {
        if(m_locked == true)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }

    void lock()
    {
        if(m_locked == false)
        {
            m_mutex.lock();
            m_locked = true;
        }
    }
    void unlock()
    {
        //std::cout << "mulock unlock" << std::endl;
        if(m_locked = true)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    bool m_locked;
    T& m_mutex;
};

// 互斥锁
class Mutex{
public:
    typedef ScopedLock<Mutex> MutexType;
    Mutex()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }
    
    ~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;
};

//读写锁
class RWMutex{
public:
    typedef ReadScopedLock<RWMutex> RdMutexType;
    typedef WriteScopedLock<RWMutex> WrMutexType;
    RWMutex()
    {
        pthread_rwlock_init(&m_rwmutex, NULL);
    }

    ~RWMutex()
    {
        pthread_rwlock_destroy(&m_rwmutex);
    }

    void rdLock()
    {
        pthread_rwlock_rdlock(&m_rwmutex);
    }

    void wrLock()
    {
        pthread_rwlock_wrlock(&m_rwmutex);
    }

    void unlock()
    {
        pthread_rwlock_unlock(&m_rwmutex);
    }

private:
    pthread_rwlock_t m_rwmutex;
};


class Semaphore{
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();
    int Wait();
    int Post();

private:
    sem_t m_semaphore;
};


class Thread : std::enable_shared_from_this<Thread>{
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, std::string name);
    ~Thread();

    //获取这个线程类的Id
    const pid_t& getId() const {return m_id;}
    //获取这个线程类的名称
    const std::string& getName() const {return m_name;} 

    void join();

    //获取当前线程的名字
    static const std::string& GetName();
    //获取当前线程的Id
    static const pid_t GetThreadId();
    //获取当前线程
    static Thread& GetThis();
    static void SetName(std::string name);
    
    static void* run(void* arg);

private:
    pid_t m_id;
    std::string m_name;
    std::function<void()> m_cb;
    pthread_t m_thread;

    Semaphore m_semaphore;
};


}

#endif