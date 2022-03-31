# SavenHttpServer
Concurrent-HttpServer Ubuntu-Epoll-based

单线程并发Http服务器，基于POSIX与EPOLL实现

运行环境Ubuntu20.04 server LTS

这是一个基础的Http服务器框架，用于后端工作时分析各类HTTP协议，支持HTTP1.0/1.1。

* 框架支持动态的协议头加载与卸载，之后会陆续更新出插件机制。

* 框架只提供Http的相关链接服务，并提供指定的协议头分析提取

* 框架本身提供秒级定时器结构，默认不接入服务框架，需要根据需求手动配置

