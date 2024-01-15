#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
using namespace std;
const int MAX_N = 5007;
int cnt = 0;
pair<string,vector<string> > exps[MAX_N];
set<string> Vns,Vts;//Vns表示非终结符的集合，Vts表示终结符的集合
map<string,set<string> > first;//first集合
map<string,set<string> > follow;//follow集合

#define DEBUG

void ERROR()
{
	cout << "分析程序出错,请检查上述提示错误\n";
	exit(0);
}

//判断是否属于终结符
bool is_terminal_char(const string & s) {
	//非大写的 属于终结符
	char ch = s[0];//非终结符第一位必然大写
	if('A'<= ch && ch <= 'Z') {
		return false;
	}
	return true;
}

void readExps() {
	string str;
	cout<<"input production (end with #):\n";

	while (getline(cin,str)) {
		if(str.find("#") != -1) break;//结束的条件，如果是#就直接跳出循环
		//输入的表达式以 -> 作为左右方向的分隔符
		int mid_pos = str.find("->");
		if(mid_pos == -1) {
			cout<<"输入的格式有误\n";
			ERROR();
		}
		int ls = 0,le = mid_pos;//表示左边的字符串起始位置和结束位置
		while (str[ls] == ' ') ls++;

		for(int i = ls; i<mid_pos; i++) {
			if(str[i] == ' ') {
				le = i;
				break;
			}
		}
		//确定表达式左边的非终结符的具体内容
		exps[cnt].first = str.substr(ls,le-ls);
		Vns.insert(exps[cnt].first);//加入到非终结符的集合中
		//继续往后读取，把表达式右边的字符全都加入进来
		int VtsLen = 0;
		int LastEnd = mid_pos+2;
		for(int i = mid_pos + 2; i<(int)str.length(); i++) {//表达式右边把所有的首个单词都读入到vector里面
			if(str[i] == ' ')
			{
				VtsLen = i - LastEnd;
				exps[cnt].second.push_back(str.substr(LastEnd,VtsLen));
				string exp_elem = str.substr(LastEnd,VtsLen);
				if(is_terminal_char(exp_elem)) {
				Vts.insert(exp_elem);
				first[exp_elem].insert(exp_elem);
				}
				break;
			}
		}
		cnt++;
	}
}


//计算first集合
void cal_first() {
	int pre_size = -1,now_size = 0;
	//设置更新的条件
	while(pre_size != now_size) {
		pre_size = now_size;
		for(int i = 0; i<cnt; i++) {
			string str = exps[i].first;
			vector<string> elements = exps[i].second;
			//如果说表达式右边的第一个字符属于终结符，那么就直接把它加入到first(str)中(规则1)
			if(is_terminal_char(elements[0])) {
				first[str].insert(elements[0]);
			}
			//规则2
			else {
				for (int j = 0; j < (int) elements.size(); j++) {
					if (is_terminal_char(elements[j])) {//用于循环体判断。如果发现已经到达了终结符的位置，就退出
						break;
					}
					//将两个集合进行合并
					for(string tmp : first[elements[j]]) {
						if(tmp != "~") {
							first[str].insert(tmp);
						}
						//如果发现所有的非终结符都可能推导出空字符，那空字符加入
						else if(j == elements.size() - 1) {
							first[str].insert(tmp);
						}
					}
					//如果发现没有空字符集，就直接退出去
					if (first[elements[j]].find("~") == first[elements[j]].end()) {
						break;
					}
				}
			}
			now_size = 0;
			for(string tmp : Vns) { //重新计算now_size的大小
				now_size += (int)first[tmp].size();
			}
		}
	}
}

void cal_follow() {
	//使用规则1
	follow["P"].insert("$");//P为文法的起始字符
	//这两个标志只用于判断是否已经生成了完全的follow集合
	int pre_size = -1,now_size = 0;
	while (pre_size !=  now_size) {
		pre_size = now_size;
		for(int i = 0; i<cnt; i++) {
			string str = exps[i].first;
			vector<string> elements = exps[i].second;
			//使用规则2
			for(int j = 0; j<(int)elements.size()-1; j++) {
				if(!is_terminal_char(elements[j])) { 
					//如果当前的是非终结符
					//并且后面的那个单元也是非终结符
					//就把后面的那个非终结符除~以外的所有first元素都加入进去
					follow[elements[j]].insert(first[elements[j + 1]].begin(), first[elements[j + 1]].end());
					//去掉空字符
					follow[elements[j]].erase("~");
					//否则就把后面的单个终结符加入进去
				}
			}
			//如果当前的是终结符，那么规则2就不用了.
			//使用规则3,为了方便起见，从后往向前遍历
			for(int j = elements.size() - 1; j>=0; j--) {
				//如果是非终结符，就把推导式中的follow元素都加入到它对应的follow集合里面
				if(!is_terminal_char(elements[j])) {
					follow[elements[j]].insert(follow[str].begin(),follow[str].end());
				} else break; //如果是终结符，就直接退出

				//表明在first集合中没有空字符，这样的话就直接退出
				if(first[elements[j]].find("~") == first[elements[j]].end()) {
					break;
				}
			}
		}
		now_size = 0;
		for(string tmp : Vns) { //重新计算now_size的大小
			now_size += (int)follow[tmp].size();
		}
	}
}

int main() {

	#ifdef DEBUG
	freopen("original_grammar.txt", "r", stdin);
	#endif

	Vts.insert("$");

	readExps();
	cal_first();
	cout<<"FIRST():\n";
	for(string n : Vns) {
		cout<<n<<":";
		for(string tmp : first[n]) {
			cout<<tmp<<" ";
		}
		cout<<endl;
	}
	cal_follow();
	cout<<"FOLLOW():\n";
	for(string n : Vns) {
		cout<<n<<":";
		for(string tmp : follow[n]) {
			cout<<tmp<<" ";
		}
		cout<<endl;
	}

	#ifdef DEBUG
	fclose(stdin);
	#endif

	return 0;
}

