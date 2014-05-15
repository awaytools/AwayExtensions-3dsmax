#pragma once

#include <iskin.h>
#include "Max.h"
#include "awd.h"

typedef struct BoolCacheItem_struct {
    void *key;
    bool val;
    BoolCacheItem_struct *next;
} BoolCacheItem;

class BoolCache
{
private:
    BoolCacheItem *firstItem;
    BoolCacheItem *lastItem;

public:
    BoolCache();
    ~BoolCache();

    void Set(void *key, bool val);
    bool Get(void *key);
    int GetItemCount();
};

typedef struct IntCacheItem_struct {
    void *key;
    int val;
    bool isordered;
    IntCacheItem_struct *next;
} IntCacheItem;

class IntCache
{
private:
    IntCacheItem *firstItem;
    IntCacheItem *lastItem;

public:
    IntCache();
    ~IntCache();

    void Set(void *key, int val);
    int Get(void *key);
    bool hasKey(void *key);
    AWDBlockList * GetKeysOrderedByVal();
};

/**
 * BlockCache
 * Cache that is used generically to store any type of block or
 * sub-element in a look-up table by MAX object.
*/
typedef struct BlockCacheItem_struct {
    void *key;
    void *val;
    BlockCacheItem_struct *next;
} BlockCacheItem;

class BlockCache
{
private:
    BlockCacheItem *firstItem;
    BlockCacheItem *lastItem;
    bool weakReference;

public:
    BlockCache(bool weakReference = true);
    ~BlockCache();

    void Set(void *key, void *val);
    void *Get(void *key);
    void *GetKeyByVal(void *val);
    void DeleteVals();
};

typedef struct StringCacheItem_struct {
    char *key;
    int keyLen;
    void *val;
    StringCacheItem_struct *next;
} StringCacheItem;

class StringCache
{
private:
    StringCacheItem *firstItem;
    StringCacheItem *lastItem;

public:
    StringCache(void);
    ~StringCache(void);

    void Set(char *key, void *val);
    void *Get(char *key);
    char *GetKeyByVal(void *val);
    void DeleteVals();
};

typedef struct LightCacheItem_struct {
    AWDLight *light;
    INode *node;
    ExclList *excelList;
    bool includeObjs;
    LightCacheItem_struct *next;
} LightCacheItem;

class LightCache
{
private:
    LightCacheItem *firstItem;
    LightCacheItem *lastItem;
    StringCache *lightPickerCache;

public:
    LightCache(void);
    ~LightCache(void);

    void Set(INode *light, AWDLight *awd_light, ExclList *, bool);
    AWDLight *Get(INode *light);
    AWDLightPicker *GetLightPickerForMesh(INode *meshNode);
};

/**
 * ColorMaterialCache
 * Cache that is used for "default" color materials applied to blocks
 * that have no material assigned. AWDMaterial blocks are stored in a
 * dictionary cache by color.
*/
typedef struct ColorMaterialCacheItem_struct {
    awd_color color;
    AWDMaterial *mtl;
    ColorMaterialCacheItem_struct *next;
} ColorMaterialCacheItem;

class ColorMaterialCache
{
private:
    ColorMaterialCacheItem *firstItem;
    ColorMaterialCacheItem *lastItem;

public:
    ColorMaterialCache(void);
    ~ColorMaterialCache(void);

    void Set(awd_color color, AWDMaterial *mtl);
    AWDMaterial *Get(awd_color color);
};

/**
 * SkeletonCache
 * Used by animation export to look up skeletons that need to be sampled
 * for animation export.
*/
typedef struct SkeletonCacheJoint_struct
{
    int index;
    INode *maxBone;
    AWDSkeletonJoint *awdJoint;
    struct SkeletonCacheJoint_struct *next;
} SkeletonCacheJoint;

class SkeletonCacheItem
{
private:
    INode *rootBone;
    SkeletonCacheJoint *cur;
    SkeletonCacheJoint *firstJoint;
    SkeletonCacheJoint *lastJoint;
    int numJoints;

    void GatherJoint(INode *parentNode, AWDSkeletonJoint *awdParent, int, Matrix3, bool);
    void AppendCacheJoint(SkeletonCacheJoint *cacheJoint);

public:
    SkeletonCacheItem(INode *rootBone, const char *, int);
    ~SkeletonCacheItem(void);

    char *awdID;
    int awdLength;
    AWDSkeleton *awdSkel;
    SkeletonCacheItem *next;

    int IndexOfBone(INode *bone);
    int get_num_joints();

    void IterReset();
    SkeletonCacheJoint *IterNext();
};

class SkeletonCache
{
private:
    SkeletonCacheItem *cur;
    SkeletonCacheItem *firstItem;
    SkeletonCacheItem *lastItem;

public:
    SkeletonCache(void);
    ~SkeletonCache(void);

    AWDSkeleton *Add(INode *maxRootBone, char *, int);
    SkeletonCacheItem *GetFromBone(INode *bone);
    SkeletonCacheItem *GetByAWDID(char *);
    bool HasItems();
    void IterReset();
    SkeletonCacheItem *IterNext();
};
