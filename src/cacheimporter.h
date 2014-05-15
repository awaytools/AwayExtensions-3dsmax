#pragma once

#include <iskin.h>
#include "Max.h"
#include "awd.h"
#include "maxawdimporterblocks.h"

/**
 * ImporterBlockCache
 * Cache that is used generically to store any type of block or
 * sub-element in a look-up table by MAX object.
*/
typedef struct ImporterBlockCacheItem_struct {
	ImportedAWDBlock * key;
	void *val;
	ImporterBlockCacheItem_struct *next;
} ImporterBlockCacheItem;

class ImporterBlockCache
{
private:
	ImporterBlockCacheItem *firstItem;
	ImporterBlockCacheItem *lastItem;
	int numItems;

public:
	ImporterBlockCache(void);
	~ImporterBlockCache(void);

	int get_num_items();
	void Set(ImportedAWDBlock *key, void *val);
	void *Get(ImportedAWDBlock *key);
	void *GetKeyByVal(void *val);
	void *GetValByIdxAndRequestedType(int, AWD_block_type);
	void *GetKeyByIdxAndRequestedType(int, AWD_block_type);
};
