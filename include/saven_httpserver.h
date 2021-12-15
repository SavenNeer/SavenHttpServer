//saven_httpserver.h

#ifndef _SAVEN_SERVER_H_
#define _SAVEN_SERVER_H_

#include"include/saven_socket.h"
#include"include/saven_headersolver.h"
#include"include/saven_headerbuilder.h"
#include<stdio.h>
#include<sys/epoll.h>
#include<signal.h>
#include<stdlib.h>
#include<errno.h>
#include<vector>
#include<string.h>

#define Parser tempParser

namespace SavenHttpServer{
    void handler(int sig);
    // int solve(int sockfd,std::string midline);


    class HttpServer{
    private:
        //服务器socket
        SavenSocket::ServerSocket server_socket;
        //服务器运行状态
        bool state_stop = false;

        //epoll注册表
        int epollfd;
        //epoll注册表大小
        static const int epoll_size = 20;
        //最大响应事件数量
        static const int max_epoll_events = 10;
        //缓冲区大小
        static const int buffer_size = 1024;
        //接收响应事件
        epoll_event* events = nullptr;
        //用户注册表:目前仅用于记录处于工作状态的文件描述符
        std::vector<int> fdKeeper;

        //添加自定义函数到信号sig
        void addsig(int sig,void(handler)(int),bool restart);
        //向epollfd指向的注册表内添加文件描述符
        void addfd(int fd);
        //从epollfd中删除指定的fd
        void delfd(int fd);
        //使fdKeeper中在线的文件描述符失效
        void delfdKeeper();
        //删除epllfd中的某个文件描述符fd并将其关闭
        void disConnect(int fd);
        //首部解析器
        HeaderSolve::HeaderSolver header_solver;
    public:
        //统一信号管道 cpp文件中使用 宏UniSigPipe 表示 sig_pipefd
        static int sig_pipefd[2];//静态成员变量的声明
        explicit HttpServer(const char* ip_ = nullptr,int port_ = 80,int(*solverptr)(int,std::string,void*,void*)=nullptr);
        //析构函数
        ~HttpServer();
        //运行服务器:正常结束返回0 错误结束返回-1
        int run();
        //自定义报文处理函数指针
        int (*solver)(int sockfd,std::string midline,void* userdata,void* data);
    };
};

#endif










/*状态转换     run()        stop()
        [isrun] false =====> true  ======> false
        [ stop] false =====> false ======> true
        */
/*State-Table
        state_num value(isrun/stop)  sys_state_flag   run():return/next  stop():return/next
            [0]        false/false      no_running           1/[1]             0/Error
            [1]        true/false       running              0/[1]             1/[2->3->0]
            [2]        true/true      go to no_running       0/Error           0/Error
            [3]        false/true       no_running     
        */









