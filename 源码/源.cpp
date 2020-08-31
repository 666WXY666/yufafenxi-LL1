#include<iostream>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<cstring>
#include<cstdio>
#include<algorithm>
#include<cstdlib>
#include<vector>
#include<string>
#include<cctype>
#include<map>
#include<set>

using namespace std;

//假定非终结符仅为一个大写字母，终结符仅为一个小写字母和各种符号，且用~代替epsilon
class PF//Production formula,产生式的类
{
public:
	string left;//产生式的左部
	set<string> right;//产生式的右部
	PF(char s[])//构造函数，确定产生式左部
	{
		left = s;
	}
	void insert(char s[])//插入产生式右部的函数
	{
		right.insert(s);
	}
};

map<string, set<char> > first;//first集
map<string, set<char> > follow;//follow集
vector<PF> PF_vector;//产生式
bool used[999];
vector<map<char, string>> predict_table;//LL1分析表
vector<char> A;//分析栈
vector<char> B;//剩余串
map<string, int> PF_map;//存储每个非终结符对应的编号，key为非终结符，value为编号
vector<char> letter;//所有的终结符+'$',在构造预测分析表的时候创建完毕
int B_point = 0, input_len = 0;//B_point为输入串指针，input_len为输入串长度

void DFS(int x)//为构造first集深度优先遍历PF，递归调用
{
	if (used[x])
		return;
	used[x] = 1;
	string& left = PF_vector[x].left;
	set<string>& right = PF_vector[x].right;
	set<string>::iterator it = right.begin();
	for (; it != right.end(); it++)//对于非终结符的产生式的所有右部
	{
		for (int i = 0; i < it->length(); i++)//对于这个右部的每个字符
		{
			if (!isupper(it->at(i)))//如果不是大写字母，即为终结符，直接加入到first集中
			{
				first[left].insert(it->at(i));
				break;//已经是终结符，不必再深度搜索，跳出
			}
			else//如果是大写字母，即为非终结符
			{
				int y;
				y = PF_map[it->substr(i, 1)] - 1;
				string& tleft = PF_vector[y].left;
				DFS(y);//递归调用
				set<char>& temp = first[tleft];
				set<char>::iterator it_point = temp.begin();
				for (; it_point != temp.end(); it_point++)
				{
					if (*it_point!='~')//把所有不为空的first集加入
					{
						first[left].insert(*it_point);
					}
				}
				set<char>::iterator it_temp = temp.find('~');//查找first集中有没有空
				if (it_temp!=temp.end())//如果存在空
				{
					if (i == it->length() - 1)//并且该符号是当前产生式的最后一个符号，把空加入到待求first集中
					{
						first[left].insert('~');
					}
					//如果该符号不是当前产生式的最后一个符号，则继续递归查找
				}
				else//如果不存在空，可以停止递归，直接跳出
					break;
			}
		}
	}
}

void first_construction()//构造并输出first集
{
	memset(used, 0, sizeof(used));
	for (int i = 0; i < PF_vector.size(); i++)
		DFS(i);//输出构造好的first集
	printf("----------------------FIRST集----------------------\n");
	map<string, set<char> >::iterator it = first.begin();
	for (; it != first.end(); it++)
	{
		printf("%s:{", it->first.c_str());
		set<char>& temp = it->second;
		set<char>::iterator it_temp = temp.begin();
		bool flag = false;
		for (; it_temp != temp.end(); it_temp++)
		{
			if (flag) printf(",");
			printf("%c", *it_temp);
			flag = true;
		}
		printf("}\n");
	}
}

void add_follow(const string& str1, const string& str2)//将str1的follow集加入到str2的follow集中
{
	set<char>& from = follow[str1];
	set<char>& to = follow[str2];
	set<char>::iterator it = from.begin();
	for (; it != from.end(); it++)
		to.insert(*it);
}

void follow_construction()//构造并输出follow集
{
	follow[PF_vector[0].left].insert('$');//首先将‘$’加入到起始符的follow集中，这里默认第一个输入的产生式的左部为起始符
	while (true)//一直循环求follow集，直到所有的非终结符的follow集都没有变化为止
	{
		bool not_finished = false;//使用一个bool类型的变量纪录follow集是否变大
		for (int i = 0; i < PF_vector.size(); i++)//遍历所有的非终结符
		{
			string& left = PF_vector[i].left;
			set<string>& right = PF_vector[i].right;
			set<string>::iterator it = right.begin();
			for (; it != right.end(); it++)//遍历所有非终结符的产生式
			{
				bool flag = true;//用来判断是否要将产生式左边非终结符的follow集加入到当前查找的非终结符的follow集中
				const string& str = *it;
				for (int j = it->length() - 1; j >= 0; j--)//遍历整个产生式右部，且从后往前看
				{
					if (isupper(str[j]))//是非终结符
					{
						int x = PF_map[it->substr(j, 1)] - 1;
						if (flag)//判断是否要将产生式左边非终结符的follow集加入到当前查找的非终结符的follow集中
						{
							int num_old = follow[it->substr(j, 1)].size();
							add_follow(left, it->substr(j, 1));
							if (!PF_vector[x].right.count("~"))//如果当前的非终结符的产生式中不含空，说明所有它前面的非终结符均不可能将
								flag = false;					//产生式左边非终结符的follow集加入到当前查找的非终结符的follow集中
							int num_new = follow[it->substr(j, 1)].size();
							if (num_new > num_old)//如果follow集没有变化，说明follow集已经构造完成
								not_finished = true;
						}
						for (int k = j + 1; k < it->length(); k++)
						{
							if (isupper(str[k]))//是非终结符
							{
								string id;
								id = it->substr(k, 1);
								set<char>& from = first[id];
								set<char>& to = follow[it->substr(j, 1)];

								set<char>::iterator it_temp = from.begin();
								int num_old = follow[it->substr(j, 1)].size();
								for (; it_temp != from.end(); it_temp++)//把待求非终结符的后面的非终结符的first集中不为空的终结符加入待求非终结符的follow集中
									if (*it_temp != '~')
										to.insert(*it_temp);
								int num_new = follow[it->substr(j, 1)].size();
								if (num_new > num_old) //如果follow集没有变化，说明follow集已经构造完成
									not_finished = true;

								if (!PF_vector[PF_map[id] - 1].right.count("~"))//如果待求非终结符的后面的非终结符的first集中没有空，就不必往后看，直接跳出
									break;
							}
							else//是终结符
							{
								int num_old = follow[it->substr(j, 1)].size();
								follow[it->substr(j, 1)].insert(str[k]);//直接将待求非终结符后面的终结符加入到待求非终结符的follow集中
								int num_new = follow[it->substr(j, 1)].size();
								if (num_new > num_old)//如果follow集没有变化，说明follow集已经构造完成
									not_finished = true;
								break;//因为待求非终结符后面是终结符，不必再往后看，直接跳出
							}
						}
					}
					else//是终结符
						flag = false;//如果中途遇到终结符，就不可能将产生式左边非终结符的follow集加入到当前查找的非终结符的follow集中
				}
			}
		}
		if (!not_finished)//如果所有的非终结符的follow集都没有变化，说明所有的follow集已经构造完成，跳出while循环
			break;
	}
	//输出构造好的follow集
	printf("----------------------FOLLOW集----------------------\n");
	map<string, set<char> >::iterator it = follow.begin();
	for (; it != follow.end(); it++)
	{
		printf("%s:{", it->first.c_str());
		set<char>& temp = it->second;
		set<char>::iterator it_temp = temp.begin();
		bool flag = false;
		for (; it_temp != temp.end(); it_temp++)
		{
			if (flag) printf(",");
			printf("%c", *it_temp);
			flag = true;
		}
		printf("}\n");
	}
}

bool check_first(const string& str, char ch)//检查ch是否属于str的first集
{
	bool hasEmpty = false;
	for (int i = 0; i < str.length(); i++)
	{
		if (!isupper(str[i]))//str第一个字符不是大写，也就是终结符或空
		{
			if (str[i] != ch)
				return false;
			else
				return true;
		}
		else//str第一个字符是大写，也就是非终结符
		{
			string temp;
			temp = str.substr(i, 1);
			set<char>& dic = first[temp];
			set<char>::iterator it = dic.begin();
			for (; it != dic.end(); it++)
			{
				if (*it == '~')//str的第一个字符的first集里有空
					hasEmpty = true;
				else//ch与str[i]的非空first集比较
				{
					hasEmpty = false;
					if (*it == ch)//str的第一个字符的first集为待查找的ch
						return true;//查找成功，返回true
				}
			}
			if (!hasEmpty)//如果str[i]的first集里没有空，就可以不找下一个字符，直接跳出循环
			{
				break;
			}
		}
	}
	if (hasEmpty && ch == '~')//如果str的所有字符的first集里都有空，并且待查找的字符就为‘~’，查找成功，返回true
	{
		return true;
	}
	return false;//查找失败，返回false
}

bool check_follow(const string& str, char ch)//检查ch是否属于str的follow集
{
	set<char>& dic = follow[str];
	set<char>::iterator it = dic.begin();
	for (; it != dic.end(); it++)//对于str也就是非终结符follow集的所有查找
		if (*it == ch)
			return true;
	return false;
}

void predict_table_construction()//构造并输出LL1预测分析表
{
	map<char, string> temp;
	bool flag[500];
	memset(flag, 0, sizeof(flag));

	for (int i = 0; i < PF_vector.size(); i++)//把所有的终结符加入到letter中
	{
		set<string>& right = PF_vector[i].right;
		set<string>::iterator it = right.begin();
		for (; it != right.end(); it++)
			for (int j = 0; j < it->length(); j++)
				if (!isupper(it->at(j)))
				{
					if (flag[it->at(j)]) 
						continue;
					flag[it->at(j)] = true;
					if (it->at(j)!='~')
					{
						letter.push_back(it->at(j));
					}
				}
	}
	for (int i = 0; i < PF_vector.size(); i++)//遍历所有非终结符的产生式，判断该产生式是否应该加入到该表项中
	{
		temp.clear();
		string& left = PF_vector[i].left;
		set<string>& right = PF_vector[i].right;
		set<string>::iterator it = right.begin();
		for (; it != right.end(); it++)//对于该非终结符的所有产生式右部
		{
			for (int j = 0; j < letter.size(); j++)//遍历所有的终结符
			{
				if (check_first(*it, letter[j]))
				{
					temp[letter[j]] = *it;
				}
			}
			if (check_first(*it, '~'))//判断该终结符是否在该产生式右部的first集中
			{
				set<char>::iterator temp_it = follow[left].begin();
				for (; temp_it!=follow[left].end();temp_it++)
				{
					temp[*temp_it] = *it;
				}
			}
		}
		predict_table.push_back(temp);
	}
	//输出构造好的LL1分析表
	printf("---------------------预测分析表---------------------\n");
	letter.push_back('$');//将‘$’加入到letter中
	for (int i = 0; i <= (letter.size() + 1) * 11; i++)
		printf("-");
	printf("\n");
	printf("|%10s|", "");
	for (int i = 0; i < letter.size(); i++)
		printf("%10c|", letter[i]);
	printf("\n");
	for (int i = 0; i <= (letter.size() + 1) * 11; i++)
		printf("-");
	printf("\n");
	for (int i = 0; i < PF_vector.size(); i++)
	{
		printf("|%10s|", PF_vector[i].left.c_str());
		for (int j = 0; j < letter.size(); j++)
			if (predict_table[i].count(letter[j]))
			{
				printf("%10s|", (PF_vector[i].left+"->"+predict_table[i][letter[j]]).c_str());
			}
			else printf("%10s|","");
		printf("\n");
		for (int i = 0; i <= (letter.size() + 1) * 11; i++)
			printf("-");
		printf("\n");
	}
}

void print_A()//输出分析栈
{
	string temp="";
	for (int a = 0;a < A.size();a++)
		temp += A[a];
	printf("%-15s|", temp.c_str());
}

void print_B()//输出剩余串
{
	string temp = "";
	for (int b = B_point;b < B.size();b++)
		temp += B[b];
	printf("%15s|", temp.c_str());
}

void analyse()//预测分析控制程序
{
	int m, n, k = 0, flag = 0;
	char ch, x;
	A.clear();//每次分析都先把分析栈置空
	B.clear();//每次分析都先把剩余串置空
	B_point = 0;//每次分析都先把输入串指针置0
	input_len = 0;//每次分析都先把输入串长度置0
	printf("请输入要分析的字符串(用i代表数字，以‘$’结尾):");
	do//读入分析串
	{
		cin >> ch;
		B.push_back(ch);
		input_len++;
	} while (ch != '$');
	ch = B[0];//当前分析字符
	A.push_back('$');
	A.push_back(PF_vector[0].left[0]);//'$',初始非终结符进栈
	printf("-------------------------------------------------------\n");
	printf("|%5s|%-15s|%15s|%15s|\n","步骤","分析栈","剩余字符","产生式");
	printf("-------------------------------------------------------\n");
	do {
		printf("|%5d|", k++);
		print_A();
		print_B();
		x = A[A.size() - 1];//x为当前栈顶字符
		A.pop_back();
		if (x == '$' && B[B_point] == '$')//栈顶文法符号x不为‘$’且所指向的输入符号a也不为‘$’时，接受（对课本上的算法进行了完善）
		{
			printf("%15s|\n", "Accept!");
			printf("-------------------------------------------------------\n");
			break;
		}
		for (int j = 0;j < letter.size();j++)//判断是否为终结符和‘$’
		{
			if (x == letter[j])
			{
				flag = 1;
				break;
			}
		}
		if (flag == 1)//如果是终结符或‘$’
		{
			if (x == ch)
			{
				string temp_str = string(1, ch) + " 匹配";
				printf("%15s|\n", temp_str.c_str());
				printf("-------------------------------------------------------\n");
				ch = B[++B_point];//下一个输入字符
				flag = 0;
			}
			else//终结符不匹配或在中途遇到了‘$’的出错处理
			{
				string temp_str = string(1, ch) + " 出错";
				printf("%15s|\n", temp_str.c_str());//输出出错终结符
				printf("-------------------------------------------------------\n");
				break;
			}
		}
		else//如果是非终结符
		{
			int point;
			for (point = 0; point < PF_vector.size(); point++)//返回x的非终结符下标
			{
				if (PF_vector[point].left[0] == x)
				{
					break;
				}
			}
			string cha = predict_table[point][ch];
			if (!cha.empty())//判断表项是否为空
			{
				string temp_str = PF_vector[point].left + "->";
				for (int i = 0; i < cha.size(); i++)
				{
					temp_str += cha[i];
				}
				printf("%15s|\n-------------------------------------------------------\n", temp_str.c_str());//输出产生式
				if (cha[0] != '~')//产生式右部为空则不进栈
				{
					for (int j = (cha.size() - 1);j >= 0;j--)//产生式逆序入栈
					{
						A.push_back(cha[j]);
					}
				}
			}
			else//表项为空的出错处理
			{
				string temp_str = string(1, x) + " 表项为空";
				printf("%15s|\n", temp_str.c_str());//输出出错非终结符
				printf("-------------------------------------------------------\n");
				break;
			}
		}
	} while (true);
}

int main()
{
	int n;
	char s[999];
	printf("请输入该文法产生式的数目:");
	scanf("%d", &n);
	printf("请输入产生式(该文法必须满足LL1文法的条件，非终结符仅为一个大写字母，终结符仅为一个小写字母和各种符号，且用~代替epsilon):\n");
	for (int i = 0; i < n; i++)
	{
		scanf("%s", s);
		int len = strlen(s), j;
		for (j = 0; j < len; j++)
			if (s[j] == '-') break;
		s[j] = 0;
		if (!PF_map[s])
		{
			PF_vector.push_back(s);
			PF_map[s] = PF_vector.size();
		}
		int x = PF_map[s] - 1;
		PF_vector[x].insert(s + j + 2);
	}
	first_construction();
	follow_construction();
	predict_table_construction();
	int main_flag = 1;
	printf("请输入是否要分析字符串(1代表是，0代表否)：");
	cin >> main_flag;
	while (main_flag)
	{
		analyse();
		printf("请输入是否要继续分析(1代表继续，0代表结束)：");
		cin >> main_flag;
	}
	return 0;
}