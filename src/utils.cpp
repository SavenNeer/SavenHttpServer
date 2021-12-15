//utils.cpp

#ifndef _SERVER_UTILS_CPP_
#define _SERVER_UTILS_CPP_

#include"include/utils.h"

std::string 
SavenUtils::String_Operator::Split_first(const std::string split_string,const std::string mid_string)
{
    size_t pos = split_string.find(mid_string);
    if(pos == split_string.npos)return "";
    std::string str1 = split_string.substr(0,pos);
    return str1;
}

std::string 
SavenUtils::String_Operator::Split_second(const std::string split_string,const std::string mid_string)
{
    size_t pos = split_string.find(mid_string);
    if(pos == split_string.npos)return "";
    size_t i = pos+mid_string.size();
    std::string str2 = split_string.substr(i,split_string.size()-i);
    return str2;
}

void SavenUtils::toupper(char& ch){
    if(ch >= 'a' && ch <= 'z'){
        ch = ch - 'a' + 'A'; 
    }
}

void SavenUtils::tolower(char& ch){
    if(ch >= 'A' && ch <= 'Z'){
        ch = ch - 'A' + 'a'; 
    }
}

//清除前后空白和换行符
std::string SavenUtils::trim(const std::string string_)
{
    std::string s = string_;
    if (s.empty())return "";
    int f = s.find_first_not_of(" \t\n");
    return s.substr(f,s.find_last_not_of(" \t\n") - f + 1);
}

#endif