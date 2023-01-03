#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h> //epoll_ctl()
#include <fcntl.h>  // fcntl()
#include <unistd.h> // close()
#include <assert.h> // close()
#include <vector>
#include <errno.h>

class Epoller {
public:
    explicit Epoller(int maxEvent = 1024);
    ~Epoller();
    bool AddFd(int fd, uint32_t events);
    bool ModFd(int fd, uint32_t events);
    bool DelFd(int fd);
    int Wait(int timeoutMs = -1);   //调用内核检测事件
    int GetEventFd(size_t i) const; //获取事件的fd
    uint32_t GetEvents(size_t i) const; //获取事件
private:
    int epollFd_;   //epoll_create()创建一个epoll对象后，会返回epollfd
    std::vector<struct epoll_event> events_;    //检测到的事件集合
};

#endif //EPOLLER_H