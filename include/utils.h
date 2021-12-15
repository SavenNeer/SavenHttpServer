//utils.h

#ifndef _SERVER_UTILS_H_
#define _SERVER_UTILS_H_

#include<string>

namespace SavenUtils{

    class String_Operator{
    public:
        static std::string Split_first(const std::string split_string,const std::string mid_string);
        static std::string Split_second(const std::string split_string,const std::string mid_string);
    };

    void toupper(char& ch);
    void tolower(char& ch);

    //清除前后空格
    std::string trim(const std::string string_);
    
};




#endif