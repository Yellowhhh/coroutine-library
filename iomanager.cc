#include "iomanager.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>


namespace myitem{

IOManager::FdContext::EventContext& IOManager::FdContext::getContext(Event event)
{
    switch(event)
    {
        case READ:
            return read;
        case WRITE:
            return write;
        default:
            std::cerr << "get context error" << std::endl;
            assert(false);
    }
}

void IOManager::FdContext::resetContext(EventContext& ctx)
{
    ctx.fiber.reset();
    ctx.scheduler = nullptr;
}

void IOManager::FdContext::triggerContext(Event event)
{
    assert(events & event);
    
    events = (Event)(events & ~event);
    EventContext& ev_ctx = getContext(event);
    Scheduler* this_scheduler = Scheduler::GetScheduler();
    //add_task函数中带了锁，所以这里不需要加锁
    //std::cout << "51 use_count = " << ev_ctx.fiber.use_count() << std::endl;
    this_scheduler->add_task(ev_ctx.fiber);
    //std::cout << "add task ok " << Thread::GetName() << std::endl;
    //std::cout << "5 use_count = " << ev_ctx.fiber.use_count() << std::endl;
    ev_ctx.fiber.reset();
    //std::cout << "52 use_count = " << ev_ctx.fiber.use_count() << std::endl;
    ev_ctx.scheduler = nullptr;
    return;
}

IOManager::IOManager(size_t threads, bool use_caller)
:Scheduler(threads, use_caller){
    epfd = epoll_create(1);
    if(epfd == -1)
    {
        std::cerr << "Failed to create epoll instance" << std::endl;
    }
    if(pipe(m_tickleFds) < 0)
    {
        std::cerr << "Failed to create pipe" << std::endl;
    }
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;    //监听可读事件
    ev.data.fd = m_tickleFds[0];
    int rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK); //修改管道的读文件描述符，非阻塞边沿触发
    assert(!rt);
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, m_tickleFds[0], &ev) == -1)
    {
        std::cerr << "Failed to add pipe[0] to epoll" << std::endl;
    }

    //初始化时首先给一个初始大小的空间，后续不够会自动扩
    reserveFdContextVector(32);

}
IOManager::~IOManager()
{
    close(epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);
    for(int i = 0; i < m_fdContexts.size(); ++i)
    {
        if(m_fdContexts[i])
        {
            delete(m_fdContexts[i]);
        }
    }
}

void IOManager::reserveFdContextVector(int nums)
{
    m_fdContexts.resize(nums, nullptr);
    for(int i = 0; i < nums; ++i)
    {
        if(!m_fdContexts[i])
        {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

IOManager* IOManager::GetThis()
{
    return dynamic_cast<IOManager*>(Scheduler::GetScheduler());
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb)
{
    FdContext* fd_ctx = nullptr;
    RWMutex::RdMutexType lock(m_mutex);
    if(fd > (int)m_fdContexts.size())
    {
        lock.unlock();
        RWMutex::WrMutexType lock2(m_mutex);
        reserveFdContextVector(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }
    else
    {
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    }

    Mutex::MutexType lock2(fd_ctx->mutex);
    //位与等于0说明fd上本来就存在该事件了，不需要再添加
    if((fd_ctx->events & event) != 0x0)
    {
        std::cout << "add event failed, event exists" << std::endl;
        assert(!(fd_ctx->events & event));
    }

    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    struct epoll_event ev;
    ev.events = EPOLLET | fd_ctx->events | event;
    ev.data.ptr = fd_ctx;
    int rt = epoll_ctl(epfd, op, fd_ctx->fd, &ev);
    if(rt)
    {
        std::cerr << "epoll ctl failed" << std::endl;
        return -1;
    }

    m_pendingEventCount++;

    //为fd_ctx指向的结构体中添加信息
    fd_ctx->events = (Event)(fd_ctx->events | event);
    IOManager::FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    assert(!event_ctx.fiber && !event_ctx.scheduler);
    event_ctx.fiber = std::make_shared<Fiber>([cb](){cb();}, 8192);
    //std::cout << "event_ctx fiber use_count : " << event_ctx.fiber.use_count() << std::endl;

    assert(event_ctx.fiber->getStatus() != Fiber::Running);
    std::cout << "add event finish " << Thread::GetName() << std::endl;


    // int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    // struct epoll_event ev;
    // ev.events = EPOLLET | fd_ctx->events | event;
    // ev.data.ptr = fd_ctx;
    // int rt = epoll_ctl(epfd, op, fd_ctx->fd, &ev);
    // if(rt)
    // {
    //     std::cerr << "epoll ctl failed" << std::endl;
    //     return -1;
    // }

    // m_pendingEventCount++;

    return 0;
}

bool IOManager::delEvent(int fd, Event event)
{
    FdContext* fd_ctx = nullptr;
    RWMutex::RdMutexType lock(m_mutex);
    if(fd >= (int)m_fdContexts.size())
    {
        lock.unlock();
        std::cerr << "fd is bigger than m_fdContexts.size" << std::endl;
        return false;
    }
    lock.unlock();
    fd_ctx = m_fdContexts[fd];
    Mutex::MutexType lock2(fd_ctx->mutex);
    
    if((fd_ctx->events & event) != 0x0)
    {
        std::cerr << "fd_ctx do not has this event" << std::endl;
        return false;
    }

    struct epoll_event ev;
    ev.events = EPOLLET | (Event)(fd_ctx->events | ~event);
    ev.data.ptr = fd_ctx;
    int op = (Event)(fd_ctx->events | ~event) ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    int rt = epoll_ctl(epfd, op, fd_ctx->fd, &ev);
    if(rt)
    {
        std::cerr << "epoll ctl failed" << std::endl;
        return -1;
    }

    m_pendingEventCount--;

    fd_ctx->resetContext(fd_ctx->getContext(event));
    fd_ctx->events = (Event)(fd_ctx->events & ~event);
    return true;
}

bool IOManager::cancelEvent(int fd, Event event)
{
    FdContext* fd_ctx = nullptr;
    RWMutex::RdMutexType lock(m_mutex);
    if(fd > (int)m_fdContexts.size())
    {
        lock.unlock();
        std::cerr << "fd is bigger than m_fdContexts.size" << std::endl;
        return false;
    }
    lock.unlock();
    fd_ctx = m_fdContexts[fd];
    Mutex::MutexType lock2(fd_ctx->mutex);
    
    if((fd_ctx->events & event) != 0x0)
    {
        std::cerr << "fd_ctx do not has this event" << std::endl;
        return false;
    }

    struct epoll_event ev;
    ev.events = EPOLLET | (Event)(fd_ctx->events | event);
    ev.data.ptr = fd_ctx;
    int op = (Event)(fd_ctx->events | event) ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    int rt = epoll_ctl(epfd, op, fd_ctx->fd, &ev);
    if(rt)
    {
        std::cerr << "epoll ctl failed" << std::endl;
        return false;
    }

    fd_ctx->triggerContext(event);
    --m_pendingEventCount;
    return true;
}

bool IOManager::cancelAll(int fd)
{
    FdContext* fd_ctx = nullptr;
    RWMutex::RdMutexType lock(m_mutex);
    if(fd > m_fdContexts.size())
    {
        lock.unlock();
        std::cerr << "fd is bigger than m_fdContexts.size()" << std::endl;
        return false;
    }
    lock.unlock();
    fd_ctx = m_fdContexts[fd];
    Mutex::MutexType lock2(fd_ctx->mutex);
    
    if(fd_ctx->events == 0x0)
    {
        std::cout << "this fd does not exist events" << std::endl;
        return false;
    }

    struct epoll_event ev;
    ev.events = 0;
    ev.data.ptr = fd_ctx;
    int op = EPOLL_CTL_DEL;
    int rt = epoll_ctl(epfd, op, fd, &ev);
    if(rt)
    {
        std::cerr << "epoll ctl failed" << std::endl;
        return false;
    }

    if(fd_ctx->events & READ)
    {
        fd_ctx->triggerContext(READ);
        --m_pendingEventCount;
    }
    if(fd_ctx->events & WRITE)
    {
        fd_ctx->triggerContext(WRITE);
        --m_pendingEventCount;
    }
    fd_ctx->events = NONE;
    return true;
}

void IOManager::tickle()
{
    std::cout << "tickle()" << std::endl;
    int rt = write(m_tickleFds[1], "T", 1);
    assert(rt == 1);
}

void IOManager::idle()
{
    uint32_t MAX_EVENTS = 64;
    //epoll_wait设置的超时时间，如果超时了还没有事件触发，则返回值rt==0，也会跳出
    std::unique_ptr<epoll_event[]> events(new epoll_event[MAX_EVENTS]);
    
    while(1)
    {
        //std::cout << "idle run in thread " << Thread::GetName() << std::endl;
        uint64_t next_time = 0;
        if(stopping())
        {
            
            next_time = getNextTime();
            if(next_time == ~0ull)
            {
                std::cout << "idle stopping" << std::endl;
                break;
            }
        }
        //std::cout << "idle test not stopping" << Thread::GetName() << std::endl;
 
        int rt = 0;
        while(1)
        {
            static const int MAX_TIMEOUT = 1000;
            next_time = next_time > MAX_TIMEOUT ? MAX_TIMEOUT : next_time;
            rt = epoll_wait(epfd, events.get(), MAX_EVENTS, (int)next_time);
            if(rt < 0 && rt == EINTR){
            }
            else{
                break;
            }
        }

        std::vector<std::function<void()>> cbs;
        listExpiredCb(cbs);
        if(!cbs.empty())
        {
            for(auto i : cbs)
            {
                add_task(std::make_shared<Fiber>([i](){i();}, 8192));
            }
        }
        cbs.clear();


        for(int i = 0; i < rt; ++i)
        {
            //std::cout << "epoll return from " << Thread::GetName() << std::endl;
            //std::cout << "rt: " << rt << std::endl;
            epoll_event& event = events[i];
            //检查是否是无名管道唤醒了epoll_wait
            char buffer[64];
            ssize_t bytesRead;
            if(event.data.fd == m_tickleFds[0])
            {
                while(1)
                {
                    bytesRead = read(event.data.fd, buffer, sizeof(buffer));
                    if(bytesRead == -1)
                    {
                        //非阻塞模式下，表示没有数据可读了
                        if(errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            break;
                        }
                        else{
                            perror("read error");
                            break;
                        }
                    }
                    else if(bytesRead == 0){
                        break;
                    }
                    else{
                        continue;
                    }
                }
                continue;
            }
            FdContext* fd_ctx = (FdContext*)event.data.ptr;
            Mutex::MutexType lock(fd_ctx->mutex);
            if(event.events & (EPOLLERR | EPOLLHUP))
            {
                event.events |= EPOLLIN | EPOLLOUT; 
            }
            int real_event = NONE;
            if(event.events & EPOLLIN)
            {
                real_event |= READ;
            }
            if(event.events & EPOLLOUT)
            {
                real_event |= WRITE;
            }
            //fd并没有设置当前事件相应的回调函数
            if(fd_ctx->events & real_event == 0)
            {
                continue;
            }
            int both_events = NONE, left_events = NONE;
            both_events = fd_ctx->events & real_event;
            left_events = fd_ctx->events & ~real_event;

            int op = (left_events == NONE) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
            if(op == EPOLL_CTL_MOD)
            {
                event.data.ptr = fd_ctx;
                event.events = EPOLLET | left_events;
                int rt2 = epoll_ctl(epfd, op, fd_ctx->fd, &event);
                if(rt2)
                {
                    std::cerr << "epoll ctl failed" << std::endl;
                    continue;
                }
            }
            else
            {
                event.data.ptr = fd_ctx;
                event.events = NONE;
                int rt2 = epoll_ctl(epfd, op, fd_ctx->fd, &event);
                if(rt2)
                {
                    std::cerr << "epoll ctl failed" << std::endl;
                    continue;
                }
            }
            if(both_events & READ)
            {
                fd_ctx->triggerContext(READ);
                m_pendingEventCount--;
            }
            if(both_events & WRITE)
            {
                fd_ctx->triggerContext(WRITE);
                m_pendingEventCount--;
            }
        }
        //std::cout << "1 1 1" << Thread::GetName() << std::endl;
        std::shared_ptr<Fiber> idle_fiber_temp = Fiber::GetThis();
        Fiber* raw_fiber = idle_fiber_temp.get();
        idle_fiber_temp.reset();
        //std::cout << "idle swapOut " << Thread::GetName() << std::endl;
        raw_fiber->swapOut();
    }
}

bool IOManager::stopping()
{
    return Scheduler::stopping() && m_pendingEventCount == 0;
}

void IOManager::onTimerInsertedAtFront()
{
    tickle();

}



}