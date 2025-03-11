#ifndef __MYITEM_TIMER_H__
#define __MYITEM_TIMER_H__

#include <memory>
#include <functional>
#include "thread.h"


namespace myitem{

class Timer : public std::enable_shared_from_this<Timer>{
public:
    Timer(uint64_t ms, std::function<void()> cb, bool recurring);
    ~Timer();



private:
    bool m_recurring = false;



};


}


#endif