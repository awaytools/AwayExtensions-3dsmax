#include "cache.h"
#include "utils.h"
#include <stdlib.h>

BlockCache::BlockCache(void)
{
	firstItem = NULL;
	lastItem = NULL;
}


BlockCache::~BlockCache(void)
{
	BlockCacheItem *cur = firstItem;
	while (cur) {
		BlockCacheItem *next = cur->next;
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




SkeletonCacheItem::SkeletonCacheItem(INode *maxRootBone)
{
	cur = NULL;
	next = NULL;
	rootBone = maxRootBone;
	firstJoint = NULL;
	lastJoint = NULL;
	numJoints = 0;

	// Create AWD skeleton structure and begin
	// gathering joints recursively
	char *name = W2A(rootBone->GetName());
	awdSkel = new AWDSkeleton(name, strlen(name));
	free(name);
	GatherJoint(rootBone, NULL);
}


SkeletonCacheItem::~SkeletonCacheItem(void)
{
	SkeletonCacheJoint *cur = firstJoint;
	while (cur) {
		SkeletonCacheJoint *next = cur->next;
		free(cur);
		cur = next;
	}

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

/**
 * Gather joints using the same order that the AWD SDK uses, so that the
 * binding code can then look-up indices using this structure.
*/
void SkeletonCacheItem::GatherJoint(INode *bone, AWDSkeletonJoint *awdParent)
{
	int i;
	char *name;
	SkeletonCacheJoint *cacheJoint;
	AWDSkeletonJoint *awdJoint;
	Matrix3 boneTM;
	awd_float64 *bindMtx;

	// Use bone transform as bind matrix for now. This will be
	// overwritten by the skin export if exporting skins.
	boneTM = bone->GetNodeTM(0);
	bindMtx = (awd_float64*)malloc(sizeof(awd_float64)*12);
	SerializeMatrix3(Inverse(boneTM), bindMtx);

	name = W2A(bone->GetName());
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
		Class_ID class_id=obj->ClassID();
		if (class_id==BONE_OBJ_CLASSID || class_id.PartA()==DUMMY_CLASS_ID) {
			GatherJoint(child, awdJoint);
		}
	}
}


void SkeletonCacheItem::ConfigureForSkin(ISkin *skin)
{
	SkeletonCacheJoint *cur;

	cur = firstJoint;
	while (cur) {
		Matrix3 invBindTM;
		awd_float64 *invBindMtx;

		// Update bind matrix with the one defined by skin
		skin->GetBoneInitTM(cur->maxBone, invBindTM);
		invBindTM = Inverse(invBindTM);
		invBindMtx = (awd_float64*)malloc(sizeof(awd_float64) * 12);
		SerializeMatrix3(invBindTM, invBindMtx);
		cur->awdJoint->set_bind_mtx(invBindMtx);

		cur = cur->next;
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


AWDSkeleton *SkeletonCache::Add(INode *rootBone)
{
	SkeletonCacheItem *item;

	item = new SkeletonCacheItem(rootBone);

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