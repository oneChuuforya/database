#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <string.h>
#include <assert.h>
#include <string>
using namespace std;


typedef struct Data
{
	int data;
	char str[13];
} Data;


const int N = 1000000;      //数据总量
const int FILE_NUM = 16;   //文件个数，归并路数 
const int MAX_PART = 62500; //每一个文件大小

FILE *fpreads[FILE_NUM];
const int MIN = -1;          //最小值
const int MAX = 2147483647;  //最大值

int cmp(const void* a, const void *b)
{
	if ((*(Data*)a).data > (*(Data *)b).data)
		return 1;
	else if ((*(Data*)a).data < (*(Data *)b).data)
		return -1;
	else
		return 0;
}

/*
读取数据
每项数据格式
int（4byte) char（12）+"\0"
*/

int readData(FILE *fp, Data *array, int N)
{
	int length = 0;
	for (int i = 0; i < MAX_PART && (EOF != fscanf(fp, "%d %s\n", &array[i].data, &array[i].str)); i++)
	{
		length++;
	}
	return length;
}


FILE* openFile(int count, char *mode)
{
	FILE *fpwrite = NULL;
	char filename[20];
	memset(filename, 0, 20);
	sprintf_s(filename, 20, "data%d.txt", count);
	fopen_s(&fpwrite, filename, mode);
	assert(fpwrite != NULL);
	return fpwrite;
}

/*
向临时文件写入已排序数据
*/
void writeData(Data *array, int N, int count)
{
	FILE *fpwrite = openFile(count, "w");
	int i = 0;
	for (i = 0; i < N; i++)
	{
		fprintf(fpwrite, "%d%s\n", array[i].data, array[i].str);
	}
	fprintf(fpwrite, "%d%s\n", MAX, "这是结束标志");  //在每个文件最后写入一个最大值，表示文件结束
	fclose(fpwrite);
}

/*
利用快排对每个文件进行排序
T:O(n * lgn)
S:O(lgn)
*/
void internalSort(void)
{
	clock_t begin = clock();
	FILE *fpread = NULL;
	fopen_s(&fpread, "data.txt", "r");
	assert(fpread != NULL);

	int count = 0;
	Data *array = new Data[MAX_PART];
	assert(array != NULL);
	while (1)
	{
		memset(array, 0, sizeof(Data)* MAX_PART);
		int length = readData(fpread, array, MAX_PART);
		if (length == 0)
		{
			break;
		}
		qsort(array, length, sizeof(Data), cmp);
		writeData(array, length, count);
		count++;
	}
	delete[] array;
	fclose(fpread);
	clock_t end = clock();
	cout << "Internal Sort time: " << (end - begin) / CLK_TCK << "s" << endl;
}

/*
败者树调整
*/
void adjust(int ls[], Data data[], int s)
{
	int t = (s + FILE_NUM) / 2;
	while (t)
	{
		if (data[s].data > data[ls[t]].data)
		{
			int temp = s;
			s = ls[t];
			ls[t] = temp;
		}
		t /= 2;
	}
	ls[0] = s;
}

/*
建立败者树
*/
void buildLoserTree(int ls[], Data data[])
{
	data[FILE_NUM].data = MIN;
	for (int i = 0; i < FILE_NUM; i++)
	{
		ls[i] = FILE_NUM;
	}
	for (int i = FILE_NUM - 1; i >= 0; i--)
	{
		adjust(ls, data, i);
	}
}

void MergeSortMain()
{
	clock_t begin = clock();
	FILE *fpreads[FILE_NUM];      //10个文件的描述符
	Data data[FILE_NUM + 1];       //10个文件的10个当前最小数据
	int ls[FILE_NUM];             //存放败者索引的节点
	int index;
	FILE *fpwrite = NULL;
	fopen_s(&fpwrite, "data-sorted.txt", "w");
	assert(fpwrite != NULL);

	for (int i = 0; i < FILE_NUM; i++)
	{
		fpreads[i] = openFile(i, "r");
	}
	for (int i = 0; i < FILE_NUM; i++)
	{
		fscanf(fpreads[i], "%d %s\n", &data[i].data, &data[i].str);
	}

	buildLoserTree(ls, data); //创建败者树

	while (data[ls[0]].data != MAX)
	{
		index = ls[0];
		fprintf(fpwrite, "%d%s\n", data[index].data, data[index].str);
		fscanf(fpreads[index], "%d  %s\n", &data[index].data, &data[index].str);
		adjust(ls, data, index);
	}
	for (int i = 0; i < FILE_NUM; i++)
	{
		fclose(fpreads[i]);
	}
	fclose(fpwrite);
	clock_t end = clock();
	cout << "Merge time:" << (end - begin) / CLK_TCK << "s" << endl;
}


int main()
{
	internalSort();
	MergeSortMain();
	cout << "Finished!" << endl;
	getchar();
	return 0;
}

