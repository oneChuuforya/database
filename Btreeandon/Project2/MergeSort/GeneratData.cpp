#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <string>
using namespace std;

const int N = 1000000;

string RandomChineseCharacters()
{
	//srand( (unsigned)time(NULL));
	int high = 0xd7 - 0xc1;//16-55Çøºº×Ö
	int low = 0xfe - 0xa1;
	int high_zn;
	int low_zn;
	char name[3];
	name[2] = '\0';
	string s;
	for (int i = 0; i < 6; i++)
	{
		high_zn = rand() % high + 0xc1;
		low_zn = rand() % low + 0xa1;
		name[0] = high_zn;
		name[1] = low_zn;
		s.append(name);
	}

	return s;
}

int main()
{
	ofstream ofp("data.txt");
	if (!ofp.is_open())
	{
		cout << "can't open file!" << endl;
		return 0;
	}
	srand((unsigned)time(NULL));
	int t, t1, t2;
	string name;
	int space[N+1];

	for(int i=1; i <= N; i++)
	{
		space[i]=i;
	}

	for (int i = 1; i <= 10*N; i++)
	{	int index1,index2;
		t1 = rand();
        t2 = rand();
        index1 = t1%1000 * 1000 + t2;
        index2 = t2%1000 * 1000 + t1;
		if (index1 > 1000000 || index2 > 1000000)
			continue;

        int temp;
        temp = space[index1];
        space[index1] = space[index2];
        space[index2] = temp;

	}
	for(int i = 1; i<=N; i++){
		t = space[i];
		name = RandomChineseCharacters();
		ofp << t << name <<endl;
	}

	return 0;

}
