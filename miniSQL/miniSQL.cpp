// miniSQL.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "BufferManager.h"

int main()
{
	BufferManager bm;
	Block test[100];
	for (int i = 0;i<50; i++)
		test[i] = bm.FetchBlock("D:/test.txt", i*BLOCK_SIZE);
	bm.WriteBlock("D:/test.txt", 0, test[14].content);
	system("pause");
    return 0;
}

