#include "webserver.h"

using namespace std;

WebServer::WebServer(int port, int threadNum)
    :port_(port), 
    isClose_(false),
    threadpool_(new ThreadPool(threadNum)), 
    epoller_(new Epoller())
    {
    srcDir_ = getcwd(nullptr, 256);
    assert(srcDir_);
    strncat(srcDir_, "/resources/", 16);
    HttpConn::userCount = 0;
    HttpConn::srcDir = srcDir_;
    listenEvent_ = EPOLLRDHUP;
    connEvent_ = EPOLLONESHOT | EPOLLRDHUP;
    listenEvent_ |= EPOLLET;
    connEvent_ |= EPOLLET;
    HttpConn::isET = (connEvent_ & EPOLLET);
    if(!InitSocket_()) { isClose_ = true;}
}

WebServer::~WebServer() {
    close(listenFd_);
    isClose_ = true;
    free(srcDir_);
}


void WebServer::Start() {
    int timeMS = -1;  /* epoll wait timeout == -1 无事件将阻塞 */
    if(!isClose_) { 
        cout<<"========== Server start =========="<<endl; 
    }
    while(!isClose_) {
        int eventCnt = epoller_->Wait(timeMS);
        for(int i = 0; i < eventCnt; i++) {
            /* 处理事件 */
            int fd = epoller_->GetEventFd(i);
            uint32_t events = epoller_->GetEvents(i);
            if(fd == listenFd_) {
                DealListen_();
            }
            else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                assert(httpConn_umap.count(fd) > 0);
                CloseConn_(&httpConn_umap[fd]);
            }
            else if(events & EPOLLIN) {
                assert(httpConn_umap.count(fd) > 0);
                DealRead_(&httpConn_umap[fd]);
            }
            else if(events & EPOLLOUT) {
                assert(httpConn_umap.count(fd) > 0);
                DealWrite_(&httpConn_umap[fd]);
            } else {
                cout<<"Unexpected event"<<endl;
            }
        }
    }
}

void WebServer::SendError_(int fd, const char*info) {
    assert(fd > 0);
    int ret = send(fd, info, strlen(info), 0);
    if(ret < 0) {
        cout<<"send error to client error!"<<endl;
    }
    close(fd);
}

void WebServer::CloseConn_(HttpConn* client) {
    assert(client);
    epoller_->DelFd(client->GetFd());
    client->Close();
}

void WebServer::AddClient_(int fd, sockaddr_in addr) {
    assert(fd > 0);
    httpConn_umap[fd].init(fd, addr);
    epoller_->AddFd(fd, EPOLLIN | connEvent_);
    SetFdNonblock(fd);
}

void WebServer::DealListen_() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    do {
        int fd = accept(listenFd_, (struct sockaddr *)&addr, &len);
        if(fd <= 0) return;
        else if(HttpConn::userCount >= MAX_FD) {
            SendError_(fd, "Server busy!");
            return;
        }
        AddClient_(fd, addr);
    } while(listenEvent_ & EPOLLET);
}

void WebServer::DealRead_(HttpConn* client) {
    assert(client);
    threadpool_->AddTask(std::bind(&WebServer::OnRead_, this, client));
}

void WebServer::DealWrite_(HttpConn* client) {
    assert(client);
    threadpool_->AddTask(std::bind(&WebServer::OnWrite_, this, client));
}

void WebServer::OnRead_(HttpConn* client) {
    assert(client);
    int ret = -1;
    int readErrno = 0;
    ret = client->read(&readErrno);
    if(ret <= 0 && readErrno != EAGAIN) {
        CloseConn_(client);
        return;
    }
    OnProcess(client);
}

void WebServer::OnProcess(HttpConn* client) {
    if(client->process()) {
        epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);
    } else {
        epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLIN);
    }
}

void WebServer::OnWrite_(HttpConn* client) {
    assert(client);
    int ret = -1;
    int writeErrno = 0;
    ret = client->write(&writeErrno);
    if(client->ToWriteBytes() == 0) {
        /* 传输完成 */
        if(client->IsKeepAlive()) {
            OnProcess(client);
            return;
        }
    }
    else if(ret < 0) {
        if(writeErrno == EAGAIN) {
            /* 继续传输 */
            epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);
            return;
        }
    }
    CloseConn_(client);
}

/* Create listenFd */
bool WebServer::InitSocket_() {
    if(port_ > 65535 || port_ < 1024){
        cout<<"invaild port!"<<endl;
        return false;
    }
    int ret;
    struct sockaddr_in saddr;
    saddr.sin_port = htons(port_);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    //1.创建socket文件描述符
    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd_  < 0){
        close(listenFd_ );
        cout<<"create listenfd fail!"<<endl;
        return false;
    }
    int optval = 1; //标志位，等于1表示需要端口复用
    ret = setsockopt(listenFd_ , SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int)); //设置端口复用，防止服务器重启时之前的端口没有释放或者程序突然退出时系统没有释放端口
    if(ret == -1){
        cout<<"setsockopt fail!"<<endl;
        return false;
    }
    //2.绑定
    ret = bind(listenFd_ , (struct sockaddr * )&saddr, sizeof(saddr));
    if(ret < 0){
        cout<<"bind fail!"<<endl;
        return false;
    }
    //3.监听
    ret = listen(listenFd_ , 8);
    if(ret < 0){
        cout<<"listen fail!"<<endl;
        return false;
    }
    //4.调用epoll_create()创建一个epoll实例
    ret = epoller_->AddFd(listenFd_ , listenEvent_ | EPOLLIN);
    if(ret == 0){
        cout<<"Add epoll fail!"<<endl;
        return false;
    }
    SetFdNonblock(listenFd_ );
    return true;
}

int WebServer::SetFdNonblock(int fd) {
    assert(fd > 0);
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}


