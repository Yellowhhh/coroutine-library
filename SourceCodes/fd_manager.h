#ifndef __MYITEM_FD_MANAGER_H__
#define __MYITEM_FD_MANAGER_H__

#include "thread.h"
#include "singleton.h"
#include "hook.h"
#include "fiber.h"
#include "scheduler.h"
#include <memory>
#include <vector>
#include <sys/stat.h>


namespace myitem{

class FdCtx : public std::enable_shared_from_this<FdCtx>{
public:
    typedef std::shared_ptr<FdCtx> ptr;

    FdCtx(int fd);
   
    ~FdCtx();

    bool init();
    bool isinit() const {return m_isInit;}
    bool isSocket() const {return m_isSocket;}
    bool isClose() const {return m_isClosed;}
    bool close();

    void setUserNonblock(bool v) { m_userNonblock = v;}
    bool getUserNonblock() const { return m_userNonblock;}

    void setSysNonblock(bool v) { m_sysNonblock = v;}
    bool getSysNonblock() const { return m_sysNonblock;}

    void setTimeout(int type, uint64_t v);
    uint64_t getTimeout(int type);

private:
    /// 是否初始化
    bool m_isInit: 1;
    /// 是否socket
    bool m_isSocket: 1;
    /// 是否hook非阻塞
    bool m_sysNonblock: 1;
    /// 是否用户主动设置非阻塞
    bool m_userNonblock: 1;
    /// 是否关闭
    bool m_isClosed: 1;
    /// 文件句柄
    int m_fd;
    /// 读超时时间毫秒
    uint64_t m_recvTimeout;
    /// 写超时时间毫秒
    uint64_t m_sendTimeout;

};

class FdManager{
public:
    FdManager();

    std::shared_ptr<FdCtx> get(int fd, bool auto_create = false);

    void del(int fd);

private:
    RWMutex m_mutex;
    std::vector<std::shared_ptr<FdCtx>> m_datas;
};

typedef Singleton<FdManager> FdMgr;

}




#endif