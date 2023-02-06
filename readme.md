# sv-JTracing(Online)

| **中文** | [English](readme_en.md) |

利用C++实现针对SystemVerilog的高性能在线编译系统，可将SystemVerilog源代码进行高鲁棒性的词法解析和常见语法分析和部分语义分析，生成可靠Abstract Syntax Tree，并提供Parser解析过程信息、报错信息和变量表，该在线编译系统通过webbenchh压力测试可以实现近万的QPS

## 功能
* 利用逐字符扫描SystemVerilog源代码进行高鲁棒性词法分析，稳定生成对应Token;
* 利用递归下降法手工构建语法分析器，实现SystemVerilog常见语法检查，生成分析结果并输出可靠抽象语法树；
* 利用IO复用技术Epoll与线程池实现多线程的Reactor高并发模型；
* 利用状态机解析HTTP请求报文并转义url字符编码，实现分析与处理静态资源的请求；

## Demo演示
----------
> * __在线编译系统整体视频演示__
<div align=center><img src="https://github.com/AllwenWeill/IMG/blob/main/video_show.gif" height="429"/> </div>

> * __样例1-module__
```bash
module m;
    // hello
    bit [4:0] b;
    int b; //注意：此处有重命名错误
    begin end
endmodule
```
![image](https://github.com/AllwenWeill/IMG/blob/main/vedio_medule.gif)

> * __样例2-for__
```bash
int a = 0;
for (int i = 0; i <= 5; i++) begin
        a = a + 1;
end
```
![image](https://github.com/AllwenWeill/IMG/blob/main/video_for.gif)

> *__样例2中SystemVerilog代码整体AST抽象语法树结构展示:__
![image](https://github.com/AllwenWeill/IMG/blob/main/AST_whole.png)

> *__其中for循环内AST抽象语法树完整展示:__
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
[@Ruslan Spivak](https://github.com/rspivak/): Let's Build A Simple Interpreter

《Linux高性能服务器编程》，游双著.

[@qinguoyi](https://github.com/qinguoyi/TinyWebServer)：TinyWebServer开源项目

[@MikePopoloski](https://github.com/MikePopoloski/slang)：slang开源项目

 
