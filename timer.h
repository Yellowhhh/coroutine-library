#ifndef __MYITEM_TIMER_H__
#define __MYITEM_TIMER_H__

#include <memory>
#include <functional>
#include <set>
#include <assert.h>
#include <ctime>
#include <chrono>
#include <vector>
#include "thread.h"


namespace myitem{
class TimerManager;

class Timer : public std::enable_shared_from_this<Timer>{
    friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;

    ~Timer();
    //从堆中删除当前定时器
    bool cancel();
    //刷新定时时间
    bool refresh();
    //重设定时器定时时间，from_now表示是否从当前时间开始计算
    bool reset(uint64_t ms, bool from_now);
private:
    Timer(uint64_t ms, std::function<void()> cb, bool recurring,
            TimerManager* manager);
    struct Comparator{
        bool operator()(const Timer::ptr& lhs,const Timer::ptr& rhs) const;
    };

private:
    bool m_recurring = false;
    //超时时间
    uint64_t m_ms;
    //绝对超时时间
    std::chrono::time_point<std::chrono::system_clock> m_next;

    std::function<void()> m_cb;
    TimerManager* m_manager = nullptr;


};

class TimerManager{
    friend class Timer;
public:
    TimerManager();
    virtual ~TimerManager();
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring);

    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb, 
                            bool recurring, std::weak_ptr<void> weak_cond);

    uint64_t getNextTime();

    void listExpiredCb(std::vector<std::function<void()> >& cbs);
protected:
    virtual void onTimerInsertedAtFront();
    
    void addTimer(Timer::ptr timer, RWMutex::WrMutexType& lock);

    bool detectClockRollover();
private:
    bool m_tickled = false;
    std::set<Timer::ptr, Timer::Comparator> m_set;
    RWMutex m_mutex;
    std::chrono::time_point<std::chrono::system_clock> m_previous_time; 
};

}

#endif