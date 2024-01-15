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

/* 0~20 ��ʾ�ؼ��֣��ճ�21~49 
 * 50~66 ��ʾ���������ճ� 67~99 
 * 100~107 ��ʾ��� */
enum KEYWORD {IF = 0, ELSE = 1, VOID = 2, RETURN = 3, 
              WHILE = 4, FOR = 5, DO = 6, INT = 7, 
              CHAR = 8, DOUBLE = 9, FLOAT = 10, UNSIGNED = 11, 
              MAIN = 12, STRING = 13, SWITCH = 14, CASE = 15, 
              CIN = 16, COUT = 17, BREAK = 18, CONST = 19, BOOL = 20};
/* �ַ������������ж������Ƿ�Ϊ�ؼ��� */
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
const int VARIABLE = 300; // ����Ϊ200��������Ϊ201���ַ���Ϊ202����ͨ��ʶ��Ϊ300;

void process(FILE* fd); // ���дʷ�������
int isKeyword( string& word);  // �ж����봮�Ƿ�Ϊ�ַ���
bool isLegalChar(char& ch); // �ж������ַ��Ƿ�Ϸ�
void output( string& word, int number); // ���ڸ�ʽ�����

int main() 
{
    char File[128];
    FILE* fd;
    cout << "��������Ҫ���дʷ��������ļ���(��.\\eval_input\\t0.in):";
    while(true) 
	{
        cin >> File;
        cout << "File" << endl;
        if((fd = fopen(File, "r")) != NULL) {break;} 
        else {
             cout << "�ļ���������ļ������ڣ�����������Ϲ���ļ�����" <<  endl;
             cin.clear();   //��������ʶ��
             cin.sync();    //�������������
        }
    }
    //����ļ�
    string outname;
    cout << "������������ļ���(��t0.out)���ļ����������output�ļ�����:";
    cin >> outname;
    outname = ".\\output\\" + outname;
    freopen(outname.c_str(), "w", stdout);
    cout << "�ʷ������Ľ��Ϊ��" << endl;
    process(fd);
    fclose(fd);
    return 0;   
}

void process(FILE* fd) 
{
    char ch = fgetc(fd);
    string word; // ��
    bool blank_flag; // �Ƿ��пհ׷�δ���Թ�
    while(ch != EOF) {
        word = "";
        blank_flag = true;
        /* ������հ׷���ע�ͣ���������ַ����򲻴��� */
        switch(ch) {
            case '/': {
                /* //ע�� */
                /* FILE*û��peek()������Ҫ�ȶ�ȡ�ٻ�����ʵ�� */
                char temp = fgetc(fd);
                if(temp == EOF) break;
                else
                if(temp == '/') {
                    while((temp = fgetc(fd)) != EOF && (temp = fgetc(fd)) != '\n');
                } // ������ '\n' ���Իص�case '\n': �����break����
                // /* */ ע�� 
                else if(temp == '*') 
				{
                    bool flag = true; // �����ж��Ƿ������ */
                    while(temp != EOF && (temp == '*' || flag))
					{
                        if(temp == '*') {
                            if((temp = fgetc(fd)) == '/')
							{
                                flag = false;
                                break;
                            } /* ���ɨ�赽 * / �����ѭ�������������ȡ */
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
                /* else ���Ǹ���ͨ��/��ָ�����һ�񲢸�ԭch����������������ch��break����*/
            }
            /* �����ֿհ׷�ֱ��������������һ������ */
            case ' ':break;
            case '\t':break;
            case '\r':break; 
            case '\n':break;
            default:
                blank_flag = false;
                break;
        }
        if(ch == EOF) break; // ��ֹ��ȥ��ע��ʱ�����ı����������
        if(blank_flag){
            ch = fgetc(fd);
            continue; // ���пհ׷���ֱ�ӽ�����һ�ζ���
        }
        
        if(!isLegalChar(ch)) 
		{
            cout << "�ַ�" << ch << "���Ϸ������Զ������ò��֡�" <<  endl;
            ch = fgetc(fd);
            continue;
        } 
		else 
		{
            /* ���ж���������Ϊ�������������ֿ�ͷ */
            if(ch >= '0' && ch <= '9')
			{
                bool dot_flag = false; // ��¼�Ƿ���С����
                while(ch != EOF && ((ch >= '0' && ch <= '9') || (ch == '.')))
				{
                    word += ch;
                    /* �ж��С���㣬���Ϸ����� */
                    if(dot_flag == true && ch == '.') 
					{
                         cout << "�������벻�Ϸ������Զ����������֣���ȷ��������ȷ�����ָ�ʽ��" <<  endl;
                        while(ch != EOF && ((ch >= '0' && ch <= '9') || (ch == '.'))) {
                            ch = fgetc(fd);
                        }
                        /* ���������ֶ��겢������������¿�ʼ��ȡ��һ������ */
                        break;
                    }
                    if(ch == '.') dot_flag = true;
                    
                    ch = fgetc(fd);
                    if(!((ch >= '0' && ch <= '9') || (ch == '.') || (ch == ';') || (ch == '[') || ch == '+' || ch == '-'
                        ||ch == '/' || ch == '*'  ||  ch == '%'  ||  ch == '='  ||  ch == ']'  || ch == '(' || ch == ')'
                        ||ch == '{' || ch == '}'  ||  ch == ','  ||  ch == '\'' ||  ch == ':'  || ch==' ')) 
					{
                        // ��ô�����У�����Ϊ��ĸ������ܻ������������ȡ���char a[4]; 4*5��;
                        cout << "���ֻ��ʶ�����벻�Ϸ������Զ������õ��ʣ���ȷ��������ȷ�ĸ�ʽ��" << endl;
                        while(ch != EOF && (ch != ' ' || ch != '\n' || ch != '\r' || ch != '\t')) {ch = fgetc(fd);
                        					} /* �������������ֿ�ͷ��������ڣ���ֱ�������õ���ֱ���հ׷��� */
                        break;
                    }
                }
                if(dot_flag) output(word, FLOAT_NUMBER);
				else output(word, INTEGER_NUMBER);
            } 
            else if ((ch >= 'a' && ch <= 'z') ||  (ch >= 'A' && ch <= 'Z'))
			{
                /* ��ʶ�� �� �ؼ���*/
                while(ch != EOF && ((ch >= 'a' && ch <= 'z') ||  (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_')) {
                    word += ch;
                    ch = fgetc(fd);
                }
                if(isKeyword(word) != -1) output(word, isKeyword(word));
				else output(word, VARIABLE);
            } 
			else 
			{ /* ������ */
                word += ch;
                switch(ch) {
                    /* ������г�ǰ���� */
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
                        /* ���������ַ��� */
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
                    /* ��Ҫ���г�ǰ���� */
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
                            cout << setw(16) << "ʶ�𵽲��Ϸ����ַ�|" << endl;
                        }
                        break;
                    }
                    case '&': {
                        if(temp == '&'){
                            word += temp;
                            output(word, AND);
                        } else {
                            cout << setw(16) << "ʶ�𵽲��Ϸ����ַ�&" << endl;
                        }
                        break;
                    }
                    case '<': {
                        if(temp == '='){
                            word += temp;
                            fgetc(fd); // ��Ϊ���Ҫ���ˣ�����Ҫ�ٶ��ȡһ������ֹ���¶���
                            output(word, LESSEQUAL);
                        } else if (temp == '<') {
                            word += temp;
                            fgetc(fd); // ��Ϊ���Ҫ���ˣ�����Ҫ�ٶ��ȡһ������ֹ���¶���
                            output(word, LEFTMOVE);
                        } else {
                            output(word, LESS);
                        }
                        break;
                    }
                    case '>': {
                        if(temp == '='){
                            word += temp;
                            fgetc(fd); // ��Ϊ���Ҫ���ˣ�����Ҫ�ٶ��ȡһ������ֹ���¶���
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
                fseek(fd, -1, SEEK_CUR); // ����һ��
                ch = fgetc(fd);
            }
        } // ��if������������һ�����ʵĶ���
    }
}

int isKeyword(string& word)
{
    for(int i = 0; i < 20; ++i)
	{
        if(keyWord[i] == word) {
            return i; // ����ö��ֵ
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
    } else if(number < 50){   //�ؼ��� 
        cout << setw(3) << number << "   " << setw(6) << word << "  ";
        if (word=="int") cout<<'i'<<endl;
        else if (word=="float"||word=="FLOAT") cout<<'f'<<endl;
        else cout<<word<<endl;
    } else if(number < 100){
        cout << setw(3) << number << "   " << setw(6) << word << "  "<<word << endl;  //������ 
    } else if(number < 200){
        cout << setw(3) << number << "   " << setw(6) << word << "  "<<word << endl; //��� 
    } else if(number == 200){
        cout << setw(3) << number << "   " << setw(6) << word << "  INT" << endl;      //���� 
    } else if(number == 201){
        cout << setw(3) << number << "   " << setw(6) << word << "  FLOAT" << endl;    //������ 
    } else if(number == 202){
        cout << setw(3) << number << "   " << setw(6) << word << "  string" << endl;       //�ַ��� 
    } else if(number == 300){
        cout << setw(3) << number << "   " << setw(6) << word << "  ID" << endl;   //��ʶ�� 
    }
}
