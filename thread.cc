#include "thread.h"
#include <sys/syscall.h>
#include <string.h>
#include <unistd.h>

namespace myitem{

static thread_local std::string t_thread_name = "UNKNOW";
static thread_local Thread* t_thread = nullptr;


Semaphore::Semaphore(uint32_t count)
{
    int rt = sem_init(&m_semaphore, 0, count);
    if(rt != 0)
    {
        throw std::logic_error("sem_init error");
    }
}

Semaphore::~Semaphore()
{
    int rt = sem_destroy(&m_semaphore);
    if(rt != 0)
    {
        perror("sem_destroy");
    }
}

int Semaphore::Wait()
{
    int rt = sem_wait(&m_semaphore);
    if(rt != 0)
    {
        throw std::logic_error("sem_wait error");
    }
    return rt;
}

int Semaphore::Post()
{
    int rt = sem_post(&m_semaphore);
    if(rt != 0)
    {
        throw std::logic_error("sem_post error");
    }
    return rt;
}



Thread::Thread(std::function<void()> cb, std::string name)
:m_cb(cb),
 m_name(name)
{
    int rt = pthread_create(&m_thread, NULL, Thread::run, this);
    if(rt != 0)
    {
        std::cerr << "pthread_create error, errno = " << errno << "error detail: " << strerror(errno);
        throw std::logic_error("pthread_create error");
    }

    //这里通过信号量阻塞等待线程初始化完成再往下执行
    m_semaphore.Wait();

}

Thread::~Thread()
{
    //std::cout << GetName() << " detach2" << std::endl;
    if(m_thread)
    {
        //std::cout << GetName() << " detach" << std::endl;
        if(pthread_detach(m_thread) != 0)
        {
            perror("pthread_detach");
        }
        m_thread = 0;
    }
}

void Thread::join()
{
    if(m_thread)
    {
        //会阻塞
        int rt = pthread_join(m_thread, nullptr);
        if(rt != 0)
        {
            std::cout << "pthread_join error, errno = " << errno << "error detail: " << strerror(errno);
        }
        m_thread = 0;
    }
}

//获取当前线程的名字
const std::string& Thread::GetName()
{
    return t_thread_name;
}

//获取当前线程的Id
const pid_t Thread::GetThreadId()
{
    return syscall(SYS_gettid);
}

//获取当前线程
Thread& Thread::GetThis()
{
    return *t_thread;
}

void Thread::SetName(std::string name)
{
    t_thread_name = name;
}

void* Thread::run(void* arg)
{
    Thread* temp = (Thread*)arg;
    t_thread_name = temp->m_name;
    t_thread = temp;


    //std::cout << "debug " << std::endl;
    temp->m_id = Thread::GetThreadId();
    std::function<void()> cb;
    //这里将temp中的m_cb交给了局部变量cb，这样就避免了重复执行，局部变量运行完后也会进行资源的释放
    cb.swap(temp->m_cb);
    temp->m_semaphore.Post();
    cb();

    return 0;

}



}