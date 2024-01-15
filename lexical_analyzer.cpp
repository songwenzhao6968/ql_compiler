#include<bits/stdc++.h>

#define ll long long
using namespace std;
inline int read()
{
    int x = 0, f = 1; char ch = getchar();
    while (!isdigit(ch)) { if (ch == '-')f = -1; ch = getchar(); }
    while (isdigit(ch)) { x = (x << 1) + (x << 3) + ch - '0'; ch = getchar(); }
    return x * f;
}

/* 0~20 表示关键字，空出21~49 
 * 50~66 表示操作符，空出 67~99 
 * 100~107 表示界符 */
enum KEYWORD {IF = 0, ELSE = 1, VOID = 2, RETURN = 3, 
              WHILE = 4, FOR = 5, DO = 6, INT = 7, 
              CHAR = 8, DOUBLE = 9, FLOAT = 10, UNSIGNED = 11, 
              MAIN = 12, STRING = 13, SWITCH = 14, CASE = 15, 
              CIN = 16, COUT = 17, BREAK = 18, CONST = 19, BOOL = 20};
/* 字符串数组用于判断输入是否为关键词 */
string keyWord[] = {"if", "else", "void", "return", "while",
                         "for", "do", "int", "char", "double", "float",
                         "unsigned", "main", "string", "switch", "case",
                         "cin", "cout", "break", "const", "bool"};
enum OPERATOR {PLUS = 50, MINUS = 51, MULTIPLY = 52, DIVIDE = 53, 
               LESS = 54, GREAT = 55, EQUAL= 56, NOT = 57, 
               EQUALEQUAL = 58, NOTEQUAL = 59, LESSEQUAL = 60, GREATEQUAL = 61, 
               MOD = 62, OR = 63, AND = 64, PLUSPLUS = 65, MINUSMINUS = 66,
               LEFTMOVE = 67, RIGHTMOVE = 68, PLUSEQUAL = 69, MINUSEQUAL = 70};
enum DELIMITER {COMMA = 100, SEMICOLON = 101, LBRACKET = 102, RBRACKET = 103,
                LSQBRACKET = 104, RSQBRACKET = 105, LBRACE = 106, RBRACE = 107,
                QUOTATION = 108, COLON = 109};
const int INTEGER_NUMBER = 200;
const int FLOAT_NUMBER = 201; 
const int STRING_WORD = 202;
const int VARIABLE = 300; // 整数为200，浮点数为201，字符串为202，普通标识符为300;

void process(FILE* fd); // 运行词法分析器
int isKeyword( string& word);  // 判断输入串是否为字符串
bool isLegalChar(char& ch); // 判断输入字符是否合法
void output( string& word, int number); // 用于格式化输出

int main() 
{
    char File[128];
    FILE* fd;
    cout << "请输入需要进行词法分析的文件名(例.\\eval_input\\t0.in):";
    while(true) 
	{
        cin >> File;
        cout << "File" << endl;
        if((fd = fopen(File, "r")) != NULL) {break;} 
        else {
             cout << "文件名有误或文件不存在，请重新输入合规的文件名：" <<  endl;
             cin.clear();   //清理错误标识符
             cin.sync();    //清楚缓冲区内容
        }
    }
    //输出文件
    string outname;
    cout << "请输入输出的文件名(例t0.out)，文件将会出现在output文件夹中:";
    cin >> outname;
    outname = ".\\output\\" + outname;
    freopen(outname.c_str(), "w", stdout);
    cout << "词法分析的结果为：" << endl;
    process(fd);
    fclose(fd);
    return 0;   
}

void process(FILE* fd) 
{
    char ch = fgetc(fd);
    string word; // 串
    bool blank_flag; // 是否有空白符未被略过
    while(ch != EOF) {
        word = "";
        blank_flag = true;
        /* 先清除空白符和注释，但如果是字符串则不处理 */
        switch(ch) {
            case '/': {
                /* //注释 */
                /* FILE*没有peek()函数，要先读取再回退来实现 */
                char temp = fgetc(fd);
                if(temp == EOF) break;
                else
                if(temp == '/') {
                    while((temp = fgetc(fd)) != EOF && (temp = fgetc(fd)) != '\n');
                } // 最后读入 '\n' 所以回到case '\n': 情况，break下沿
                // /* */ 注释 
                else if(temp == '*') 
				{
                    bool flag = true; // 用于判断是否检索到 */
                    while(temp != EOF && (temp == '*' || flag))
					{
                        if(temp == '*') {
                            if((temp = fgetc(fd)) == '/')
							{
                                flag = false;
                                break;
                            } /* 如果扫描到 * / 则结束循环，否则继续读取 */
                            else fseek(fd, -1, SEEK_CUR);
                        }
                        temp = fgetc(fd);
                    }
                } 
				else 
				{
                    blank_flag = false;
                    fseek(fd, -1, SEEK_CUR);
                }
                /* else 就是个普通的/，指针回退一格并复原ch。接下来继续操作ch，break下沿*/
            }
            /* 这四种空白符直接跳过，继续下一次输入 */
            case ' ':break;
            case '\t':break;
            case '\r':break; 
            case '\n':break;
            default:
                blank_flag = false;
                break;
        }
        if(ch == EOF) break; // 防止在去除注释时读完文本继续往后读
        if(blank_flag){
            ch = fgetc(fd);
            continue; // 仍有空白符，直接进行下一次读入
        }
        
        if(!isLegalChar(ch)) 
		{
            cout << "字符" << ch << "不合法，已自动抛弃该部分。" <<  endl;
            ch = fgetc(fd);
            continue;
        } 
		else 
		{
            /* 先判断整数，因为变量不能以数字开头 */
            if(ch >= '0' && ch <= '9')
			{
                bool dot_flag = false; // 记录是否有小数点
                while(ch != EOF && ((ch >= '0' && ch <= '9') || (ch == '.')))
				{
                    word += ch;
                    /* 有多个小数点，不合法数字 */
                    if(dot_flag == true && ch == '.') 
					{
                         cout << "数字输入不合法，已自动舍弃该数字，请确保输入正确的数字格式。" <<  endl;
                        while(ch != EOF && ((ch >= '0' && ch <= '9') || (ch == '.'))) {
                            ch = fgetc(fd);
                        }
                        /* 将整个数字读完并舍弃，随后重新开始读取下一个单词 */
                        break;
                    }
                    if(ch == '.') dot_flag = true;
                    
                    ch = fgetc(fd);
                    if(!((ch >= '0' && ch <= '9') || (ch == '.') || (ch == ';') || (ch == '[') || ch == '+' || ch == '-'
                        ||ch == '/' || ch == '*'  ||  ch == '%'  ||  ch == '='  ||  ch == ']'  || ch == '(' || ch == ')'
                        ||ch == '{' || ch == '}'  ||  ch == ','  ||  ch == '\'' ||  ch == ':'  || ch==' ')) 
					{
                        // 这么多特判，是因为字母后面可能会跟操作运算符等。如char a[4]; 4*5等;
                        cout << "数字或标识符输入不合法，已自动舍弃该单词，请确保输入正确的格式。" << endl;
                        while(ch != EOF && (ch != ' ' || ch != '\n' || ch != '\r' || ch != '\t')) {ch = fgetc(fd);
                        					} /* 变量不能由数字开头，如果存在，则直接舍弃该单词直到空白符。 */
                        break;
                    }
                }
                if(dot_flag) output(word, FLOAT_NUMBER);
				else output(word, INTEGER_NUMBER);
            } 
            else if ((ch >= 'a' && ch <= 'z') ||  (ch >= 'A' && ch <= 'Z'))
			{
                /* 标识符 或 关键字*/
                while(ch != EOF && ((ch >= 'a' && ch <= 'z') ||  (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_')) {
                    word += ch;
                    ch = fgetc(fd);
                }
                if(isKeyword(word) != -1) output(word, isKeyword(word));
				else output(word, VARIABLE);
            } 
			else 
			{ /* 操作符 */
                word += ch;
                switch(ch) {
                    /* 无须进行超前搜索 */
                    case '*':
                        output(word, MULTIPLY);
                        break;
                    case '/':
                        output(word, DIVIDE);
                        break;
                    case '%':
                        output(word, MOD);
                        break;
                    case ',':
                        output(word, COMMA);
                        break;
                    case ';':
                        output(word, SEMICOLON);
                        break;
                    case ':':
                        output(word, COLON);
                        break;
                    case '(':
                        output(word, LBRACKET);
                        break;
                    case ')':
                        output(word, RBRACKET);
                        break;
                    case '[':
                        output(word, LSQBRACKET);
                        break;
                    case ']':
                        output(word, RSQBRACKET);
                        break;
                    case '{':
                        output(word, LBRACE);
                        break;
                    case '}':
                        output(word, RBRACE);
                        break;
                    case '"':
                        output(word, QUOTATION);
                        /* 单独处理字符串 */
                        word = "";
                        ch = fgetc(fd);
                        while(ch != EOF && ch != '\"') {
                            word += ch;
                            ch = fgetc(fd);
                        }
                        output(word, STRING_WORD);
                        word = "\"";
                        output(word, QUOTATION);
                    default:
                        break;
                }
                char temp = fgetc(fd);
                if(temp == EOF) break;
                switch(ch) {
                    /* 需要进行超前搜索 */
                    case '+': {
                        if(temp == '+'){
                            word += temp;
                            fgetc(fd);
                            output(word, PLUSPLUS);
                        } else if (temp == '=') {
                            word += temp;
                            fgetc(fd);
                            output(word, PLUSEQUAL);
                        } else {
                            output(word, PLUS);
                        }
                        break;
                    }
                    case '-': {
                        if(temp == '-'){
                            word += temp;
                            fgetc(fd);
                            output(word, MINUSMINUS);
                        } else if (temp == '=') {
                            word += temp;
                            fgetc(fd);
                            output(word, MINUSEQUAL);
                        } else {
                            output(word, MINUS);
                        }
                        break;
                    }
                    case '|': {
                        if(temp == '|'){
                            word += temp;
                            output(word, OR);
                        } else {
                            cout << setw(16) << "识别到不合法的字符|" << endl;
                        }
                        break;
                    }
                    case '&': {
                        if(temp == '&'){
                            word += temp;
                            output(word, AND);
                        } else {
                            cout << setw(16) << "识别到不合法的字符&" << endl;
                        }
                        break;
                    }
                    case '<': {
                        if(temp == '='){
                            word += temp;
                            fgetc(fd); // 因为最后要回退，所以要再多读取一个，防止重新读入
                            output(word, LESSEQUAL);
                        } else if (temp == '<') {
                            word += temp;
                            fgetc(fd); // 因为最后要回退，所以要再多读取一个，防止重新读入
                            output(word, LEFTMOVE);
                        } else {
                            output(word, LESS);
                        }
                        break;
                    }
                    case '>': {
                        if(temp == '='){
                            word += temp;
                            fgetc(fd); // 因为最后要回退，所以要再多读取一个，防止重新读入
                            output(word, GREATEQUAL);
                        } else if (temp == '>') {
                            word += temp;
                            fgetc(fd);
                            output(word, RIGHTMOVE);
                        } else {
                            output(word, GREAT);
                        }
                        break;
                    }
                    case '=': {
                        if(temp == '='){
                            word += temp;
                            fgetc(fd);
                            output(word, EQUALEQUAL);
                        } else {
                            output(word, EQUAL);
                        }
                        break;
                    }
                    case '!': {
                        if(temp == '='){
                            word += temp;
                            fgetc(fd);
                            output(word, NOTEQUAL);
                        } else {
                            output(word, NOT);
                        }
                        break;
                    }
                    default:
                        break;
                }
                fseek(fd, -1, SEEK_CUR); // 回退一格
                ch = fgetc(fd);
            }
        } // 大if结束，进行下一个单词的读入
    }
}

int isKeyword(string& word)
{
    for(int i = 0; i < 20; ++i)
	{
        if(keyWord[i] == word) {
            return i; // 返回枚举值
        }
    }
    return -1;
}

bool isLegalChar(char& ch)
{
    if( (ch >= '0' && ch <= '9')
    ||  (ch >= 'a' && ch <= 'z')
    ||  (ch >= 'A' && ch <= 'Z')
    ||  (ch == '+') || (ch == '-') || (ch == '*')
    ||  (ch == '/') || (ch == '<') || (ch == '>')
    ||  (ch == '=') || (ch == '!') || (ch == '%')
    ||  (ch == '|') || (ch == '&') || (ch == ',')
    ||  (ch == ';') || (ch == '(') || (ch == ')')
    ||  (ch == '[') || (ch == ']') || (ch == '{')
    ||  (ch == '}') || (ch == ' ') || (ch == '\t')
    ||  (ch == '\r')|| (ch == '\n')|| (ch == '.')
    ||  (ch == '"') || (ch == '_') || (ch == ':')) 
        return true;
    return false;
}

void output(string& word, int number) 
{
    if(number < 0){
        cout << "something was wrong." << endl;
    } else if(number < 50){   //关键字 
        cout << setw(3) << number << "   " << setw(6) << word << "  ";
        if (word=="int") cout<<'i'<<endl;
        else if (word=="float"||word=="FLOAT") cout<<'f'<<endl;
        else cout<<word<<endl;
    } else if(number < 100){
        cout << setw(3) << number << "   " << setw(6) << word << "  "<<word << endl;  //操作符 
    } else if(number < 200){
        cout << setw(3) << number << "   " << setw(6) << word << "  "<<word << endl; //界符 
    } else if(number == 200){
        cout << setw(3) << number << "   " << setw(6) << word << "  INT" << endl;      //整数 
    } else if(number == 201){
        cout << setw(3) << number << "   " << setw(6) << word << "  FLOAT" << endl;    //浮点数 
    } else if(number == 202){
        cout << setw(3) << number << "   " << setw(6) << word << "  string" << endl;       //字符串 
    } else if(number == 300){
        cout << setw(3) << number << "   " << setw(6) << word << "  ID" << endl;   //标识符 
    }
}
