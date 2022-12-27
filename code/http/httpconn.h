#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <sys/types.h>
#include <sys/uio.h>     // readv/writev
#include <arpa/inet.h>   // sockaddr_in
#include <stdlib.h>      // atoi()
#include <errno.h>      

#include "../buffer/buffer.h"
#include "httprequest.h"
#include "httpresponse.h"

class HttpConn {
public:
    HttpConn();
    ~HttpConn();
    void init(int sockFd, const sockaddr_in& addr);
    ssize_t read(int* saveErrno);
    ssize_t write(int* saveErrno);
    void Close();
    int GetFd() const;
    int GetPort() const;
    const char* GetIP() const;
    sockaddr_in GetAddr() const;
    bool process();
    int ToWriteBytes() { 
        return iov_[0].iov_len + iov_[1].iov_len; 
    }
    bool IsKeepAlive() const {
        return request_.IsKeepAlive();
    }
    static bool isET;
    static const char* srcDir;
    static std::atomic<int> userCount;
private:
    int fd_;
    struct  sockaddr_in addr_;
    bool isClose_;
    int iovCnt_;
    struct iovec iov_[2];//iov_base指向一个缓冲区，这个缓冲区是存放readv所接收的数据或是writev将要发送的数据,iov_len确定了接收的最大长度以及实际写入的长度
    Buffer readBuff_; // 读缓冲区
    Buffer writeBuff_; // 写缓冲区
    HttpRequest request_;
    HttpResponse response_;
};


#endif //HTTP_CONN_H