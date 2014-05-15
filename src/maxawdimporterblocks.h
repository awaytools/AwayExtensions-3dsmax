#ifndef _MAXAWD_IMPORTEDAWDBLOCK_H
#define _MAXAWD_IMPORTEDAWDBLOCK_H
#include <stdlib.h>
#include "awd_types.h"

class ImportedAWDBlock
{
    private:
        awd_baddr addr;
		bool isValid;
        AWD_block_type type;

    public:
        ImportedAWDBlock(AWD_block_type, awd_baddr);

        awd_baddr get_addr();
        AWD_block_type get_type();

        bool get_isValid();
        void set_isValid(bool);
};

typedef struct _list_imported_block
{
    ImportedAWDBlock *block;
    struct _list_imported_block *next;
} list_imported_block;

class ImportedAWDBlockList
{
    private:
        int num_blocks;

    public:
        list_imported_block *first_block;
        list_imported_block *last_block;

        ImportedAWDBlockList();
        ~ImportedAWDBlockList();

        bool append(ImportedAWDBlock *);
        void force_append(ImportedAWDBlock *);
        bool contains(ImportedAWDBlock *);
        bool replace(ImportedAWDBlock *, ImportedAWDBlock *oldBlock);

		ImportedAWDBlock* getByIndex(int);

        int get_num_blocks();
};

class ImportedAWDBlockIterator
{
    private:
        ImportedAWDBlockList * list;
        list_imported_block * cur_block;

    public:
        ImportedAWDBlockIterator(ImportedAWDBlockList *);
        ImportedAWDBlock * next();
        void reset();
};
#endif
