//saven_headerbuilder.h

#ifndef _SAVEN_HEADERBUILDER_H_
#define _SAVEN_HEADERBUILDER_H_

//用于生成Http首部

#include"include/utils.h"
#include"include/saven_headerparse.h"
#include<string>
#include<utility>
#include<map>

namespace SavenHeaderBuilder{
    /* 以下函数生成的行的行末都会存在一个"\r\n" */

    //用于组合生成请求报文首部第一行的函数
    std::string buildFirstLine(const std::string method,const std::string index,const std::string httpVer);

    //用于生成一行首部字段
    std::string buildHeaderLine(const std::string key,const std::string value);

    //用于生成一至多行首部字段
    std::string buildHeaderLine(std::map<std::string,std::string>& mapper);

    //用于生成一行首部字段
    std::string buildHeaderLine(std::pair<std::string,std::string>& pairs);

};



#endif


