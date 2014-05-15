#include "maxawdexporter.h"
/*

*/
unsigned long createARGB(int a, int r, int g, int b);
/*

*/
unsigned long createRGB(int r, int g, int b);
/**
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

void read_transform_position_into_Pose(INode *, int , AWDSkeletonPose *);
/**
 * Calculate total number of descendant nodes by recursing tree.
*/
int CalcNumDescendants(INode *node);

/**
 * Get a ParamBlock from a 3dsmax Object.
*/
IParamBlock2* GetParamBlock2ByIndex(ReferenceMaker* obj, int);
IParamBlock2* GetParamBlock2ByName(ReferenceMaker* obj, const char *);

/**
 * Get a ParamBlock from a 3dsmax Object.
*/
IParamBlock* GetParamBlockByIndex(ReferenceMaker* obj, int);

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

/**
 * Get The BaseObject oand SuperClassID for a 3dsmax Node (because of the modifier-system, the api might not give back the base-object)
*/
void getBaseObjectAndID( Object*&, SClass_ID&);
