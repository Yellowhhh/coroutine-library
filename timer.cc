#include "timer.h"

namespace myitem{
bool Timer::Comparator::operator()(const Timer::ptr& lhs,const Timer::ptr& rhs) const
{
    assert(lhs != nullptr && rhs != nullptr);
    return lhs->m_next < rhs->m_next;
}

bool Timer::cancel()
{
    RWMutex::WrMutexType lock(m_manager->m_mutex);
    m_cb = nullptr;
    m_manager->m_set.erase(shared_from_this());
    return true;
}

//在TimerManager堆中的位置只会往后推
bool Timer::refresh()
{
    RWMutex::WrMutexType lock(m_manager->m_mutex);
    if(m_cb == nullptr)
    {
        std::cerr << "m_cb is nullptr" << std::endl;
        return false;
    }
    m_manager->m_set.erase(shared_from_this());
    m_next = std::chrono::system_clock::now() + std::chrono::milliseconds(m_ms);
    //因为只会往后推，所以不需要使用addTimer方法来监测，可以直接插入
    m_manager->m_set.insert(shared_from_this());
    return true;
}

bool Timer::reset(uint64_t ms, bool from_now)
{
    if(ms == m_ms && from_now == false)
    {
        std::cerr << "ms == m_ms, from_now == false, do not need operator" << std::endl;
        return false;
    }
    RWMutex::WrMutexType lock(m_manager->m_mutex);
    if(m_cb == nullptr)
    {
        std::cerr << "m_cb is nullptr" << std::endl;
        return false;
    }
    m_manager->m_set.erase(shared_from_this());
    if(from_now == true)
    {
        m_ms = ms;
        m_next = std::chrono::system_clock::now() + std::chrono::milliseconds(m_ms);
    }
    else
    {
        m_next = m_next - std::chrono::milliseconds(m_ms) + std::chrono::milliseconds(ms);
        m_ms = ms;
    }
    m_manager->addTimer(shared_from_this(), lock);
    return true;
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring,
                TimerManager* manager)
:m_ms(ms), m_cb(cb), m_recurring(recurring), m_manager(manager)
{
    m_next = std::chrono::system_clock::now() + std::chrono::milliseconds(m_ms);
}

Timer::~Timer()
{

}

TimerManager::TimerManager()
{
    m_previous_time = std::chrono::system_clock::now();
}

TimerManager::~TimerManager()
{

}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring)
{
    Timer::ptr t_timer(new Timer(ms, cb, recurring, this));
    RWMutex::WrMutexType lock(m_mutex);
    TimerManager::addTimer(t_timer, lock);
    return t_timer;
}

void TimerManager::addTimer(Timer::ptr timer, RWMutex::WrMutexType& lock)
{
    m_set.insert(timer);
    auto it = m_set.begin();
    //插入的位置是首部的话，说明添加的定时器是最小的，应该唤醒epoll_wait进行更新，原来的定时器有点大了
    lock.unlock();
    if(*(it) == timer && m_tickled == false)
    {
        m_tickled = true;
        onTimerInsertedAtFront();
    }
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb)
{
    std::shared_ptr<void> tmp = weak_cond.lock();
    if(tmp)
    {
        cb();
    }
}

Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb, 
                                            bool recurring, std::weak_ptr<void> weak_cond)
{
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTime()
{
    RWMutex::RdMutexType lock(m_mutex);
    //getNextTime表示epoll_wait要获取下一次超时时间了
    m_tickled = false;
    if(m_set.size() == 0)
    {
        return ~0ull;
    }

    Timer::ptr temp_t = *(m_set.begin());
    // uint64_t nextTime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(temp_t->m_next.time_since_epoch()).count());
    // uint64_t nowTime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    // std::cout << "nextTime: " << nextTime << " nowTime: " << nowTime << std::endl;
    
    if(temp_t->m_next <= std::chrono::system_clock::now())
    {
        return 0;
    }
    return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(temp_t->m_next - std::chrono::system_clock::now()).count();
}

bool TimerManager::detectClockRollover()
{
    bool rollover = false;
    auto now = std::chrono::system_clock::now();
    if(now < m_previous_time - std::chrono::milliseconds(60 * 60 * 1000))
    {
        rollover = true;
    }
    m_previous_time = now;
    return rollover;
}

void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs)
{
    std::chrono::_V2::system_clock::time_point nowTime = std::chrono::system_clock::now();
    {
        RWMutex::RdMutexType lock(m_mutex);
        if(m_set.size() == 0)
        {
            return;
        }
    }
    RWMutex::WrMutexType lock(m_mutex);
    bool rollover = detectClockRollover();

    
    if(rollover == false)
    {
        for(auto it = m_set.begin(); it != m_set.end(); ++it)
        {
            if((*it)->m_next <= nowTime)
            {
                cbs.push_back((*it)->m_cb);
                if((*it)->m_recurring == true)
                {
                    auto temp = (*it);
                    m_set.erase(it);
                    temp->m_next = std::chrono::milliseconds(temp->m_ms) + nowTime;
                    m_set.insert(temp);
                    continue;
                }
                //这里设置为nullptr是为了防止m_cb中有含智能指针的东西，如果不置为nullptr就不会减一了，空间就不会释放了
                (*it)->m_cb = nullptr;
                m_set.erase(it);
            }
            else
            {
                break;
            }
        }
    }
    //时间回滚了，就将所有的任务都触发一遍
    else
    {
        for(auto it = m_set.begin(); it != m_set.end(); ++it)
        {
            cbs.push_back((*it)->m_cb);
                if((*it)->m_recurring == true)
                {
                    auto temp = (*it);
                    m_set.erase(it);
                    temp->m_next = std::chrono::milliseconds(temp->m_ms) + nowTime;
                    m_set.insert(temp);
                    continue;
                }
                //这里设置为nullptr是为了防止m_cb中有含智能指针的东西，如果不置为nullptr就不会减一了，空间就不会释放了
                (*it)->m_cb = nullptr;
                m_set.erase(it);
        }
    }
}

void TimerManager::onTimerInsertedAtFront()
{

}

}