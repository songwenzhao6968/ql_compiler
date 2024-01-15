#include<cstdio>
#include<cstring>
#include<iostream>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<cmath>
#include<queue>
#include<stack>
using namespace std;

//符号表Tab及其操作
struct Tab;
struct Item
{
    string name,type;
    int offset; Tab* mytab;
    string etype; int dims,dim[6],base; //至多6维数组
};
struct Tab
{
    Tab* outer;
    int width;
    string code;
    string rtype;
    int args; vector<string> arglist;
    vector<Item> list;
    Tab(){outer=NULL,width=0,code="",args=0;}
};
stack<Tab*> symtab;
vector<Tab*> tablist;
void bind(Tab &tb,string name,string type)
{
    Item tp;
    tp.name=name,tp.type=type;
    tb.list.push_back(tp);
}
void lookup_c(Tab &tb,string name,string seg_name,string str="",int it=0,Tab* add=NULL) //修改
{
    for(int i=0;i<(int)tb.list.size()-1;i++)
        if(tb.list[i].name==name)
        {
            if(seg_name=="name")tb.list[i].name=str;
            else if(seg_name=="type")tb.list[i].type=str;
            else if(seg_name=="offset")tb.list[i].offset=it;
            else if(seg_name=="mytab")tb.list[i].mytab=add;
            else if(seg_name=="etype")tb.list[i].etype=str;
            else if(seg_name=="dims")tb.list[i].dims=it;
            else if(seg_name=="dim[0]")tb.list[i].dim[0]=it;
            else if(seg_name=="dim[1]")tb.list[i].dim[1]=it;
            else if(seg_name=="dim[2]")tb.list[i].dim[2]=it;
            else if(seg_name=="dim[3]")tb.list[i].dim[3]=it;
            else if(seg_name=="dim[4]")tb.list[i].dim[4]=it;
            else if(seg_name=="dim[5]")tb.list[i].dim[5]=it;
            else if(seg_name=="base")tb.list[i].base=it;
        }
}
string lookup_t(string name) //在symtab的头两个符号表里查type
{
    Tab* tp=symtab.top();
    for(auto J:tp->list)
        if(J.name==name)return J.type;
    if(symtab.size()>1)
    {
        symtab.pop();
        for(auto J:symtab.top()->list)
            if(J.name==name)
            {
                symtab.push(tp);
                return J.type;
            }
        symtab.push(tp);
    }
    return "UNBOUND";
}
int lookup_d(string name,int i=-1) //在symtab的头两个符号表里查dims,dim[i]
{
    Tab* tp=symtab.top();
    for(auto J:tp->list)
        if(J.name==name)
        {
            if(i==-1)return J.dims;
            else return J.dim[i];
        }
    if(symtab.size()>1)
    {
        symtab.pop();
        for(auto J:symtab.top()->list)
            if(J.name==name)
            {
                symtab.push(tp);
                if(i==-1)return J.dims;
                else return J.dim[i];
            }
        symtab.push(tp);
    }
    return -1;
}
int lookup_b(string name) //在symtab的头两个符号表里查base
{
    Tab* tp=symtab.top();
    for(auto J:tp->list)
        if(J.name==name)return J.base;
    if(symtab.size()>1)
    {
        symtab.pop();
        for(auto J:symtab.top()->list)
            if(J.name==name)
            {
                symtab.push(tp);
                return J.base;
            }
        symtab.push(tp);
    }
    return -1;
}
void merge_tab(Tab &A,Tab &B)
{
    for(auto J:B.list)
    {
        A.list.push_back(J);
        if(J.type=="ARRAY")A.list[(int)A.list.size()-1].base+=A.width;
        else A.list[(int)A.list.size()-1].offset+=A.width;
    }
    A.width+=B.width;
}

//辅助函数
void error(string msg)
{
    cerr<<"错误："+msg;
    while(!symtab.empty())
        delete symtab.top(),symtab.pop();
    while(!tablist.empty())
        delete tablist.back(),tablist.pop_back();
    exit(0);
}
string to_str(int x)
{
    char a[20];
    int w=0,flg=0;
    if(x<0)flg=1,x=-x;
    while(x)
    {
        a[w++]=x%10+'0';
        x=x/10;
    }
    if(!w)a[w++]='0';
    if(flg)a[w++]='-';
    reverse(a,a+w);
    a[w++]=0;
    return string(a);
}
string gen(string a1,string a2="",string a3="",string a4="",string a5="",string a6="",string a7="",string a8="")
{
    string c=a1;
    if(a2!="")c=c+" "+a2;
    if(a3!="")c=c+" "+a3;
    if(a4!="")c=c+" "+a4;
    if(a5!="")c=c+" "+a5;
    if(a6!="")c=c+" "+a6;
    if(a7!="")c=c+" "+a7;
    if(a8!="")c=c+" "+a8;
    return c+"\n";
}
int tot_var;
string newvar()
{
    return "t"+to_str(tot_var++);
}
int tot_label;
string newlabel()
{
    return "t"+to_str(tot_label++);
}



//树节点及属性定义
//TODO：补全树节点定义
struct Ǐ
{
    vector<int> place;
};
struct E
{
    string place,code;
};
struct Ě
{
    vector<string> place,code;
};
struct B
{
    string tc,fc,code;
};
struct S
{
    string code;
};
struct Š
{
    vector<string> code;
};
struct T
{
    string type;
    int width;
};
struct D
{
    Tab tab;
    string place;
    bool func_or_arr;
};
struct Ď
{
    vector<string> place;
    bool func_or_arr;
};
struct d
{
    string name;
};
struct i
{
    int value;
};
struct r
{
    string name;
};

namespace ATTR //属性计算
{
    //P后的数字为产生式编号
    void P1(Ď &Ď,Š &Š)
    {
        Tab* tab=symtab.top();
        symtab.pop();
        tab->outer=NULL;
        tab->args=Ď.place.size();
        tab->arglist=Ď.place; 
        for(auto J:Š.code)
            tab->code=tab->code+J;
        tab->rtype="END";
        tablist.push_back(tab);
    }
    void P2(Ď &Ď)
    {
        symtab.push(new Tab());
        Ď.func_or_arr=false;
    }
    void P3(Ď &Ď0,Ď &Ď1,D &D)
    {
        merge_tab(*symtab.top(),D.tab);
        Ď0.place=Ď1.place;
        Ď0.place.push_back(D.place);
        if(D.func_or_arr||Ď1.func_or_arr)
            Ď0.func_or_arr=true;
        else Ď0.func_or_arr=false;
    }
    void P4(Š &Š,S &S)
    {
        Š.code.push_back(S.code);
    }
    void P5(Š &Š0,Š &Š1,S &S)
    {
        Š0.code=Š1.code;
        Š0.code.push_back(S.code);
    }
    void P6(D &D,T &T,d &d)
    {
        D.tab=Tab();
        bind(D.tab,d.name,T.type);
        lookup_c(D.tab,d.name,"offset",0);
        D.tab.width+=T.width;
        D.place=d.name;
        D.func_or_arr=false;
    }
    void P7(D &D,T &T,d &d,Ǐ &Ǐ)
    {
        if(Ǐ.place.size()>6)error("数组维数不能超过6\n");
        if(Ǐ.place.size()<1)error("数组维数不能小于1\n");
        D.tab=Tab();
        bind(D.tab,d.name,"ARRAY"); 
        lookup_c(D.tab,d.name,"dims","",Ǐ.place.size());
        int size=1;
        char ch[2]={'0',0};
        for(auto J:Ǐ.place)
        {
            lookup_c(D.tab,d.name,"dim["+string(ch)+"]","",J);
            size*=J;
            ++ch[0];
        }
        lookup_c(D.tab,d.name,"etype",T.type);
        lookup_c(D.tab,d.name,"base",0); 
        D.tab.width+=size*T.width; 
        D.place=d.name;
        D.func_or_arr=true;
    }
    void P8(D &D,T &T,d &d,Ď &Ď0,Ď &Ď1,Š &Š)
    {
        if(Ď0.func_or_arr)error("过程和数组不能作为其他过程的参数\n");
        Tab* tab_inner=symtab.top();
        symtab.pop();
        Tab* tab=symtab.top();
        symtab.pop();
        merge_tab(*tab,*tab_inner);
        delete tab_inner;
        tab->outer=symtab.top();
        tab->args=Ď0.place.size();
        tab->arglist=Ď0.place; 
        for(auto J:Š.code)
            tab->code=tab->code+J;
        tab->rtype=T.type;
        tablist.push_back(tab);

        D.tab=Tab();
        bind(D.tab,d.name,"FUNC");
        lookup_c(D.tab,d.name,"offset",0);
        lookup_c(D.tab,d.name,"mytab","",0,tab); 
        D.tab.width+=8; 
        D.place=d.name;
        D.func_or_arr=true;
    }
    void P9(T &T)
    {
        T.type="INT";
        T.width=2;
    }
    void P10(T &T)
    {
        T.type="FLO";
        T.width=2;
    }
    void P11(T &T)
    {
        T.type="VOID";
        T.width=0;
    }
    void P12(Ǐ &Ǐ,i &i)
    {
        Ǐ.place.push_back(i.value);
    }
    void P13(Ǐ &Ǐ0,Ǐ &Ǐ1,i &i)
    {
        Ǐ0.place=Ǐ1.place;
        Ǐ0.place.push_back(i.value);
    }
    void P14(S &S,d &d,E &E)
    {
        string tp=lookup_t(d.name);
        if(tp!="INT"&&tp!="FLO")error(d.name+"未声明或声明类型与调用方式不匹配\n");
        S.code=E.code+gen(d.name,"=",E.place);
    }
    void P15(S &S0,B &B,S &S1)
    {
        S0.code=B.code+gen("label",B.tc)+S1.code+gen("label",B.fc);
    }
    void P16(S &S0,B &B,S &S1,S &S2)
    {
        string l=newlabel();
        S0.code=B.code+gen("label",B.tc)+S1.code+gen("goto",l)+gen("label",B.fc)+S2.code+gen("label",l);
    }
    void P17(S &S0,B &B,S &S1)
    {
        string l=newlabel(); 
        S0.code=gen("label",l)+B.code+gen("label",B.tc)+S1.code+gen("goto",l)+gen("label",B.fc);
    }
    void P18(S &S,E &E)
    {
        S.code=E.code+gen("return",E.place);
    }
    void P19(S &S,Š &Š)
    {
        for(auto J:Š.code)S.code=S.code+J;
    }
    void P20(S &S,d &d,Ě &Ě)
    {
        string type=lookup_t(d.name);
        if(type!="FUNC")error(d.name+"未声明或声明类型与调用方式不匹配\n");
        string cd="";
        for(auto J:Ě.code)cd=cd+J;
        for(auto J:Ě.place)S.code=gen("par",J)+S.code;
        S.code=S.code+gen("call",d.name,",",to_str(Ě.place.size()));
        S.code=cd+S.code;
    }
    void P21(Ě &Ě,E &E)
    {
        Ě.place.push_back(E.place);
        Ě.code.push_back(E.code);
    }
    void P22(Ě &Ě0,Ě &Ě1,E &E)
    {
        Ě0.place=Ě1.place;
        Ě0.place.push_back(E.place);
        Ě0.code=Ě1.code;
        Ě0.code.push_back(E.code);
    }
    void P23(E &E,i &i)
    {
        string t=newvar();
        E.place=t;
        E.code=gen(t,"=",to_str(i.value));
    }
    void P24(E &E,d &d)
    {
        string type=lookup_t(d.name);
        if(type!="INT"&&type!="FLO")error(d.name+"未声明或声明类型与调用方式不匹配\n");
        E.place=d.name;
        E.code="";
    }
    void P25(E &E,d &d,Ě &Ě)
    {
        string type=lookup_t(d.name);
        if(type!="ARRAY")error(d.name+"未声明或声明类型与调用方式不匹配\n");
        int dims=lookup_d(d.name,-1);
        if(dims!=Ě.place.size())error(d.name+"数组下标个数错误\n");

        for(auto J:Ě.code)E.code=E.code+J;
        string t1=newvar(); 
        E.code=E.code+gen(t1,"=",Ě.place[0]); 
        for(int j=1;j<dims;j++)
        {
          string t=newvar();
          int dim=lookup_d(d.name,j);
          E.code=E.code+gen(t,"=",t1,"*",to_str(dim));
          t1=newvar(); 
          E.code=E.code+gen(t1,"=",t,"+",Ě.place[j]); 
        }
        string t=newvar();
        E.place=t;
        int base=lookup_b(d.name);
        E.code=E.code+gen(t,"=",t1,"[",to_str(base),"]");
    }
    void P26(E &E,d &d,Ě &Ě)
    {
        string type=lookup_t(d.name);
        if(type!="FUNC")error(d.name+"未声明或声明类型与调用方式不匹配\n");
        string cd="";
        for(auto J:Ě.code)cd=cd+J;
        for(auto J:Ě.place)E.code=gen("par",J)+E.code;
        E.code=E.code+gen("call",d.name,",",to_str(Ě.place.size()));
        E.code=cd+E.code;
        E.place="REG0";
    }
    void P27(E &E0,E &E1,E &E2)
    {
        E0.place=newvar();
        E0.code=E1.code+E2.code+gen(E0.place,"=",E1.place,"+",E2.place);
    }
    void P28(E &E0,E &E1,E &E2)
    {
        E0.place=newvar();
        E0.code=E1.code+E2.code+gen(E0.place,"=",E1.place,"*",E2.place);
    }
    void P29(B &B,E &E0,r r,E &E1)
    {
        string l1=newlabel(),l2=newlabel();
        B.tc=l1;
        B.fc=l2;
        B.code=E0.code+E1.code+gen("if",E0.place,r.name,E1.place,"then",l1,"else",l2);
    }
    void P30(B &B,E &E)
    {
        string l1=newlabel(),l2=newlabel();
        B.tc=l1;
        B.fc=l2; 
        B.code=E.code+gen("if",E.place,"!=","0","then",l1,"else",l2);
    }
}
//TODO：
//需要存一个从token和变元到其在SLR表中的下标的对应

void reduce(int idP)//归约，idP是产生式的编号，共30个
{
    //写30个if判断是哪个产生式
    //对于每个产生式，弹栈，new出新的变元的节点，并将出栈的节点地址作为该节点的儿子；将新变元节点的地址入栈，在GOTO表中找到下一个状态入栈；调用一次ATTR中与产生式编号对应的函数即可，即可完成属性计算和中间代码生成
}
void movein()//移进
{
    //读token，new出的终结符节点入栈，计算下一个状态，下一个状态入栈
}
int main()
{
    IO::readSLR();
    IO::transSLR();
    //TODO：语法分析主程序
    //语法分析完成后，符号表存在全局数组tablist中，符号表的code域即为对应的中间代码
    //程序结束时，需要将tablist中的符号表全部delete
    return 0;
}