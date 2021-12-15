//saven_timewheel.h

#ifndef _SAVEN_TIMEWHEEL_H_
#define _SAVEN_TIMEWHEEL_H_

#include<stdio.h>

//时间轮模块:事件HTTP定时服务

namespace SavenTimeWheel{

    //定时器事件类
    class tw_timer{
    public:
        //双向链表指针
        tw_timer* next;
        tw_timer* pre;

        //回调函数以及传参指针
        void (*cb_func)(void*);
        void* data;

        //定时器属性
        bool isCycle;//周期定时器
        bool onWheel;//是否被插入时间轮
        int rawTimeout;//相对定时时长
        int round;//剩余圈数
        int myslot;//定时器所属槽

    public:
        //构造函数
        tw_timer(int round,int myslot,void(*cb_func)(void*),void*data=nullptr);
        //设置周期性定时机制
        void setCycle(bool cycle = false);
        //设置相对定时时长
        int setRawTimeout(int rawTimeout);
    };

    //时间轮
    class Time_Wheel{
    private:
        //时间轮长度
        static const int N = 60;
        //时间轮单槽的单元事件
        static const int SI = 1;//1秒
        //时间轮的槽
        tw_timer* slots[N];
        //时间轮当前指向的槽
        int cur_slot;

        /* 内置的基本双向链表操作 */
        
        //从槽链表中摘下某定时事件
        /*RETURN
        0:成功摘取
        -1:独立的定时器事件变量一定不在当前时间轮上
        -2:指定了无效的槽链表
        -3:指定的定时器并不在当前的时间轮槽链表中 */
        int pickDown(tw_timer* tw,bool needCheck=true);

        //向某槽链表中添加某定时事件
        /*RETURN
        0:添加成功
        -1:事件已经被加入到时间轮 添加重复
        -2:无效的定时器事件
        -3:指定了无效的槽链表
        -4:回调函数指针为空
        -5:插入的定时器事件可能不是独立的事件变量 */
        int addOn(tw_timer* tw,bool needCheck=true);

        //更新循环定时器事件使其进入下一个循环
        /*RETURN
        0:成功
        -1:参数指针指向的事件是不可循环的
        -2:无法从时间轮上摘取该定时器事件
        -3:无法将新设置好的定时器事件重新添加回时间轮上 */
        int GetintoNextCycle(tw_timer* tw);

    public:
        //构造函数
        Time_Wheel();
        //析构函数
        ~Time_Wheel();
        //tick函数:用于驱动时间轮转动
        void tick();
        //添加一个定时事件:默认建立一个非循环的定时事件
        tw_timer* add_timer(int timeout,void(*cb_func)(void*),void*data=nullptr,bool isCycle=false);
        //删除一个定时器事件
        //RETURN 成功0 失败-1
        int del_timer(tw_timer* deltw);
    };

    /*如何驱动该时间轮?
    1、使用setitimer()函数设置一个SI事件长度的定时器
    2、为SIGALARM信号设置自定义信号处理函数sig_handler()
    3、在sig_handler()中调用需要驱动的时间轮的tick()函数即可
    */
}



#endif


