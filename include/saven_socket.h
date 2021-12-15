//基础的socket类

#ifndef _SAVEN_SOCKET_H_
#define _SAVEN_SOCKET_H_

#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include<string>


namespace SavenSocket{
    
    //设置端口重用
    void SetReusePort(int fd);
    //设置文件描述符为非阻塞
    int SetNonBlocking(int fd);

    class Socket{
    public:
        //文件描述符
        int fd;
        //socket_address
        sockaddr_in address;
        socklen_t address_length;
        
        //构造函数
        Socket(const char* ip = nullptr,int port = 80);
        //关闭文件描述符
        void close();
    };

    class ClientSocket;

    class ServerSocket:public Socket{
    public:
        //构造函数
        ServerSocket(const char* ip_ = nullptr,int port_ = 80);
        //socket地址绑定
        int bind();
        //socket监听
        int listen(int backlog = 5);
        //accpet()
        int accept(ClientSocket& client)const;
        //获取fd
        int getSocketfd();
    };

    class ClientSocket:public Socket{
    public:
        ClientSocket();

        //目前仅仅支持IPv4
        std::string client_ip_address;
    };

};


#endif

