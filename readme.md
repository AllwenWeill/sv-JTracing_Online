# WebServer
用C++实现的高性能WEB服务器，经过webbenchh压力测试可以实现近万的QPS

## 功能
* 利用IO复用技术Epoll与线程池实现多线程的Reactor高并发模型；
* 利用正则与状态机解析HTTP请求报文，实现处理静态资源的请求
* 利用标准库容器封装char，实现自动增长的缓冲区；

问题：
1. 主线程Reactor只负责用epoll进行监听，当epoll返回内核监听的事件数量后，由子线程负责判断事件类型进行业务处理，主线程可同时监听1024fd（linux通过ulimit -n默认设置最多打开1024个fd），由14个子线程并发处理业务，那么如何支持1w+并发呢？
答： 但是对于服务器本身来说，一直都是用 1316 这个端口跟客户端建立连接。所以，端口的个数跟服务器的并发数没有直接关系。并不会因为端口满了，就不能处理客户端的连接请求。socket通过绑定操作占领 8000 端口，接下来其他程序将不能使用 8000 端口。一旦 8000 端口收到数据，系统都会转发给该程序，所以不会出现微信好友发送的数据，被QQ给收到了。可以简单地理解成，操作系统通过端口号，把不同的应用程序区分开。那 accept 建立的新的连接怎么理解？如果没有新增端口，服务器怎么区分不同的客户端？socket 建立的 TCP 连接包含了两个信息，一个是源 IP 和端口，就是数据来自哪里，一个是目的IP和端口，就是把数据发送到哪。虽然他们用的都是 8000 端口，但是socket可以根据源 IP 和端口号区分出不同的连接，在Linux里面，就是对应不同的文件描述符。所以不用担心数据会乱掉。但源IP+源Port生成一个fd，服务器只支持1024个fd,那岂不是最多还是只支持1024个客户端？这个想法是不对的，因为epoll会创建一个简单的文件系统，运行在内核创建的cache中，它会存储将要监听的socket句柄，该句柄应该并不是linux中可以ulimit -n修改的文件描述符，而只是适用于epoll创建的文件系统中的一个句柄。所以。实际上在你调用epoll_create后，内核就已经在内核态開始准备帮你存储要监控的句柄了。每次调用epoll_ctl仅仅是在往内核的数据结构里塞入新的socket句柄。在内核里，一切皆文件。所以，epoll向内核注冊了一个文件系统，用于存储上述的被监控socket。当你调用epoll_create时，就会在这个虚拟的epoll文件系统里创建一个file结点。当然这个file不是普通文件，它仅仅服务于epoll。（那么epoll支持的最大链接数是进程最大可打开的文件的数目，仍没有解答为什么进程最多支持1024fd却可以支持1w并发？）


## 开发需求：
1. 通过解析http请求得知用户点击“编译按钮”后，然后开始解析SystemVerilog代码，将生产解析结果放在文本框类组件中，由html静态网页

## 环境要求
* Linux
* C++14

## 版本迭代
```
.初代版本http请求解析方法非常笨拙，通过固定范围位区间对应的含义，通过逐字符扫描进行解析
.---->更新：利用正则运算和有限状态机解析HTTP请求报文

```


## 项目启动
```bash
make
./bin/server
```

## Webbench测试结果
```bash
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://192.168.48.128:1316/
7000 clients, running 5 sec.

Speed=132156 pages/min, 6923630 bytes/sec.
Requests: 11013 susceed, 0 failed.
```


## 致谢
Linux高性能服务器编程，游双著.

[@qinguoyi](https://github.com/qinguoyi/TinyWebServer)
