//saven_headersolver.cpp

#ifndef _SAVEN_HEADERSOLVER_CPP_
#define _SAVEN_HEADERSOLVER_CPP_

#include"saven_headersolver.h"

/* class HeaderMSG */

//默认构造函数
HeaderSolve::HeaderMSG::HeaderMSG(){
    MSG_Type = NOFLAG;
    MSG_Header = MSG_Content = "";
}

//HeaderMSG构造函数
HeaderSolve::HeaderMSG::HeaderMSG(const std::string MSG_Header_,const std::string MSG_Content_,HeaderSolve::HeaderMsgType MSG_Type_ = NOFLAG):
MSG_Header(MSG_Header_),MSG_Content(MSG_Content_),MSG_Type(MSG_Type_){
    if(MSG_Type!=FIRSTLINE&&MSG_Type!=BODY&&MSG_Type!=SECRETBODY&&MSG_Type!=NOFLAG){
        MSG_Type = NOFLAG;
    }
}



//基础操作函数
void HeaderSolve::HeaderMSG::clear(){MSG_Type = NOFLAG;MSG_Header=MSG_Content="";};
std::string HeaderSolve::HeaderMSG::getHeader(){return this->MSG_Header;}
std::string HeaderSolve::HeaderMSG::getContent(){return this->MSG_Content;}
HeaderSolve::HeaderMsgType HeaderSolve::HeaderMSG::getMsgType(){return this->MSG_Type;}
std::string HeaderSolve::HeaderMSG::setHeader(const std::string new_header){
    std::string s = this->MSG_Header;
    this->MSG_Header = new_header;
    return s;
}
std::string HeaderSolve::HeaderMSG::setContent(const std::string new_content){
    std::string s = this->MSG_Content;
    this->MSG_Content = new_content;
    return s;
}
HeaderSolve::HeaderMsgType HeaderSolve::HeaderMSG::setType(HeaderSolve::HeaderMsgType new_type){
    HeaderSolve::HeaderMsgType s = this->MSG_Type;
    this->MSG_Type = new_type;
    return s;
}



/* class HeaderSolver */

HeaderSolve::HeaderSolver::HeaderSolver(){
    this->tempParser = new SavenHeaderParse::HeaderParser;
}

//获取指向首部解析器的指针
SavenHeaderParse::HeaderParser* HeaderSolve::HeaderSolver::getPtrParser(){
    return this->tempParser;
}

//设置头部解析器 返回旧的解析器
SavenHeaderParse::HeaderParser* HeaderSolve::HeaderSolver::setHeaderParser(SavenHeaderParse::HeaderParser* new_parser){
    SavenHeaderParse::HeaderParser* oldParser = this->tempParser;
    this->tempParser = new_parser;
    return oldParser;
}

//获取指向内部头部解析器的指针
SavenHeaderParse::HeaderParser* HeaderSolve::HeaderSolver::getHeaderParser(){
    return this->tempParser;
}

//向内部解析器添加对某首部的支持
int HeaderSolve::HeaderSolver::addHeaderSupport(const std::string header_name){
    return this->tempParser->addNewHeader(header_name);
}

//向内部解析器删除对某首部的支持
int HeaderSolve::HeaderSolver::deleteHeaderSupport(const std::string header_name){
    return this->tempParser->deleteHeader(header_name);
}







/* 函数功能:从recv中获取"一行"的数据
 * 从sockfd中读取一行数据,行终止标志有三种:'\n'和'\r'和"\r\n"
 * 如果读取出现上述三种情况下的任何一种,直接提取本行并保留行终止标志,并再其后用'\0'截断
 * 如果超出了size规定区域仍未出现'\n'则强行使用'\0'截断已经读取的数据
 * size指缓冲区大小,size考虑到最后一位是'\0'并为其留有预备空间,即最多接收(size-1)个字符
 * 缓冲区大小size限制 size ≥ 5
 * 
 * 支持对"限存截断"的识别:
 * 由于size本身支持的是buf缓存区的大小,但是也有可能出现这种情况:
 * 由于缓存区大小限制,原本该存入的"\r\n"在存入'\r'后就被强行截断,
 * 单独留下的'\n'被当成了单独的一行.为了避免这种情况的发生,
 * 当cutter参数为true时,函数自动判定是否出现了限存截断.
 * 如果出现了该情况,程序会读取之后的'\n'但不会放入缓存中,
 * 其返回值会变为读入字符数量的相反数,即:(1-size).(size>=5)
 * cutter为false时不支持限存截断的识别,可能会发生限存截断
 * */
/*返回值如下:
成功 ≥0
缓冲区过小 -1
读取单独的"\r\n" -2
数据读取失败(可能是链接断开) -3
发生限存截断 ≤-4 */
int HeaderSolve::HeaderSolver::getCharsFromSockfd(int sockfd,char* buf,int size,bool cutter){
    if(size <= 5){//缓存区过小
        printf("[buf size error]:size is less than 2\n");
        return -1;
    }

    int cur = 0;//指向当前需要填充字符的缓冲区位置
    char ch = '\0';//用于接收字符
    int num = 0;//recv接收字符的个数

    //开始接收字符
    while(cur < size - 1){
        //接收一个字符
        num = ::recv(sockfd,&ch,1,0);
        //判断情况
        if(num > 0){//如果接收到了一个字符
            buf[cur++] = ch;//先把当前接收到的字符放入缓冲区
            if(cur == size - 1)break;//如果现在超出缓冲区则立即输出缓冲区
            //下面开始判断情况
            if(ch == '\n'){
                break;
            }else if(ch == '\r'){
                num = ::recv(sockfd,&ch,1,MSG_PEEK);
                if(num > 0 && ch == '\n'){
                    ::recv(sockfd,&ch,1,0);//接收字符'\n'
                    buf[cur++] = '\n';
                }
                break;
            }
        }else{
            if(num == 0 || (num == -1 && errno != EINTR)){
                return -3;//数据读取失败(可能是链接断开)
            }
            break;
        }
    }
    int return_value = cur;
    //限存截断的判定
    if(cutter && cur == size - 1 && buf[cur-1]=='\r'){
        num = ::recv(sockfd,&ch,1,MSG_PEEK);
        if(num > 0 && ch == '\n'){
            ::recv(sockfd,&ch,1,0);
            return_value = -cur;
        }
    }
    buf[cur] = '\0';
    if(cur == 2 && buf[0]=='\r' && buf[1]=='\n'){
        return -2;//表明缓存中只有"\r\n"
    }
    //返回长度值或负数长度值
    return return_value;
}


//从sockfd中读取并解析一行,结果写入result
/*返回值如下:
读取成功但未识别到"\r\n" 2
成功读取单独的"\r\n" 1
数据读取失败(可能是链接断开) 0
缓冲区过小而失败 -1
解析出现行格式错误问题而中断读取 -2 */
int HeaderSolve::HeaderSolver::parseLine(int sockfd,HeaderMSG& result){
    char buf[HeaderSolver::LineBufferSize];
    memset(buf,0,sizeof(buf));
    int ret = 0;
    std::string line = "";
    //获取一行信息
    ret = getCharsFromSockfd(sockfd,buf,HeaderSolver::LineBufferSize);
    switch(ret){
        case -3://没有可以读取的数据
            printf("[sys]:parseLine():no data can be readin\n");
            return 0;
        case -2://出现单独的"\r\n"
            result.setContent("\r\n");
            result.setHeader("");
            return 1;
        case -1://缓冲区过小
            return -1;
        default://成功或发生限存截断
            line += std::string(buf);
            //修正限存截断
            if(ret <= 4){line+='\n';};
    }
    if(this->isFirstLine){//如果是首部的首行
        result.setContent(line);
        this->isFirstLine = false;
        return 2;//读取成功但未识别到"\r\n"
    }
    //读取成功,开始切割
    std::size_t flag = line.find_first_of(":");
    if(flag == line.npos){//指出出错信息的原文
        printf("[Header Parse Error]:parseLine():\n");
        for(int i=0;i<line.size();i++){
            char ch = line[i];
            switch(ch){
            case '\r':
                printf("\\r");
                break;
            case '\n':
                printf("\\n");
                break;
            default:
                printf("%c",ch);
            }
        }
        printf("\n");
        return -2;//解析出现行格式错误问题而中断读取
    }
    //如果不是首行且不是最后一行"\r\n"
    std::string key = SavenUtils::String_Operator::Split_first(line,":");
    std::string value = SavenUtils::String_Operator::Split_second(line,":");
    //解析去留白
    key = this->tempParser->Parse(key.c_str(),key.size());
    value = SavenUtils::trim(value);
    //构建HeaderMSG
    result.setHeader(key);
    result.setContent(value);
    return 2;//读取成功但未识别到"\r\n"
}

//解析首部
/*返回值如下:
读取成功但未识别到"\r\n" 2
因识别到"\r\n"而成功结束读取 1
数据读取失败(可能是链接断开) 0
缓冲区过小而失败 -1
解析出现行格式错误问题而中断读取 -2 */
int HeaderSolve::HeaderSolver::HeaderParse(int sockfd,HeaderSolve::HeaderMSGs& results){
    this->isFirstLine = true;
    //清空结果容器
    results.clear();
    //开始分析
    HeaderSolve::HeaderMSG temp;
    int num = 0;
    int ret = 0;
    while(1){
        ret = parseLine(sockfd,temp);
        switch(ret){
        case 2://读取成功但未识别到"\r\n"
        case 1://成功读取单独的"\r\n"
            results.push_back(temp);
            if(ret==1)return 1;
            break;
        case 0://数据读取失败(可能是链接断开)
        case -1://缓冲区过小而失败
        case -2://解析出现行格式错误问题而中断读取
            return ret;
        }
        temp.clear();
        if(ret>2 || ret<-2)break;
    }
    return 2;
}

//快速解析:忽略信息的类型直接返回std::vector<pair<string(key),string(value)> >类型的结果
HeaderSolve::QuickMSG HeaderSolve::HeaderSolver::QuickParse(int sockfd){
    printf("[Call func]:QuickParse()\n");
    std::vector<std::pair<std::string,std::string> > ans;
    HeaderSolve::HeaderMSGs results;
    int ret = HeaderParse(sockfd,results);
    if(ret <= 0){
        printf("[Quick Parse Error]:QuickParse()\n");
    }else{
        for(int i=0;i<results.size();++i){
            HeaderSolve::HeaderMSG& rhs = results[i];
            ans.push_back(std::make_pair(rhs.getHeader(),rhs.getContent()));
        }
        printf("[Quick Parse]:OK\n");
    }
    return ans;
}


#endif




//下面是TinyHttp中对读取一行数据的处理方案
/**********************************************************************/
/* 函数功能:从recv中获取一行的数据
 * 从socket中读取一行数据,行终止标志有三种:'\n'和'\r'和"\r\n"
 * 用'\0'终止读取的字符串
 * 如果读取出现上述三种情况下的任何一种,直接提取本行并按照"\n\0"结尾
 * 如果超出了size规定区域仍未出现'\n'则强行使用'\0'截断已经读取的数据
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/
// int get_line(int sock, char *buf, int size)
// {
//     int i = 0;
//     char c = '\0';
//     int n;

//     while ((i < size - 1) && (c != '\n'))
//     {
//         n = recv(sock, &c, 1, 0);//先接收一个字节
//         /* DEBUG printf("%02X\n", c); */
//         if (n > 0)
//         {
//             if (c == '\r')
//             {
//                 /*通常flags设置为0，此时recv()函数读取tcp 缓冲区中的数据到buf中，
//                 并从tcp缓冲区中移除已读取的数据。如果把flags设置为MSG_PEEK，
//                 仅仅是把tcp 缓冲区中的数据读取到buf中，
//                 没有把已读取的数据从tcp 缓冲区中移除，
//                 如果再次调用recv()函数仍然可以读到刚才读到的数据。*/
//                 n = recv(sock, &c, 1, MSG_PEEK);
//                 /* DEBUG printf("%02X\n", c); */
//                 if ((n > 0) && (c == '\n'))
//                     recv(sock, &c, 1, 0);
//                 else
//                     c = '\n';
//             }
//             buf[i] = c;
//             i++;
//         }
//         else{//数据接收(recv)失败
//             c = '\n';
//         }
//     }
//     buf[i] = '\0';
//     return(i);//返回字节数
// }