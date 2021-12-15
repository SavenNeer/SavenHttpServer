

#include"include/saven_headerbuilder.h"

std::string SavenHeaderBuilder::buildFirstLine(const std::string method,const std::string index,const std::string httpVer){
    std::string line = SavenUtils::trim(method) + " " + SavenUtils::trim(index) + " " + SavenUtils::trim(httpVer);
    printf("[HeadBuild]:%s\n",line.c_str());
    return (line + "\r\n");
}



//用于生成一行首部字段
std::string SavenHeaderBuilder::buildHeaderLine(const std::string key,const std::string value){
    std::string line = SavenHeaderParse::format_keys(SavenUtils::trim(key)) + ": " + value;
    printf("[HeadBuild]:%s\n",line.c_str());
    return (line + "\r\n");
}


//用于生成一至多行首部字段
std::string SavenHeaderBuilder::buildHeaderLine(std::map<std::string,std::string>& mapper){
    std::string lines = "";
    std::map<std::string,std::string>::iterator iter = mapper.begin();
    while(iter != mapper.end()){
        lines += (buildHeaderLine(iter->first,iter->second));
        iter++;
    }
    return lines;
}


//用于生成一行首部字段
std::string SavenHeaderBuilder::buildHeaderLine(std::pair<std::string,std::string>& pairs){
    return buildHeaderLine(pairs.first,pairs.second);
}


