#include "maxawd.h"
/*
* convert a 3dsmax color to awdColor (change RGBA to ARGB)
*/
awd_color convertColor(DWORD);

/**
 * Generate an AWD compatible serialized matrix from a 3ds Max
 * Matrix3 class instance.
*/
void SerializeMatrix3(Matrix3 &mtx, double *output);

/**
 * Find skin modifier in an object's modifier stack and return
 * the index as well as the derived object (through pointer arg)
*/
int IndexOfSkinMod(Object *obj, IDerivedObject **derivedObject);

/**
 * Calculate total number of descendant nodes by recursing tree.
*/
int CalcNumDescendants(INode *node);

/**
 * Find and load the animation sequence description file.
*/
SequenceMetaData *LoadSequenceFile(const char *awdFullPath, char *sequencesTxtPath);

/**
 * Parse animation sequence file.
*/
SequenceMetaData *ParseSequenceFile(const char *path);

/**
 * Replace a single occurrence of a substring within a string with 
 * another string, overwriting the input buffer. Returns the number
 * of occurrences that were replaced (0 or 1).
*/
int ReplaceString(char *buf, int *size, char *find, char *rep);

/**
 * Check if a file exists.
*/
bool FileExists(const char *path);

/**
 * Checks if a path is absolute (includes a drive letter.)
*/
bool PathIsAbsolute(const char *path);