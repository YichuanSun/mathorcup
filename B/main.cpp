#include <bits/stdc++.h>
#define MAX_N 10005         //С�����������
#define MAX_H 10005         //h��hollow����д�������ڱ�ʾ��������
using namespace std;

class car{      //����
    public:
        int num;    //���
        int str,en=0;     //��㣬�յ㡣��A��Ϊ������B��Ϊ����
        double blockTime=0;    //����ʱ��
        double emptyTime=0;    //����ʱ��
        double fullTime=0;     //����ʱ��
        double pos=0;          //��ǰλ��
        int enough=0;          //��ǰ���0��ʾ���أ�1��ʾ����
        int ste=0;             //��ǰ״̬��0��ʾͨ����1��ʾ����
        int loadtime=0;        //װ��������Ҳ����ж������
};

class inh{      //������in-hollow
    public:
        double pos;    //��ǰλ��
        int num;
        int sz;     //���������ڵ�������Ҳ����ʣ�������
        int busy=0;     //�Ƿ�����ռ�ã�0��ʾ���У�1��ʾæµ
        double tm=0;     //�����ڵ�ǰʣ��ʱ�䣬���Ϊ0����Ϊ��ǰ���У����Ϊ0~10����Ϊ����ж��
};

class outh{     //������out-hollow
    public:
        int num;
        int busy=0;     //�Ƿ�����ռ�ã�0��ʾ���У�1��ʾæµ
        double pos;
        double tm=0;
};

struct comp1{
    bool operator ()(car &c1,car &c2)   {
        return c1.pos<c2.pos;
    }
};

const double l1=94; //ֱ�߹������
const double l2=6;  //����������
const double v=1.5; //������ʻ�ٶ�
const double ls=0;  //���󳵳��ȣ��˴����ǳ��ȣ���Ϊ0
const double threshhold=0.1;    //�ܹ�ȷ��������������ȵ���ֵ
const double dt=0.01;
int N,ma,mb,na,nb;  //��N�����󳵣�A�������ma��B�������mb��A�������na��B�������nb
double stepa,stepb; //A��B�Ͻ������ڵļ������

queue<int> qah[MAX_N];  //��¼A���i�������ڵķ�������ÿ����һ������ǰ���оͳ���һ��
int vbh[MAX_N];         //��ʾB���i�������ڵĵ�ǰ������ÿ����һ������ǰ������-1
car cs[MAX_N];          //С������
inh ihsa[MAX_H],ihsb[MAX_H];            //�����ڼ��ϣ���AB
outh ohsa[MAX_H],ohsb[MAX_H];           //�����ڼ��ϣ���AB
bool block_car[MAX_N];                   //�������ĳ��ļ��ϣ�������װж���ĳ�
priority_queue<car,vector<car>,comp1> block_position_first;
priority_queue<car,vector<car>,comp1> run_position_first;
priority_queue<car,vector<car>,comp1> total_car_position_first;
double anst=0;

bool around_equal(double a,double b);
void init_car();        //��ʼ��С��λ��
void init_h();          //��ʼ����������λ��
void programm();        //�����㷨
bool is_end();          //���ȫ��������ϣ�����1������Դ���δ������ϣ�����0
void car_block_or_not();//���ಢ�ж�С���Ƿ�block,�����Ǽ��뵽���ȶ�����
void car_operate(car& t);
bool is_blocked_by_car(car& t);   //�ж��Ƿ���������ס
void march(car& t);               //ʹС��ǰ��
double moddouble(double a,int b);   //��double��ȡ�࣬��ֻ����������ȡ��
int _find_farthest(int nu);              //����A�൱ǰ���е���Զ������
//void car_block_operate(car tcar); //�Ա��������Ĵ���

int main()  {
    cout<<"��ֱ����룺С��������A������ڡ�B������ڡ�A������ڡ�B�������"<<endl;
    cin>>N>>ma>>mb>>na>>nb;
    stepa=1.0*47/(ma+na);      //����A��B�Ͻ������ڵļ������
    stepb=1.0*47/(mb+nb);
    int atob,btoa,t;      //atob�Ǵ�A�����������B������ڵ���������
    cout<<"��ֱ�����A->B��B->A����������"<<endl;
    cin>>atob>>btoa;
    freopen("1.txt","ra",stdin);
    for (int i=0;i<atob;i++)   {
        for (int j=1;j<=ma;j++) {
            cin>>t;
            qah[j].push(t);     //��ʾ��ǰA���j�����ڣ�Ҫ����B��t������
        }
    }
    //cout<<btoa<<endl;
    freopen("2.txt","ra",stdin);
    for (int i=0;i<btoa;i++)    {
        for (int j=1;j<=mb;j++)  {
            cin>>t;
            vbh[j]+=t;          //��ʾ��ǰB���j�����ڣ���������t(0��1)
        }
    }
    init_car();                 //��ʼ��С��λ��
    init_h();                   //��ʼ����������λ��
    programm();                 //�����㷨
    for (int i=1;i<=N;i++)            //�ò��ֲ������е�С����a��ڵĳ�ʼλ��
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

void init_car() {               //��ʼ�����г�����Ϣ
    int cnta=0,cntb=0;
    if (N<=ma+mb)    {                  //���С����С�ڵ����ܽ�����������ÿ���������Է���һ����������
        for (int i=1;i<=N;i++)  {       //����ÿ��С���ĳ�ʼλ��
            cs[i].num=i;
            if (i&1)    {               //���iΪ�������ͷ���A��
                int tt=ma*stepa+cnta*stepa;
                cs[i].pos=moddouble(tt,100);
                cnta++;
            }
            else    {                   //iΪż��,�ͷ���B��
                cs[i].pos=moddouble(50.0+1.0*cntb*2*stepb,100);
                cntb++;
            }
        }
    }
    else {      //���С���������ܽ�����������ֻ�ܰ���������㷨��ֻ��Ҫ˳���������С��
        int clca=0,clcb=0;              //ѭ��Ȧ����
        double tp=0;
        for (int i=1;i<=N;i++)  {
            if (i&1)    {               //���iΪ�������ͷ���A��
                tp=1.0*na*stepa+1.0*cnta*stepa+1.0*clca*ls;      //�ݶ��ĵ�ǰλ��
                cs[i].pos=moddouble(tp,100);
                cnta++;
                clca=cnta/ma;
            }
            else    {                   //iΪż��,�ͷ���B��
                if (1.0*clcb*ls<stepb)   {      //���������������֮��С����ÿ������ڳ����˼�����ȣ��򡣡���
                                                //��û�����������������ʱ���������ܰ�С���ŵ������ȥ����Ϊ���̫����
                                                //���Դ˴��Ҽ�����С���ڼ���ڵ��ܳ������ᳬ���������
                    tp=50.0+1.0*cntb*2*stepb+1.0*clcb*ls;
                    cs[i].pos=moddouble(tp,100);
                }
                else    {               //�����Ҽ�����С���ڼ���ڵ��ܳ������ᳬ��������ȣ����Դ˴�����֧��ͬ
                    tp=50.0+1.0*cntb*2*stepb+1.0*clcb*ls;
                    cs[i].pos=moddouble(tp,100);
                }
                cntb++;
                clcb=cntb/mb;
            }
        }
    }
}

void init_h()   {       //��ʼ�����н������ڵ�λ��
    //���ȶ�A
    for (int i=1;i<=na;i++)  {
        ohsa[i].pos=0+(i-1)*stepa;
    }
    for (int i=1;i<=ma;i++) {
        ihsa[i].pos=na*stepa+(i-1)*stepa;
    }
    //Ȼ���B
    for (int i=1;i<=nb;i++)  {
        ohsb[i].pos=50+2.0*(i-1)*stepb;
    }
    for (int i=1;i<=mb;i++) {
        ihsb[i].pos=50+2.0*i*stepb;
    }
}

//�ò��ֳ�������
bool is_end()   {           //�ж��Ƿ���ﶼж����
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
    for (double t=0;(int)t<100000;t+=dt) {      //ȡʱ������dt=0.01�����α���
        if (is_end())   {
            cout<<"OKK"<<endl;
            anst=t;
            break;
        }
        car_block_or_not();             //�ж����г��Ƿ���������Ǳ�������
        //����ע���е�˼·��ʱ����
//        while (!block_position_first.empty())   {   //�ȴ��������ĳ�����������ǰ��ĵ�һ������ʼ
//            car tcar=block_position_first.top();
//            block_position_first.pop();
//            car_block_operate(tcar);
//        }
        //ֱ�Ӵ������г�
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

//void car_block_operate(car tcar)  { //���������֣�һ������װж������һ���Ǳ�ǰ��ĳ���ס
//    for (int i=1;i<=ma;i++)  {   //������A������ڱȽ�
//        if (around_equal(tcar.pos,ihsa[i].pos)) {
//            if (tcar.enough==0)  {  //���������ز��ҵ���װ���ڣ���ʼװ��
//                tcar.blockTime+
//            }
//        }
//        else
//    }
//}

void car_operate(car &t)  {
    //ÿ������������������������������ڶ��������������������ڶ�������ǰ��ĳ�������ǰ��
    if (t.enough==0)    {       //t����
        if (t.pos>=0&&t.pos<=47)    {       //����������A�������
            for (int i=1;i<=ma;i++) {           //��A������н����ڱȽ�λ��
                if (around_equal(t.pos,ihsa[i].pos))    {
                    ihsa[i].busy=1;
                    ihsa[i].sz--;
                    ihsa[i].tm=moddouble(ihsa[i].tm-dt,10);
                    t.str=i;        //str�и����⣬û���ж���A�������B������ҽ������ÿ��ǵ�
                    t.en=-qah[i].front();qah[i].pop();      //������B�࣬����Ϊ����
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
        else if (t.pos>=50&&t.pos<=97)  {       //����������B�������
            for (int i=1;i<=mb;i++) {
                if (around_equal(t.pos,ihsb[i].pos))   {
                    ihsb[i].sz--;
                    ihsb[i].tm=moddouble(ihsb[i].tm-dt,10);
                    t.str=-i;
                    t.en;       //���ڵö�̬�ж�
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
        else  {      //������û����������
            //��ʱ���������ж�λ�ã��Ƿ��������������ǰ����������ͣ�ڴ˴���
            if (is_blocked_by_car(t))   {return;}
            //����������ǰ��
            else march(t);
            return;
        }
    }
    else {      //t����
        if (t.pos>=50.0&&t.pos<=99.9)   {           //�����A�������֮ǰ�����赽��ǰ���е���Զ���Ǹ�
            int temp=_find_farthest(t.num);
            if (temp==-1)   {       //˵����ǰû�п�ж��λ��,����ǰ��
            }
            else {                  //��ǰ�п�ж��λ�ã����յ���A�࣬Ϊ����
                if (t.en>0) t.en=max(t.en,temp);
            }
        }
        if (t.str>0&&around_equal(ohsb[t.en].pos,t.pos))  {       //�������B������ڣ���������Ƿ����Լ�Ҫȥ���Ǹ�
            //��������Լ�Ҫȥ���Ǹ�����ʼж��
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
        if (is_blocked_by_car(t))   {return;}             //��ʱ���������ж�λ�ã��Ƿ��������������ǰ����������ͣ�ڴ˴���
        else march(t);                                    //����������ǰ��
        return;
    }
}

bool around_equal(double a,double b)    {
    double delta=fabs(a-b);
    if (delta<threshhold)   return 1;
    else if (fabs(delta-100)<threshhold)    return 1;       //������0��ʱ����ȡ�ർ�µ��������
    else return 0;
}

bool is_blocked_by_car(car& t)    {      //�ж��Ƿ񱻶���
    for (int i=1;i<=N;i++)  {
        if (i==t.num)   continue;
        if (around_equal(cs[i].pos-t.pos,ls)&&cs[i].ste==1) {
                                //���������ࣨ����Ⱥ�Լ����һ����������ǰ��block�ˣ���t��Ҳblock��
            t.blockTime+=dt;
            if (t.enough==0)   t.emptyTime+=dt;
            else    t.fullTime+=dt;
            t.ste=1;
            return true;
        }
    }
    return false;
}

void march(car& t)   {                   //ǰ��
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
            if (cs[j].str<0&&cs[j].en==tvi[i])   break;      //�������С��Ҳ����B��͵�ǰС����Ŀ�ĵ���ͬ�����軻��Ŀ�ĵ�
            return tvi[i];
        }
    }
    return -1;
}




