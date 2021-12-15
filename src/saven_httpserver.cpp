//saven_httpserver.cpp

#ifndef _SAVEN_HTTPSERVER_CPP_
#define _SAVEN_HTTPSERVER_CPP_

#include"include/saven_httpserver.h"
#include<algorithm>
#include<fstream>

//统一信号管道:Unified signal pipeline
#define UniSigPipe SavenHttpServer::HttpServer::sig_pipefd
// #define UniSigPipe (HttpServer::sig_pipefd)

//静态成员变量的定义
int SavenHttpServer::HttpServer::sig_pipefd[2];

//信号自定义函数
void SavenHttpServer::handler(int sig){
    int save_errno = errno;
    int msg = sig;
    send(SavenHttpServer::HttpServer::sig_pipefd[1],&msg,1,0);
    errno = save_errno;
}



//信号设置自定义函数
void SavenHttpServer::HttpServer::addsig(int sig,void(handler)(int),bool restart = true){
    struct sigaction sa;
    bzero(&sa,sizeof(sa));
    sa.sa_handler = handler;
    if(restart)sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);
    int ret = sigaction(sig,&sa,NULL);
    if(ret < 0){
        printf("sigaction error in file <%s> at line %d\n",__FILE__,__LINE__);
        exit(0);
    }
}

//向epollfd中注册fd文件描述符
void SavenHttpServer::HttpServer::addfd(int fd){
    epoll_event event;
    bzero(&event,sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    SavenSocket::SetNonBlocking(fd);
}

//从epollfd中删除指定的文件描述符fd
void SavenHttpServer::HttpServer::delfd(int fd){
    epoll_event event;
    bzero(&event,sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&event);
}

//清除fdKeeper中现存的fd
void SavenHttpServer::HttpServer::delfdKeeper(){
    for(int i=0;i<fdKeeper.size();++i){
        delfd(fdKeeper[i]);
        ::close(fdKeeper[i]);
    }
    fdKeeper.clear();
}

//删除epllfd中的某个文件描述符fd并将其关闭
void SavenHttpServer::HttpServer::disConnect(int fd){
    delfd(fd);
    ::close(fd);
    std::vector<int>::iterator iter = std::find(fdKeeper.begin(),fdKeeper.end(),fd);
    if(iter == fdKeeper.end()){
        printf("[fdKeeper find error]:disConnect()\n");
    }else{
        fdKeeper.erase(iter);
    }
}


//构造函数
SavenHttpServer::HttpServer::HttpServer(const char* ip_,int port_,int(*solverptr)(int,std::string,void*,void*))
:server_socket(ip_,port_),solver(solverptr){
    //初始化服务器socket(非阻塞)
    printf("[sys-init] init server..\n");
    server_socket.bind();
    server_socket.listen();
    //初始化信号管道
    printf("[sys-init] init sigpipe..\n");
    int ret = socketpair(PF_UNIX,SOCK_STREAM,0,UniSigPipe);
    if(ret < 0){
        printf("create socketpair error in file <%s> at line %d\n" ,__FILE__,__LINE__);
        exit(0);
    }
    SavenSocket::SetNonBlocking(UniSigPipe[0]);
    SavenSocket::SetNonBlocking(UniSigPipe[1]);
    //初始化信号
    printf("[sys-init] init sig handler()..\n");
    addsig(SIGCHLD,handler);
    addsig(SIGINT,handler);
    addsig(SIGTERM,handler);
    addsig(SIGPIPE,SIG_IGN);
    //初始化epoll注册表
    printf("[sys-init] init epoll..\n");
    this->epollfd = epoll_create(HttpServer::epoll_size);
    addfd(server_socket.getSocketfd());//监听服务器
    addfd(UniSigPipe[0]);//监听信号管道
    //初始化epoll_event列表
    printf("[sys-init] init epoll_event events..\n");
    this->events = new epoll_event[max_epoll_events];
}



//返回值: 正常退出0 错误退出-1
int SavenHttpServer::HttpServer::run(){
    printf("[sys-prepare]:run()\n");
    //检查报文解析函数指针是否为空
    if(this->solver == nullptr){
        printf("[sys-check]:int (*solver)(int,std::string,void*,void*)==nullptr\n");
        return -1;
    }
    this->fdKeeper.clear();
    this->state_stop = false;
    int return_value = 0;
    printf("[Server]:run()\n");
    while(!state_stop){
        int number = epoll_wait(epollfd,events,max_epoll_events-1,-1);
        if(number < 0 && errno != EINTR){
            //如果epoll_wait()发生错误且错误不是因为信号处理函数的阻断而产生
            //那么就停止循环
            this->state_stop = true;
            return_value = -1;
            continue;
        }
        int ret = -1;
        //正式处理epoll事件
        for(int i=0;i<number;i++){
            int sockfd = events[i].data.fd;
            if((sockfd == server_socket.fd) && (events[i].events & EPOLLIN)){
                //主socket中存在客户端连接请求
                SavenSocket::ClientSocket oneClient;
                ret = server_socket.accept(oneClient);
                if(ret == -1){
                    printf("accept(Client&) error\n");
                    continue;
                }
                //注册对该socket的监听
                addfd(ret);
                //之后需要识别客户的socket
                fdKeeper.push_back(ret);
                continue;
            }else if((sockfd == UniSigPipe[0])&&(events[i].events&EPOLLIN)){
                //信号处理
                char signals[100];
                memset(signals,0,sizeof(signals));
                ret = recv(sockfd,(void*)signals,99,0);
                for(int k=0;k<ret;k++){
                    switch(signals[k]){
                        case SIGINT:
                        case SIGTERM:{
                            if(signals[k] == SIGINT)printf("[sig]:SIGINT\n");
                            else printf("[sig]:SIGTERM\n");
                            this->state_stop = true;
                            break;
                        }
                        case SIGCHLD:{
                            printf("[sig]:SIGCHLD\n");
                            //目前是单进程程序,不理会该信号
                            break;
                        }
                        default:{
                            printf("[sig-other]:%d\n",signals[k]);
                            break;
                        }
                    }
                }//完成信号处理
            }else if(events[i].events & EPOLLIN){
                //存在输入,启动http相关模块
                HeaderSolve::HeaderMSGs headerInfo;
                ret = header_solver.HeaderParse(sockfd,headerInfo);
                if(ret == 0){
                    //数据读取失败(可能是链接断开)
                    printf("[Sockfd:%d]:Header Parse Error:LinkBreak\n",sockfd);
                    disConnect(sockfd);
                    continue;
                }else if(ret < 0){
                    //缓冲区过小而失败 -1
                    //解析出现行格式错误问题而中断读取 -2
                    printf("[Sockfd:%d]:Header Parse Error:LinkAlive\n",sockfd);
                    continue;
                }else{
                    //可以进行首部检查了
                    //插入检测首行信息的程序标签
                    // solve(sockfd,headerInfo);
                    std::string firstline = headerInfo[0].getContent();
                    SavenHeaderParse::FirstLine fli;
                    ret = this->header_solver.getPtrParser()->firstlineParse(firstline,fli);
                    if(ret == 0){
                        printf("[Sockfd:%d]:FirstLine Parse Error\n",sockfd);
                        disConnect(sockfd);
                        continue;
                    }
                    printf("[Scokfd:%d]:Head:%s\n",sockfd,fli.midLine.c_str());
                    //开始调用
                    // solve(sockfd,fli.midLine);
                    if(this->solver){
                        //传参说明:   socket,url请求目录,http请求字段,暂不定
                        this->solver(sockfd,fli.midLine,&headerInfo,NULL);
                        printf("[Sockfd:%d]:Call-for solver()\n",sockfd);
                    }
                    disConnect(sockfd);
                    /*HTTP是一个无状态的面向连接的协议
                    无状态不代表HTTP不能保持TCP连接
                    更不能代表HTTP使用的是UDP协议*/
                    continue;
                }
            }else{
                continue;
            }
        }//处理epoll事件
    }//循环服务器

    printf("[Server]:run():stop\n");
    //每次运行结束必须结束所有的用户fd
    this->delfdKeeper();
    return return_value;
}

//析构函数
SavenHttpServer::HttpServer::~HttpServer(){
    //删除epoll事件内存
    delete[] events;
    //关闭统一信号管道
    ::close(UniSigPipe[0]);
    ::close(UniSigPipe[1]);
    //关闭epoll文件描述符
    ::close(epollfd);
    //关闭服务器socket文件描述符
    server_socket.close();
    //其余需要关闭的文件描述符应当通过用户注册表关闭
    for(int i=0;i<fdKeeper.size();i++){
        ::close(fdKeeper[i]);
    }
    fdKeeper.clear();
}






//screen命令与后台运行
//https://www.jianshu.com/p/09d86aad9fa4


// map<string,string> mapper{
//     {"night","night"},
//     {"allday","allday"},
//     {"all","all"},
//     {"am","am"},
//     {"pm","pm"},
//     {"AM","am"},
//     {"PM","pm"},
//     {"A","0102"},
//     {"B","0304"},
//     {"C","0506"},
//     {"D","0708"}
// };

#endif








