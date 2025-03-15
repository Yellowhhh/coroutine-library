#ifndef __MYITEM_IOMANAGER_H__
#define __MYITEM_IOMANAGER_H__

#include "scheduler.h"
#include "fiber.h"
#include "thread.h"
#include "timer.h"


namespace myitem{
class IOManager : public Scheduler, public TimerManager{
public:
    typedef std::shared_ptr<IOManager> ptr;
    
    enum Event{
        NONE = 0x0,
        READ = 0x1,
        WRITE = 0x4
    };

    IOManager(size_t threads, bool use_caller);
    ~IOManager();

    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);
    bool cancelAll(int fd);

    void tickle() override;
    void idle() override;
    bool stopping() override;

    void onTimerInsertedAtFront() override;
    
    void reserveFdContextVector(int nums);

    IOManager* GetThis();

private:
    RWMutex m_mutex;
    int epfd;
    int m_tickleFds[2];

    struct FdContext{
        typedef Mutex MutexType;
        struct EventContext{
            Scheduler* scheduler = nullptr;
            Fiber::ptr fiber;
        };

        EventContext& getContext(Event event);
        void resetContext(EventContext& ctx);
        void triggerContext(Event event);

        EventContext read;
        EventContext write;
        int fd = 0;
        Event events = NONE;
        MutexType mutex;
    };
    std::vector<FdContext*> m_fdContexts;
    std::atomic<size_t> m_pendingEventCount = {0};
};

}

#endif