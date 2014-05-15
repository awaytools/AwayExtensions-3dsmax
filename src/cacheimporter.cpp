#include "cacheimporter.h"
#include "utils.h"
#include <stdlib.h>

ImporterBlockCache::ImporterBlockCache(void)
{
	firstItem = NULL;
	lastItem = NULL;
	numItems=0;
}

ImporterBlockCache::~ImporterBlockCache(void)
{
	ImporterBlockCacheItem *cur = firstItem;
	while (cur) {
		ImporterBlockCacheItem *next = cur->next;
		free(cur);
		cur = next;
	}

	firstItem = lastItem = NULL;
}

int ImporterBlockCache::get_num_items()
{
	return numItems;
}
void ImporterBlockCache::Set(ImportedAWDBlock *key, void *val)
{
	ImporterBlockCacheItem *item;

	item = (ImporterBlockCacheItem *)malloc(sizeof(ImporterBlockCacheItem));
	item->key = key;
	item->val = val;
	item->next = NULL;

	if (!firstItem) {
		firstItem = item;
	}
	else {
		lastItem->next = item;
	}
	numItems++;
	lastItem = item;
}

void *ImporterBlockCache::Get(ImportedAWDBlock *key)
{
	ImporterBlockCacheItem *cur;

	cur = firstItem;
	while (cur) {
		if (cur->key == key)
			return cur->val;

		cur = cur->next;
	}

	return NULL;
}
void *ImporterBlockCache::GetKeyByVal(void *val)
{
	ImporterBlockCacheItem *cur;

	cur = firstItem;
	while (cur) {
		if (cur->val == val)
			return cur->key;

		cur = cur->next;
	}

	return NULL;
}
void *ImporterBlockCache::GetKeyByIdxAndRequestedType(int idx, AWD_block_type blockType)
{
	if (numItems<idx)
		return NULL;
	ImporterBlockCacheItem *cur;

	cur = firstItem;
	int cntIdx=0;
	while (cur) {
		if (cntIdx == idx){
			ImportedAWDBlock * thisBlock=cur->key;
			if (thisBlock->get_isValid()){
				if (thisBlock->get_type() == blockType)
					return cur->key;
				else
					return NULL;// block is not of correct type
			}
			else
				return NULL;// block is not Valid
		}
		cntIdx++;
		cur = cur->next;
	}
	return NULL;
}
void *ImporterBlockCache::GetValByIdxAndRequestedType(int idx, AWD_block_type blockType)
{
	if (numItems<idx)
		return NULL;

	ImporterBlockCacheItem *cur;

	cur = firstItem;
	int cntIdx=0;
	while (cur) {
		if (cntIdx == idx){
			ImportedAWDBlock * thisBlock=cur->key;
			if (thisBlock->get_isValid()){
				if (thisBlock->get_type() == blockType)
					return cur->val;
				else
					return NULL;// block is not of correct type
			}
			else
				return NULL;// block is not Valid
		}
		cntIdx++;
		cur = cur->next;
	}
	return NULL;
}