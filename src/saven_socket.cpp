//saven_socket.cpp

#ifndef _SAVEN_SOCKET_CPP_
#define _SAVEN_SOCKET_CPP_

#include"include/saven_socket.h"
#include<string.h>
#include<iostream>

/*通用函数*/

//设置端口可重用
void SavenSocket::SetReusePort(int fd){
    int opt = 1;
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(const void*)&opt,sizeof(opt));
}

//设置文件描述符为非阻塞
int SavenSocket::SetNonBlocking(int fd){
    int old_option = fcntl(fd,F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}

/*Socket基类*/

//构造函数
SavenSocket::Socket::Socket(const char* ip,int port){
    fd = -1;
    address_length = sizeof(address);
    bzero(&address,address_length);
    if( !(ip==nullptr && port==80) ){
        if(ip == nullptr){
            address.sin_addr.s_addr = htonl(INADDR_ANY);
        }else{
            inet_pton(AF_INET,ip,&address.sin_addr);
        }
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
    }
}

//关闭文件描述符
void SavenSocket::Socket::close(){
    if(this->fd >= 0){
        ::close(this->fd);
        fd = -1;
    }
}

/*ServerSocket服务器套接字*/

//构造函数
SavenSocket::ServerSocket::ServerSocket(const char* ip_,int port_):Socket(ip_,port_){
    int ret = -1;
    ret = socket(AF_INET,SOCK_STREAM,0);
    if(ret == -1){
        printf("create socket error in file <%s> at line %d\n" ,__FILE__,__LINE__);
        exit(0);
    }
    this->fd = ret;
    SetNonBlocking(this->fd);
    SetReusePort(this->fd);//设置端口可重用
}

//绑定操作
int SavenSocket::ServerSocket::bind(){
    int ret = -1;
    ret = ::bind(this->fd,(struct sockaddr*)&this->address,this->address_length);
    if(ret == -1){
        printf("socket bind error in file <%s> at line %d\n" ,__FILE__,__LINE__);
        exit(0);
    }
    return ret;
}

//监听端口
int SavenSocket::ServerSocket::listen(int backlog){
    int ret = ::listen(this->fd,backlog);
    if(ret == -1){
        printf("socket listen error in file <%s> at line %d\n" ,__FILE__,__LINE__);
        exit(0);
    }
    return ret;
}

//接受客户端socket
int SavenSocket::ServerSocket::accept(ClientSocket& client)const{
    int listenfd = -1;
    client.client_ip_address = "";
    listenfd = ::accept(this->fd,(struct sockaddr*)&client.address,&client.address_length);
    if(listenfd == -1){
        printf("socket accpet error in file <%s> at line %d\n" ,__FILE__,__LINE__);
        // exit(0); //如果一个客户端接受失败不能影响到主进程的正常运行
        client.fd = -1;
        return -1;
    }
    //如果客户端连接成功
    //这里添加关于客户端程序的ip地址解析程序
    char* temp = inet_ntoa(client.address.sin_addr);
    if(*temp == INADDR_NONE){
        printf("[sys]:INADDR_NONE\n");
        client.fd = listenfd;
        return listenfd;
    }
    //获取IPv4地址
    std::string line = std::string(temp);
    printf("[sys]:Connecttion:%s\n",temp);
    client.client_ip_address = line;
    client.fd = listenfd;
    return listenfd;
}

//获取服务器端的socket_fd
int SavenSocket::ServerSocket::getSocketfd(){
    return this->fd;
}


/*ServerSocket客户端套接字*/

SavenSocket::ClientSocket::ClientSocket(){
    //直接初始化,无代码
}



#endif










// //获取fd
// int SavenSocket::ServerSocket::getSocketfd(){
//     return this->fd;
// }

// // int SavenSocket::ClientSocket::



// //构造函数-支持ip地址自动分配
// //输入: ip地址 端口号
// SavenSocket::ServerSocket::ServerSocket(const char* ip_,int port_){
//     ip = ip_;
//     port = port_;
//     bzero(&address,sizeof(address));
//     address.sin_family = AF_INET;//协议
//     address.sin_port = htons(port);//端口
//     //分配服务器ip地址
//     if(ip == nullptr){//数值型的ip地址转换用htonl
//         address.sin_addr.s_addr = htonl(INADDR_ANY);
//     }else{//字符串类型的用inet_pton
//         inet_pton(AF_INET,ip,&address.sin_addr);
//     }
//     //分配socket
//     fd = socket(AF_INET,SOCK_STREAM,0);
//     if(fd == -1){
//         printf("create socket error in file <%s> at line %d\n" ,__FILE__,__LINE__);
//         exit(0);
//     }
//     SavenSocket::SetReusePort(fd);//端口可重用
//     SavenSocket::SetNonBlocking(fd);//设置文件描述符为非阻塞
// }

// //绑定socket描述符与socket地址
// int SavenSocket::ServerSocket::bind(){
//     int ret = ::bind(fd,(const sockaddr*)&address,sizeof(address));
//     if(ret == -1){
//         printf("bind error in file <%s> at line %d\n" ,__FILE__,__LINE__);
//         exit(0);
//     }
//     return ret;
// }

// int SavenSocket::ServerSocket::listen(int backlog){
//     int ret = ::listen(fd,backlog);
//     if(ret == -1){
//         printf("listen error in file <%s> at line %d\n" ,__FILE__,__LINE__);
//         exit(0);
//     }
//     return ret;
// }

// int SavenSocket::ServerSocket::accept(ClientSocket& client)const{
//     //阻塞accept()
//     int ret = ::accept(this->fd,(struct sockaddr*)&client.address,&client.address_length);
//     if(ret == -1){
//         printf("accept error in file <%s> at line %d\n" ,__FILE__,__LINE__);
//         exit(0);
//     }
//     client.fd = ret;
//     return ret;
// }

// void SavenSocket::ServerSocket::close(){
//     ::close(this->fd);
// }

// void SavenSocket::ClientSocket::close(){
//     ::close(this->fd);
// }

