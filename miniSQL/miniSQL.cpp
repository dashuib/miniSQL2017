#include "stdafx.h"
#include "BufferManager.h"

#pragma warning(disable : 4996)

int main()
{
	BufferManager bm;
	CatalogManager cm(bm);
	/*Attribute bno(-4, "bno", true, true), name(-4, "name", false, false), stock(0, "stock", false, false);
	vector<Attribute> book_attrs;
	book_attrs.push_back(bno);
	book_attrs.push_back(name);
	book_attrs.push_back(stock);
	cm.create_table("book", &book_attrs);
	Attribute cno(-4, "cno", true, true), cname(-4, "name", true, false);
	vector<Attribute> card_attrs;
	card_attrs.push_back(cno);
	card_attrs.push_back(cname);
	cm.create_table("card", &card_attrs);
	cm.create_index("cname_idx", "card", "name");*/
	
	vector<Attribute>* a1 = cm.attr("card");
	vector<Index>* i1 = cm.idx("card");
	/*cm.drop_index("cname_idx");
	cm.drop_table("book");
	cm.drop_table("card");*/
	int p = bm.FileSize("card.tlog");
	return 0;
}

