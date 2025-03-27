#ifndef __MYITEM_SCHEDULER_H__
#define __MYITEM_SCHEDULER_H__

#include "fiber.h"
#include "thread.h"
#include <vector>
#include <list>
#include <functional>
#include <atomic>



namespace myitem{

class Scheduler{
public:
    typedef std::shared_ptr<Scheduler> ptr;

    Scheduler();
    Scheduler(size_t threads, bool use_caller = false);
    ~Scheduler();

    void start();
    void run();
    void stop();

    virtual bool stopping();
    virtual void idle();
    virtual void tickle();

    void add_task(Fiber::ptr fiber);
    //获取实际的调度协程指针（执行run方法的协程）
    static const Fiber::ptr& GetSchedulerFiber();

    static Scheduler* GetScheduler();

private:
    Mutex m_mutex;
    RWMutex m_rwmutex;
    std::vector<Fiber::ptr> fiber_pool; //需要对数组进行删减操作的话，复杂度是不是高了点，换成链表会更好吧
    std::atomic<uint32_t> m_active_fiber_num {0};
    std::vector<Thread::ptr> m_thread_pool;
    int m_threads;
    bool m_use_caller;
    bool m_stopping;
};


}


#endif