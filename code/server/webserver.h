#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "epoller.h"
#include "../pool/threadpool.h"
#include "../http/httpconn.h"

class WebServer {
public:
    WebServer(
        int port, int connPoolNum = 12, int threadNum = 14);
    ~WebServer();
    void Start();
private:
    bool InitSocket_(); 
    void AddClient_(int fd, sockaddr_in addr);
    void DealListen_();
    void DealWrite_(HttpConn* client);
    void DealRead_(HttpConn* client);
    void SendError_(int fd, const char*info);
    void ExtentTime_(HttpConn* client);
    void CloseConn_(HttpConn* client);
    void OnRead_(HttpConn* client);
    void OnWrite_(HttpConn* client);
    void OnProcess(HttpConn* client);
    static const int MAX_FD = 65536;
    static int SetFdNonblock(int fd);   //设置文件描述符非阻塞
    int port_;
    bool isClose_;
    int listenFd_;
    char* srcDir_; 
    uint32_t listenEvent_;
    uint32_t connEvent_;
    std::shared_ptr<ThreadPool> threadpool_;
    std::shared_ptr<Epoller> epoller_;
    std::unordered_map<int, HttpConn> httpConn_umap; //客户端连接的信息
};


#endif //WEBSERVER_H