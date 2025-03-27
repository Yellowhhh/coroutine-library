#include "fd_manager.h"


namespace myitem{

FdCtx::FdCtx(int fd)
:m_fd(fd), m_recvTimeout(-1), m_sendTimeout(-1)
{
    init();
}

FdCtx::~FdCtx()
{

}

bool FdCtx::init()
{
    if(isinit())
    {
        return true;
    }
    struct stat statbuf;
    if(-1 == fstat(m_fd, &statbuf))
    {
        m_isInit = false;
        m_isSocket = false;
    }
    else
    {
        m_isInit = true;
        m_isSocket = S_ISSOCK(statbuf.st_mode);
    }
    if(m_isSocket)
    {
        int flags = fcntl_f(m_fd, F_GETFL);
        if(!(flags & O_NONBLOCK))   //未设置非阻塞
        {
            fcntl_f(m_fd, F_SETFL, O_NONBLOCK); //设置非阻塞
        }
        m_sysNonblock = true;
    }
    else
    {
        m_sysNonblock = false;
    }
    m_userNonblock = false;
    m_isClosed = false;
    return m_isInit;
}


void FdCtx::setTimeout(int type, uint64_t v)
{
    if(type == SO_RCVTIMEO)
    {
        m_recvTimeout = v;
    }
    else
    {
        m_sendTimeout = v;
    }
}

uint64_t FdCtx::getTimeout(int type)
{
    if(type == SO_RCVTIMEO)
    {
        return m_recvTimeout;
    }
    else
    {
        return m_sendTimeout;
    }
}

FdManager::FdManager()
{
    m_datas.resize(64);
}

std::shared_ptr<FdCtx> FdManager::get(int fd, bool auto_create)
{
    if(fd < 0)
    {
        return nullptr;
    }
    RWMutex::RdMutexType lock(m_mutex);
    if((int)m_datas.size() <= fd)
    {
        if(!auto_create)
        {
            return nullptr;
        }
    }
    else
    {
        if(m_datas[fd] || !auto_create)
        {
            return m_datas[fd];
        }
    }
    lock.unlock();
    RWMutex::WrMutexType lock2(m_mutex);
    if((int)m_datas.size() <= fd)
    {
        m_datas.resize(fd * 1.5);
    }
    FdCtx::ptr ctx(new FdCtx(fd));
    m_datas[fd] = ctx;
    return ctx;
}

void FdManager::del(int fd)
{
    RWMutex::WrMutexType lock(m_mutex);
    if((int)m_datas.size() < fd)
    {
        return;
    }
    m_datas[fd].reset();
}

}