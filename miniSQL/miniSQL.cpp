#include "stdafx.h"
#include "BufferManager.h"

#pragma warning(disable : 4996)

int main()
{
	BufferManager bm;
	Block tp1("123"), tp2("456");
	strcpy(tp1.file_name, "1.txt");
	strcpy(tp2.file_name, "1.txt");
	tp1.tag = 0;
	tp2.tag = 4096;
	bm.CreateFile("1.txt");
	bm.WriteBlock(tp1);
	bm.WriteBlock(tp2);
	/*Block temp = bm.FetchBlock("1.txt", 0);
	temp = bm.FetchBlock("1.txt", 4097);
	Byte content[4];
	strcpy((char*)content, "789");
	bm.AppendRecord("1.txt", 0, content, 3);
	Block temp = bm.FetchBlock("1.txt", 0);*/
}

