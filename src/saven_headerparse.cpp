//saven_headerparse.cpp

#ifndef _SAVEN_HEADERPARSE_CPP_
#define _SAVEN_HEADERPARSE_CPP_

#include"include/saven_headerparse.h"
#include"include/utils.h"

std::string SavenHeaderParse::format_keys(std::string keys){
    SavenUtils::toupper(keys[0]);
    for(int i=1;i<keys.size();i++){
        SavenUtils::tolower(keys[i]);
    }
    return keys;
}

std::string SavenHeaderParse::splitter(const std::string str){
    std::string oneflag = str;
    std::string pre = "";
    std::vector<std::string> keys;
    //拆分
    while(1){
        pre = SavenUtils::String_Operator::Split_first(oneflag,"-");
        if(pre == ""){
            keys.push_back(format_keys(oneflag));
            break;
        }else{
            keys.push_back(format_keys(pre));
            oneflag = SavenUtils::String_Operator::Split_second(oneflag,"-");
        }
    }
    //重组
    int keysLength = keys.size();
    std::string ans = "";
    for(int i=0;i<keysLength;i++){
        if(i)ans += '-';
        ans += keys[i];
    }
    return ans;
}

SavenHeaderParse::Http_Method SavenHeaderParse::check_Method(const std::string line){
    std::string pt = SavenUtils::trim(line);
    if(pt == "GET"){
        return SavenHeaderParse::Http_Method::GET;
    }else if(pt == "POST"){
        return SavenHeaderParse::Http_Method::POST;
    }else if(pt == "HEAD"){
        return SavenHeaderParse::Http_Method::PUT;
    }else if(pt == "DELETE"){
        return SavenHeaderParse::Http_Method::DELETE;
    }
    return SavenHeaderParse::Http_Method::UNMETHOD;
}

SavenHeaderParse::Http_Version SavenHeaderParse::check_Version(const std::string line){
    std::string pt = SavenUtils::trim(line);
    std::string front = SavenUtils::String_Operator::Split_first(pt,"/");
    std::string back = SavenUtils::String_Operator::Split_second(pt,"/");
    if(front == "HTTP"){
        if(back == "1.1"){
            return SavenHeaderParse::Http_Version::HTTP_11;
        }else if(back == "1.0"){
            return SavenHeaderParse::Http_Version::HTTP_10;
        }
    }
    return SavenHeaderParse::Http_Version::UNVERSION;
}



SavenHeaderParse::HttpHeader::HttpHeader(const std::string string_header_){
    this->string_header = this->format(string_header_);
}

//格式化字符串 例如"Upgrade-Insecure-Requests"
std::string SavenHeaderParse::HttpHeader::format(const std::string format_str){
    //替换所有的'_'为'-'
    std::string str = SavenUtils::trim(format_str);//这里清除了前后的空格
    for(int i=0;i<str.size();i++){
        if(str[i] == '_') str[i] = '-';
    }
    //按照'-'拆分重组
    return splitter(format_str);
}

//该函数是上述格式化函数format的开放接口版本
std::string SavenHeaderParse::HttpHeader_Format(const std::string format_str){
    //替换所有的'_'为'-'
    std::string str = SavenUtils::trim(format_str);
    for(int i=0;i<str.size();i++){
        if(str[i] == '_') str[i] = '-';
    }
    //按照'-'拆分重组
    return splitter(format_str);
}

bool SavenHeaderParse::HttpHeader::equal(const std::string str){
    std::string format_str = this->format(str);
    if(format_str == this->string_header)return true;
    return false;
}

//http首部变量的比较
bool SavenHeaderParse::HttpHeader::operator== (const SavenHeaderParse::HttpHeader& rhs)const{
    return (this->string_header == rhs.string_header);
}

//http首部变量的比较
bool SavenHeaderParse::HttpHeader::operator!= (const SavenHeaderParse::HttpHeader& rhs)const{
    return (this->string_header != rhs.string_header);
}

//获取首部规范字符串
std::string SavenHeaderParse::HttpHeader::getHeader()const{
    return this->string_header;
}


//初始化静态成员变量:Local_Inter_No_Header
SavenHeaderParse::HttpHeader SavenHeaderParse::HeaderParser::local_no_header = SavenHeaderParse::HttpHeader("LINH");

//构造函数
SavenHeaderParse::HeaderParser::HeaderParser(){
    printf("[sys-init]:Parser Headers Init\n");
    for(std::string s : SavenHeaderParse::InitHeaders){
        // this->addNewHeader(s);
        this->headers.push_back(s);
        printf("[init HTTP support] %s\n",s.c_str());
    }
}

//查找是否支持header_name指定的首部字段
bool SavenHeaderParse::HeaderParser::support(const std::string header_name){
    if( this->inter_search(header_name) == NOHEADER) return false;
    return true;
}

//添加新的解析头部
int SavenHeaderParse::HeaderParser::addNewHeader(const std::string string_header_){
    if( this->inter_search(string_header_) != NOHEADER )return 0;//插入元素重复
    HttpHeader new_header(string_header_);
    this->headers.push_back(new_header);
    printf("[addNewHeader] support %s\n",new_header.string_header.c_str());
    return 1;
}

//清空可识别首部列表
void SavenHeaderParse::HeaderParser::clearAllHeaders(){
    this->headers.clear();
    printf("[clearAllHeaders]\n");
}


SavenHeaderParse::HttpHeader& SavenHeaderParse::HeaderParser::inter_search(const std::string header_name){
    SavenHeaderParse::HttpHeader rhs(header_name);
    HeaderListIter iter = std::find(this->headers.begin(),this->headers.end(),rhs);
    if(iter == this->headers.end())return NOHEADER;
    return *iter;
}


int SavenHeaderParse::HeaderParser::deleteHeader(const SavenHeaderParse::HttpHeader rhs){
    HeaderListIter iter = std::find(this->headers.begin(),this->headers.end(),rhs);
    if(iter == this->headers.end())return 0;
    this->headers.erase(iter);
    return 1;
}


int SavenHeaderParse::HeaderParser::deleteHeader(const std::string string_header_){
    SavenHeaderParse::HttpHeader aim(string_header_);
    int ret = this->deleteHeader(aim);
    return ret;
}


std::string SavenHeaderParse::HeaderParser::Parse(const char* t,size_t len){
    std::string ch = "";
    for(int i=0;t[i]!='\0'&&i<len;i++) ch+=t[i];
    printf("Check for [%s]\n",ch.c_str());
    HttpHeader temp = this->inter_search(ch);
    if(temp == NOHEADER)return "";
    return temp.string_header;
}

//首部字段的首行解析器 成功1 失败0
int SavenHeaderParse::HeaderParser::firstlineParse(const std::string firstline,SavenHeaderParse::FirstLine& rhs){
    std::string line = SavenUtils::trim(firstline);
    if(!line.size())return 0;
    std::string myMethod = SavenUtils::String_Operator::Split_first(line," ");//请求方法
    if(!myMethod.size())return 0;
    std::string back = SavenUtils::String_Operator::Split_second(line," ");
    if(!back.size())return 0;
    std::string uri = SavenUtils::String_Operator::Split_first(back," ");//URI
    if(!uri.size())return 0;
    std::string httpVer = SavenUtils::String_Operator::Split_second(back," ");//HTTP版本
    if(!httpVer.size())return 0;
    //参数转换
    rhs.clear();
    rhs.method = SavenHeaderParse::check_Method(myMethod);
    rhs.version = SavenHeaderParse::check_Version(httpVer);
    rhs.midLine = SavenUtils::trim(uri);
    return 1;
}

#endif


