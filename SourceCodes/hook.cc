#include "hook.h"


#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept)  \
    XX(fcntl) \
    XX(getsockopt) \
    XX(sendmsg) \
    XX(recvfrom) \
    XX(close) \
    XX(recv) \
    XX(setsockopt) \
    XX(sendto) \
    XX(read) \
    XX(ioctl) \
    XX(readv) \
    XX(write) \
    XX(send) \
    XX(writev) \
    XX(recvmsg)


namespace myitem{
static thread_local bool t_hook_enable = false;

struct timer_info{
    int cancelled = 0;
};

template<typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name, uint32_t event, int timeout_so, Args&&... args)
{
    if(!is_hook_enable())
    {
        return fun(fd, std::forward<Args>(args)...);
    }
    FdCtx::ptr ctx = FdMgr::GetInstance()->get(fd);
    if(!ctx)
    {
        return fun(fd, std::forward<Args>(args)...);
    }
    if(ctx->isClose())
    {
        errno = EBADF;
        return -1;
    }
    if(!ctx->isSocket() || ctx->getUserNonblock())
    {
        return fun(fd, std::forward<Args>(args)...);
    }

    uint64_t timeout = ctx->getTimeout(timeout_so);

    std::shared_ptr<timer_info> tinfo(new timer_info);
retry:
    ssize_t n = fun(fd, std::forward<Args>(args)...);
    while(n == -1 && errno == EINTR)
    {
        n = fun(fd, std::forward<Args>(args)...);
    }
    if(n == -1 && errno == EAGAIN)
    {
        std::cout << "11111 222" << std::endl;
        IOManager* iom = IOManager::GetThis();
        Fiber::ptr fiber = Fiber::GetThis();
        std::weak_ptr<timer_info> wPtr(tinfo);
        Timer::ptr timer;
        if(timeout != (uint64_t)-1)
        {
            timer = iom->addConditionTimer(timeout, [wPtr, fd, iom, event](){
                std::shared_ptr<timer_info> t = wPtr.lock();
                if(!t || t->cancelled)
                {
                    return;
                }
                t->cancelled = ETIMEDOUT;
                iom->cancelEvent(fd, (IOManager::Event)event);  //触发事件，唤醒被挂起的协程
            }, false, wPtr);
        }
        int rt = iom->addEvent(fd, (IOManager::Event)event);    //这里没有传cb回调函数，说明将当前协程当作了回调
        if(rt == -1)
        {
            std::cout << hook_fun_name << " addEvent(" << fd << ", " << event << ")";
            if(timer)
            {
                timer->cancel();
            }
            return -1;
        }
        else
        {
            fiber->swapOut();
            if(timer)
            {
                timer->cancel();
            }
            if(tinfo->cancelled == ETIMEDOUT)
            {
                errno = tinfo->cancelled;
                return -1;
            }
        }
        goto retry;
    }
    return n;
}

bool is_hook_enable()
{
    return t_hook_enable;
}

void set_hook_enable(bool flag)
{
    t_hook_enable = flag;
}

static uint64_t s_connect_timeout = -1;

void hook_init()
{
    static bool is_inited = false;
    if(is_inited)
    {
        return;
    }

#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name); 
    HOOK_FUN(XX); 
#undef XX
}

struct _HookIniter {
    _HookIniter(){
        hook_init();
        s_connect_timeout = 5000;
    }
};

static _HookIniter s_hook_initer;



}

extern "C"{

#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX)
#undef XX

unsigned int sleep(unsigned int seconds)
{
    if(!myitem::is_hook_enable())
    {
        return sleep_f(seconds);
    }

    myitem::Fiber::ptr fiber = myitem::Fiber::GetThis();
    myitem::IOManager* iom = myitem::IOManager::GetThis();
    iom->addTimer(seconds * 1000, [fiber, iom](){
        iom->add_task(fiber);
    }, false);
    fiber->swapOut();
    return 0;
}

int usleep(useconds_t usec)
{
    if(!myitem::is_hook_enable())
    {
        return usleep(usec);
    }

    myitem::Fiber::ptr fiber = myitem::Fiber::GetThis();
    myitem::IOManager* iom = myitem::IOManager::GetThis();
    iom->addTimer(usec * 1000 * 1000, [fiber, iom](){
        iom->add_task(fiber);
    }, false);
    fiber->swapOut();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem)
{
    if(!myitem::is_hook_enable())
    {
        return nanosleep(req, rem);
    }

    myitem::Fiber::ptr fiber = myitem::Fiber::GetThis();
    myitem::IOManager* iom = myitem::IOManager::GetThis();
    iom->addTimer(req->tv_sec * 1000 + req->tv_nsec /1000 / 1000, [fiber, iom](){
        iom->add_task(fiber);
    }, false);
    fiber->swapOut();
    return 0;
}

int socket(int domain, int type, int protocol)
{
    if(!myitem::is_hook_enable())
    {
        socket_f(domain, type, protocol);
    }
    
    int fd = socket_f(domain, type, protocol);
    if(fd < 0)
    {
        perror("socket() failed");
        return fd;
    }

    myitem::FdMgr::GetInstance()->get(fd, true);
    return fd;
}

int connect_with_timeout(int sockfd, const struct sockaddr *addr, 
                            socklen_t addrlen, uint64_t timeout_ms)
{
    if(!myitem::t_hook_enable)
    {
        return connect_f(sockfd, addr, addrlen);
    }
    myitem::FdCtx::ptr ctx = myitem::FdMgr::GetInstance()->get(sockfd);
    if(!ctx || ctx->isClose())
    {
        errno = EBADF;
        return -1;
    }
    if(!ctx->isSocket())
    {
        return connect_f(sockfd, addr, addrlen);
    }
    if(ctx->getUserNonblock())
    {
        return connect_f(sockfd, addr, addrlen);
    }

    int n = connect_f(sockfd, addr, addrlen);
    if(n == 0)
    {
        return 0;
    }
    else if(n != -1 || errno != EINPROGRESS)
    {
        return n;
    }

    myitem::IOManager* iom = myitem::IOManager::GetThis();
    myitem::Timer::ptr timer;
    std::shared_ptr<myitem::timer_info> tinfo(new myitem::timer_info);
    std::weak_ptr<myitem::timer_info> winfo(tinfo);
    if(timeout_ms != (uint64_t)-1)
    {
        timer = iom->addConditionTimer(timeout_ms, [winfo, sockfd, iom](){
            auto t = winfo.lock();
            if(!t || t->cancelled)
            {
                return;
            }
            t->cancelled = ETIMEDOUT;
            //取消事件并触发它
            iom->cancelEvent(sockfd, myitem::IOManager::WRITE);
        }, false, winfo);
    }
    int rt = iom->addEvent(sockfd, myitem::IOManager::WRITE);
    if(rt == 0)
    {
        myitem::Fiber::ptr fiber = myitem::Fiber::GetThis();
        std::cout << "con swapOut " << std::endl;
        fiber->swapOut();
        std::cout << "con swapOut back " << std::endl;
        if(timer)
        {
            timer->cancel();
        }
        if(tinfo->cancelled)
        {
            errno = tinfo->cancelled;
            return -1;
        }
    }
    else{
        if(timer)
        {
            timer->cancel();
        }
    }

    int error = 0;
    socklen_t len = sizeof(int);
    if(-1 == getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len))
    {
        return -1;
    }
    if(!error)
    {
        return 0;
    }
    else{
        errno = error;
        return -1;
    }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    return connect_with_timeout(sockfd, addr, addrlen, myitem::s_connect_timeout);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int fd = myitem::do_io(sockfd, accept_f, "accept", myitem::IOManager::Event::READ, SO_RCVTIMEO, addr, addrlen);
    if(fd >= 0)
    {
        myitem::FdMgr::GetInstance()->get(fd, true);
    }
    return fd;
}

ssize_t read(int fd, void *buf, size_t count)
{
    return myitem::do_io(fd, read_f, "read", myitem::IOManager::Event::READ, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
    return myitem::do_io(fd, readv_f, "readv", myitem::IOManager::Event::READ, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
    return myitem::do_io(sockfd, recv_f, "recv", myitem::IOManager::Event::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    return myitem::do_io(sockfd, recvfrom_f, "recvfrom", myitem::IOManager::Event::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
{
    return myitem::do_io(sockfd, recvmsg_f, "recvmsg", myitem::IOManager::Event::READ, SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count)
{
    return myitem::do_io(fd, write_f, "write", myitem::IOManager::Event::WRITE, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    return myitem::do_io(fd, writev_f, "writev", myitem::IOManager::Event::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
    return myitem::do_io(sockfd, send_f, "send", myitem::IOManager::Event::WRITE, SO_SNDTIMEO, buf, len, flags);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
    const struct sockaddr *dest_addr, socklen_t addrlen)
{
    return myitem::do_io(sockfd, sendto_f, "sendto", myitem::IOManager::Event::WRITE, SO_SNDTIMEO, buf, len, flags, dest_addr, addrlen);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
    return myitem::do_io(sockfd, sendmsg_f, "sendmsg", myitem::IOManager::Event::WRITE, SO_SNDTIMEO, msg, flags);
}

int close(int fd)
{
    if(!myitem::is_hook_enable())
    {
        return close_f(fd);
    }
    myitem::FdCtx::ptr ctx = myitem::FdMgr::GetInstance()->get(fd);
    if(ctx)
    {
        myitem::IOManager* iom = myitem::IOManager::GetThis();
        if(iom)
        {
            iom->cancelAll(fd);
        }
        myitem::FdMgr::GetInstance()->del(fd);
    }
    return close_f(fd);
}

int fcntl(int fd, int cmd, ... /* arg */ )
{
    va_list va;
    va_start(va, cmd);
    switch(cmd)
    {
        case F_SETFL:
            {
                int arg = va_arg(va, int);
                va_end(va);
                std::shared_ptr<myitem::FdCtx> ctx = myitem::FdMgr::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket())
                {
                    return fcntl_f(fd, cmd, arg);
                }

                ctx->setUserNonblock(arg & O_NONBLOCK); //判断下用户是否主动设置非阻塞
                if(ctx->getSysNonblock())
                {
                    arg |= O_NONBLOCK;
                }
                else
                {
                    arg &= ~O_NONBLOCK;
                }
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETFL:
            {
                va_end(va);
                int arg = fcntl_f(fd, cmd);
                myitem::FdCtx::ptr ctx = myitem::FdMgr::GetInstance()->get(fd);
                if(!ctx || ctx->isClose() || !ctx->isSocket())
                {
                    return arg;
                }
                if(ctx->getUserNonblock())
                {
                    return arg |= O_NONBLOCK;
                }
                else
                {
                    return arg & ~O_NONBLOCK;
                }
            }
            break;
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
        case F_SETPIPE_SZ:
            {
                int arg = va_arg(va, int);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
        case F_GETPIPE_SZ:
            {
                va_end(va);
                return fcntl_f(fd, cmd);
            }
            break;
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
            {
                struct flock * arg = va_arg(va, struct flock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            {
                struct f_owner_ex* arg = va_arg(va, struct f_owner_ex*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        default:
            va_end(va);
            return fcntl_f(fd, cmd);
    }
}

int ioctl(int fd, unsigned long request, ...)
{
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);

    if(FIONBIO == request)
    {
        bool user_nonblock = !!*(int*)arg;
        myitem::FdCtx::ptr ctx = myitem::FdMgr::GetInstance()->get(fd);
        if(!ctx || ctx->isClose() || !ctx->isSocket())
        {
            return ioctl_f(fd, request, arg);
        }
        ctx->setUserNonblock(user_nonblock);    //用户自己主动设置非阻塞
    }
    return ioctl_f(fd, request, arg);
}

int getsockopt(int sockfd, int level, int optname,
    void *optval, socklen_t *optlen)
{
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname,
    const void *optval, socklen_t optlen)
{
    if(!myitem::t_hook_enable) {
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    if(level == SOL_SOCKET) {
        if(optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {
            myitem::FdCtx::ptr ctx = myitem::FdMgr::GetInstance()->get(sockfd);
            if(ctx) {
                const timeval* v = (const timeval*)optval;
                ctx->setTimeout(optname, v->tv_sec * 1000 + v->tv_usec / 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}




}