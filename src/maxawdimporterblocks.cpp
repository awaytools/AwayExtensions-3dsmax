#include "maxawdimporter.h"
#include "awd_types.h"
#include "maxawdimporterblocks.h"
#include "util.h"
#include "platform.h"

ImportedAWDBlock::ImportedAWDBlock(AWD_block_type type, awd_baddr addr)
{
    this->type = type;
	this->addr = addr;
	this->isValid =true; //will be set to false, if the block wasnt read correctly
	/*

    // TODO: Allow setting flags
	this->isExported =false; //true if block was exported by the export process (for all files)
	this->isExportedToFile =false; //true if block was exported for one file (gets reset befor exporting one file)
	*/
}

AWD_block_type ImportedAWDBlock::get_type(){
	return this->type;
}
awd_baddr ImportedAWDBlock::get_addr(){
	return this->addr;
}
bool ImportedAWDBlock::get_isValid(){
	return this->isValid;
}
void ImportedAWDBlock::set_isValid(bool isValid){
	this->isValid=isValid;
}

ImportedAWDBlockList::ImportedAWDBlockList()
{
    this->first_block = NULL;
    this->last_block = NULL;
    this->num_blocks = 0;
}

ImportedAWDBlockList::~ImportedAWDBlockList()
{
    list_imported_block *cur;

    cur = this->first_block;
    while(cur) {
        list_imported_block *next = cur->next;
        cur->next = NULL;
        delete cur->block;
		delete cur;
        cur = next;
    }

    // Already deleted as part
    // of above loop
    this->first_block = NULL;
    this->last_block = NULL;
}

bool
ImportedAWDBlockList::append(ImportedAWDBlock *block)
{
    if (!this->contains(block)) {
        list_imported_block *ctr = (list_imported_block *)malloc(sizeof(list_imported_block));
        ctr->block = block;
        if (this->first_block == NULL) {
            this->first_block = ctr;
        }
        else {
            this->last_block->next = ctr;
        }

        this->last_block = ctr;
        this->last_block->next = NULL;
        this->num_blocks++;

        return true;
    }
    else {
        return false;
    }
}
bool
ImportedAWDBlockList::replace(ImportedAWDBlock *block, ImportedAWDBlock *oldBlock)
{
    list_imported_block *cur;

    cur = this->first_block;
    while (cur) {
        if (cur->block == block){
			cur->block=oldBlock;
            return true;
		}

        cur = cur->next;
    }

    return false;
}

void
ImportedAWDBlockList::force_append(ImportedAWDBlock *block)
{
    list_imported_block *ctr = (list_imported_block *)malloc(sizeof(list_imported_block));
    ctr->block = block;
    if (this->first_block == NULL) {
        this->first_block = ctr;
    }
    else {
        this->last_block->next = ctr;
    }

    this->last_block = ctr;
    this->last_block->next = NULL;
    this->num_blocks++;
}

ImportedAWDBlock *
ImportedAWDBlockList::getByIndex(int idx)
{
	if (idx>=this->num_blocks)
		return NULL;
    list_imported_block *cur;
    cur = this->first_block;
	int i;
	for (i=0;i<idx;i++){
        cur = cur->next;
	}
	return cur->block;
}

bool
ImportedAWDBlockList::contains(ImportedAWDBlock *block)
{
    list_imported_block *cur;

    cur = this->first_block;
    while (cur) {
        if (cur->block == block)
            return true;

        cur = cur->next;
    }

    return false;
}

int
ImportedAWDBlockList::get_num_blocks()
{
    return this->num_blocks;
}

ImportedAWDBlockIterator::ImportedAWDBlockIterator(ImportedAWDBlockList *list)
{
    this->list = list;
    this->cur_block = NULL;
    this->reset();
}

void
ImportedAWDBlockIterator::reset()
{
    if (this->list != NULL)
        this->cur_block = this->list->first_block;
}

ImportedAWDBlock *
ImportedAWDBlockIterator::next()
{
    list_imported_block *tmp;

    tmp = this->cur_block;
    if (this->cur_block != NULL)
        this->cur_block = this->cur_block->next;

    if (tmp)
        return tmp->block;
    else return NULL;
}