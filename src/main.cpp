#include<iostream>
#include<string>
#include<fstream>
#include"include/utils.h"
#include"include/saven_headerparse.h"
#include"include/saven_socket.h"
#include"include/saven_httpserver.h"
#include"include/saven_headerbuilder.h"
#include<unordered_map>
using namespace std;
//主函数模式
#define MAINCPP



//服务器主函数代码
#ifdef MAINCPP



//回复主页面
int SendIndexPage(int sockfd){
    //打开index.html文件
    int indexfile = open("index.html",O_RDONLY);
    char buf[3000];
    memset(buf,0,sizeof(buf));
    int ret = read(indexfile,buf,3000-1);
    if(ret == -1){
        printf("[SendIndexPage()]:read index.html error\n");
        return 0;
    }
    close(indexfile);
    int lenth = strlen(buf);
    printf("[index.html]:length == %d\n",lenth);
    char len[10];
    sprintf(len,"%d",lenth);
    string content_length = string(len);
    string head = "";
    head += "HTTP/1.1 200 OK\r\n";
    head += ("Content-Length: " + content_length + "\r\n");
    head += "Content-Type: text/html; charset=UTF-8\r\n";
    head += "Date: Fri, 26 Feb 2021 07:40:31 GMT\r\n";
    head += "\r\n";
    head += string(buf);
    //发送主要文件
    printf("[index.html]:send\n");
    send(sockfd,head.c_str(),head.size(),0);
    return 0;
}

string search_headinfo(void* headdata,const string key){
    HeaderSolve::HeaderMSGs &temp = *((HeaderSolve::HeaderMSGs*)headdata);
    int lenth = temp.size();
    for(int i=0;i<lenth;i++){
        HeaderSolve::HeaderMSG& msg = temp[i];
        if(msg.getHeader() == key){
            return SavenUtils::trim(msg.getContent());
        }
    }
    return "";
}

//适配到服务器接口
int LocalSolve(int sockfd,const string midline,void* userdata,void* data){
    printf("[LocalSolve]:[midline]:%s\n",midline.c_str());
    SendIndexPage(sockfd);
    return 0;
}


//2
//0 1
int main(int argc,char* argv[]){
    SavenHttpServer::HttpServer myServer("172.17.0.6",12345,LocalSolve);
    myServer.solver = LocalSolve;//配置响应函数
    // SavenHttpServer::HttpServer myServer("172.17.0.6",12345);
    myServer.run();
    return 0;
}






#else







//测试代码块

char aim[20] = "HOst";



int main(){
    SavenHeaderParse::HeaderParser localparser;
    string ans = localparser.Parse(aim,strlen(aim));
    if(localparser.support("hoSt"))cout << "ok" << endl;
    cout << "[" << ans << "]" << endl;
    return 0;
}












#endif


/*
int LocalSolve(int sockfd,const std::string midline,void* userdata,void* data){
    printf("[solve()]:ok\n");
    //请求格式说明
//    /room?[order]
//    /room?night
    int lenth = midline.size();
    int cur = midline.find_first_of("?");
    if(lenth <= 6 || cur == midline.npos){
        const char* buf = "nolink";
        send(sockfd,buf,5,0);
        return -1;
    }
    std::string flag = "";
    cur++;
    while(cur < lenth){flag+=midline[cur++];}
    std::string order = "./Search_classroom " + flag;
    // system(order.c_str());
    printf("[order]:%s\n",order.c_str());
    system(order.c_str());
//  获取需要发送的信息 
    std::string infos = "";
    std::ifstream inner;
    inner.open("jsresult.rslt",std::ios::in);
    while(std::getline(inner,flag)){
        // printf("[%s]\n",flag.c_str());
        infos += flag;
        infos += '\n';
    }
    inner.close();
//  接下来发送信息
    //计算字符串长度
    lenth = infos.size();//获取信息长度
    char num[10];
    memset(num,0,sizeof(num));
    sprintf(num,"%d",lenth);
    std::string hd1 = "HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=UTF-8\r\nDate: Fri, 26 Feb 2021 07:40:31 GMT\r\n";
    std::string hd2 = "Content-Length: " + std::string(num) + "\r\n\r\n";
    //组合信息并发送
    infos = hd1 + hd2 +infos;
    lenth = infos.size();
    char* buf = new char[lenth+3];
    memset(buf,0,sizeof(buf));
    memcpy(buf,infos.c_str(),lenth);
    // printf("[MSG]:\n%s\n",buf);
    lenth = strlen(buf);
    printf("[MSG-size]:%d\n",lenth);
    send(sockfd,buf,lenth,0);
    delete[] buf;
    //
    printf("[send()]:ok\n");

    return 0;
}
*/

/*

//适配到服务器接口
int LocalSolve(int sockfd,const string midline,void* userdata,void* data){
    printf("[LocalSolve]:[midline]:%s\n",midline.c_str());
    SendIndexPage(sockfd);
    //下面需要分情况回应
    // if(midline == "/"){
    //     SendIndexPage(sockfd);
    // }else if(midline == "/favicon.ico"){
    //     printf("[favicon.ico]...\n");
    // }else if(midline == "/upload"){
    //     printf("[UpLoad]....\n");
    //     char buf[300];
    //     printf("\n=============\n");
    //     while(1){
    //         memset(buf,0,sizeof(buf));
    //         int ret = recv(sockfd,buf,300-1,0);
    //         if(ret < 0){
    //             if(errno != EINTR){
    //                 close(sockfd);
    //                 break;
    //             }
    //             break;
    //         }else if(ret == 0){
    //             close(sockfd);
    //             break;
    //         }else{
    //             for(int i=0;i<ret;i++){
    //                 char ch = buf[i];
    //                 if(ch == '\r'){
    //                     printf("\\r");
    //                 }else if(ch == '\n'){
    //                     printf("\\n");
    //                 }else{
    //                     printf("%c",ch);
    //                 }
    //             }
    //         }
    //     }
    //     printf("\n=============\n");
    //     //获取文件的结束标志
    //     string fileType = search_headinfo(userdata,"Content-Type");
    //     cout << "[" << fileType << "]" << endl;
    // }else{
    //     printf("[Else]:SendIndexPage()....\n");
    //     SendIndexPage(sockfd);
    // }
    return 0;
}
*/