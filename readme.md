# sv-JTracing(Online)
利用C++实现针对SystemVerilog的高性能在线编译系统，可将SystemVerilog源代码进行高鲁棒性的词法解析和常见语法分析和部分语义分析，生成可靠Abstract Syntax Tree，并提供Parser解析过程信息、报错信息和变量表，该在线编译系统通过webbenchh压力测试可以实现近万的QPS

## 功能
* 利用IO复用技术Epoll与线程池实现多线程的Reactor高并发模型；
* 利用正则与状态机解析HTTP请求报文，实现处理静态资源的请求
* 利用标准库容器封装char，实现自动增长的缓冲区；

## 效果展示
样例SystemVerilog代码整体AST抽象语法树结构展示
![image](https://github.com/AllwenWeill/IMG/blob/main/AST_whole.png)

其中for循环内AST抽象语法树完整展示
![image](https://github.com/AllwenWeill/IMG/blob/main/AST_for.png)

## 环境要求
* Linux环境
* C++17
* 服务器程序运行端口默认使用4466，运行前请查看是否占用该端口

## 版本迭代
.__HTTP请求解析__：
初代版本http请求解析方法非常笨拙，通过固定范围位区间对应的含义，通过逐字符扫描进行解析

.---->更新：利用正则运算和有限状态机解析HTTP请求报文

.__SystemVerilog代码编译结果回传__：
初代版本识别到“编译按钮”后，将修改响应报文content-type为text/plain，然后将编译结果生成string字符串添加到响应体内回传给浏览器

.---->_更新_：将SystemVerilog代码编译结果写入resource目录中的.txt文本中，然后浏览器端请求该.txt资源并获取其文本内容，因为是对同一文件写入再清除，该文件则为临界资源，本server程序在业务处理时采用多线程方式为对该临界资源造成读写错误，则采用加互斥量来保证原子性

.---->_更新_：由于server程序在高并发状态时采用互斥锁会明显抑制整体程序的效率，因此采用将每个线程随机生成文件并写入的方式，在线程执行结束后将该文件删除，核心思想是利用空间换时间从而提高程序效率。(但目前问题是该线程创建的.txt文件如何让浏览器知道去请求哪个.txt文件资源？)


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
《Linux高性能服务器编程》，游双著.

[@qinguoyi](https://github.com/qinguoyi/TinyWebServer)：TinyWebServer开源项目
