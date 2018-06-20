#include "stdafx.h"
#include "RecordManager.h"
#include <iostream>
#include <cstring>

/**
 * generate a special filename for recordfile
 * @param tableName
 * @return record file name
 */
string RecordManager::record_filename(string tableName)
{
	string filename = "";
	return "record_" + tableName;
}

/**
 * create a file to store the record
 * @param tablenName: the name of the record
 * @return a bool variable
 */
bool RecordManager::create_table(string tableName)
{
	string recordFileName = record_filename(tableName);
	/*if(bm.CreateFile(recordFileName))
		return true;
	else 
		return false;*/
	bm.CreateFile(recordFileName);
}

/**
 * delete a file of record
 * @param tablenName: the name of the record
 * @return a bool variable
 */
bool RecordManager::delete_table(string tableName)
{
	string recordFileName = record_filename(tableName);
	/*if(bm.DeleteFile(recordFileName))
		return true;
	else
		return false;*/
	bm.DeleteFile(recordFileName);
}

/** 
 * calculate the left space os current block
 * @param current_block: the name of the block
 * @return the bytes that not used
 */
int RecordManager::left_blockspace(Block current_block)
{
	return BLOCK_SIZE-current_block.byte_used;
}

/**
 * insert a record to table
 * @param tableName: the name of the table
 * @param record: the value of record inserted
 * @param recordSize: the length of this record
 * @return bool: successed with true, else false
 */
bool RecordManager::insert_record(string tableName, char * record, int recordSize)
{
	string recordFileName = record_filename(tableName);
	int offset = 0;
	Block current_block = bm.FetchBlock(recordFileName, offset);
	while(true)
	{
		if(current_block.byte_used == 0)
			return false;
		if(left_blockspace(current_block) >= recordSize)
		{
			bm.AppendRecord(recordFileName, offset, (Byte*)record, recordSize);
			bm.WriteBlock(current_block);//write to block after insert record
			return true;
		}
		else
		{
			offset += BLOCK_SIZE; //change offset to find next block
			current_block = bm.FetchBlock(recordFileName, offset);
		}
	}
	return false;
}

/** 
 * get the pointer of a record
 * @param current_block: the block operated now
 * @param offset: the offset of record
 * @return the point of a record
 */
char * RecordManager::get_recordpoint(Block current_block, int offset)
{
	return ((char*)(&current_block.content[0] + offset));
}

/** 
 * show all the records according to the requirements
 * @param tableName: name of table that need to show
 * @param attributeNameVector: the attribute need to show
 * @param conditionVector: the requirments that record need meet
 */
int RecordManager::show_allrecord(string tableName, vector<string> * attributeNameVector, vector<Condition> * conditionVector)
{
	string recordFileName = record_filename(tableName);
	int offset = 0;
	int recordSize = api->get_recordSize(tableName);
	Block current_block = bm.FetchBlock(recordFileName, offset);

	while(true)
	{	
		//if the block is empty, just return
		if(current_block.byte_used == 0)
			return -1;
		//the block is the last block, return after showing
		if(left_blockspace(current_block) >= recordSize)
		{
			show_blockrecord(tableName,current_block, attributeNameVector, conditionVector, recordSize);
			return 1;
		}
		else
		{
			//show current block and find next block
			show_blockrecord(tableName, current_block, attributeNameVector, conditionVector, recordSize);
			offset += BLOCK_SIZE;
			current_block = bm.FetchBlock(recordFileName, offset);
		}
	}
	return -1;
}

/** 
 * show records meet the requirements in a block
 * @param current_blcok: the block we operate now
 * @param attributeNameVector: the attribute need to show
 * @param condition: the requirements records need to satisfy
 * @param recordSize: the length of the record
 */
int RecordManager::show_blockrecord(string tableName, Block current_block, vector<string> * attributeNameVector, vector<Condition> * conditionVector, int recordSize)
{
	//if the block is empty, do nothing
	if(current_block.byte_used == 0)
		return -1;
	int offset = 0;
	vector<Attribute> attributeVector;
	api->attributeGet(tableName, &attributeVector);
	char * recordpoint = get_recordpoint(current_block, offset);
	int usingByte = BLOCK_SIZE - left_blockspace(current_block);

	//just check all the records to judge if it satisfies the requirements
	while(offset + recordSize <= usingByte)
	{
		if(record_fitcondition(recordpoint, recordSize, &attributeVector, conditionVector))
		{
			print_record(recordpoint, recordSize, &attributeVector, attributeNameVector);
			cout << endl;
		}
		offset += recordSize;
		recordpoint = get_recordpoint(current_block, offset);
	}
	return 0;
}

/** 
 * judge if the record satisfies the requirements
 * @param recordpoint: the pointer of this record need to check
 * @param recordSize: the length of record
 * @param attributeVector: a set of the attribute of the table
 * @param conditionVector: a set of the requirments the record need to meet
 * @return if satisfy return true, else return false
 */
bool RecordManager::record_fitcondition(char * recordpoint, int recordSize, vector<Attribute> * attributeVector, vector<Condition> * conditionVector)
{
	//if there is no requirement, we need do nothing
	if(conditionVector == NULL)
		return true;
	int type;
	string attributeName;
	int typeSize;
	char content[255];
	char *contentBegin = recordpoint;

	for(int i = 0; i < (*attributeVector).size(); i++)
	{
		type = (*attributeVector)[i].type;
		attributeName = (*attributeVector)[i].name;
		typeSize = api->typeSizeGet(type);

		//just remain the content as string.
		memset(content, 0, 255);
		memcpy(content, contentBegin, typeSize);
		for(int j = 0; j < (*conditionVector).size(); j++)
		{
			if((*conditionVector)[j].attributeName == attributeName)
			{
				if(!content_fitcondition(content, type, &(*conditionVector)[j]))
					return false;
			}
		}

		contentBegin += typeSize;
	}
	return true;
}

/**
 * judge if the content of a record meet requiements
 * @param content: the point of the content of a record
 * @param type: the type of attribute
 * @param condition: a pointer to the requirement struct
 * @return if satisfy return true, else return false
 */
bool RecordManager::content_fitcondition(char * content, int type, Condition * condition)
{
	return condition->isright(content, type);
}

/** 
 * print a record that meets the requirements
 * @param recordpoint: the point to a record
 * @param recordSize: the length of record
 * @param attributeVector: the set of the attributes of the table
 * @param attributeNameVector: a set of attributes need to show
 */
void RecordManager::print_record(char * recordpoint, int recordSize, vector<Attribute> * attributeVector, vector<string> * attributeNameVector)
{
	int type;
	int typeSize;
	char content[255];
	string attributeName;

	char * contentBegin = recordpoint;
	for(int i = 0; i < (*attributeVector).size(); i++)
	{
		type = (*attributeVector)[i].type;
		typeSize = api->typeSizeGet(type);

		memset(content, 0, 255);
		memcpy(content, contentBegin, typeSize);

		//just check if the attribute need to show
		for(int j = 0; j < (*attributeNameVector).size(); j++)
		{
			if((*attributeNameVector)[j] == (*attributeVector)[i].name)
			{
				// judge the type of attribtue and show
				if(type == INT)
				{
					int temp = *((int *) content);
					printf("%d ", temp);
				}
				else if(type == REAL)
				{
					float temp = *((float *) content);
					printf("%f ", temp);
				}
				else if(type == CHAR)
				{
					string temp = content;
					printf("%s ", temp.c_str());
				}
			}
		}
		contentBegin += typeSize;
	}
}

/**
 * delete all records of the table that satisfy the requirements
 * @param tableName: the name of the table
 * @param conditionVector: a set of requirements that records need to satisfy
 * @return if delete successfully, return true, else return false
 */
bool RecordManager::delete_allrecord(string tableName, vector<Condition> * conditionVector)
{
	string recordFileName = record_filename(tableName);
	int offset = 0;
	Block current_block = bm.FetchBlock(recordFileName, offset);
	int recordSize = api->get_recordSize(tableName);

	while(true)
	{
		//if the block is empty, just return
		if(current_block.byte_used == 0)
			return false;
		//to find the last block and delete records meet requiements
		if(left_blockspace(current_block) >= recordSize)
		{
			delete_blockrecord(current_block, conditionVector, tableName);
			bm.WriteBlock(current_block);
			return true;
		}
		else
		{
			//delete records in current block and get next block
			delete_blockrecord(current_block, conditionVector, tableName);
			bm.WriteBlock(current_block);
			offset += BLOCK_SIZE;
			current_block = bm.FetchBlock(recordFileName, offset);
		}
	}
}

/**
 * delete records meet requirements in block
 * @param current_block: the block we operate now
 * @param conditionVextor: a set of requirements
 * @return if success return true else return false
 */
bool RecordManager::delete_blockrecord(Block current_block, vector<Condition> * conditionVector, string tableName)
{
	if(current_block.byte_used == 0)
		return false;
	vector<Attribute> attributeVector;
	int recordSize = api->get_recordSize(tableName);
	int offset = 0;
	char * recordpoint = get_recordpoint(current_block, offset);

	api->attributeGet(tableName, &attributeVector);
	while(offset + recordSize <= current_block.byte_used)
	{
		if(record_fitcondition(recordpoint, recordSize, &attributeVector, conditionVector))
		{
			//delete index of this record
			int i = 0;
			for(i = 0; i + recordSize + offset < current_block.byte_used; i++)
			{
				recordpoint[i] = recordpoint[i + recordSize];
			}
			memset(recordpoint + i, 0, recordSize);
			current_block.byte_used -= recordSize;
		}
		else
		{
			offset += recordSize;
			recordpoint = get_recordpoint(current_block, offset);
		}
	}
	return true;
}