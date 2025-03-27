#include "fiber.h"
#include "thread.h"
#include "scheduler.h"

namespace myitem{

    //主协程指针
    static thread_local Fiber::ptr main_fiber = nullptr;
    //当前执行的协程的指针
    static thread_local Fiber::ptr t_fiber = nullptr;
    // //协程调度协程指针
    // static thread_local Fiber::ptr t_scheduler_fiber = std::make_shared<Fiber>([](){
    //     std::cout << "this is t_shceduler_fiber callback function" << std::endl;
    // }, 8192, false);


    static bool temp_debug = true;

    static std::atomic<uint32_t> m_fiber_id {0};
    static std::atomic<uint32_t> fiber_num {0};

    Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool is_main)
    {
        m_stackPointer = malloc(stacksize);
        m_stackSize = stacksize;
        m_cb = cb;
        m_fid = m_fiber_id;
        ++m_fiber_id;
        m_status = Fiber::fiber_status::READY;

        //std::cout << "fiber_" << m_fid << " create" << std::endl;

        //将此时的上下文信息保存到m_ctx中
        getcontext(&m_ctx);
        m_ctx.uc_stack.ss_sp = m_stackPointer;
        m_ctx.uc_stack.ss_size = m_stackSize;
        m_ctx.uc_stack.ss_flags = 0;
        m_ctx.uc_link = nullptr;    //不设置后继上下文，全部由MainFunc函数来控制

        makecontext(&m_ctx, (void(*)(void))&Func, 0);

        //这里不需要跳转到Func函数，跳转由swapIn或call方法提供，由scheduler控制跳转
    }

    Fiber::Fiber(std::function<void()> cb, size_t stacksize)
    {
        m_stackPointer = malloc(stacksize);
        m_stackSize = stacksize;
        m_cb = cb;
        m_fid = m_fiber_id;
        ++m_fiber_id;
        m_status = Fiber::fiber_status::READY;

        //std::cout << "fiber_" << m_fid << " create" << std::endl;

        //将此时的上下文信息保存到m_ctx中
        getcontext(&m_ctx);
        m_ctx.uc_stack.ss_sp = m_stackPointer;
        m_ctx.uc_stack.ss_size = m_stackSize;
        m_ctx.uc_stack.ss_flags = 0;
        m_ctx.uc_link = nullptr;    //不设置后继上下文，全部由MainFunc函数来控制

        makecontext(&m_ctx, (void(*)(void))&MainFunc, 0);

        //这里不需要跳转到MainFunc函数，跳转由swapIn或call方法提供，由scheduler控制跳转
    }

    Fiber::~Fiber()
    {
        if(m_ctx.uc_stack.ss_sp != nullptr)
        {
            free(m_ctx.uc_stack.ss_sp);
        }

        //该静态变量主协程智能指针重置为空
        //main_fiber.reset(); //这里千万不能reset掉main_fiber，否则第一个fiber析构就把main_fiber给析构了
        if(t_fiber == main_fiber)
        {
            main_fiber.reset();
            t_fiber.reset();
        }
        
        // if(temp_debug){
        //     std::cout << "fiber id = " << m_fid << " delete" << std::endl;
        // }
    }

    Fiber::ptr Fiber::GetThis()
    {
        //主线程才需要主协程，其他线程不需要
        if(Thread::GetName() == "main_thread")
        {
            //如果主协程指针为空，则创建主协程
            if(main_fiber == nullptr)
            {
                main_fiber = std::make_shared<Fiber>([](){
                    std::cout << "main fiber callback" << std::endl;
                    }, 8192, true);
                std::cout << "create main fiber, return main_fiber" << std::endl;
                return main_fiber;
            }
        }
        
        //若t_fiber当前协程指针有指向则返回当前携程的指针，否则返回主协程的指针
        return t_fiber ? t_fiber : main_fiber;
    }

    void Fiber::SetThis(Fiber::ptr ptr)
    {
        //这里为什么编译器报错：因为SetThis()设置的是static，这样的话就不能够使用对象实例的shared_from_this()
        //t_fiber = std::enable_shared_from_this<Fiber>::shared_from_this();
        t_fiber = ptr;
    }

    //重置回调函数,并重新设置上下文
    bool Fiber::reset(std::function<void()> cb)
    {
        m_status = Fiber::fiber_status::READY;
        m_cb = cb;

        m_ctx.uc_stack.ss_sp = m_stackPointer;
        m_ctx.uc_stack.ss_size = m_stackSize;
        //0: 表示没有特殊标志，使用默认行为。
        //SS_DISABLE: 表示栈被禁用，通常用于指示不使用用户自定义栈。
        m_ctx.uc_stack.ss_flags = 0;

        makecontext(&m_ctx, (void(*)(void))&MainFunc, 0);
        return true;
    }

    //将当前上下文保存到主协程，然后切换到该Fiber
    void Fiber::call()
    {
        t_fiber = shared_from_this();
        assert(m_status != Running);
        m_status = Running;
        swapcontext(&(main_fiber->m_ctx), &m_ctx);
        
    }

    //返回到主协程
    void Fiber::back()
    {
        //将当前执行协程设置为主协程
        t_fiber = main_fiber;
        swapcontext(&m_ctx, &(main_fiber->m_ctx));
        if(temp_debug)
        {
            std::cout << "reach back function back" << std::endl;
        }
    }

    void Fiber::swapIn()
    {
        //设置当前运行的协程的指针
        SetThis(shared_from_this());
        assert(m_status != Running);
        m_status = Running;
        //将协程调度上下文存入协程调度指针，并切换至当前协程指针的回调函数
        //swapcontext(&(t_scheduler_fiber->m_ctx), &m_ctx);
        //这里直接跳转，GetSchedulerFiber的计数并不会减少，因此Scheduler::GetSchedulerFiber()的返回值
        //改为了引用，为了降低返回引用导致原来对象的意外修改，又将GetSchedulerFiber()用const修饰了
        if(swapcontext(&(Scheduler::GetSchedulerFiber()->m_ctx), &m_ctx))
        {
            assert(false);
        }
    }
    
    void Fiber::swapOut()
    {
        //设置当前运行的协程指针为协程调度指针
        SetThis(Scheduler::GetSchedulerFiber());
        swapcontext(&m_ctx, &(Scheduler::GetSchedulerFiber()->m_ctx));
        // if(temp_debug)
        // {
        //     std::cout << "reach swapOut back" << std::endl;
        // }
    }

    void Fiber::YieldToReady()
    {
        Fiber::ptr cur = GetThis();
        cur->m_status = READY;
        cur->swapOut();
    }

    void Fiber::MainFunc()
    {
        //运行回调函数
        t_fiber->m_cb();
        t_fiber->m_status = TERM;
        //回调函数执行完毕，跳转回协程调度函数
        t_fiber->swapOut();
    }

    void Fiber::Func()
    {
        //运行回调函数
        t_fiber->m_cb();
        t_fiber->m_status = TERM;
        //回调函数执行完毕，跳转回主函数
        t_fiber->back();
    }
}