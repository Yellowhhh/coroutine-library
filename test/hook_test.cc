#include "hook.h"
#include "iomanager.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>

void test_sleep()
{
    //myitem::IOManager iom(1, true);

    // iom->add_task(std::make_shared<myitem::Fiber>([](){
    //     sleep(2);
    //     std::cout << "sleep 2" << std::endl;
    // }, 8192));

    // iom->add_task(std::make_shared<myitem::Fiber>([](){
    //     sleep(2);
    //     std::cout << "sleep 2 2" << std::endl;
    // }, 8192));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "220.181.111.1", &addr.sin_addr.s_addr);
    
    // iom->addEvent(sock, myitem::IOManager::WRITE, [](){
    //     std::cout << "sock write " << std::endl;
    // });
    std::cout << "begin connect" << std::endl;
    int rt = connect(sock, (const sockaddr *)&addr, sizeof(addr));
    std::cout << "connect rt = " << rt << " errno = " << errno << std::endl;
    if(rt)
    {
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    std::cout << "send rt = " << rt << " errno = " << errno << std::endl;
    if(rt <= 0)
    {
        return;
    }

    std::string buff;
    buff.resize(4096);

    rt = recv(sock, &buff[0], buff.size(), 0);
    std::cout << "recv rt = " << rt << " errno = " << errno << std::endl;

    if(rt <= 0)
    {
        return;
    }

    buff.resize(rt);
    std::cout << buff << std::endl;
}


int main()
{
    myitem::IOManager::ptr iom = std::make_shared<myitem::IOManager>(2, true);
    iom->start();
    iom->add_task(std::make_shared<myitem::Fiber>([](){test_sleep();}, 8192));
    iom->stop();
    return 0;
}