# WebServer
用C++实现的高性能WEB服务器，经过webbenchh压力测试可以实现上万的QPS

## 功能
* 利用IO复用技术Epoll与线程池实现多线程的Reactor高并发模型；
* 利用正则与状态机解析HTTP请求报文，实现处理静态资源的请求(初代版本http请求解析方法非常笨拙，通过固定范围位区间对应的含义进行解析)
* 利用标准库容器封装char，实现自动增长的缓冲区；

问题：
1. 主线程Reactor只负责用epoll进行监听，当epoll返回内核监听的事件数量后，由子线程负责判断事件类型进行业务处理，主线程可同时监听1024fd，由14个子线程并发处理业务，那么如何支持1w+并发呢？

## 环境要求
* Linux
* C++14

## 目录树
```
.
├── code           源代码
│   ├── buffer
│   ├── config
│   ├── http
│   ├── log
│   ├── timer
│   ├── pool
│   ├── server
│   └── main.cpp
├── test           单元测试
│   ├── Makefile
│   └── test.cpp
├── resources      静态资源
│   ├── index.html
│   ├── image
│   ├── video
│   ├── js
│   └── css
├── bin            可执行文件
│   └── server
├── log            日志文件
├── webbench-1.5   压力测试
├── build          
│   └── Makefile
├── Makefile
├── LICENSE
└── readme.md
```


## 项目启动
```bash
make
./bin/server
```

## Webbench测试结果
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://192.168.48.128:1316/
7000 clients, running 5 sec.

Speed=132156 pages/min, 6923630 bytes/sec.
Requests: 11013 susceed, 0 failed.



## 致谢
Linux高性能服务器编程，游双著.

[@qinguoyi](https://github.com/qinguoyi/TinyWebServer)
