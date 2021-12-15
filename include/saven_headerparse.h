//saven_headerparse.h

#ifndef _SAVEN_HEADERPARSE_H_
#define _SAVEN_HEADERPARSE_H_


#include"include/utils.h"
#include<unordered_map>
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<typeinfo>
#include<iterator>


#define LR '\n'
#define CR '\r'
#define LRCR "\r\n"
#define LINEFLAG LRCR
#define NOHEADER SavenHeaderParse::HeaderParser::local_no_header
#define MAXPARSECHAR 1024


namespace SavenHeaderParse{

    std::string format_keys(std::string keys);
    std::string splitter(const std::string str);
    std::string HttpHeader_Format(const std::string format_str);
    //http请求方法:具体解析需要见check_Method()函数
    enum Http_Method{GET=0,POST,HEAD,PUT,DELETE,UNMETHOD};
    //http请求版本
    enum Http_Version{HTTP_10=0,HTTP_11,UNVERSION};
    enum Http_Header{Host = 0, User_Agent, Connection, 
                    Accept_Encoding,Accept_Language, Accept,
                    Cache_Control,Upgrade_Insecure_Requests};
    
    Http_Method check_Method(const std::string line);
    Http_Version check_Version(const std::string line);

    
    //这些用于初始化的首部应当为规范表示的字符串
    //因为他们不会被[规范化函数]操作 而是直接进入解析器的[允许解析首部目录headers]当中
    const std::vector<std::string> InitHeaders={
        "Host",
        "User-Agent",
        "Connection",
        "Accept-Encoding",
        "Accept-Language",
        "Accept",
        "Referer",
        "Cache-Control",
        "Pragma",
        "Upgrade-Insecure-Requests",
        "Set-Cookie",
        "Content-Length",
        "Content-Type",
        "Proxy-Connection",
        "Origin",
    };

    //首部的首行信息
    class FirstLine{
    public:
        Http_Method method;
        Http_Version version;
        std::string midLine;
        FirstLine():method(UNMETHOD),version(UNVERSION){
            midLine = "";
        }
        void clear(){
            method = UNMETHOD;
            version = UNVERSION;
            midLine = "";
        }
    };
    
    class HeaderParser;//声明
    
    class HttpHeader{
    public:
        friend class HeaderParser;
        HttpHeader(const std::string string_header_);
        bool equal(const std::string str);//验证字符串是否相同[带有字符串规范化]
        std::string getHeader()const;
        bool operator== (const HttpHeader& rhs)const;//直接在被格式化的字符串进行比较
        bool operator!= (const HttpHeader& rhs)const;//直接在被格式化的字符串进行比较
    private:
        std::string string_header;//被格式化后的字符串
        std::string format(const std::string str);//字符串格式化
    };

    typedef std::vector<HttpHeader> HeaderList;
    typedef std::vector<HttpHeader>::iterator HeaderListIter;

    /*首部解析器:用于注册或取消注册被支持的首部解析功能
    同时解析器提供了对被注册首部字段的解析功能
    应用不同配置的解析器可以便捷的实现http解析机制的转换*/
    class HeaderParser{
    public:
        static HttpHeader local_no_header;
        //构造函数:默认构建支持InitHeaders中的首部字段解析
        HeaderParser();
        //查找是否支持header_name
        bool support(const std::string header_name);
        //添加指定首部:成功返回1 元素插入重复即失败返回0
        int addNewHeader(const std::string string_header_);
        //取消指定首部:成功返回1 失败返回0
        int deleteHeader(const std::string string_header_);
        //取消指定首部:成功返回1 失败返回0
        int deleteHeader(const SavenHeaderParse::HttpHeader rhs);
        //清除所有注册的首部关键字
        void clearAllHeaders();
        //首部字段解析器:您可以通过使用0截断字符串或者传入len参数来指定待解析首部关键字的大小
        std::string Parse(const char* t,size_t len = MAXPARSECHAR);
        //首行解析器
        int firstlineParse(const std::string firstline,FirstLine& rhs);
        // //注册自定义的头部解析函数
        // int setHeaderParser(const std::string header_name,void (*local_parser)(char* p,size_t len));
    private:
        //查找并返回对应的引用
        HttpHeader& inter_search(const std::string header_name);
        //所有可以能够记录的首部
        HeaderList headers;
    };
};


#endif


