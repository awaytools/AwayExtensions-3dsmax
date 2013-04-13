#include "utils.h"

void SerializeMatrix3(Matrix3 &mtx, double *output)
{
	Point3 row;
	
	row = mtx.GetRow(0);
	output[0] = row.x;
	output[1] = row.z;
	output[2] = row.y;

	row = mtx.GetRow(2);
	output[3] = row.x;
	output[4] = row.z;
	output[5] = row.y;

	row = mtx.GetRow(1);
	output[6] = row.x;
	output[7] = row.z;
	output[8] = row.y;

	row = mtx.GetRow(3);
	output[9] = row.x;
	output[10] = row.z;
	output[11] = row.y;
}


int IndexOfSkinMod(Object *obj, IDerivedObject **derivedObject)
{
	if (obj != NULL && obj->SuperClassID() == GEN_DERIVOB_CLASS_ID) {
		int i;

		IDerivedObject *derived = (IDerivedObject *)obj;

		for (i=0; i < derived->NumModifiers(); i++) {
			Modifier *mod = derived->GetModifier(i);

			void *skin = mod->GetInterface(I_SKIN);
			if (skin != NULL) {
				*derivedObject = derived;
				return i;
			}
		}
	}
	
	return -1;
}


int CalcNumDescendants(INode *node)
{
	int i;
	int num = 1;
	
	for (i=0; i<node->NumberOfChildren(); i++) {
		num += CalcNumDescendants(node->GetChildNode(i));
	}

	return num;
}


SequenceMetaData *ParseSequenceFile(const char *path)
{
	FILE *fp;

	fp = fopen(path, "r");
	if (fp) {
		char line[256];
		SequenceMetaData *first, *last;

		first = NULL;
		last = NULL;

		while (fgets(line, 256, fp) != NULL) {
			SequenceMetaData *cur;
			int nameLen;
			char *name, *start, *end;

			// Skip empty lines and comments
			if (strlen(line)==0 || line[0]=='#')
				continue;

			// Read name and skip if missing.
			name = strtok(line, " ");
			if (!name) continue;
			nameLen = strlen(name);

			// Read start frame and skip if missing.
			start = strtok(NULL, " ");
			if (!start) continue;

			// Read end frame and skip if missing.
			end = strtok(NULL, " ");
			if (!end) continue;

			cur = (SequenceMetaData*)malloc(sizeof(SequenceMetaData));
			cur->start = strtol(start, NULL, 10);
			cur->stop = strtol(end, NULL, 10);
			cur->name = (char*)malloc(nameLen+1);
			memcpy(cur->name, name, nameLen+1);

			if (!first) {
				first = cur;
			}
			else {
				last->next = cur;
			}

			last = cur;
			last->next = NULL;
		}

		return first;
	}

	return NULL;
}


bool FileExists(const char *path)
{
  DWORD dwAttrib = GetFileAttributes(path);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool PathIsAbsolute(const char *path)
{
	char pathDrive[4];

	_splitpath_s(path, pathDrive, 4, NULL, 0, NULL, 0, NULL, 0);
	return (strlen(pathDrive) > 0);
}

SequenceMetaData *LoadSequenceFile(const char *awdFullPath, char *sequencesTxtPath)
{
	if (PathIsAbsolute(sequencesTxtPath)) {
		if (!FileExists(sequencesTxtPath))
			return NULL;

		return ParseSequenceFile(sequencesTxtPath);
	}
	else {
		char awdDrive[4];
		char awdPath[1024];
		char txtPath[1024];

		_splitpath_s(awdFullPath, awdDrive, 4, awdPath, 1024, NULL, 0, NULL, 0);
		_makepath_s(txtPath, 1024, awdDrive, awdPath, sequencesTxtPath, NULL);

		if (!FileExists(txtPath))
			return NULL;
	
		return ParseSequenceFile(txtPath);
	}
}


int ReplaceString(char *buf, int *size, char *find, char *rep)
{
    char *p;
    char *tmp;
    int findLen;
    int repLen;
    int endLen;

    p = strstr(buf, find);
    if (p == NULL)
        return 0;

    // Take trailing part and store temporarily
    findLen = strlen(find);
    endLen = strlen(p) - findLen;
    tmp = (char *)malloc(endLen);
    memcpy(tmp, p+findLen, endLen);

    // Replace string in buffer and move to end
    // of replaced string
    repLen = strlen(rep);
    memcpy(p, rep, repLen);
    p += repLen;

    // Append trailing string
    memcpy(p, tmp, endLen);
	memset(p+endLen, 0, 1);

    free(tmp);

	// Save new size of buffer
    *size = (p-buf) + endLen;

	return 1;
}