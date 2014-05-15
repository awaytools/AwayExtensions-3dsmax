#include "cache.h"
#include "utils.h"
#include <stdlib.h>

BoolCache::BoolCache()
{
    firstItem = NULL;
    lastItem = NULL;
}
BoolCache::~BoolCache()
{
    BoolCacheItem *cur = firstItem;
    while (cur) {
        BoolCacheItem *next = cur->next;
        free(cur);
        cur = next;
    }
    firstItem = lastItem = NULL;
}
void BoolCache::Set(void *key, bool val)
{
    BoolCacheItem *item;

    item = (BoolCacheItem *)malloc(sizeof(BoolCacheItem));
    item->key = key;
    item->val = val;
    item->next = NULL;

    if (!firstItem) {
        firstItem = item;
    }
    else {
        lastItem->next = item;
    }

    lastItem = item;
}
bool BoolCache::Get(void *key)
{
    BoolCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (cur->key == key)
            return cur->val;

        cur = cur->next;
    }

    return false;
}
int BoolCache::GetItemCount()
{
    BoolCacheItem *cur;
    int itemCnt=0;
    cur = firstItem;
    while (cur) {
        itemCnt++;
        cur = cur->next;
    }

    return itemCnt;
}

IntCache::IntCache()
{
    firstItem = NULL;
    lastItem = NULL;
}
IntCache::~IntCache()
{
    IntCacheItem *cur = firstItem;
    while (cur!=NULL) {
        IntCacheItem *next = cur->next;
        free(cur);
        cur = next;
    }
    firstItem = lastItem = NULL;
}
AWDBlockList * IntCache::GetKeysOrderedByVal()
{
    AWDBlockList * newValList=new AWDBlockList();

    IntCacheItem *cur;
    cur = firstItem;
    int cnt=0;
    int smallesVal=999999;
    IntCacheItem *cur2;
    while (cur) {
        cnt++;
        if (cur->val<smallesVal){
            smallesVal=cur->val;
            cur2=cur;
        }
        cur = cur->next;
    }
    for (int i=0; i<cnt; i++){
        AWDBlock * thisAWDBlock = (AWDBlock*)cur2->key;
        cur2->isordered=true;
        if (thisAWDBlock!=NULL)
            newValList->append(thisAWDBlock);
        cur2->isordered=true;
        smallesVal=999999;
        cur = firstItem;
        while (cur) {
            if ((cur->val<smallesVal)&&(!cur->isordered)){
                smallesVal=cur->val;
                cur2=cur;
            }
            cur = cur->next;
        }
    }
    return newValList;
}

void IntCache::Set(void *key, int val)
{
    IntCacheItem *item;

    item = (IntCacheItem *)malloc(sizeof(IntCacheItem));
    item->key = key;
    item->val = val;
    item->isordered = false;
    item->next = NULL;

    if (!firstItem) {
        firstItem = item;
    }
    else {
        lastItem->next = item;
    }

    lastItem = item;
}
int IntCache::Get(void *key)
{
    IntCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (cur->key == key)
            return cur->val;

        cur = cur->next;
    }

    return 0;
}
bool IntCache::hasKey(void *key)
{
    IntCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (cur->key == key)
            return true;

        cur = cur->next;
    }

    return false;
}

BlockCache::BlockCache(bool weakReference)
{
    firstItem = NULL;
    lastItem = NULL;
    this->weakReference=weakReference;
}
BlockCache::~BlockCache()
{
    BlockCacheItem *cur = firstItem;
    while (cur) {
        BlockCacheItem *next = cur->next;
        if(!this->weakReference)
            delete cur->val;
        free(cur);
        cur = next;
    }

    firstItem = lastItem = NULL;
}
void BlockCache::Set(void *key, void *val)
{
    BlockCacheItem *item;

    item = (BlockCacheItem *)malloc(sizeof(BlockCacheItem));
    item->key = key;
    item->val = val;
    item->next = NULL;

    if (!firstItem) {
        firstItem = item;
    }
    else {
        lastItem->next = item;
    }

    lastItem = item;
}
void *BlockCache::Get(void *key)
{
    BlockCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (cur->key == key)
            return cur->val;

        cur = cur->next;
    }

    return NULL;
}
void *BlockCache::GetKeyByVal(void *val)
{
    BlockCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (cur->val == val)
            return cur->key;

        cur = cur->next;
    }

    return NULL;
}
void BlockCache::DeleteVals()
{
    BlockCacheItem *cur;

    cur = firstItem;
    while (cur) {
        delete cur->val;
        cur = cur->next;
    }
}

LightCache::LightCache(void)
{
    firstItem = NULL;
    lastItem = NULL;
    this->lightPickerCache = new StringCache();
}
LightCache::~LightCache(void)
{
    LightCacheItem *cur = firstItem;
    while (cur) {
        LightCacheItem *next = cur->next;
        free(cur);
        cur = next;
    }

    firstItem = lastItem = NULL;
    //lightPickerCache->DeleteVals();
    delete lightPickerCache;
}
void LightCache::Set(INode *node, AWDLight *light, ExclList *exclList, bool includeObjs)
{
    LightCacheItem *item;

    item = (LightCacheItem *)malloc(sizeof(LightCacheItem));
    item->node = node;
    item->light = light;
    item->excelList = exclList;
    item->includeObjs = includeObjs;
    item->next = NULL;

    if (!firstItem) {
        firstItem = item;
    }
    else {
        lastItem->next = item;
    }

    lastItem = item;
}

AWDLightPicker *LightCache::GetLightPickerForMesh(INode *meshNode)
{
    LightCacheItem *cur;
    AWDBlockList *lightBlocks = new AWDBlockList();

    int namelength=0;
    //get all AWDLightBlocks used by the mesh-node
    cur = firstItem;
    while (cur) {
        char * spacer = "#";
        ExclList * excludeList = cur->excelList;
        int isInList=excludeList->FindNode(meshNode);
        // TO DO: read out and implement "in/Exlude shadows"
        if (cur->includeObjs){
            if (isInList>=0){
                lightBlocks->append(cur->light);
                namelength += (int)strlen(spacer);
                namelength += (int)strlen(cur->light->get_awdID());
            }
        }
        else {
            if (isInList<0){
                lightBlocks->append(cur->light);
                namelength += (int)strlen(spacer);
                namelength += (int)strlen(cur->light->get_awdID());
            }
        }
        cur = cur->next;
    }
    if (lightBlocks->get_num_blocks()>0) {
        AWDBlockIterator *it;
        AWDLight *block;
        char *newName = (char*)malloc(namelength+1);
        it = new AWDBlockIterator(lightBlocks);
        char * spacer = "#";
        strcpy(newName, "");
        while ((block = (AWDLight *)it->next()) != NULL){
            strcat(newName, spacer);
            strcat(newName, block->get_awdID());
        }
        delete it;
        newName[namelength]=0;
        AWDLightPicker * awdLightPicker=(AWDLightPicker *)this->lightPickerCache->Get(newName);
        if (awdLightPicker==NULL){
            char * name=(char *)malloc(12);
            strcpy(name, "LightPicker");
            name[11]=0;
            awdLightPicker=new AWDLightPicker(name,strlen(name));
            free(name);
            awdLightPicker->set_lights(lightBlocks);
            lightPickerCache->Set(newName, awdLightPicker);
        }
        else
            delete lightBlocks;
        free(newName);
        return awdLightPicker;
    }
    else{
        delete lightBlocks;
    }
    return NULL;
}
AWDLight *LightCache:: Get(INode *node)
{
    LightCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (cur->node==node)
            return cur->light;
        cur = cur->next;
    }

    return NULL;
}

StringCache::StringCache(void)
{
    firstItem = NULL;
    lastItem = NULL;
}
StringCache::~StringCache(void)
{
    StringCacheItem *cur = firstItem;
    while (cur) {
        StringCacheItem *next = cur->next;
        if (cur->keyLen>0)
            free(cur->key);
        cur->keyLen=0;
        free(cur);
        cur = next;
    }

    firstItem = lastItem = NULL;
}
void StringCache::Set(char *key, void *val)
{
    StringCacheItem *item;

    item = (StringCacheItem *)malloc(sizeof(StringCacheItem));
    item->key = (char * )malloc(strlen(key)+1);
    strncpy(item->key, key, strlen(key));
    item->keyLen=strlen(key);
    item->key[strlen(key)] = 0;

    item->val = val;
    item->next = NULL;

    if (!firstItem) {
        firstItem = item;
    }
    else {
        lastItem->next = item;
    }

    lastItem = item;
}
void *StringCache::Get(char *key)
{
    StringCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (ATTREQ(cur->key,key))
            return cur->val;

        cur = cur->next;
    }

    return NULL;
}
void StringCache::DeleteVals()
{
    StringCacheItem *cur;

    cur = firstItem;
    while (cur) {
        AWDBlockList * thisBlockList=(AWDBlockList *)cur->val;
        if (thisBlockList!=NULL)
            delete thisBlockList;
        cur = cur->next;
    }
}

ColorMaterialCache::ColorMaterialCache(void)
{
    firstItem = NULL;
    lastItem = NULL;
}
ColorMaterialCache::~ColorMaterialCache(void)
{
    ColorMaterialCacheItem *cur = firstItem;
    while (cur) {
        ColorMaterialCacheItem *next = cur->next;
        free(cur);
        cur = next;
    }

    firstItem = lastItem = NULL;
}
void ColorMaterialCache::Set(awd_color color, AWDMaterial *mtl)
{
    ColorMaterialCacheItem *item;

    item = (ColorMaterialCacheItem *)malloc(sizeof(ColorMaterialCacheItem));
    item->color = color;
    item->mtl = mtl;
    item->next = NULL;

    if (!firstItem) {
        firstItem = item;
    }
    else {
        lastItem->next = item;
    }

    lastItem = item;
}
AWDMaterial *ColorMaterialCache::Get(awd_color color)
{
    ColorMaterialCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (cur->color == color)
            return cur->mtl;

        cur = cur->next;
    }

    return NULL;
}

SkeletonCacheItem::SkeletonCacheItem(INode *maxRootBone, const char * awdIDthis, int neutralTime)
{
    cur = NULL;
    next = NULL;
    rootBone = NULL;
    rootBone = maxRootBone;
    firstJoint = NULL;
    lastJoint = NULL;
    numJoints = 0;
    awdID=NULL;
    awdLength=strlen(awdIDthis);
    awdID = (char*)malloc(strlen(awdIDthis)+1);
    strncpy(awdID, awdIDthis, strlen(awdIDthis));
    awdID[strlen(awdIDthis)] = 0;
    // Create AWD skeleton structure and begin
    // gathering joints recursively
    char *name = W2A(rootBone->GetName());
    awdSkel = new AWDSkeleton(name, strlen(name), neutralTime);
    free(name);
    GatherJoint(rootBone, NULL, neutralTime, Inverse(rootBone->GetNodeTM(neutralTime)), true);//* Inverse(rootBone->GetParentTM(neutralTime))), true);// 
}
SkeletonCacheItem::~SkeletonCacheItem(void)
{
    SkeletonCacheJoint *cur = firstJoint;
    while (cur) {
        SkeletonCacheJoint *next = cur->next;
        free(cur);
        cur = next;
    }
    rootBone=NULL;
    if (awdLength>0)
        free(awdID);
    awdID=NULL;
    firstJoint = lastJoint = NULL;
}
void SkeletonCacheItem::AppendCacheJoint(SkeletonCacheJoint *cacheJoint)
{
    if (!firstJoint) {
        firstJoint = cacheJoint;
    }
    else {
        lastJoint->next = cacheJoint;
    }

    lastJoint = cacheJoint;
    lastJoint->index = numJoints++;
    lastJoint->next = NULL;
}

void SkeletonCacheItem::GatherJoint(INode *bone, AWDSkeletonJoint *awdParent, int time, Matrix3 inverseOffsetMatrix, bool isRoot)
{
    int i=0;
    SkeletonCacheJoint *cacheJoint;
    AWDSkeletonJoint *awdJoint;
    Matrix3 boneTM;
    awd_float64 *bindMtx;
    // Use bone transform as bind matrix.
    boneTM = bone->GetNodeTM(time) * inverseOffsetMatrix;
    boneTM.NoScale(); // get rid of the scele part of the parent matrix
    if(isRoot)
        boneTM.IdentityMatrix();
    bindMtx = (awd_float64*)malloc(sizeof(awd_float64)*12);
    SerializeMatrix3(Inverse(boneTM), bindMtx);

    char *name = W2A(bone->GetName());
    awdJoint = new AWDSkeletonJoint(name, strlen(name), bindMtx);
    free(name);

    if (awdParent != NULL) {
        awdParent->add_child_joint(awdJoint);
    }
    else {
        awdSkel->set_root_joint(awdJoint);
    }

    // Add to cache
    cacheJoint = (SkeletonCacheJoint*)malloc(sizeof(SkeletonCacheJoint));
    cacheJoint->awdJoint = awdJoint;
    cacheJoint->maxBone = bone;
    AppendCacheJoint(cacheJoint);

    for (i=0; i<bone->NumberOfChildren(); i++) {
        INode *child = bone->GetChildNode(i);
        Object *obj = bone->GetObjectRef();
        if(!obj)
        {
            continue;
        }
        GatherJoint(child, awdJoint, time, inverseOffsetMatrix, false);
    }
}
int SkeletonCacheItem::IndexOfBone(INode *bone)
{
    SkeletonCacheJoint *cur;

    cur = firstJoint;
    while (cur) {
        if (cur->maxBone == bone)
            return cur->index;

        cur = cur->next;
    }

    return -1;
}
int SkeletonCacheItem::get_num_joints()
{
    return this->numJoints;
}
void SkeletonCacheItem::IterReset()
{
    cur = firstJoint;
}
SkeletonCacheJoint *SkeletonCacheItem::IterNext()
{
    // Stop if end was reached
    if (!cur)
        return NULL;

    // Move on and return previous
    SkeletonCacheJoint *ret = cur;
    cur = cur->next;
    return ret;
}

SkeletonCache::SkeletonCache(void)
{
    firstItem = NULL;
    lastItem = NULL;
    cur = NULL;
}
SkeletonCache::~SkeletonCache(void)
{
    SkeletonCacheItem *cur = firstItem;
    while (cur) {
        SkeletonCacheItem *next = cur->next;
        delete cur;
        cur = next;
    }

    firstItem = lastItem = NULL;
}

AWDSkeleton *SkeletonCache::Add(INode *rootBone, char * awdID, int neutralTime)
{
    SkeletonCacheItem *item;

    item = new SkeletonCacheItem(rootBone, awdID, neutralTime);

    if (!firstItem) {
        firstItem = item;
    }
    else {
        lastItem->next = item;
    }

    lastItem = item;

    // Return skeleton
    return item->awdSkel;
}
SkeletonCacheItem *SkeletonCache::GetFromBone(INode *bone)
{
    SkeletonCacheItem *cur;

    cur = firstItem;
    while (cur) {
        // Check if this cache item is a skeleton
        // containing the supplied bone.
        if (cur->IndexOfBone(bone) >= 0)
            return cur;

        cur = cur->next;
    }

    return NULL;
}
SkeletonCacheItem *SkeletonCache::GetByAWDID(char *awdID)
{
    SkeletonCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (strcmp(cur->awdID,awdID) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}
bool SkeletonCache::HasItems()
{
    return (firstItem != NULL);
}
void SkeletonCache::IterReset()
{
    cur = firstItem;
}
SkeletonCacheItem *SkeletonCache::IterNext()
{
    // Stop if end was reached
    if (!cur)
        return NULL;

    SkeletonCacheItem *ret = cur;
    cur = cur->next;
    return ret;
}