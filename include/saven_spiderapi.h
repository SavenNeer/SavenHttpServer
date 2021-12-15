#ifndef SPIDER_H
#define SPIDER_H


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<curl/curl.h>
#include<vector>
#include<fstream>
#include<iostream>
#include<ctime>
#include<algorithm>
#include<map>
#include<string>



//下面的宏用于Params对象在format()中拼装URL
#define ADDER "&"
#define IS "="

/*下面的宏用于空教室查询函数get_classroom_info()的idleTime参数值
用于指定需要查询的空教室的时间段*/
//全天都是空的教室
#define CLASSROOM_ALLDAY "allday"
//上午的空教室
#define CLASSROOM_AM "am"
//下午的空教室
#define CLASSROOM_PM "pm"
//晚上的空教室
#define CLASSROOM_NIGHT "night"
//分别在一天四大节课的空教室
#define CLASSROOM_A "0102"
#define CLASSROOM_B "0304"
#define CLASSROOM_C "0506"
#define CLASSROOM_D "0708"



/*debug模式下每个函数的调用的关键信息都会输出到stdout中*/
//启用debug模式
#define DEBUG 1
//关闭debug模式
#define NONDEBUG 0
//判断debug模式
#define CHECK_DEBUG_MODE if(this->debug_mode == 1)



class Params{
private:
    std::map<std::string,std::string> info;

    std::string construct(std::vector<std::string>& pt){
        std::string line_info = "";
        int lenth = pt.size();
        int flag = 0;
        for(int i=0;i<lenth;i++){
            std::string ch = info[pt[i]];
            if(ch == "")continue;
            if(flag++)line_info += ADDER;
            line_info += pt[i];
            line_info += IS;
            line_info += ch;
        }
        return line_info;
    }
public:
    
    //将信息格式化为URL字符串
    std::string format(std::vector<std::string>& pt){
        return this->construct(pt);
    }

 /* 0-method:方法
    1-xh:学号账户
    2-pwd:密码
    3-currDate:当前时间
    4-xnxqid:学期id
    5-zc:
    6-time:时间
    7-idleTime:选取空教室时间*/
    std::string& operator[] (const std::string index){
        if(index == "method")return info["method"];
        if(index == "xh")return info["xh"];
        if(index == "pwd")return info["pwd"];
        if(index == "currDate")return info["currDate"];
        if(index == "xnxqid")return info["xnxqid"];
        if(index == "zc")return info["zc"];
        if(index == "time")return info["time"];
        if(index == "idleTime")return info["idleTime"];
        return info["None"];
    }

    Params(){
        info["None"] = "";
    };

};


class StuNumInfo{
public:
    int year;//年级
    int school;//校区编号
    int college_num;//学院编号
    int class_num;//年级内班级编号
    int person_num;//班内个人编号

    StuNumInfo(){};

    StuNumInfo(const std::string stu_num){
        if(stu_num.size() == 12){
            year = (stu_num[0]-'0')*1000+(stu_num[1]-'0')*100+(stu_num[2]-'0')*10+(stu_num[3]-'0');
            school = (stu_num[4]-'0')*10+(stu_num[5]-'0');
            college_num = (stu_num[6]-'0')*10+(stu_num[7]-'0');
            class_num = (stu_num[8]-'0')*10+(stu_num[9]-'0');
            person_num = (stu_num[10]-'0')*10+(stu_num[11]-'0');
        }
    };

};




class Spider{
private:
    std::string req_headurl = "http://jwgl.sdust.edu.cn/app.do?";
    std::string token = "";//token
    std::string User = "";//学号账户
    std::string Name = "";//用户姓名
    std::string Password = "";//登录密码
    std::string college = "";//学院
    StuNumInfo student_num_info;//学号中反映出来的信息

    FILE* temp_file;//本地临时存储文件
    const char* temp_file_name = "local_temp.json";//临时存储文件的名称
    Params params;//请求选项

    int debug_mode;
    CURL* curl;
    CURLcode Response;

    std::string get_time(){
        time_t now = time(0);
        tm* temp = localtime(&now);
        char nowdate[30];
        memset(nowdate,0,sizeof(nowdate));
        strftime(nowdate,29,"%Y-%m-%d",temp);
        std::string line = nowdate;
        return line;
    }

    std::string Trim(const std::string pre_str,int pre_str_length){
        if(pre_str_length == 0)return "";
        std::string s = pre_str;
        s.erase(0,s.find_first_not_of(" "));
        s.erase(s.find_last_not_of(" ") + 1);
        return s;
    }

    int EasySearch(const std::string aim_str,const std::string key,std::string& value){
        //对一维json中的属性快速获取key对应值value 且value只能是字符串
        int cur = 0;
        int str_lenth = aim_str.size();
        int key_lenth = key.size();
        value = "";
        while( cur < str_lenth && (cur = aim_str.find(key,cur)) != std::string::npos ){
            // cout<<"flag"<<endl;
            int t = cur + key_lenth;
            if(cur>=1 && t+2<str_lenth){
                if(aim_str[cur-1]=='\"'&&aim_str[t]=='\"'&&aim_str[t+1]==':'&&aim_str[t+2]=='\"'){
                    int temp = t + 3;
                    while(temp < str_lenth && aim_str[temp]!='\"') value+=aim_str[temp++];
                    return 0;
                }
            }
            cur++;
        }
        std::cout << "None for [" << key << "]" <<std::endl;
        return -1;
    }

    CURLcode get(){
        return curl_easy_perform(this->curl);
    }

    void clear_url(){
        if(this->curl) curl_easy_cleanup(this->curl);
    }
    
    /*八个学期从1到8进行编号,输出每个学期开始学年到结束学年以及相应的学期数*/
    //例如:2019级学生 1 表示 "2019-2020-1"
    std::string Convert_Term(int term_num){
        if(term_num < 1 || term_num > 8)return "";
        //获取入学年份
        int inter_year = this->student_num_info.year;
        //分析所在学年的学期序号
        int num = (term_num & 1)? 1 : 2;
        //分析所在学年
        int start_year =  inter_year + (int)((term_num - 1) / 2);
        //设置下一学年
        int end_year = start_year + 1;
        //生成学期字符串
        std::string terminfo = std::to_string(start_year)+"-"+ std::to_string(end_year)+"-"+std::to_string(num);
        return terminfo;
    }

    //获取信息的核心函数
    void get_info_to_file(const std::string method_type,std::vector<std::string>& ins){
        //设置method类型
        params["method"] = method_type;
        //合成请求URL
        std::string comurl = req_headurl + params.format(ins);
        //生成C语言版本url字符串
        char com_url[80];
        memset(com_url,0,sizeof(com_url));
        memcpy(com_url,comurl.c_str(),comurl.size());
        //注册URL
        curl_easy_setopt(this->curl,CURLOPT_URL,com_url);
        //启用本地临时存储文件
        this->temp_file = fopen(this->temp_file_name,"w");
        //设置数据自动输出到临时文件
        curl_easy_setopt(this->curl,CURLOPT_WRITEDATA,this->temp_file);
        //程序进行get请求并获取回应
        Response = this->get();
        //关闭文件
        fclose(this->temp_file);
        this->temp_file = NULL;
    }

    int login(){
        //生成特定url并设置到curl变量中
        std::vector<std::string> ins = {"method","xh","pwd"};
        this->get_info_to_file("authUser",ins);
        //分析文件内容
        int result = this->AnalyseMsg();
        if(result == -1){
            std::cout << "Login-Error" <<std::endl;
            return -1;
        }
        std::cout << "Login-Success" <<std::endl;
        std::cout << "Welcome! " << this->Name << " [" << this->college << "]" << std::endl;
        return 0;
    }

    int AnalyseMsg(){
        //获取信息
        std::ifstream inner;
        inner.open(temp_file_name,std::ios::in);
        std::string line;
        getline(inner,line);
        inner.close();
        //获取元素
        std::string flag="";
        this->EasySearch(line,"flag",flag);
        if(flag != "1")return -1;
        this->EasySearch(line,"userdwmc",this->college);
        this->EasySearch(line,"userrealname",this->Name);
        this->EasySearch(line,"token",this->token);
        CHECK_DEBUG_MODE{  
            std::cout <<"get token is [" << this->token << "]" <<std::endl;
            std::cout << "token size == " <<  this->token.size() << std::endl;
        }
        //向url请求头部设置token字段
        char temp_token[200];
        memset(temp_token,0,sizeof(temp_token));
        std::string mytoken = "token: " + this->token;
        memcpy(temp_token,mytoken.c_str(),mytoken.size());//生成C风格的首部token字段字符串
        curl_slist *plist = curl_slist_append(NULL,temp_token);
        curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, plist);//将token字段写入首部
        return 0;
    }
public:
    //从服务器获取时间
    void get_current_time(){
        CHECK_DEBUG_MODE std::cout << "[get_current_time]:self" << std::endl;
        params["currDate"] = this->get_time();
        std::vector<std::string> ins = {"method","currDate"};
        this->get_info_to_file("getCurrentTime",ins);
    }

public:

    Spider(const std::string user,const std::string pwd):User(user),Password(pwd){
        this->curl = curl_easy_init();//分配服务程序
        params["xh"] = User;
        params["pwd"] = Password;
        this->student_num_info = StuNumInfo(this->User);//解析学号
        this->login();//自动登录
    }

    ~Spider(){
        this->clear_url();
        //默认不启用debug模式
        this->debug_mode = NONDEBUG;
    }

    //设置debug模式
    int set_debug_mode(int mode){
        if(mode != NONDEBUG && mode != DEBUG)return -1;//设置失败
        this->debug_mode = mode;
        return 0;//设置成功
    }

    //获取课程成绩信息-默认获取全部学期成绩
    void get_grade_info(int term_num = 0){
        CHECK_DEBUG_MODE std::cout << "[get_grade_info]:self" << std::endl;
        std::string sy = this->Convert_Term(term_num);
        params["xnxqid"] = sy;
        std::vector<std::string> ins = {"method","xh","xnxqid"};
        this->get_info_to_file("getCjcx",ins);
    }

    //获取空教室信息-建议idleTime变量使用预定义的宏
    void get_classroom_info(const std::string idleTime = CLASSROOM_ALLDAY){
        CHECK_DEBUG_MODE std::cout << "[get_classroom_info]:self" << std::endl;
        params["time"] = this->get_time();
        params["idleTime"] = idleTime;
        std::vector<std::string> ins = {"method","time","idleTime"};
        this->get_info_to_file("getKxJscx",ins);
    }

    //获取考试信息
    void get_exam_info(){
        CHECK_DEBUG_MODE std::cout << "[get_exam_info]:self" << std::endl;
        std::vector<std::string> ins = {"method","xh"};
        this->get_info_to_file("getKscx",ins);
    }

    //获取课程信息
    void get_class_info(int week = -1){
        CHECK_DEBUG_MODE std::cout << "[get_class_info]:self" << std::endl;
        //获取并设置学期信息
        this->get_current_time();
        std::ifstream inner;
        inner.open(temp_file_name,std::ios::in);
        std::string line;
        getline(inner,line);
        inner.close();
        this->EasySearch(line,"xnxqh",this->params["xnxqid"]);//设置学期信息
        params["zc"] = std::to_string(week);//设置周次
        //请求开始
        std::vector<std::string> ins = {"method","xnxqid","zc","xh"};
        this->get_info_to_file("getKbcxAzc",ins);
    }

    //根据学号获取任意学生的信息-慎用
    int get_student_info(const std::string userAccount){
        CHECK_DEBUG_MODE std::cout << "[get_student_info]:" << userAccount << std::endl;
        this->params["xh"] = userAccount;
        std::vector<std::string> ins = {"method","xh"};
        this->get_info_to_file("getUserInfo",ins);
        this->params["xh"] = this->User;
        //检测是否查找到有用的信息
        std::ifstream inner;
        inner.open(this->temp_file_name,std::ios::in);
        std::string a; getline(inner,a);
        inner.close();
        if(a[0] == '{' && a[1] == '}')return -1;//查找失败
        return 0;//0:查找成功
    }

    //获取当前学生的信息
    void get_student_info(){
        CHECK_DEBUG_MODE std::cout << "[get_student_info]:self" << std::endl;
        std::vector<std::string> ins = {"method","xh"};
        this->get_info_to_file("getUserInfo",ins);
    }
};


#endif


// int main(){
//     Spider pt("201901060902","cymasd147");
//     pt.get_grade_info(3);
//     // pt.set_debug_mode(NONDEBUG);
//     // pt.get_student_info();
//     // pt.get_exam_info();
//     // pt.get_class_info();
//     return 0;
// }


// int main(int argc,char* argv[]){
//     if(argc < 2){
//         printf("usage: %s UserAccount Password\n",basename(argv[0]));
//         return -1;
//     }
//     const char* user = argv[1];
//     const char* passwd = argv[2];
//     Spider pt(user,passwd);
//     return 0;
// }



// int getinfo(const char* filename){
//     FILE* save_file = fopen(filename,"w");
//     if(save_file == NULL){
//         printf("file [%s] open error.\n",filename);
//         return -1;
//     }
//     CURL *curl = curl_easy_init();
//     if(curl) {
//         CURLcode res;
//         curl_easy_setopt(curl, CURLOPT_URL, url);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, save_file);
//         res = curl_easy_perform(curl);
//         curl_easy_cleanup(curl);
//     }
//     fclose(save_file);
//     return 0;
// }


//编译时请在编译命令后添加参数 -lcurl
// g++ past.cpp -o past -lcurl


