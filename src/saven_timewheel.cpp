//saven_timewheel.h

#ifndef _SAVEN_TIMEWHEEL_CPP_
#define _SAVEN_TIMEWHEEL_CPP_

#include"saven_timewheel.h"

/* 定时器事件类 */

//构造函数
SavenTimeWheel::tw_timer::tw_timer(int round,int myslot,void(*cb_func)(void*),void*data){
    this->round = round;
    this->myslot = myslot;
    this->cb_func = cb_func;
    this->data = data;
    this->next = this->next = nullptr;
    this->isCycle = true;
    this->onWheel = false;
}

//设置周期性定时机制
void SavenTimeWheel::tw_timer::setCycle(bool cycle){
    this->isCycle = cycle;
}

//设置相对定时时长
int SavenTimeWheel::tw_timer::setRawTimeout(int rawTimeout){
    this->rawTimeout = rawTimeout;
}


/* 时间轮类 */


//从槽链表中摘下某定时事件
/*RETURN
0:成功摘取
-1:独立的定时器事件变量一定不在当前时间轮上
-2:指定了无效的槽链表
-3:指定的定时器并不在当前的时间轮槽链表中 */
int SavenTimeWheel::Time_Wheel::pickDown(tw_timer* tw,bool needCheck){
    if(needCheck){
        if(!tw->onWheel){ return -1; }
        //检查tw的槽属性是否有效
        if(tw->myslot >= N || tw->myslot < 0){
            printf("[Time_Wheel]:pickDown():myslot invaild\n");
            return -2;
        }
        //检查定时器事件是否在当前的时间轮上
        tw_timer* tmp = this->slots[tw->myslot];
        bool onThisWheel = false;
        while(tmp){
            if(tmp == tw){
                onThisWheel = true;
                break;
            }
            tmp = tmp->next;
        }
        if(!onThisWheel){
            printf("[Time_Wheel]:pickDown():not On-this-Wheel\n");
            return -3;
        }
    }
    //将时间轮从当前时间轮中摘下来
    if(!tw->pre){//如果当前元素就在槽链表的首位
        tw_timer* nxt = tw->next;
        this->slots[tw->myslot] = nxt;
        if(nxt){ nxt->pre = nullptr; }
    }else{
        //如果在槽链表中间
        tw_timer* nxt = tw->next;
        tw->pre->next = nxt;
        if(nxt){ nxt->pre = tw->pre; }
    }
    //独立出定时器事件模块
    tw->next = tw->pre = nullptr;
    //设置下轮标记
    tw->onWheel = false;
    return 0;
}


//向某槽链表中添加某定时事件
/*RETURN
0:添加成功
-1:事件已经被加入到时间轮 添加重复
-2:无效的定时器事件
-3:指定了无效的槽链表
-4:回调函数指针为空
-5:插入的定时器事件可能不是独立的事件变量 */
int SavenTimeWheel::Time_Wheel::addOn(tw_timer* tw,bool needCheck){
    if(needCheck){
        //检查该定时事件是否已经加入到时间轮中
        if(tw->onWheel){
            printf("[Time_Wheel]:addOn():Error:tw has On-One-Wheel\n");
            return -1;
        }
        //检验定时器事件内部变量格式是否正常
        if(tw->rawTimeout <= 0){
            printf("[Time_Wheel]:addOn():Error:rawTimeout<=0\n");
            return -2;
        }
        if(tw->myslot >= N || tw->myslot < 0){
            printf("[Time_Wheel]:addOn():Error:myslot invaild\n");
            return -3;
        }
        if(!tw->cb_func){
            printf("[Time_Wheel]:addOn():Error:cb_func() is nullptr\n");
            return -4;
        }
        if(tw->next || tw->pre){//保证这是一个独立的定时器事件变量
            printf("[Time_Wheel]:addOn():Error:Pointer next* or pre* is not nullptr\n");
            return -5;
        }
    }
    //下面开始添加定时器事件
    if(!this->slots[tw->myslot]){
        // printf("[not head-insert]\n");
        this->slots[tw->myslot] = tw;
    }else{
        //采用头插法维护链表
        // printf("[head-insert]\n");
        tw_timer* tmp = this->slots[tw->myslot];
        tmp->pre = tw;
        tw->next = tmp;
        this->slots[tw->myslot] = tw;
    }
    //上轮标记
    tw->onWheel = true;
    return 0;
}


//更新循环定时器事件使其进入下一个循环
/*RETURN
0:成功
-1:参数指针指向的事件是不可循环的
-2:无法从时间轮上摘取该定时器事件
-3:无法将新设置好的定时器事件重新添加回时间轮上 */
int SavenTimeWheel::Time_Wheel::GetintoNextCycle(tw_timer* tw){
    //检查tw是否为可循环定时器事件
    if(!tw->isCycle){
        printf("[Time_Wheel]:GetintoNextCycle():not a Cyclable TW_Event\n");
        return -1;
    }
    //从时间轮上取下该定时器事件
    int ret = this->pickDown(tw);
    if(ret){
        printf("[Time_Wheel]:GetintoNextCycle():cannot pick tw Down\n");
        return -2;
    }
    //更新tw数据
    int timeout = tw->rawTimeout;
    tw->round = timeout / N;
    tw->myslot = ( (timeout % N) + cur_slot ) % N;
    //重新将定时器事件插入到时间轮
    ret = this->addOn(tw);
    if(ret){
        printf("[Time_Wheel]:GetintoNextCycle():cannot addOn this Cycle event\n");
        return -3;
    }
    //完成
    return 0;
}

//构造函数
SavenTimeWheel::Time_Wheel::Time_Wheel(){
    printf("[Time_Wheel]:Time_Wheel():begin...\n");
    this->cur_slot = 0;
}

//析构函数
SavenTimeWheel::Time_Wheel::~Time_Wheel(){
    printf("[Time_Wheel]:~Time_Wheel()\n");
    //消除当前位于时间轮上的所有未完成的事件
    int allnum = 0;
    for(int i=0;i<N;i++){
        tw_timer* tmp = this->slots[i];
        while(tmp){
            tw_timer* nxt = tmp->next;
            delete tmp;
            allnum++;
            tmp = nxt;
        }
    }
    printf("[Time_Wheel]:~Time_Wheel():delete %d event(s)\n",allnum);
}


//tick函数:用于驱动时间轮转动
void SavenTimeWheel::Time_Wheel::tick(){
    // printf("[Time_Wheel]:tick():Begin...\n");
    tw_timer* tmp = this->slots[this->cur_slot];
    while(tmp){
        tw_timer* nxt = tmp->next;
        if(!tmp->round){
            //需要执行该定时器事件
            // printf("[Time_Wheel]:tick():Running cb_func()...\n");
            tmp->cb_func(tmp->data);
            // printf("[Time_Wheel]:tick():cb_func() Run-OK\n");
            //检查是否需要循环定时
            if(tmp->isCycle){
                //重新将该事件从槽链表上摘下来并加入到新的槽链表中
                this->GetintoNextCycle(tmp);
            }else{
                //不需要再循环定时 删除该事件
                this->pickDown(tmp,false);
                delete tmp;
            }
        }else{
            //当前轮数不需要执行
            printf("cur round:%d\n",tmp->round);
            tmp->round--;
        }
        tmp = nxt;
    }
    cur_slot = ++cur_slot % N;//时间轮转动
    // printf("[Time_Wheel]:tick():Over...\n");
}

//添加一个定时事件:默认建立一个非循环的定时事件
SavenTimeWheel::tw_timer* SavenTimeWheel::Time_Wheel::add_timer(int timeout,void(*cb_func)(void*),void*data,bool isCycle){
    //检查变量
    if(timeout <= 0){
        printf("[Time_Wheel]:add_timer():Error:timeout <= 0\n");
        return (tw_timer*)0;
    }
    if(cb_func == nullptr){
        printf("[Time_Wheel]:add_timer():Error:cb_func is nullptr\n");
        return (tw_timer*)0;
    }
    //计算属性变量
    int rawtimeout = (timeout < SI) ? SI : timeout;//保持最低精度
    int round = timeout / N;//计算剩余轮数
    int myslot = (( timeout % N ) + cur_slot) % N;//计算所在槽
    printf("[Time_Wheel]:new timer:round=%d,myslot=%d,timeout=%d\n",round,myslot,rawtimeout);
    //生成定时器事件变量
    tw_timer* newtw = new tw_timer(round,myslot,cb_func,data);
    newtw->setRawTimeout(rawtimeout);//记录源事件参数
    newtw->setCycle(isCycle);//设置是否开启循环定时
    //定时器事件插入时间轮
    int ret = this->addOn(newtw);
    if(ret){
        printf("[Time_Wheel]:add_timer():Error:cannot addOn()\n");
        return (tw_timer*)0;
    }
    //如果定时器事件插入成功
    return newtw;
}

//删除一个定时器事件
//RETURN 成功0 失败-1
int SavenTimeWheel::Time_Wheel::del_timer(tw_timer* deltw){
    //检查变量
    if(!deltw){ return -1; }
    //将定时器事件从该时间轮上摘下来
    int ret = this->pickDown(deltw);
    if(ret){
        printf("[Time_Wheel]:del_timer():Error:pickDown()\n");
        return -1;
    }
    //销毁该定时器事件
    delete deltw;
    return 0;
}

#endif


