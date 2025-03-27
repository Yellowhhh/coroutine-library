#include "scheduler.h"
#include "hook.h"

namespace myitem{

//指向调度协程的指针（执行run方法的协程）
static thread_local Fiber::ptr t_scheduler_fiber = nullptr; 

static thread_local Scheduler* t_scheduler = nullptr;

Scheduler::Scheduler()
{
    //创建主协程
    Fiber::GetThis();
    
}

Scheduler::Scheduler(size_t threads, bool use_caller)
{
    //创建主协程
    Fiber::GetThis();
    if(use_caller == true)
    {
        --threads;
    }
    m_threads = threads;
    m_use_caller = use_caller;
    t_scheduler = this;
}

Scheduler::~Scheduler()
{
    //清理实际的调度协程
    if(t_scheduler_fiber != nullptr)
    {
        std::cout << "t_scheduler_fiber.reset() mid = " << t_scheduler_fiber->getMid() << std::endl;
        std::cout << "t_scheduler_fiber use_count : " << t_scheduler_fiber.use_count() << std::endl;
        t_scheduler_fiber.reset();
    }
    if(GetScheduler() != nullptr)
    {
        t_scheduler = nullptr;
    }
}

const Fiber::ptr& Scheduler::GetSchedulerFiber()
{
    return t_scheduler_fiber;
}

Scheduler* Scheduler::GetScheduler()
{
    return t_scheduler;
}

//创建实际的调度协程（执行run方法的协程），然后调用该调度协程类的call方法，将当前上下文传给主协程类，然后跳转去执行run方法
void Scheduler::start()
{

    Mutex::MutexType lock(m_mutex);
    //添加设定好的线程数量的线程
    for(int i = 0; i < m_threads; i++)
    {
        m_thread_pool.push_back(std::make_shared<myitem::Thread>([this](){this->run();}, "thread" + std::to_string(i)));
    }
    lock.unlock();

    m_stopping = false;

}

void Scheduler::run()
{
    //std::cout << "run through " << Thread::GetName() << std::endl;
    //非主线程，需要初始化协程调度线程指针
    set_hook_enable(true);
    if(Thread::GetName() != "main_thread")
    {
        t_scheduler_fiber = std::make_shared<Fiber>([](){std::cout << "t_scheduler_fiber callback" << std::endl;}, 8192);
        t_scheduler = this;
    }

    Fiber::ptr idle_fiber = std::make_shared<Fiber>(std::bind(&Scheduler::idle, this), 8192);

    while(1)
    {
        RWMutex::WrMutexType lock(m_rwmutex);
        //从协程池中取待执行的协程
        //std::cout << "1 " << Thread::GetName() << " fiber_pool size = " << fiber_pool.size() << std::endl;
        if(fiber_pool.size() != 0)
        {
            //std::cout << "000 " << Thread::GetName() << " " << fiber_pool.size() << std::endl;
            Fiber::ptr running_fiber = fiber_pool.front();
            //std::cout << "e1 use_count = " << running_fiber.use_count() << Thread::GetName() << std::endl;
            fiber_pool.erase(fiber_pool.begin());
            //std::cout << "e2 use_count = " << running_fiber.use_count() << std::endl;
            lock.unlock();
            //std::cout << "wrlock unlock" << std::endl;
            ++m_active_fiber_num;
            //swapIn会重新设置t_fiber的指向，这样fiber实例析构时被重置为空的t_fiber就重新有值了，就不会出错
            //但是如果fiber任务都执行完毕了，会回到start中的call()那里，即fiber.cc:179，这时因为t_fiber为空，所以
            //会出错，需要修改
            //std::cout << "running fiber swapIn before " << Thread::GetName() << std::endl;
            //std::cout << &running_fiber << "use_count = " << running_fiber.use_count() << std::endl;
            running_fiber->swapIn();
            //std::cout << " 66666" << std::endl;
            //std::cout << "running fiber swapIn back " << Thread::GetName() << std::endl;
            if(running_fiber->getStatus() == Fiber::READY)
            {
                add_task(running_fiber);
            }
            else if(running_fiber->getStatus() == Fiber::TERM)  //TERM说明协程执行结束了，将running_fiber重置
            {
                running_fiber.reset();
            }
            else    //协程内部直接调用了swapOut跳出的，如果别的地方还会再次使用该协程，那么就需要自己保存好running_fiber了，如果不保存好，这里直接reset，相当于直接丢弃该协程了，当作不再执行了
            {
                running_fiber.reset();  //这里虽然重置了，但是如果别的地方还持有共享智能指针的话，该协程对象就不会被回收
            }
            --m_active_fiber_num;

        }
        else
        {
            lock.unlock();
            idle_fiber->swapIn();
            if(idle_fiber->getStatus() == Fiber::TERM)
            {
                std::cout << "idle fiber term " << Thread::GetName() << std::endl;
                break;
            }
            else
            {
                idle_fiber->setStatus(Fiber::READY);
            }
        }
    }

}

void Scheduler::stop()
{
    m_stopping = true;
    //使用caller线程来做任务
    if(m_use_caller)
    {
        //这里创建调度协程，并将其回调函数设置为FunC，以便能通过back方法回到主函数
        Fiber::ptr scheduler_fiber = std::make_shared<Fiber>([this](){this->run();}, 8192, true);
        t_scheduler_fiber = scheduler_fiber;
        //std::cout << "t_scheduler_fiber use_count_start : " << t_scheduler_fiber.use_count() << std::endl;


        scheduler_fiber->call();
        //std::cout << "t_scheduler_fiber use_count_start_call_back : " << t_scheduler_fiber.use_count() << std::endl;
    }

    while(1)
    {
        if(stopping())
        {
            //全部都停止了，将线程池中的线程都join掉，然后将线程池中的智能指针清空，让线程对象析构
            for(size_t i = 0; i < m_thread_pool.size(); i++)
            {
                m_thread_pool[i]->join();
            }
            m_thread_pool.clear();
            break;
        }
        else
        {
            continue;
        }
    }
}

bool Scheduler::stopping()
{
    RWMutex::RdMutexType lock(m_rwmutex);
    return fiber_pool.empty() && m_active_fiber_num == 0 && m_stopping == true;
}
    
void Scheduler::add_task(Fiber::ptr fiber)
{
    //加锁防止多线程同时对线程池进行操作
    //std::cout << "5  5  7 " << Thread::GetName() << std::endl;
    RWMutex::WrMutexType lock(m_rwmutex);
    //std::cout << "5  5  6" << Thread::GetName() << std::endl;
    fiber_pool.push_back(fiber);
}

void Scheduler::idle()
{
    std::cout << "scheduler idle" << std::endl;
}

void Scheduler::tickle()
{

}

}