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

//�ٶ����ս����Ϊһ����д��ĸ���ս����Ϊһ��Сд��ĸ�͸��ַ��ţ�����~����epsilon
class PF//Production formula,����ʽ����
{
public:
	string left;//����ʽ����
	set<string> right;//����ʽ���Ҳ�
	PF(char s[])//���캯����ȷ������ʽ��
	{
		left = s;
	}
	void insert(char s[])//�������ʽ�Ҳ��ĺ���
	{
		right.insert(s);
	}
};

map<string, set<char> > first;//first��
map<string, set<char> > follow;//follow��
vector<PF> PF_vector;//����ʽ
bool used[999];
vector<map<char, string>> predict_table;//LL1������
vector<char> A;//����ջ
vector<char> B;//ʣ�മ
map<string, int> PF_map;//�洢ÿ�����ս����Ӧ�ı�ţ�keyΪ���ս����valueΪ���
vector<char> letter;//���е��ս��+'$',�ڹ���Ԥ��������ʱ�򴴽����
int B_point = 0, input_len = 0;//B_pointΪ���봮ָ�룬input_lenΪ���봮����

void DFS(int x)//Ϊ����first��������ȱ���PF���ݹ����
{
	if (used[x])
		return;
	used[x] = 1;
	string& left = PF_vector[x].left;
	set<string>& right = PF_vector[x].right;
	set<string>::iterator it = right.begin();
	for (; it != right.end(); it++)//���ڷ��ս���Ĳ���ʽ�������Ҳ�
	{
		for (int i = 0; i < it->length(); i++)//��������Ҳ���ÿ���ַ�
		{
			if (!isupper(it->at(i)))//������Ǵ�д��ĸ����Ϊ�ս����ֱ�Ӽ��뵽first����
			{
				first[left].insert(it->at(i));
				break;//�Ѿ����ս�����������������������
			}
			else//����Ǵ�д��ĸ����Ϊ���ս��
			{
				int y;
				y = PF_map[it->substr(i, 1)] - 1;
				string& tleft = PF_vector[y].left;
				DFS(y);//�ݹ����
				set<char>& temp = first[tleft];
				set<char>::iterator it_point = temp.begin();
				for (; it_point != temp.end(); it_point++)
				{
					if (*it_point!='~')//�����в�Ϊ�յ�first������
					{
						first[left].insert(*it_point);
					}
				}
				set<char>::iterator it_temp = temp.find('~');//����first������û�п�
				if (it_temp!=temp.end())//������ڿ�
				{
					if (i == it->length() - 1)//���Ҹ÷����ǵ�ǰ����ʽ�����һ�����ţ��ѿռ��뵽����first����
					{
						first[left].insert('~');
					}
					//����÷��Ų��ǵ�ǰ����ʽ�����һ�����ţ�������ݹ����
				}
				else//��������ڿգ�����ֹͣ�ݹ飬ֱ������
					break;
			}
		}
	}
}

void first_construction()//���첢���first��
{
	memset(used, 0, sizeof(used));
	for (int i = 0; i < PF_vector.size(); i++)
		DFS(i);//�������õ�first��
	printf("----------------------FIRST��----------------------\n");
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

void add_follow(const string& str1, const string& str2)//��str1��follow�����뵽str2��follow����
{
	set<char>& from = follow[str1];
	set<char>& to = follow[str2];
	set<char>::iterator it = from.begin();
	for (; it != from.end(); it++)
		to.insert(*it);
}

void follow_construction()//���첢���follow��
{
	follow[PF_vector[0].left].insert('$');//���Ƚ���$�����뵽��ʼ����follow���У�����Ĭ�ϵ�һ������Ĳ���ʽ����Ϊ��ʼ��
	while (true)//һֱѭ����follow����ֱ�����еķ��ս����follow����û�б仯Ϊֹ
	{
		bool not_finished = false;//ʹ��һ��bool���͵ı�����¼follow���Ƿ���
		for (int i = 0; i < PF_vector.size(); i++)//�������еķ��ս��
		{
			string& left = PF_vector[i].left;
			set<string>& right = PF_vector[i].right;
			set<string>::iterator it = right.begin();
			for (; it != right.end(); it++)//�������з��ս���Ĳ���ʽ
			{
				bool flag = true;//�����ж��Ƿ�Ҫ������ʽ��߷��ս����follow�����뵽��ǰ���ҵķ��ս����follow����
				const string& str = *it;
				for (int j = it->length() - 1; j >= 0; j--)//������������ʽ�Ҳ����ҴӺ���ǰ��
				{
					if (isupper(str[j]))//�Ƿ��ս��
					{
						int x = PF_map[it->substr(j, 1)] - 1;
						if (flag)//�ж��Ƿ�Ҫ������ʽ��߷��ս����follow�����뵽��ǰ���ҵķ��ս����follow����
						{
							int num_old = follow[it->substr(j, 1)].size();
							add_follow(left, it->substr(j, 1));
							if (!PF_vector[x].right.count("~"))//�����ǰ�ķ��ս���Ĳ���ʽ�в����գ�˵��������ǰ��ķ��ս���������ܽ�
								flag = false;					//����ʽ��߷��ս����follow�����뵽��ǰ���ҵķ��ս����follow����
							int num_new = follow[it->substr(j, 1)].size();
							if (num_new > num_old)//���follow��û�б仯��˵��follow���Ѿ��������
								not_finished = true;
						}
						for (int k = j + 1; k < it->length(); k++)
						{
							if (isupper(str[k]))//�Ƿ��ս��
							{
								string id;
								id = it->substr(k, 1);
								set<char>& from = first[id];
								set<char>& to = follow[it->substr(j, 1)];

								set<char>::iterator it_temp = from.begin();
								int num_old = follow[it->substr(j, 1)].size();
								for (; it_temp != from.end(); it_temp++)//�Ѵ�����ս���ĺ���ķ��ս����first���в�Ϊ�յ��ս�����������ս����follow����
									if (*it_temp != '~')
										to.insert(*it_temp);
								int num_new = follow[it->substr(j, 1)].size();
								if (num_new > num_old) //���follow��û�б仯��˵��follow���Ѿ��������
									not_finished = true;

								if (!PF_vector[PF_map[id] - 1].right.count("~"))//���������ս���ĺ���ķ��ս����first����û�пգ��Ͳ������󿴣�ֱ������
									break;
							}
							else//���ս��
							{
								int num_old = follow[it->substr(j, 1)].size();
								follow[it->substr(j, 1)].insert(str[k]);//ֱ�ӽ�������ս��������ս�����뵽������ս����follow����
								int num_new = follow[it->substr(j, 1)].size();
								if (num_new > num_old)//���follow��û�б仯��˵��follow���Ѿ��������
									not_finished = true;
								break;//��Ϊ������ս���������ս�������������󿴣�ֱ������
							}
						}
					}
					else//���ս��
						flag = false;//�����;�����ս�����Ͳ����ܽ�����ʽ��߷��ս����follow�����뵽��ǰ���ҵķ��ս����follow����
				}
			}
		}
		if (!not_finished)//������еķ��ս����follow����û�б仯��˵�����е�follow���Ѿ�������ɣ�����whileѭ��
			break;
	}
	//�������õ�follow��
	printf("----------------------FOLLOW��----------------------\n");
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

bool check_first(const string& str, char ch)//���ch�Ƿ�����str��first��
{
	bool hasEmpty = false;
	for (int i = 0; i < str.length(); i++)
	{
		if (!isupper(str[i]))//str��һ���ַ����Ǵ�д��Ҳ�����ս�����
		{
			if (str[i] != ch)
				return false;
			else
				return true;
		}
		else//str��һ���ַ��Ǵ�д��Ҳ���Ƿ��ս��
		{
			string temp;
			temp = str.substr(i, 1);
			set<char>& dic = first[temp];
			set<char>::iterator it = dic.begin();
			for (; it != dic.end(); it++)
			{
				if (*it == '~')//str�ĵ�һ���ַ���first�����п�
					hasEmpty = true;
				else//ch��str[i]�ķǿ�first���Ƚ�
				{
					hasEmpty = false;
					if (*it == ch)//str�ĵ�һ���ַ���first��Ϊ�����ҵ�ch
						return true;//���ҳɹ�������true
				}
			}
			if (!hasEmpty)//���str[i]��first����û�пգ��Ϳ��Բ�����һ���ַ���ֱ������ѭ��
			{
				break;
			}
		}
	}
	if (hasEmpty && ch == '~')//���str�������ַ���first���ﶼ�пգ����Ҵ����ҵ��ַ���Ϊ��~�������ҳɹ�������true
	{
		return true;
	}
	return false;//����ʧ�ܣ�����false
}

bool check_follow(const string& str, char ch)//���ch�Ƿ�����str��follow��
{
	set<char>& dic = follow[str];
	set<char>::iterator it = dic.begin();
	for (; it != dic.end(); it++)//����strҲ���Ƿ��ս��follow�������в���
		if (*it == ch)
			return true;
	return false;
}

void predict_table_construction()//���첢���LL1Ԥ�������
{
	map<char, string> temp;
	bool flag[500];
	memset(flag, 0, sizeof(flag));

	for (int i = 0; i < PF_vector.size(); i++)//�����е��ս�����뵽letter��
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
	for (int i = 0; i < PF_vector.size(); i++)//�������з��ս���Ĳ���ʽ���жϸò���ʽ�Ƿ�Ӧ�ü��뵽�ñ�����
	{
		temp.clear();
		string& left = PF_vector[i].left;
		set<string>& right = PF_vector[i].right;
		set<string>::iterator it = right.begin();
		for (; it != right.end(); it++)//���ڸ÷��ս�������в���ʽ�Ҳ�
		{
			for (int j = 0; j < letter.size(); j++)//�������е��ս��
			{
				if (check_first(*it, letter[j]))
				{
					temp[letter[j]] = *it;
				}
			}
			if (check_first(*it, '~'))//�жϸ��ս���Ƿ��ڸò���ʽ�Ҳ���first����
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
	//�������õ�LL1������
	printf("---------------------Ԥ�������---------------------\n");
	letter.push_back('$');//����$�����뵽letter��
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

void print_A()//�������ջ
{
	string temp="";
	for (int a = 0;a < A.size();a++)
		temp += A[a];
	printf("%-15s|", temp.c_str());
}

void print_B()//���ʣ�മ
{
	string temp = "";
	for (int b = B_point;b < B.size();b++)
		temp += B[b];
	printf("%15s|", temp.c_str());
}

void analyse()//Ԥ��������Ƴ���
{
	int m, n, k = 0, flag = 0;
	char ch, x;
	A.clear();//ÿ�η������Ȱѷ���ջ�ÿ�
	B.clear();//ÿ�η������Ȱ�ʣ�മ�ÿ�
	B_point = 0;//ÿ�η������Ȱ����봮ָ����0
	input_len = 0;//ÿ�η������Ȱ����봮������0
	printf("������Ҫ�������ַ���(��i�������֣��ԡ�$����β):");
	do//���������
	{
		cin >> ch;
		B.push_back(ch);
		input_len++;
	} while (ch != '$');
	ch = B[0];//��ǰ�����ַ�
	A.push_back('$');
	A.push_back(PF_vector[0].left[0]);//'$',��ʼ���ս����ջ
	printf("-------------------------------------------------------\n");
	printf("|%5s|%-15s|%15s|%15s|\n","����","����ջ","ʣ���ַ�","����ʽ");
	printf("-------------------------------------------------------\n");
	do {
		printf("|%5d|", k++);
		print_A();
		print_B();
		x = A[A.size() - 1];//xΪ��ǰջ���ַ�
		A.pop_back();
		if (x == '$' && B[B_point] == '$')//ջ���ķ�����x��Ϊ��$������ָ����������aҲ��Ϊ��$��ʱ�����ܣ��Կα��ϵ��㷨���������ƣ�
		{
			printf("%15s|\n", "Accept!");
			printf("-------------------------------------------------------\n");
			break;
		}
		for (int j = 0;j < letter.size();j++)//�ж��Ƿ�Ϊ�ս���͡�$��
		{
			if (x == letter[j])
			{
				flag = 1;
				break;
			}
		}
		if (flag == 1)//������ս����$��
		{
			if (x == ch)
			{
				string temp_str = string(1, ch) + " ƥ��";
				printf("%15s|\n", temp_str.c_str());
				printf("-------------------------------------------------------\n");
				ch = B[++B_point];//��һ�������ַ�
				flag = 0;
			}
			else//�ս����ƥ�������;�����ˡ�$���ĳ�����
			{
				string temp_str = string(1, ch) + " ����";
				printf("%15s|\n", temp_str.c_str());//��������ս��
				printf("-------------------------------------------------------\n");
				break;
			}
		}
		else//����Ƿ��ս��
		{
			int point;
			for (point = 0; point < PF_vector.size(); point++)//����x�ķ��ս���±�
			{
				if (PF_vector[point].left[0] == x)
				{
					break;
				}
			}
			string cha = predict_table[point][ch];
			if (!cha.empty())//�жϱ����Ƿ�Ϊ��
			{
				string temp_str = PF_vector[point].left + "->";
				for (int i = 0; i < cha.size(); i++)
				{
					temp_str += cha[i];
				}
				printf("%15s|\n-------------------------------------------------------\n", temp_str.c_str());//�������ʽ
				if (cha[0] != '~')//����ʽ�Ҳ�Ϊ���򲻽�ջ
				{
					for (int j = (cha.size() - 1);j >= 0;j--)//����ʽ������ջ
					{
						A.push_back(cha[j]);
					}
				}
			}
			else//����Ϊ�յĳ�����
			{
				string temp_str = string(1, x) + " ����Ϊ��";
				printf("%15s|\n", temp_str.c_str());//���������ս��
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
	printf("��������ķ�����ʽ����Ŀ:");
	scanf("%d", &n);
	printf("���������ʽ(���ķ���������LL1�ķ������������ս����Ϊһ����д��ĸ���ս����Ϊһ��Сд��ĸ�͸��ַ��ţ�����~����epsilon):\n");
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
	printf("�������Ƿ�Ҫ�����ַ���(1�����ǣ�0�����)��");
	cin >> main_flag;
	while (main_flag)
	{
		analyse();
		printf("�������Ƿ�Ҫ��������(1���������0�������)��");
		cin >> main_flag;
	}
	return 0;
}