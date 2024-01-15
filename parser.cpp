#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <stack>
#include <map>
using namespace std;
vector<vector<string> > slr(100);
vector<vector<pair<char, int> > > slrp(100);
vector<pair<int, string> > tokens(10000);
stack<int> state;
stack<void *> symb;
int token_lines;
namespace IO
{
    void readSLR()
    {
        // ifstream inFile("/Users/songwenzhao/Desktop/qlcompiler/SLR1.csv", ios::in);
        ifstream inFile("/Users/songwenzhao/Desktop/qlcompiler/SLR1.csv", ios::in);
        if (!inFile)
        {
            cerr << "---OPEN SLR1 FAILED---" << endl;
            exit(0);
        }
        int i = 0, width = 33;
        string line;
        string field;
        getline(inFile, line);
        getline(inFile, line); //去掉前两行
        while (getline(inFile, line))
        {
            string field;
            istringstream sin(line);
            for (int j = 0; j < width; j++)
            {
                getline(sin, field, ',');
                // cout << field << " ";
                slr[i].push_back(field);
            }
            i++;
        }
        inFile.close();
        cerr << "Read " << i << " lines" << endl;
        cerr << "---LOAD SLR1 FINISHED---" << endl;
    }
    // FINISHED：读入的SLR表元素都是字符串，把字符串转换成pair<char,int>，比如"s27"就变成('s',27)
    void transSLR()
    {
        int i = 0, j = 0;
        int width = 33, length = 98;
        // for (auto iter = slr.begin(); iter != slr.end(); iter++)
        for (i = 0; i < length; i++)
        {
            slrp[i].resize(35);
            // for (auto subiter = (*iter).begin(); subiter != (*iter).end(); subiter++)
            for (j = 0; j < width; j++)
            {
                if (slr[i][j] == "")
                {
                    slrp[i][j].first = 'f';
                    slrp[i][j].second = -1;
                    continue;
                }
                if (j < 23)
                {
                    slrp[i][j].first = slr[i][j][0];
                    slrp[i][j].second = atoi(slr[i][j].substr(1).c_str());
                }
                else
                {
                    slrp[i][j].second = atoi(slr[i][j].substr(0).c_str());
                }
                // j++;
            }
            // i++;
        }
        cerr << "---FINISHED TRANS SLR---" << endl;
    }
    void readtoken()
    {
        char File[128];
        FILE *fd;
        ifstream inFile("/Users/songwenzhao/Desktop/qlcompiler/output/t2r2.out", ios::in);
        if (!inFile)
        {
            cout << "---OPEN TOKEN DOC FAILED---" << endl;
            exit(0);
        }
        int i = 0;
        string line;
        string field;
        // getline(inFile, line); //读掉第一行
        while (getline(inFile, line))
        {
            string field;
            inFile >> tokens[i].first >> tokens[i].second >> field;
            // istringstream sin(line);
            // getline(sin, field, ' ');
            // tokens[i].first = atoi(field.c_str());
            // getline(sin, field, ' ');
            // tokens[i].second = field;
            // getline(sin, field, ' ');
            i++;
        }
        tokens[i].first = 20;
        tokens[i].second = "$";
        inFile.close();
        token_lines = i + 1;
        cerr << "Read " << i << " lines" << endl;
        cerr << "---LOAD TOKEN DOC FINISHED---" << endl;
    }
}

//符号表Tab及其操作
struct Tab;
struct Item
{
    string name, type;
    int offset;
    Tab *mytab;
    string etype;
    int dims, dim[6], base; //至多6维数组
};
struct Tab
{
    Tab *outer;
    int width;
    string code;
    string rtype;
    int args;
    vector<string> arglist;
    vector<Item> list;
    Tab() { outer = NULL, width = 0, code = "", args = 0; }
};
stack<Tab *> symtab;
vector<Tab *> tablist;
void bind(Tab &tb, string name, string type)
{
    Item tp;
    tp.name = name, tp.type = type;
    tb.list.push_back(tp);
}
void lookup_c(Tab &tb, string name, string seg_name, int it)
{
    for (int i = 0; i < (int)tb.list.size() - 1; i++)
        if (tb.list[i].name == name)
        {
            if (seg_name == "offset")
                tb.list[i].offset = it;
            else if (seg_name == "dims")
                tb.list[i].dims = it;
            else if (seg_name == "dim[0]")
                tb.list[i].dim[0] = it;
            else if (seg_name == "dim[1]")
                tb.list[i].dim[1] = it;
            else if (seg_name == "dim[2]")
                tb.list[i].dim[2] = it;
            else if (seg_name == "dim[3]")
                tb.list[i].dim[3] = it;
            else if (seg_name == "dim[4]")
                tb.list[i].dim[4] = it;
            else if (seg_name == "dim[5]")
                tb.list[i].dim[5] = it;
            else if (seg_name == "base")
                tb.list[i].base = it;
        }
}
void lookup_c(Tab &tb, string name, string seg_name, Tab *add = NULL)
{
    for (int i = 0; i < (int)tb.list.size() - 1; i++)
        if (tb.list[i].name == name)
        {
            if (seg_name == "mytab")
                tb.list[i].mytab = add;
        }
}
void lookup_c(Tab &tb, string name, string seg_name, string str) //修改
{
    for (int i = 0; i < (int)tb.list.size() - 1; i++)
        if (tb.list[i].name == name)
        {
            if (seg_name == "name")
                tb.list[i].name = str;
            else if (seg_name == "type")
                tb.list[i].type = str;
            else if (seg_name == "etype")
                tb.list[i].etype = str;
        }
}
string lookup_t(string name) //在symtab的头两个符号表里查type
{
    Tab *tp = symtab.top();
    for (auto J : tp->list)
        if (J.name == name)
            return J.type;
    if (symtab.size() > 1)
    {
        symtab.pop();
        for (auto J : symtab.top()->list)
            if (J.name == name)
            {
                symtab.push(tp);
                return J.type;
            }
        symtab.push(tp);
    }
    return "UNBOUND";
}
int lookup_d(string name, int i = -1) //在symtab的头两个符号表里查dims,dim[i]
{
    Tab *tp = symtab.top();
    for (auto J : tp->list)
        if (J.name == name)
        {
            if (i == -1)
                return J.dims;
            else
                return J.dim[i];
        }
    if (symtab.size() > 1)
    {
        symtab.pop();
        for (auto J : symtab.top()->list)
            if (J.name == name)
            {
                symtab.push(tp);
                if (i == -1)
                    return J.dims;
                else
                    return J.dim[i];
            }
        symtab.push(tp);
    }
    return -1;
}
int lookup_b(string name) //在symtab的头两个符号表里查base
{
    Tab *tp = symtab.top();
    for (auto J : tp->list)
        if (J.name == name)
            return J.base;
    if (symtab.size() > 1)
    {
        symtab.pop();
        for (auto J : symtab.top()->list)
            if (J.name == name)
            {
                symtab.push(tp);
                return J.base;
            }
        symtab.push(tp);
    }
    return -1;
}
void merge_tab(Tab &A, Tab &B)
{
    for (auto J : B.list)
    {
        A.list.push_back(J);
        if (J.type == "ARRAY")
            A.list[(int)A.list.size() - 1].base += A.width;
        else
            A.list[(int)A.list.size() - 1].offset += A.width;
    }
    A.width += B.width;
}

//辅助函数
void error(string msg)
{
    cerr << "---SEMANTIC ERROR---" << endl
         << msg;
    while (!symtab.empty())
        delete symtab.top(), symtab.pop();
    while (!tablist.empty())
        delete tablist.back(), tablist.pop_back();
    exit(0);
}
string to_str(int x)
{
    char a[20];
    int w = 0, flg = 0;
    if (x < 0)
        flg = 1, x = -x;
    while (x)
    {
        a[w++] = x % 10 + '0';
        x = x / 10;
    }
    if (!w)
        a[w++] = '0';
    if (flg)
        a[w++] = '-';
    reverse(a, a + w);
    a[w++] = 0;
    return string(a);
}
string gen(string a1, string a2 = "", string a3 = "", string a4 = "", string a5 = "", string a6 = "", string a7 = "", string a8 = "")
{
    string c = a1;
    if (a2 != "")
        c = c + " " + a2;
    if (a3 != "")
        c = c + " " + a3;
    if (a4 != "")
        c = c + " " + a4;
    if (a5 != "")
        c = c + " " + a5;
    if (a6 != "")
        c = c + " " + a6;
    if (a7 != "")
        c = c + " " + a7;
    if (a8 != "")
        c = c + " " + a8;
    return c + "\n";
}
int tot_var;
string newvar()
{
    return "t" + to_str(tot_var++);
}
int tot_label;
string newlabel()
{
    return "t" + to_str(tot_label++);
}

//树节点及属性定义
// FINISHED：补全树节点定义
struct P
{
    string code;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct Ip
{
    vector<int> place;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct E
{
    string place, code;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct Ep
{
    vector<string> place, code;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct B
{
    string tc, fc, code;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct S
{
    string code;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct Sp
{
    vector<string> code;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct T
{
    string type;
    int width;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct D
{
    Tab tab;
    string place;
    bool func_or_arr;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct Dp
{
    vector<string> place;
    bool func_or_arr;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct d
{
    string name;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct i
{
    int value;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
struct r
{
    string name;
    void *parent;
    int son_num = 0;
    vector<void *> son;
};
// struct sym
// {
//     string content;
//     long parent;
//     int son_num = 0;
//     vector<long> son;
// };

namespace ATTR //属性计算
{
    // P后的数字为产生式编号
    void P1(Dp &Dp, Sp &Sp)
    {
        Tab *tab = symtab.top();
        symtab.pop();
        tab->outer = NULL;
        tab->args = Dp.place.size();
        tab->arglist = Dp.place;
        for (auto J : Sp.code)
            tab->code = tab->code + J;
        tab->rtype = "END";
        tablist.push_back(tab);
    }
    void P2(Dp &Dp)
    {
        symtab.push(new Tab());
        Dp.func_or_arr = false;
    }
    void P3(Dp &Dp0, Dp &Dp1, D &D)
    {
        merge_tab(*symtab.top(), D.tab);
        Dp0.place = Dp1.place;
        Dp0.place.push_back(D.place);
        if (D.func_or_arr || Dp1.func_or_arr)
            Dp0.func_or_arr = true;
        else
            Dp0.func_or_arr = false;
    }
    void P4(Sp &Sp, S &S)
    {
        Sp.code.push_back(S.code);
    }
    void P5(Sp &Sp0, Sp &Sp1, S &S)
    {
        Sp0.code = Sp1.code;
        Sp0.code.push_back(S.code);
    }
    void P6(D &D, T &T, d &d)
    {
        D.tab = Tab();
        bind(D.tab, d.name, T.type);
        lookup_c(D.tab, d.name, "offset", 0);
        D.tab.width += T.width;
        D.place = d.name;
        D.func_or_arr = false;
    }
    void P7(D &D, T &T, d &d, Ip &Ip)
    {
        if (Ip.place.size() > 6)
            error("数组维数不能超过6\n");
        if (Ip.place.size() < 1)
            error("数组维数不能小于1\n");
        D.tab = Tab();
        string typ = "ARRAY";
        bind(D.tab, d.name, typ);
        lookup_c(D.tab, d.name, "dims", Ip.place.size());
        int size = 1;
        char ch[2] = {'0', 0};
        for (auto J : Ip.place)
        {
            lookup_c(D.tab, d.name, "dim[" + string(ch) + "]", J);
            size *= J;
            ++ch[0];
        }
        lookup_c(D.tab, d.name, "etype", T.type);
        lookup_c(D.tab, d.name, "base", 0);
        D.tab.width += size * T.width;
        D.place = d.name;
        D.func_or_arr = true;
    }
    void P8(D &D, T &T, d &d, Dp &Dp0, Dp &Dp1, Sp &Sp)
    {
        if (Dp0.func_or_arr)
            error("过程和数组不能作为其他过程的参数\n");
        Tab *tab_inner = symtab.top();
        symtab.pop();
        Tab *tab = symtab.top();
        symtab.pop();
        merge_tab(*tab, *tab_inner);
        delete tab_inner;
        tab->outer = symtab.top();
        tab->args = Dp0.place.size();
        tab->arglist = Dp0.place;
        for (auto J : Sp.code)
            tab->code = tab->code + J;
        tab->rtype = T.type;
        tablist.push_back(tab);

        D.tab = Tab();
        // cerr<<endl<<endl<<"!!!!!!!!!!!!!!!!!"<<endl<<endl;
        string typ = "FUNC";
        bind(D.tab, d.name, typ);
        // cerr<<"::;;;;;;;;;;"<<endl<<endl;
        lookup_c(D.tab, d.name, "offset", 0);
        lookup_c(D.tab, d.name, "mytab", tab);
        D.tab.width += 8;
        D.place = d.name;
        D.func_or_arr = true;
    }
    void P9(T &T)
    {
        T.type = "INT";
        T.width = 2;
    }
    void P10(T &T)
    {
        T.type = "FLO";
        T.width = 2;
    }
    void P11(T &T)
    {
        T.type = "VOID";
        T.width = 0;
    }
    void P12(Ip &Ip, i &i)
    {
        Ip.place.push_back(i.value);
    }
    void P13(Ip &Ip0, Ip &Ip1, i &i)
    {
        Ip0.place = Ip1.place;
        Ip0.place.push_back(i.value);
    }
    void P14(S &S, d &d, E &E)
    {
        string tp = lookup_t(d.name);
        if (tp != "INT" && tp != "FLO")
            error(d.name + "未声明或声明类型与调用方式不匹配\n");
        S.code = E.code + gen(d.name, "=", E.place);
    }
    void P15(S &S0, B &B, S &S1)
    {
        S0.code = B.code + gen("label", B.tc) + S1.code + gen("label", B.fc);
    }
    void P16(S &S0, B &B, S &S1, S &S2)
    {
        string l = newlabel();
        S0.code = B.code + gen("label", B.tc) + S1.code + gen("goto", l) + gen("label", B.fc) + S2.code + gen("label", l);
    }
    void P17(S &S0, B &B, S &S1)
    {
        string l = newlabel();
        S0.code = gen("label", l) + B.code + gen("label", B.tc) + S1.code + gen("goto", l) + gen("label", B.fc);
    }
    void P18(S &S, E &E)
    {
        S.code = E.code + gen("return", E.place);
    }
    void P19(S &S, Sp &Sp)
    {
        for (auto J : Sp.code)
            S.code = S.code + J;
    }
    void P20(S &S, d &d, Ep &Ep)
    {
        string type = lookup_t(d.name);
        if (type != "FUNC")
            error(d.name + "未声明或声明类型与调用方式不匹配?\n");
        string cd = "";
        for (auto J : Ep.code)
            cd = cd + J;
        for (auto J : Ep.place)
            S.code = gen("par", J) + S.code;
        S.code = S.code + gen("call", d.name, ",", to_str(Ep.place.size()));
        S.code = cd + S.code;
    }
    void P21(Ep &Ep, E &E)
    {
        Ep.place.push_back(E.place);
        Ep.code.push_back(E.code);
    }
    void P22(Ep &Ep0, Ep &Ep1, E &E)
    {
        Ep0.place = Ep1.place;
        Ep0.place.push_back(E.place);
        Ep0.code = Ep1.code;
        Ep0.code.push_back(E.code);
    }
    void P23(E &E, i &i)
    {
        string t = newvar();
        E.place = t;
        E.code = gen(t, "=", to_str(i.value));
    }
    void P24(E &E, d &d)
    {
        string type = lookup_t(d.name);
        if (type != "INT" && type != "FLO")
            error(d.name + "未声明或声明类型与调用方式不匹配\n");
        E.place = d.name;
        E.code = "";
    }
    void P25(E &E, d &d, Ep &Ep)
    {
        string type = lookup_t(d.name);
        if (type != "ARRAY")
            error(d.name + "未声明或声明类型与调用方式不匹配\n");
        int dims = lookup_d(d.name, -1);
        if (dims != Ep.place.size())
            error(d.name + "数组下标个数错误\n");

        for (auto J : Ep.code)
            E.code = E.code + J;
        string t1 = newvar();
        E.code = E.code + gen(t1, "=", Ep.place[0]);
        for (int j = 1; j < dims; j++)
        {
            string t = newvar();
            int dim = lookup_d(d.name, j);
            E.code = E.code + gen(t, "=", t1, "*", to_str(dim));
            t1 = newvar();
            E.code = E.code + gen(t1, "=", t, "+", Ep.place[j]);
        }
        string t = newvar();
        E.place = t;
        int base = lookup_b(d.name);
        E.code = E.code + gen(t, "=", t1, "[", to_str(base), "]");
    }
    void P26(E &E, d &d, Ep &Ep)
    {
        // cerr<<"?";
        // cerr<<symtab.top()->list[2].name;
        string type = lookup_t(d.name);
        if (type != "FUNC")
            error(d.name + "未声明或声明类型与调用方式不匹配\n");
        string cd = "";
        for (auto J : Ep.code)
            cd = cd + J;
        for (auto J : Ep.place)
            E.code = gen("par", J) + E.code;
        E.code = E.code + gen("call", d.name, ",", to_str(Ep.place.size()));
        E.code = cd + E.code;
        E.place = "REG0";
    }
    void P27(E &E0, E &E1, E &E2)
    {
        E0.place = newvar();
        E0.code = E1.code + E2.code + gen(E0.place, "=", E1.place, "+", E2.place);
    }
    void P28(E &E0, E &E1, E &E2)
    {
        E0.place = newvar();
        E0.code = E1.code + E2.code + gen(E0.place, "=", E1.place, "*", E2.place);
    }
    void P29(B &B, E &E0, r r, E &E1)
    {
        string l1 = newlabel(), l2 = newlabel();
        B.tc = l1;
        B.fc = l2;
        B.code = E0.code + E1.code + gen("if", E0.place, r.name, E1.place, "then", l1, "else", l2);
    }
    void P30(B &B, E &E)
    {
        string l1 = newlabel(), l2 = newlabel();
        B.tc = l1;
        B.fc = l2;
        B.code = E.code + gen("if", E.place, "!=", "0", "then", l1, "else", l2);
    }
}
// TODO：
//需要存一个从token和变元到其在SLR表中的下标的对应
map<int, int> trans_map = {{300, 1}, {200, 2}, {102, 3}, {103, 4}, {104, 5}, {105, 6}, {106, 7}, {107, 8}, {100, 9}, {101, 10}, {56, 11}, {50, 12}, {52, 13}, {54, 14}, {55, 14}, {58, 14}, {7, 15}, {10, 16}, {2, 17}, {0, 18}, {1, 19}, {4, 20}, {3, 21}, {20, 22}};

int sit = 0;           //当前状态，等于state栈顶
vector<long> temp_son; //预备儿子们

int transfer_token(pair<int, string> pr)
{
    return trans_map[pr.first];
}

void reduce(int idP) //归约，idP是产生式的编号，共30个
{
    //写30个if判断是哪个产生式
    /*对于每个产生式，弹栈，new出新的变元的节点，并将出栈的节点地址作为该节点的儿子；
    将新变元节点的地址入栈，在GOTO表中找到下一个状态入栈；调用一次ATTR中与产生式编号
    对应的函数即可，即可完成属性计算和中间代码生成*/
    int n; //归约式右侧元素数
    vector<int> s_red(10);
    vector<void *> y_red(10);
    void *newsymbol;
    if (idP == 1)
    {
        n = 2;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        P *red_fa = new P();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P1(*(Dp *)y_red[1], *(Sp *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][23].second);
    }
    else if (idP == 2)
    {
        n = 0;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        Dp *red_fa = new Dp();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P2(*(Dp *)red_fa);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][24].second);
    }
    else if (idP == 3)
    {
        n = 2;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        Dp *red_fa = new Dp();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P3(*(Dp *)red_fa, *(Dp *)y_red[1], *(D *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][24].second);
    }
    else if (idP == 4)
    {
        n = 1;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        Sp *red_fa = new Sp();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P4(*(Sp *)red_fa, *(S *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][25].second);
    }
    else if (idP == 5)
    {
        n = 3;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        Sp *red_fa = new Sp();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P5(*(Sp *)red_fa, *(Sp *)y_red[2], *(S *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][25].second);
    }
    else if (idP == 6)
    {
        n = 2;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        D *red_fa = new D();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        // cerr<<"!";
        ATTR::P6(*(D *)red_fa, *(T *)y_red[1], *(d *)y_red[0]);
        // cerr<<"!";
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][26].second);
    }
    else if (idP == 7)
    {
        n = 5;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        D *red_fa = new D();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P7(*(D *)red_fa, *(T *)y_red[4], *(d *)y_red[3], *(Ip *)y_red[1]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][26].second);
    }
    else if (idP == 8)
    {
        n = 9;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        D *red_fa = new D();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P8(*(D *)red_fa, *(T *)y_red[8], *(d *)y_red[7], *(Dp *)y_red[5], *(Dp *)y_red[2], *(Sp *)y_red[1]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][26].second);
    }
    else if (idP == 9)
    {
        n = 1;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        T *red_fa = new T();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P9(*(T *)red_fa);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][27].second);
        // cerr<<sit<<","<<slrp[sit][27].second<<endl;
    }
    else if (idP == 10)
    {
        n = 1;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        T *red_fa = new T();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P10(*(T *)red_fa);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][27].second);
    }
    else if (idP == 11)
    {
        n = 1;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        T *red_fa = new T();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P11(*(T *)red_fa);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][27].second);
    }
    else if (idP == 12)
    {
        n = 1;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        Ip *red_fa = new Ip();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P12(*(Ip *)red_fa, *(i *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][28].second);
    }
    else if (idP == 13)
    {
        n = 3;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        Ip *red_fa = new Ip();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P13(*(Ip *)red_fa, *(Ip *)y_red[2], *(i *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][28].second);
    }
    else if (idP == 14)
    {
        n = 3;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        S *red_fa = new S();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P14(*(S *)red_fa, *(d *)y_red[2], *(E *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][29].second);
    }
    else if (idP == 15)
    {
        n = 5;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        S *red_fa = new S();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P15(*(S *)red_fa, *(B *)y_red[4], *(S *)y_red[2]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][29].second);
    }
    else if (idP == 16)
    {
        n = 7;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        S *red_fa = new S();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P16(*(S *)red_fa, *(B *)y_red[4], *(S *)y_red[2], *(S *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][29].second);
    }
    else if (idP == 17)
    {
        n = 5;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        S *red_fa = new S();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P17(*(S *)red_fa, *(B *)y_red[2], *(S *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][29].second);
    }
    else if (idP == 18)
    {
        n = 2;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        S *red_fa = new S();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P18(*(S *)red_fa, *(E *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][29].second);
    }
    else if (idP == 19)
    {
        n = 3;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        S *red_fa = new S();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P19(*(S *)red_fa, *(Sp *)y_red[1]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][29].second);
    }
    else if (idP == 20)
    {
        n = 4;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        S *red_fa = new S();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P20(*(S *)red_fa, *(d *)y_red[3], *(Ep *)y_red[1]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][29].second);
    }
    else if (idP == 21)
    {
        n = 1;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        Ep *red_fa = new Ep();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P21(*(Ep *)red_fa, *(E *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][30].second);
    }
    else if (idP == 22)
    {
        n = 3;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        Ep *red_fa = new Ep();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P22(*(Ep *)red_fa, *(Ep *)y_red[2], *(E *)y_red[1]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][30].second);
    }
    else if (idP == 23)
    {
        n = 1;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        E *red_fa = new E();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P23(*(E *)red_fa, *(i *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][31].second);
    }
    else if (idP == 24)
    {
        n = 1;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        E *red_fa = new E();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P24(*(E *)red_fa, *(d *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][31].second);
    }
    else if (idP == 25)
    {
        n = 4;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        E *red_fa = new E();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P25(*(E *)red_fa, *(d *)y_red[3], *(Ep *)y_red[1]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][31].second);
    }
    else if (idP == 26)
    {
        n = 4;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        E *red_fa = new E();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P26(*(E *)red_fa, *(d *)y_red[3], *(Ep *)y_red[1]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][31].second);
    }
    else if (idP == 27)
    {
        n = 3;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        E *red_fa = new E();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P27(*(E *)red_fa, *(E *)y_red[2], *(E *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][31].second);
    }
    else if (idP == 28)
    {
        n = 3;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        E *red_fa = new E();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P28(*(E *)red_fa, *(E *)y_red[2], *(E *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][31].second);
    }
    else if (idP == 29)
    {
        n = 3;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        B *red_fa = new B();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P29(*(B *)red_fa, *(E *)y_red[2], *(r *)y_red[1], *(E *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][32].second);
    }
    else if (idP == 30)
    {
        n = 1;
        for (int i = 0; i < n; i++)
        {
            s_red[i] = state.top();
            state.pop();
            y_red[i] = symb.top();
            symb.pop();
        }
        sit = state.top();
        B *red_fa = new B();
        red_fa->son_num = n;
        for (int i = 0; i < n; i++)
        {
            red_fa->son.push_back(y_red[i]);
        }
        ATTR::P30(*(B *)red_fa, *(E *)y_red[0]);
        newsymbol = (void *)red_fa;
        symb.push(newsymbol);
        state.push(slrp[sit][32].second);
    }
}
void movein(int stP, int tkP, string s) //移进
{
    //读token，new出的终结符节点入栈，计算下一个状态，下一个状态入栈
    state.push(stP);
    void *temp_pt;
    if (tkP == 2)
    {
        i *tempp = new i();
        tempp->value = atoi(s.c_str());
        temp_pt = (void *)tempp;
    }
    else if (tkP == 1)
    {
        d *tempp = new d();
        tempp->name = s;
        temp_pt = (void *)tempp;
    }
    else if (tkP == 14)
    {
        r *tempp = new r();
        tempp->name = s;
        temp_pt = (void *)tempp;
    }
    else
    {
        string *tempp = new string(s);
        temp_pt = (void *)tempp;
    }
    symb.push(temp_pt);
}
void show()
{
    for(auto p:tablist)
    {
        cerr<<p->code<<endl<<endl;
    }
}
void clean(){}
int main()
{
    IO::readSLR();
    IO::transSLR();
    IO::readtoken();

    // TODO：语法分析主程序
    state.push(0); //初始状态为0
    string *templdol = new string("$");
    symb.push((void *)templdol); //符号表预先留一个$
    sit = 0;
    bool acc = false;
    int tk;
    pair<char, int> motion;

    for (int i = 0; i < token_lines; i++)
    {
        sit = state.top();
        tk = transfer_token(tokens[i]);
        motion = slrp[sit][tk];
        while (motion.first == 'r')
        {
            reduce(motion.second);
            sit = state.top();
            tk = transfer_token(tokens[i]);
            motion = slrp[sit][tk];
        }
        /*cerr<<i<<endl;
        cerr<<tokens[i].second<<endl;
        cerr<<sit<<","<<tk<<endl;
        cerr<<":"<<motion.first<<","<<motion.second<<endl;*/
        if (motion.first == 'a')
        {
            cerr << "---COMPILATION SUCCESS---" << endl;
            show(), clean();
            return 0;
        }
        if (motion.first == 's')
        {
            movein(motion.second, tk, tokens[i].second);
        }
        else
            break;
        // cerr<<i<<":"<<state.top()<<endl;
    }
    // todo : 输出
    //语法分析完成后，符号表存在全局数组tablist中，符号表的code域即为对应的中间代码
    //程序结束时，需要将tablist中的符号表全部delete
    cerr << "---GRAMMATICAL ERROR---" << endl;
    clean();
    return 0;
}