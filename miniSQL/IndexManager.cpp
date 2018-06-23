#include "stdafx.h"
#include "utils.h"
#include "BPTree.h"
#include "IndexManager.h"

const char *IndexManager::getTreeFilename(const char *indexName) {
	return (string(indexName) + ".index").c_str();
}

int IndexManager::Find(const char *indexName, const char *key)
{
	BPTree *tree = new BPTree(getTreeFilename(indexName));
	int ret = tree->find(key);
	delete tree;
	return ret;
}

bool IndexManager::Insert(const char *indexName, const char *key, int value)
{
	BPTree *tree = new BPTree(getTreeFilename(indexName));
	if (!tree->add(key, value))
	{
		delete tree;
		cerr << "Insert key error: Duplicated key in index `" << indexName << "`." << endl;
		return false;
	}
	delete tree;
	return true;
}

bool IndexManager::Remove(const char *indexName, const char *key)
{
	BPTree *tree = new BPTree(getTreeFilename(indexName));
	if (!tree->remove(key))
	{
		delete tree;
		cerr << "Remove key error: Cannot find key in index `" << indexName << "`." << endl;
		return false;
	}
	delete tree;
	return true;
}

bool IndexManager::CreateIndex(const char *indexName)
{
	CatalogManager *catalogManager = Utils::GetCatalogManager();
	Index *index = catalogManager->get_index(indexName);
	if (index == NULL)
		return false;
	int keyLength = Utils::GetTypeSize(index->attr->type);
	return BPTree::CreateFile(getTreeFilename(indexName), keyLength);
}

bool IndexManager::DropIndex(const char * indexName)
{
	BufferManager *bufferManager = Utils::GetBufferManager();
	bufferManager->DeleteFile(getTreeFilename(indexName));
	return true;
}