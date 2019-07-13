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


const int N = 1000000;      //��������
const int FILE_NUM = 16;   //�ļ��������鲢·�� 
const int MAX_PART = 62500; //ÿһ���ļ���С

FILE *fpreads[FILE_NUM];
const int MIN = -1;          //��Сֵ
const int MAX = 2147483647;  //���ֵ

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
��ȡ����
ÿ�����ݸ�ʽ
int��4byte) char��12��+"\0"
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
����ʱ�ļ�д������������
*/
void writeData(Data *array, int N, int count)
{
	FILE *fpwrite = openFile(count, "w");
	int i = 0;
	for (i = 0; i < N; i++)
	{
		fprintf(fpwrite, "%d%s\n", array[i].data, array[i].str);
	}
	fprintf(fpwrite, "%d%s\n", MAX, "���ǽ�����־");  //��ÿ���ļ����д��һ�����ֵ����ʾ�ļ�����
	fclose(fpwrite);
}

/*
���ÿ��Ŷ�ÿ���ļ���������
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
����������
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
����������
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
	FILE *fpreads[FILE_NUM];      //10���ļ���������
	Data data[FILE_NUM + 1];       //10���ļ���10����ǰ��С����
	int ls[FILE_NUM];             //��Ű��������Ľڵ�
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

	buildLoserTree(ls, data); //����������

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

