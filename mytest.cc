#include "thread.h"
#include "fiber.h"
#include "scheduler.h"
#include "iomanager.h"
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

myitem::RWMutex m_mutex;
int temp = 0;


void test1()
{
    for(int i = 0; i < 500000; i++)
    {
        myitem::RWMutex::RdMutexType lock(m_mutex);
        ++temp;
    }
}

void test_fiber()
{  
    std::cout << myitem::Thread::GetName() << " work" << std::endl;

    myitem::Scheduler::ptr scheduler_ptr = std::make_shared<myitem::Scheduler>();
    //创建调度协程类的实例对象，其构造函数中会创建主协程，即指向当前上下文的协程，
    //myitem::Scheduler::ptr scheduler_ptr = std::make_shared<myitem::Scheduler>();
    //为协程池中添加任务
    scheduler_ptr->add_task(std::make_shared<myitem::Fiber>([](){
        std::cout << "fiber_1 running" << std::endl;
    }, 8192));

    scheduler_ptr->add_task(std::make_shared<myitem::Fiber>([](){
        std::cout << "fiber_2 running" << std::endl;
    }, 8192));

    scheduler_ptr->start();
    scheduler_ptr->stop();
    
}


int main()
{
    std::cout << "start main" << std::endl;
    // std::vector<myitem::Thread::ptr> thr;
    myitem::Thread::SetName("main_thread");

    myitem::IOManager::ptr IOManager_ptr = std::make_shared<myitem::IOManager>(2, true);
    IOManager_ptr->start();

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "220.181.111.1", &addr.sin_addr.s_addr);
    
    IOManager_ptr->addEvent(sock, myitem::IOManager::WRITE, [](){
        std::cout << "sock write " << std::endl;
    });
    int rt = connect(sock, (const sockaddr *)&addr, sizeof(addr));

    IOManager_ptr->stop();

    // myitem::Scheduler::ptr scheduler_ptr = std::make_shared<myitem::Scheduler>(2, true);
    // scheduler_ptr->add_task(std::make_shared<myitem::Fiber>([](){
    //     std::cout << "fiber_1 running by " << myitem::Thread::GetName() << std::endl;
    //     sleep(1);
    // }, 8192));

    // scheduler_ptr->add_task(std::make_shared<myitem::Fiber>([](){
    //     std::cout << "fiber_2 running by " << myitem::Thread::GetName() << std::endl;
    //     sleep(1);
    // }, 8192));

    // scheduler_ptr->add_task(std::make_shared<myitem::Fiber>([](){
    //     std::cout << "fiber_3 running by " << myitem::Thread::GetName() << std::endl;
    //     myitem::Fiber::YieldToReady();
    //     std::cout << "fiber_3(2) running by " << myitem::Thread::GetName() << std::endl;
    //     sleep(1);
    // }, 8192));

    // scheduler_ptr->add_task(std::make_shared<myitem::Fiber>([](){
    //     std::cout << "fiber_4 running by " << myitem::Thread::GetName() << std::endl;
    //     sleep(1);
    // }, 8192));
    // scheduler_ptr->start();
    // scheduler_ptr->stop();



    std::cout << "end of main" << std::endl;
    return 0;
}