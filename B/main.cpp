#include <bits/stdc++.h>
#define MAX_N 10005         //小车的最大数量
#define MAX_H 10005         //h是hollow的缩写，我用于表示进出货口
using namespace std;

class car{      //穿梭车
    public:
        int num;    //编号
        int str,en=0;     //起点，终点。设A处为正数，B处为负数
        double blockTime=0;    //堵塞时间
        double emptyTime=0;    //空载时间
        double fullTime=0;     //满载时间
        double pos=0;          //当前位置
        int enough=0;          //当前货物，0表示空载，1表示满载
        int ste=0;             //当前状态，0表示通畅，1表示阻塞
        int loadtime=0;        //装货次数，也等于卸货次数
};

class inh{      //进货口in-hollow
    public:
        double pos;    //当前位置
        int num;
        int sz;     //进货口现在的容量，也就是剩余货物量
        int busy=0;     //是否正被占用，0表示空闲，1表示忙碌
        double tm=0;     //进货口当前剩余时间，如果为0，意为当前空闲，如果为0~10，意为正在卸货
};

class outh{     //出货口out-hollow
    public:
        int num;
        int busy=0;     //是否正被占用，0表示空闲，1表示忙碌
        double pos;
        double tm=0;
};

struct comp1{
    bool operator ()(car &c1,car &c2)   {
        return c1.pos<c2.pos;
    }
};

const double l1=94; //直线轨道长度
const double l2=6;  //弯道轨道长度
const double v=1.5; //穿梭车行驶速度
const double ls=0;  //穿梭车长度，此处不记长度，设为0
const double threshhold=0.1;    //能够确定两个浮点数相等的阈值
const double dt=0.01;
int N,ma,mb,na,nb;  //共N辆穿梭车，A侧进货口ma，B侧进货口mb，A侧出货口na，B侧出货口nb
double stepa,stepb; //A和B上进出货口的间隔距离

queue<int> qah[MAX_N];  //记录A侧第i个进货口的发货请求，每出货一个，当前序列就出队一个
int vbh[MAX_N];         //表示B侧第i个进货口的当前容量，每出货一个，当前容量就-1
car cs[MAX_N];          //小车集合
inh ihsa[MAX_H],ihsb[MAX_H];            //进货口集合，分AB
outh ohsa[MAX_H],ohsb[MAX_H];           //出货口集合，分AB
bool block_car[MAX_N];                   //被堵塞的车的集合，包括在装卸货的车
priority_queue<car,vector<car>,comp1> block_position_first;
priority_queue<car,vector<car>,comp1> run_position_first;
priority_queue<car,vector<car>,comp1> total_car_position_first;
double anst=0;

bool around_equal(double a,double b);
void init_car();        //初始化小车位置
void init_h();          //初始化进出货口位置
void programm();        //调度算法
bool is_end();          //如果全部调度完毕，返回1，如果仍存在未调度完毕，返回0
void car_block_or_not();//分类并判断小车是否block,将它们加入到优先队列中
void car_operate(car& t);
bool is_blocked_by_car(car& t);   //判断是否被其他车堵住
void march(car& t);               //使小车前进
double moddouble(double a,int b);   //对double型取余，即只对整数部分取余
int _find_farthest(int nu);              //返回A侧当前空闲的最远出货口
//void car_block_operate(car tcar); //对被阻塞车的处理

int main()  {
    cout<<"请分别输入：小车总数、A侧进货口、B侧进货口、A侧出货口、B侧出货口"<<endl;
    cin>>N>>ma>>mb>>na>>nb;
    stepa=1.0*47/(ma+na);      //计算A和B上进出货口的间隔距离
    stepb=1.0*47/(mb+nb);
    int atob,btoa,t;      //atob是从A侧进货口送至B侧出货口的命令条数
    cout<<"请分别输入A->B，B->A的命令条数"<<endl;
    cin>>atob>>btoa;
    freopen("1.txt","ra",stdin);
    for (int i=0;i<atob;i++)   {
        for (int j=1;j<=ma;j++) {
            cin>>t;
            qah[j].push(t);     //表示当前A侧第j进货口，要送至B侧t出货口
        }
    }
    //cout<<btoa<<endl;
    freopen("2.txt","ra",stdin);
    for (int i=0;i<btoa;i++)    {
        for (int j=1;j<=mb;j++)  {
            cin>>t;
            vbh[j]+=t;          //表示当前B侧第j进货口，容量增加t(0或1)
        }
    }
    init_car();                 //初始化小车位置
    init_h();                   //初始化进出货口位置
    programm();                 //调度算法
    for (int i=1;i<=N;i++)            //该部分测试所有的小车和a侧口的初始位置
        cout<<cs[i].pos<<endl;
//    for (int i=1;i<=na;i++)
//        cout<<ohsa[i].pos<<endl;
    cout<<anst<<endl;
    return 0;
}

double moddouble(double a,int b)    {
    double ans=((int)a)%b+a-(int)a;
    return ans;
}

void init_car() {               //初始化所有车的信息
    int cnta=0,cntb=0;
    if (N<=ma+mb)    {                  //如果小车数小于等于总进货口数，则每辆车都可以放在一个进货口上
        for (int i=1;i<=N;i++)  {       //计算每辆小车的初始位置
            cs[i].num=i;
            if (i&1)    {               //如果i为奇数，就放在A侧
                int tt=ma*stepa+cnta*stepa;
                cs[i].pos=moddouble(tt,100);
                cnta++;
            }
            else    {                   //i为偶数,就放在B侧
                cs[i].pos=moddouble(50.0+1.0*cntb*2*stepb,100);
                cntb++;
            }
        }
    }
    else {      //如果小车数大于总进货口数，就只能按照上面的算法，只是要顺序向后排列小车
        int clca=0,clcb=0;              //循环圈数；
        double tp=0;
        for (int i=1;i<=N;i++)  {
            if (i&1)    {               //如果i为奇数，就放在A侧
                tp=1.0*na*stepa+1.0*cnta*stepa+1.0*clca*ls;      //暂定的当前位置
                cs[i].pos=moddouble(tp,100);
                cnta++;
                clca=cnta/ma;
            }
            else    {                   //i为偶数,就放在B侧
                if (1.0*clcb*ls<stepb)   {      //如果车长计算在内之后，小车在每个间隔内超过了间隔长度，则。。。
                                                //我没做处理，计算后发现这样时根本不可能把小车排到弯道上去，因为弯道太短了
                                                //所以此处我假设了小车在间隔内的总车长不会超过间隔长度
                    tp=50.0+1.0*cntb*2*stepb+1.0*clcb*ls;
                    cs[i].pos=moddouble(tp,100);
                }
                else    {               //由于我假设了小车在间隔内的总车长不会超过间隔长度，所以此处两分支相同
                    tp=50.0+1.0*cntb*2*stepb+1.0*clcb*ls;
                    cs[i].pos=moddouble(tp,100);
                }
                cntb++;
                clcb=cntb/mb;
            }
        }
    }
}

void init_h()   {       //初始化所有进出货口的位置
    //首先对A
    for (int i=1;i<=na;i++)  {
        ohsa[i].pos=0+(i-1)*stepa;
    }
    for (int i=1;i<=ma;i++) {
        ihsa[i].pos=na*stepa+(i-1)*stepa;
    }
    //然后对B
    for (int i=1;i<=nb;i++)  {
        ohsb[i].pos=50+2.0*(i-1)*stepb;
    }
    for (int i=1;i<=mb;i++) {
        ihsb[i].pos=50+2.0*i*stepb;
    }
}

//该部分出问题了
bool is_end()   {           //判断是否货物都卸完了
    int n1=0,n2=0;
    for (int i=1;i<=ma;i++)  {
        if (ihsa[i].sz!=0)  {
            n1=1;break;
        }
    }
    for (int i=1;i<=mb;i++) {
        if (ihsb[i].sz!=0)  {
            n2=1;break;
        }
    }
    if (n1==0&&n2==0)   return 1;
    else return 0;
}

void programm() {
    cout<<"OK"<<endl;
    for (double t=0;(int)t<100000;t+=dt) {      //取时间粒度dt=0.01，依次遍历
        if (is_end())   {
            cout<<"OKK"<<endl;
            anst=t;
            break;
        }
        car_block_or_not();             //判断所有车是否被阻塞，标记被阻塞车
        //下面注释中的思路暂时作废
//        while (!block_position_first.empty())   {   //先处理被阻塞的车，从排在最前面的第一辆车开始
//            car tcar=block_position_first.top();
//            block_position_first.pop();
//            car_block_operate(tcar);
//        }
        //直接处理所有车
        while (!total_car_position_first.empty())   {
            car tcar=total_car_position_first.top();
            total_car_position_first.pop();
            car_operate(tcar);
        }
    }
}

void car_block_or_not() {
    for (int i=1;i<=N;i++)  {
        if (cs[i].ste==1)   block_car[i]=1;
        else block_car[i]=0;
    }
    for (int i=1;i<=N;i++)  {
        if (block_car[i]==1)
            block_position_first.push(cs[i]);
        else
            run_position_first.push(cs[i]);
        total_car_position_first.push(cs[i]);
    }
}

//void car_block_operate(car tcar)  { //堵塞有两种，一种是在装卸货，另一种是被前面的车挡住
//    for (int i=1;i<=ma;i++)  {   //和所有A侧进货口比较
//        if (around_equal(tcar.pos,ihsa[i].pos)) {
//            if (tcar.enough==0)  {  //这辆车空载并且到了装货口，开始装货
//                tcar.blockTime+
//            }
//        }
//        else
//    }
//}

void car_operate(car &t)  {
    //每辆车有四种情况：空载且碰到进货口堵塞、满载且碰到出货口堵塞、被前面的车堵塞、前进
    if (t.enough==0)    {       //t空载
        if (t.pos>=0&&t.pos<=47)    {       //空载且碰到A侧进货口
            for (int i=1;i<=ma;i++) {           //与A侧的所有进货口比较位置
                if (around_equal(t.pos,ihsa[i].pos))    {
                    ihsa[i].busy=1;
                    ihsa[i].sz--;
                    ihsa[i].tm=moddouble(ihsa[i].tm-dt,10);
                    t.str=i;        //str有个问题，没有判断是A侧进还是B侧进，我接下来得考虑到
                    t.en=-qah[i].front();qah[i].pop();      //出口在B侧，则设为负数
                    t.ste=1;
                    t.blockTime+=dt;
                    t.loadtime++;
                    if (around_equal(ihsa[i].tm,10.0))  {
                        ihsa[i].busy=0;
                        t.ste=0;
                        t.enough=1;
                    }
                    return;
                }
            }
        }
        else if (t.pos>=50&&t.pos<=97)  {       //空载且碰到B侧进货口
            for (int i=1;i<=mb;i++) {
                if (around_equal(t.pos,ihsb[i].pos))   {
                    ihsb[i].sz--;
                    ihsb[i].tm=moddouble(ihsb[i].tm-dt,10);
                    t.str=-i;
                    t.en;       //出口得动态判断
                    t.ste=1;
                    t.blockTime+=dt;
                    t.loadtime++;
                    if (around_equal(ihsb[i].tm,10.0))  {
                        ihsb[i].busy=0;
                        t.ste=0;
                        t.enough=1;
                    }
                    return;
                }
            }
        }
        else  {      //空载且没碰到进货口
            //这时与其他车判断位置，是否堵塞，不堵塞则前进，堵塞则停在此处。
            if (is_blocked_by_car(t))   {return;}
            //不堵塞，则前进
            else march(t);
            return;
        }
    }
    else {      //t满载
        if (t.pos>=50.0&&t.pos<=99.9)   {           //如果在A侧出货口之前，则需到当前空闲的最远处那个
            int temp=_find_farthest(t.num);
            if (temp==-1)   {       //说明当前没有空卸货位置,则尝试前进
            }
            else {                  //当前有空卸货位置，且终点在A侧，为正数
                if (t.en>0) t.en=max(t.en,temp);
            }
        }
        if (t.str>0&&around_equal(ohsb[t.en].pos,t.pos))  {       //如果碰到B侧出货口，则需检验是否是自己要去的那个
            //如果就是自己要去的那个，则开始卸货
            t.blockTime+=dt;
            t.ste=1;
            ohsb[t.en].tm+=dt;
            if (around_equal(ohsb[t.en].tm,10.0))   {
                t.enough=0;
                t.ste=0;
                ohsb[t.en].busy=0;
            }
            return;
        }
        if (is_blocked_by_car(t))   {return;}             //这时与其他车判断位置，是否堵塞，不堵塞则前进，堵塞则停在此处。
        else march(t);                                    //不堵塞，则前进
        return;
    }
}

bool around_equal(double a,double b)    {
    double delta=fabs(a-b);
    if (delta<threshhold)   return 1;
    else if (fabs(delta-100)<threshhold)    return 1;       //这是在0处时由于取余导致的特殊情况
    else return 0;
}

bool is_blocked_by_car(car& t)    {      //判断是否被堵塞
    for (int i=1;i<=N;i++)  {
        if (i==t.num)   continue;
        if (around_equal(cs[i].pos-t.pos,ls)&&cs[i].ste==1) {
                                //如果两车间距（需分先后）约等于一个车长，且前车block了，则t车也block了
            t.blockTime+=dt;
            if (t.enough==0)   t.emptyTime+=dt;
            else    t.fullTime+=dt;
            t.ste=1;
            return true;
        }
    }
    return false;
}

void march(car& t)   {                   //前进
    t.pos+=dt*v;
    t.pos=moddouble(t.pos,100);
    if (t.enough==0)   t.emptyTime+=dt;
    else t.fullTime+=dt;
    t.ste=0;
}

int _find_farthest(int nu)   {
    vector<int> tvi;
    tvi.push_back(-1);
    for (int i=1;i<=na;i++) {
        if (ohsa[i].busy)   continue;
        tvi.push_back(i);
    }
    for (int i=(int)tvi.size()-1;i>=0;i--)  {
        for (int j=1;j<=N;j++)  {
            if (j==nu)  continue;
            if (cs[j].str<0&&cs[j].en==tvi[i])   break;      //如果其他小车也来自B侧和当前小车的目的地相同，则需换个目的地
            return tvi[i];
        }
    }
    return -1;
}




