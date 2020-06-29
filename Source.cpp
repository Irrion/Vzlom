#include <iostream>
#include <fstream>
#include <thread>
#include <functional>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>


using namespace std;

#define MAX_THREADS std::thread::hardware_concurrency() - 1;

condition_variable triger;
mutex mtx;
vector<int> key;
vector<int> possible_key_size;
vector<bool> possible_key_size_ch;
vector<string> arr_alp;
vector<string> encr;
bool f = true;
void swap(int* a, int i, int j)
{

	int s = a[i];
	a[i] = a[j];
	a[j] = s;
}
bool NextSet(int * a, int n)
{
	int j = n - 2;
	while (j != -1 && a[j] >= a[j + 1]) j--;
	if (j == -1)
	{
		return false;
	}
	int k = n - 1;
	while (a[j] >= a[k]) 
	{ 
		k--; 
	}
	swap(a, j, k);
	int l = j + 1, r = n - 1;
	while (l < r)
	{
		swap(a, l++, r--);
	}
	return true;
}


bool decrypt(int *a, int size)
{
	vector<string>deciph;
	string block;
	int count = 0;
	for (int i = 0; i < encr.size(); i++)
	{
		string deciph_str;
		for (int j = 0; j < encr[i].size(); j++)
		{
			block.push_back(encr[i][j]);
			if (size == count + 1)
			{
				
				for (int k = 0; k < size; k++)
				{
					for (int z = 0; z < size; z++)
					{
						if (k + 1 == a[z])
						{
							deciph_str.push_back(block[z]);
						}
					}
				}
				block.clear();
				count = -1;
			}
			count++;
		}
		deciph.push_back(deciph_str);
	}

	

	

	int corect = 0;
	int uncorect = 0;
	for (int i = 0; i < deciph.size(); i++) 
	{
		bool tr = true;
		string word;
		for (int j = 0; j < deciph[i].size(); j++) 
		{
			if (deciph[i][j] == ' ' || deciph[i][j] == '#') 
			{
				if (tr)
				{

				}
				else 
				{
					bool word_right = false;
					for (int k = 0; k < arr_alp.size(); k++) 
					{
						if (word == arr_alp[k]) 
						{
							word_right = true;
						}
					}
					if (word_right)
					{
						corect++;
					}
					else 
					{
						uncorect++;
					}

					word.clear();
					tr = true;
				}
			}
			else 
			{
				if (tr)
				{
					tr = false;
				}
				word.push_back(deciph[i][j]);
			}
		}
	}

	if (corect > uncorect && uncorect == 0) {
		return true;
	}
	return false;

		
	
}

void transp(int size)
{
	
	int *arr = new int[size];
	bool tr = false;
	for (int i = 0; i < size; i++)
	{
		arr[i] = i + 1;
	}
	while (NextSet(arr, size))
	{
		if (decrypt(arr, size))
		{
			tr = true;
			break;
		}
	}
	if (tr) 
	{
		if (f) 
		{
			mtx.lock();
			for (int i = 0; i < size; i++) 
			{
				key.push_back(arr[i]);
			}
			f = false;
			mtx.unlock();
			triger.notify_one();
		}
	}

	/*if (tr)
	{
		for (int i = 0; i < size; i++)
		{
			key.push_back(arr[i]);
		}
	}*/
}



class ThreadPool
{
private:
	vector<thread>pool;
	mutex block;

	void start()
	{
		bool ex = true;
		while (ex) 
		{
			int el = -1;
			for (int i = 0; i < possible_key_size_ch.size(); i++) 
			{
				if (possible_key_size_ch[i] == 0) 
				{
					el = i;
					block.lock();
					possible_key_size_ch[i] = 1;
					block.unlock();
					break;
				}
			}
			if (el == -1) 
			{
				ex = false;
			}
			else 
			{
				transp(possible_key_size[el]);
			}
		}
	}

public:

	void push() 
	{
		int tmp = MAX_THREADS;
		for (int i = 0; i < tmp; i++) 
		{
			pool.push_back(thread(&ThreadPool::start, this));
			pool.back().detach();
		}
	}

};



int main()
{
	setlocale(LC_CTYPE, "Russian");
	string path_alph;
	string path_ciph;
	cout << "Введите путь к файлу с алфавитом: " << path_alph;
	cin >> path_alph;
	cout << "Введите путь к файлу с шифром: " << path_ciph;
	cin >> path_ciph;
	ifstream file_al(path_alph);
	string word;
	if (file_al.is_open())
	{
		while (!file_al.eof())
		{
			getline(file_al, word);
			if (word.size() == 0)
			{

			}
			else
			{
				arr_alp.push_back(word);
			}
		}
	}
	else
	{
		cout << "Файл не найден" << endl;
		system("pause");
		return 0;
	}
	file_al.close();

	ifstream file_ciph(path_ciph);
	string text;
	if (file_ciph.is_open())
	{
		while (!file_ciph.eof())
		{
			getline(file_ciph, text);
			if (word.size() == 0)
			{ 

			}
			else
			{
				encr.push_back(text);
			}
		}

		

		for (int i = 1; i < encr[0].size() + 1; i++) {
			if (i == 1) {
			}
			else {
				int r = encr[0].size() % i;
				if (r == 0) {
					possible_key_size.push_back(i);
					possible_key_size_ch.push_back(0);
				}
			}
		}
	}
	else
	{
		cout << "Файл не найден" << endl;
		system("pause");
		return 0;
	}
	file_ciph.close();
	
	//for (int i = 0; i < possible_key_size.size(); i++)
	//{
	//	transp(possible_key_size[i]);
	//	if (!key.empty())
	//	{
	//		break;
	//	}
	//}

	ThreadPool pool;
	pool.push();
	unique_lock<mutex> lock(mtx);
	triger.wait(lock);

	

	string path_deciph;
	vector<int>right_key = key;
	cout << "Введите путь к файлу, чтобы сохранить расшифрованный текст: ";
	cin >> path_deciph;
	ofstream file_de(path_deciph);
	string key_str;
	for (int i = 0; i < right_key.size(); i++) {
		key_str.push_back(to_string(right_key[i])[0]);
	}
	file_de << "Ключ: " << key_str << endl;

	string block;
	int count = 0;
	for (int i = 0; i < encr.size(); i++)
	{
		string deciph_str;
		for (int j = 0; j < encr[i].size(); j++)
		{
			block.push_back(encr[i][j]);
			if (right_key.size() == count + 1)
			{

				for (int k = 0; k < right_key.size(); k++)
				{
					for (int z = 0; z < right_key.size(); z++)
					{
						if (k + 1 == right_key[z])
						{
							deciph_str.push_back(block[z]);
						}
					}
				}
				block.clear();
				count = -1;
			}
			count++;
		}

		file_de << deciph_str << endl;
	}
	file_de.close();
	cout << "Файл расшифрован" << endl;

	
	system("pause");
	return 0;
}



