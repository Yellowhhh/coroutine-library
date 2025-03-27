#ifndef __MYITEM_FIBER_H__
#define __MYITEM_FIBER_H__

#include <memory>
#include <stdio.h> //为什么有时候包含了这个库无法编译通过？
#include <ucontext.h>
#include <unistd.h>
#include <functional>
#include <atomic>
#include <iostream>
#include <assert.h>


namespace myitem{

class Fiber : public std::enable_shared_from_this<Fiber>{
public:
    typedef std::shared_ptr<Fiber> ptr;
    Fiber(std::function<void()> cb, size_t stacksize, bool is_main);
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();

    enum fiber_status{
        READY,
        Running,
        TERM,
        Hold
    };

    fiber_status getStatus()
    {
        return m_status;
    }

    void setStatus(fiber_status stat)
    {
        m_status = stat;
        return;
    }

    //重置回调函数
    bool reset(std::function<void()> cb);

    void call();
    void back();
    void swapIn();
    void swapOut();


    uint16_t getMid()
    {
        return m_fid;
    }

    static void MainFunc();
    static void Func();

    static Fiber::ptr GetThis();
    static void SetThis(Fiber::ptr ptr);
    //自己暂停并重新放入待执行队列中，让出执行权
    static void YieldToReady();

private:
    ucontext_t m_ctx; //上下文
    uint16_t m_fid;   //fiber的id
    fiber_status m_status;
    void* m_stackPointer;
    size_t m_stackSize;

    std::function<void()> m_cb;


};


}



#endif