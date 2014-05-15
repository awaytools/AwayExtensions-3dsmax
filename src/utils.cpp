#include "utils.h"
unsigned long createARGB(int a, int r, int g, int b)
{
    return ((a & 0xff) << 24) + ((r & 0xff) << 16) + ((g & 0xff) << 8)
           + (b & 0xff);
}
unsigned long createRGB( int r, int g, int b)
{
    return (((r & 0xff) << 16) + ((g & 0xff) << 8)
           + (b & 0xff));
}
enum color_component
{
    A,B,G,R
};
unsigned int get_component(unsigned int color, color_component component)
{
    switch (component)
    {
        case R:
        case G:
        case B:
        case A:
        {
            const unsigned int shift = component * 8;
            const unsigned int mask = 0xFF << shift;
            return (color & mask) >> shift;
        }

        default:
            return 0;
            //donothing throw std::invalid_argument("invalid color component");
    }

    return 0;
}
awd_color convertColor(DWORD input)
{
        awd_color color = input;
        int r = get_component(color,A);
        int g = get_component(color,B);
        int b = get_component(color,G);
        int a = get_component(color,R);

        return createARGB(255, r, g, b);
}

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
        int i=0;

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

void read_transform_position_into_Pose(INode *node, int time, AWDSkeletonPose * skelPose )
{
    //TODO: Check if there has been any movment, and if not, than dont export frame (append duration to last frame)
    Matrix3 parentMtx=node->GetParentTM(time);
    parentMtx.NoScale(); // get rid of the scale part of the parent matrix
    Matrix3 tm = node->GetNodeTM(time) * Inverse(parentMtx);
    awd_float64 *mtx = (awd_float64*)malloc(sizeof(awd_float64)*12);
    SerializeMatrix3(tm, mtx);
    skelPose->set_next_transform(mtx);
    int i=0;
    for (i=0; i<node->NumberOfChildren(); i++) {
        read_transform_position_into_Pose(node->GetChildNode(i), time, skelPose);
    }
}

int CalcNumDescendants(INode *node)
{
    int i=0;
    int num = 1;

    for (i=0; i<node->NumberOfChildren(); i++) {
        num += CalcNumDescendants(node->GetChildNode(i));
    }

    return num;
}

IParamBlock* GetParamBlockByIndex(ReferenceMaker* obj, int index)
{
    int nRefs = obj->NumRefs();
    int found=0;
    for ( int i = 0; i < nRefs; ++i )
    {
        ReferenceTarget* ref = obj->GetReference(i);

        if ( ref && ref->SuperClassID() == PARAMETER_BLOCK_CLASS_ID )
        {
            if (found==index)
                return dynamic_cast<IParamBlock*>( ref );
            found++;
        }
    }
    return NULL;
}

IParamBlock2* GetParamBlock2ByName(ReferenceMaker* obj, const char * name)
{
    int nRefs = obj->NumRefs();
    for ( int i = 0; i < nRefs; ++i )
    {
        ReferenceTarget* ref = obj->GetReference(i);

        if ( ref && ref->SuperClassID() == PARAMETER_BLOCK2_CLASS_ID )
        {
            IParamBlock2* thisblock=dynamic_cast<IParamBlock2*>( ref );
            char * blockName=W2A(thisblock->GetLocalName());
            if (ATTREQ(blockName, name)){
                free (blockName);
                return thisblock;
            }
            free (blockName);
        }
    }
    return NULL;
}
IParamBlock2* GetParamBlock2ByIndex(ReferenceMaker* obj, int index)
{
    int nRefs = obj->NumRefs();
    int found=0;
    for ( int i = 0; i < nRefs; ++i )
    {
        ReferenceTarget* ref = obj->GetReference(i);

        if ( ref && ref->SuperClassID() == PARAMETER_BLOCK2_CLASS_ID )
        {
            if (found==index)
                return dynamic_cast<IParamBlock2*>( ref );
            found++;
        }
    }
    return NULL;
}

bool FileExists(const char *path)
{
  DWORD dwAttrib = GetFileAttributesA(path);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool PathIsAbsolute(const char *path)
{
    char pathDrive[4];

    _splitpath_s(path, pathDrive, 4, NULL, 0, NULL, 0, NULL, 0);
    return (strlen(pathDrive) > 0);
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

void getBaseObjectAndID( Object*& object, SClass_ID& sid )
    {
        if( object == NULL )
            return ;

        sid = object->SuperClassID();

        if( sid == WSM_DERIVOB_CLASS_ID || sid == DERIVOB_CLASS_ID || sid == GEN_DERIVOB_CLASS_ID )
        {
            IDerivedObject * derivedObject =( IDerivedObject* ) object;

            if( derivedObject->NumModifiers() > 0 )
            {
                // Remember that 3dsMax has the mod stack reversed in its internal structures.
                // So that evaluating the zero'th modifier implies evaluating the whole modifier stack.
                ObjectState state = derivedObject->Eval( 0, 0 );
                object = state.obj;
            }
            else
            {
                object = derivedObject->GetObjRef();
            }

            sid = object->SuperClassID();
        }
    }