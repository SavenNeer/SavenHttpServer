//saven_headersolver.h

#ifndef _SAVEN_HEADERSOLVER_H_
#define _SAVEN_HEADERSOLVER_H_

#include"include/saven_headerparse.h"
#include<string>
#include<sys/types.h>
#include<cstring>
#include<sys/socket.h>


namespace HeaderSolve{

    //首部字段信息类型
    enum HeaderMsgType{FIRSTLINE=0,BODY,SECRETBODY,NOFLAG};

    //HeaderMSG用于存储一条首部字段信息
    class HeaderMSG{
    private:
        //首部关键字:必须是HttpHeader格式化后的字符串
        std::string MSG_Header;
        //首部内容字符串:保持原生字符串不变
        std::string MSG_Content;
        //信息类型:可选填
        HeaderMsgType MSG_Type;
    public:
        //构造函数
        HeaderMSG();
        HeaderMSG(const std::string MSG_Header_,const std::string MSG_Content_,HeaderMsgType MSG_Type_);
        //基础操作函数
        void clear();
        std::string getHeader();
        std::string getContent();
        HeaderMsgType getMsgType();
        std::string setHeader(const std::string new_header);
        std::string setContent(const std::string new_content);
        HeaderMsgType setType(HeaderMsgType new_type);
    };

    typedef std::vector<HeaderMSG> HeaderMSGs;
    typedef std::vector<HeaderMSG>::iterator MSGsIter;
    typedef std::vector<std::pair<std::string,std::string> > QuickMSG;

    // enum SolveState{};
    //信息的读取采用单字节的recv()
    class HeaderSolver{
    private:
        //首部的解析器
        SavenHeaderParse::HeaderParser* tempParser;
        //单行解析缓冲区大小
        static const int LineBufferSize = 1024;
        //区别将要读取的首部是否是首行
        bool isFirstLine = true;
    public:
        //构造函数
        HeaderSolver();
        //设置头部解析器 返回旧的解析器
        SavenHeaderParse::HeaderParser* setHeaderParser(SavenHeaderParse::HeaderParser* new_parser);
        //获取指向内部头部解析器的指针
        SavenHeaderParse::HeaderParser* getHeaderParser();
        //向内部解析器添加对某首部的支持 成功返回1 元素插入重复即失败返回0
        int addHeaderSupport(const std::string header_name);
        //向内部解析器删除对某首部的支持 取消指定首部:成功返回1 失败返回0
        int deleteHeaderSupport(const std::string header_name);
        //获取指向首部解析器的指针
        SavenHeaderParse::HeaderParser* getPtrParser();

        /* 首部解析函数的两种实现 */
        /*返回值如下:
        读取成功但未识别到"\r\n" 2
        因识别到"\r\n"而成功结束读取 1
        数据读取失败(可能是链接断开) 0
        缓冲区过小而失败 -1
        解析出现行格式错误问题而中断读取 -2 */
        int HeaderParse(int sockfd,HeaderMSGs& results);
        //快速解析:忽略信息的类型和容错返回值,直接返回std::vector<pair<string(key),string(value)> >类型的结果
        QuickMSG QuickParse(int sockfd);

    private:
        /* 核心功能函数 */

        /* 函数功能:从recv中获取"一行"的数据
        * 从sockfd中读取一行数据,行终止标志有三种:'\n'和'\r'和"\r\n"
        * 如果读取出现上述三种情况下的任何一种,直接提取本行并保留行终止标志,并再其后用'\0'截断
        * 如果超出了size规定区域仍未出现'\n'则强行使用'\0'截断已经读取的数据
        * size指缓冲区大小,size考虑到最后一位是'\0'并为其留有预备空间,即最多接收(size-1)个字符
        * 返回值:读入的字符数量 发生限存截断:长度的负值 缓冲区过小:-1
        * 
        * 支持对"限存截断"的识别:
        * 由于size本身支持的是buf缓存区的大小,但是也有可能出现这种情况:
        * 由于缓存区大小限制,原本该存入的"\r\n"在存入'\r'后就被强行截断,
        * 单独留下的'\n'被当成了单独的一行.为了避免这种情况的发生,
        * 当cutter参数为true时,函数自动判定是否出现了限存截断.
        * 如果出现了该情况,程序会读取之后的'\n'但不会放入缓存中,
        * 其返回值会变为读入字符数量的相反数,即:(1-size).
        * cutter为false时不支持限存截断的识别,可能会发生限存截断 */
        
        /*返回值如下:
        成功 ≥0
        缓冲区过小 -1
        读取单独的"\r\n" -2
        数据读取失败(可能是链接断开) -3
        发生限存截断 ≤-4 */
        int getCharsFromSockfd(int sockfd,char* buf,int size,bool cutter = true);

        //从sockfd中读取并解析一行,结果写入result
        /*返回值如下:
        读取成功但未识别到"\r\n" 2
        成功读取单独的"\r\n" 1
        数据读取失败(可能是链接断开) 0
        缓冲区过小而失败 -1
        解析出现行格式错误问题而中断读取 -2 */
        int parseLine(int sockfd,HeaderMSG& result);

    };
};







#endif