//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR:
//***************************************************************************/

#include <cs/bipexp.h>
#include <Windows.h>
#include <icustattribcontainer.h>
#include <custattrib.h>
#include <iparamb2.h>
#include <MeshNormalSpec.h>
#include <direct.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #define ZLIB_WINAPI
#endif
#include "zlib.h"

#define CHUNK 16384

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#include <assert.h>
#include "awd.h"
#include "method.h"
#include "util.h"
#include "platform.h"
#include "geomUtil.h"
#include "maxawdimporter.h"
#include "utils.h"
#include "maxawdimporterblocks.h"

#include "cacheimporter.h"

#include "msxml2.h"
#include "decomp.h"

#include "dummy.h"
#include "IGame.h"
#include "IGameObject.h"
#include "IGameProperty.h"
#include "IGameControl.h"
#include "IGameModifier.h"
#include "IConversionManager.h"
#include "IGameError.h"
#include "IGameFX.h"
#include "iSkinPose.h"

#define MaxAWDImporter_CLASS_ID Class_ID(0x63c63505, 0x26f56a4f)

char *W2A( const TCHAR *s ) {
#ifdef UNICODE
    int size = (wcslen(s) + 1) * 2;
    char *out = (char*)malloc(size);
    wcstombs(out,s,size);
    return out;
#else
    return strdup(s);
#endif
}

TCHAR *A2W( const char  *s ) {
#ifdef UNICODE
    int size = (strlen(s) + 1) * sizeof(TCHAR);
    TCHAR *out = (TCHAR*)malloc(size);
    mbstowcs(out, s, size);
    return out;
#else
    return strdup(s);
#endif
}

static unsigned char s_depth=0;
static void output_debug_string(const char* str)
{
    for(unsigned char uc=0;uc<s_depth;uc++)
    {
        OutputDebugStringA("    ");
    }
    OutputDebugStringA(str);
    OutputDebugStringA("\r\n");
}

class MaxAWDImporterClassDesc : public ClassDesc2
{
public:
    virtual int IsPublic() 							{ return TRUE; }
    virtual void* Create(BOOL /*loading = FALSE*/) 	{ return new MaxAWDImporter(); }
    virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
    virtual SClass_ID SuperClassID() 				{ return SCENE_IMPORT_CLASS_ID; }
    virtual Class_ID ClassID() 						{ return MaxAWDImporter_CLASS_ID; }
    virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

    virtual const TCHAR* InternalName() 			{ return _T("MaxAWDImporter"); }		// returns fixed parsable name (scripter-visible name)
    virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
};

ClassDesc2* GetMaxAWDImporterDesc() {
    static MaxAWDImporterClassDesc MaxAWDImporterDesc;
    return &MaxAWDImporterDesc;
}

/**
 * Invoked by the Interface::ProgressStart() method to execute
 * the export operation while showing the progress bar. The
 * argument is specified by MaxAWDImporter::DoImport() as the
 * instance of the exporter.
*/
static DWORD WINAPI ExecuteImportCallback(LPVOID arg)
{
    MaxAWDImporter *exporter = (MaxAWDImporter*)arg;
    return exporter->ExecuteImport();
}

MaxAWDImporter::MaxAWDImporter()
{
    exportAll=TRUE;
    error = false;
    awdFullPath = NULL;
}

MaxAWDImporter::~MaxAWDImporter()
{
    free(awdFullPath);
}

int MaxAWDImporter::ExtCount()
{
    return 1;
}

const TCHAR *MaxAWDImporter::Ext(int n)
{
    return _T("awd");
}

const TCHAR *MaxAWDImporter::LongDesc()
{
    return _T("Away3D AWD2.1 File");
}

const TCHAR *MaxAWDImporter::ShortDesc()
{
    return _T("Away3D");
}

const TCHAR *MaxAWDImporter::AuthorName()
{
    return _T("The Away3D Team");
}

const TCHAR *MaxAWDImporter::CopyrightMessage()
{
    return _T("Copyright 2014 The Away3D Team");
}

const TCHAR *MaxAWDImporter::OtherMessage1()
{
    return _T("");
}

const TCHAR *MaxAWDImporter::OtherMessage2()
{
    return _T("");
}

unsigned int MaxAWDImporter::Version()
{
    return 102;
}

void MaxAWDImporter::ShowAbout(HWND hWnd)
{
    // Optional
}

BOOL MaxAWDImporter::SupportsOptions(int ext, DWORD options)
{
    return TRUE;
}

// Handy file class

class WorkFile {
private:
    FILE *stream;
public:
                    WorkFile(const TCHAR *filename,const TCHAR *mode) { stream = _tfopen(filename,mode); };
                    ~WorkFile() { if(stream) fclose(stream); stream = NULL; };
    FILE *			Stream() { return stream; };
    };

static FILE *stream = NULL;
static FILE *dStream = NULL;

bool MaxAWDImporter::is_bit_set(unsigned value, unsigned bitindex)
{
    return (value & (1 << bitindex)) != 0;
}
char * read_string(FILE *stream,int maxsize)
{
    char * magic = (char *)malloc(sizeof(char)*maxsize+1);
    if(!(fread(magic,maxsize,1,stream)))
        return NULL;
    magic[maxsize] = 0;
    return magic;	/* Too long */
}
char * read_varstring(FILE *stream)
{
    awd_uint16 stringSize;
    if(!(fread(&stringSize,2,1,stream)))
        return NULL;
    else{
        return read_string(stream, stringSize);
     }
    return NULL;
}

int	MaxAWDImporter::DoImport(const TCHAR *path,ImpInterface *i,Interface *gi, BOOL suppressPrompts)
{
    this->imp_i=i;
    showPrompts = suppressPrompts ? FALSE : TRUE;
    awdFullPath = W2A(path);
    maxInterface = gi;
    WorkFile theFile(path,_T("rb"));

    // Open the dialog (provided that prompts are not suppressed) and
    // if it returns false, return to cancel the export.

    opts = new MaxAWDImporterOpts();
    if (showPrompts && !opts->ShowDialog()) {
        delete opts;
        return IMPEXP_FAIL;
    }
    dStream = i->DumpFile();

    stream = theFile.Stream();
    if(stream == NULL) {
        //if(showPrompts)
            //MessageBox(IDS_TH_ERR_OPENING_FILE, IDS_TH_3DSIMP);
        return IMPEXP_FAIL;
        }

    char * magicString=read_string(stream, 3);
    if (magicString==NULL)
        return IMPEXP_FAIL;
    if (!ATTREQ(magicString,"AWD"))
        return IMPEXP_FAIL;

    awd_uint8 majorVersion;
    if(!(fread(&majorVersion,1,1,stream)))
        return IMPEXP_FAIL;						// Didn't open!
    awd_uint8 minVersion;
    if(!(fread(&minVersion,1,1,stream)))
        return IMPEXP_FAIL;						// Didn't open!
    awd_uint16 headerflag;
    if(!(fread(&headerflag,2,1,stream)))
        return IMPEXP_FAIL;						// Didn't open!

    bool _streaming=is_bit_set(headerflag, 0);

    awd_uint8 compression;
    if(!(fread(&compression,1,1,stream)))
        return IMPEXP_FAIL;						// Didn't open!
    awd_uint32 length;
    if(!(fread(&length,4,1,stream)))
        return IMPEXP_FAIL;						// Didn't open!
    bool isAWD_2_1=false;
    if ((majorVersion==2)&&(minVersion==1)){
        isAWD_2_1=true;}

    int curPos = ftell(stream);
    fseek(stream,0,SEEK_END);
    int size = ftell(stream);
    fseek(stream, curPos, SEEK_SET);
    int curPos2 = ftell(stream);
    if (length!=size){
        // file is corrupt, but we still can try to read it
    }
    cache=new ImporterBlockCache();
    if (compression==0){
        // if no compression, we can directly read the blocks
        while (curPos<size){
            read_new_block(stream, isAWD_2_1);
            curPos = ftell(stream);
        }
    }
    else if (compression==1){
        /*
        //TODO: implement uncompress zlip
        FILE * pFile;
        pFile = tmpfile ();
        uLong ucompSize = size-11; // "Hello, world!" + NULL delimiter.
        uLong compSize = compressBound(ucompSize);
        //uncompress((Bytef *)pFile, &ucompSize, (Bytef *)pFile, compSize);
        fseek(pFile,0,SEEK_END);
        int size2 = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);
        int curPos2 = ftell(stream);
        //inf(stream, pFile);
        while (curPos2<size2){
            read_new_block(stream, isAWD_2_1);
            curPos2 = ftell(stream);
        }
        */

        return IMPEXP_FAIL;						// Didn't open!
    }
    else if (compression==2){
        //TODO: implement uncompress zlip
        return IMPEXP_FAIL;						// Didn't open!
    }
    else{
        //unknown compression type
        return IMPEXP_FAIL;						// Didn't open!
    }

    fclose(stream);
    //if (options==SCENE_EXPORT_SELECTED){
    //	exportAll=FALSE;
    //}
    // Execute export while showing a progress bar. Send this as argument
    // to the execute callback, which will invoke MaxAWDImporter::ExecuteImport();
    //maxInterface->ProgressStart(TEXT("Importing AWD file"), TRUE, ExecuteImportCallback, this);

    // Export worked
    return IMPEXP_SUCCESS;
}

int MaxAWDImporter::ExecuteImport()
{
    return TRUE;
}

void MaxAWDImporter::DieWithError(void)
{
    error = true;
}

void MaxAWDImporter::DieWithErrorMessage(char *message, char *caption)
{
    if (!showPrompts) {
        Interface *i = GetCOREInterface();
        MessageBoxA(i->GetMAXHWnd(), message, caption, MB_OK);
    }

    DieWithError();
}

void MaxAWDImporter::UpdateProgressBar(int phase, double phaseProgress)
{
    int phaseStart;
    int phaseFinish;
    TCHAR *title;

    switch (phase) {
        case MAXAWD_PHASE_PREPROCESS_SCENEGRAPH:
            phaseStart = 0;
            phaseFinish = 20;
            title = TEXT("PreProcess SceneGraph");
            break;
        case MAXAWD_PHASE_EXPORT_SCENEGRAPH:
            phaseStart = 20;
            phaseFinish = 40;
            title = TEXT("Export SceneGraph");
            break;
        case MAXAWD_PHASE_PROCESS_GEOMETRY:
            phaseStart = 40;
            phaseFinish = 60;
            title = TEXT("Convert Geometry");
            break;
        case MAXAWD_PHASE_PROCESS_ANIMATIONS:
            phaseStart = 60;
            phaseFinish = 80;
            title = TEXT("Process Animations");
            break;
        case MAXAWD_PHASE_FLUSH:
            phaseStart = 80;
            phaseFinish = 100;
            title = TEXT("Writing file");
            break;
    }

    int phaseLen = phaseFinish - phaseStart;
    int progress = phaseStart + floor(phaseProgress*phaseLen + 0.5);
    maxInterface->ProgressUpdate(progress, FALSE, title);
}

ImportedAWDBlock * MaxAWDImporter::read_new_block(FILE * fd, bool isAWD2_1)
{
    awd_uint32 blockID;
    if(!(fread(&blockID,4,1,fd)))
        return NULL;// No data!
    awd_uint8 nameSpaceID;
    if(!(fread(&nameSpaceID,1,1,fd)))
        return NULL;// No data!

    awd_uint8 blockType;
    if(!(fread(&blockType,1,1,fd)))
        return NULL;

    awd_uint8 blockFlags;
    if(!(fread(&blockFlags,1,1,fd)))
        return NULL;// No data!

    bool accuracyMatrix=is_bit_set(blockFlags, 1);
    bool accuracyGeo=is_bit_set(blockFlags, 2);
    bool accuracyProps=is_bit_set(blockFlags, 4);
    bool blockCompression=is_bit_set(blockFlags, 8); // not used yet
    bool blockCompressionLZMA=is_bit_set(blockFlags, 16); // not used yet

    awd_uint32 blockLength;
    if(!(fread(&blockLength,4,1,fd)))
        return NULL;// No data!

    int calculatedBlockEndPos = ftell(fd)+blockLength;
    Animatable * maxObject=NULL;
    ImportedAWDBlock* newAWDBlock = new ImportedAWDBlock((AWD_block_type)blockType, (awd_baddr)cache->get_num_items());
    if (blockType==TRI_GEOM)
        maxObject=(Animatable *)readTriGeomBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
    else if (blockType==CONTAINER)
        maxObject=(Animatable *)readContainerBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
    else if (blockType==MESH_INSTANCE)
        maxObject=(Animatable *)readMeshInstanceBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
    else if (blockType==SIMPLE_MATERIAL)
        maxObject=(Animatable *)readMaterialBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
    else if (blockType==BITMAP_TEXTURE)
        maxObject=(Animatable *)readTextureBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
    else if (blockType==SKELETON)
        maxObject=(Animatable *)readSkeletonBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
    else if (blockType==SKELETON_POSE)
        maxObject=(Animatable *)readSkeletonPoseBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
    else if (blockType==SKELETON_ANIM)
        maxObject=(Animatable *)readSkeletonAnimBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);

    if (isAWD2_1){
        if (blockType==PRIM_GEOM)
            maxObject=(Animatable *)readPrimGeomBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==SKYBOX)
            maxObject=(Animatable *)readSkyBoxBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==LIGHT)
            maxObject=(Animatable *)readLightBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==CAMERA)
            readCameraBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==LIGHTPICKER)
            maxObject=(Animatable *)readLightPickerBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==CUBE_TEXTURE)
            maxObject=(Animatable *)readCubeTextureBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==CUBE_TEXTURE_ATF)
            maxObject=(Animatable *)readCubeTextureATFBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==EFFECT_METHOD)
            maxObject=(Animatable *)readEffectMethodBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==SHADOW_METHOD)
            maxObject=(Animatable *)readShadowtMethodBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==VERTEX_POSE)
            maxObject=(Animatable *)readVertexPoseBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==VERTEX_ANIM)
            maxObject=(Animatable *)readVertexAnimBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==ANIMATION_SET)
            maxObject=(Animatable *)readAnimationSetBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==ANIMATOR)
            maxObject=(Animatable *)readAnimatorBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==UV_ANIM)
            maxObject=(Animatable *)readUVAnimationBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==COMMAND)
            maxObject=(Animatable *)readCommandBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==NAMESPACE)
            maxObject=(Animatable *)readNameSpaceBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else if (blockType==METADATA)
            maxObject=(Animatable *)readMetadataBlock(newAWDBlock, accuracyMatrix, accuracyGeo, accuracyProps,fd);
        else{
        }
    }

    cache->Set(newAWDBlock, maxObject);
    int endBlockPos = ftell(fd);
    if (calculatedBlockEndPos!=endBlockPos)
        fseek(fd, calculatedBlockEndPos, SEEK_SET);
    return NULL;
}

SceneBlock_struct MaxAWDImporter::read_sceneblock_common(bool mtx_prec , FILE * fd){
    SceneBlock_struct returnStruct;
    awd_baddr parentID;
    returnStruct.parent=NULL;
    returnStruct.mtx=NULL;
    returnStruct.name=NULL;
    if(!(fread(&parentID,4,1,fd)))
        return returnStruct;// No data!
    // todo: get parent from cache
    if(mtx_prec){
        awd_float64 * thisMtx=(awd_float64*)malloc(sizeof(awd_float64)*12);
        if(!(fread(thisMtx,8,12,fd)))
            return returnStruct;// No data!
        returnStruct.mtx=thisMtx;
    }
    else{
        awd_float32 * thisMtx=(awd_float32*)malloc(sizeof(awd_float32)*12);
        if(!(fread(thisMtx,4,12,fd)))
            return returnStruct;// No data!
        //returnStruct.mtx=thisMtx;
    }
    returnStruct.name=read_varstring(fd);
    return returnStruct;
}
void * MaxAWDImporter::readTriGeomBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readContainerBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    SceneBlock_struct headerData=read_sceneblock_common(mtx_prec, fd);
    ImpNode *node = this->imp_i->CreateNode();
    if(!node) {
        return NULL;
        }
    DummyObject * dummy = new DummyObject();
    dummy->SetBox(Box3(
        -Point3(0.5f,0.5f,0.5f),
         Point3(0.5f,0.5f,0.5f)));
    node->Reference(dummy);
    Matrix3 tm;
    tm.IdentityMatrix();// Reset initial matrix to identity
    node->SetTransform(0,tm);
    this->imp_i->AddNodeToScene(node);
	node->SetName(A2W(headerData.name));
    return node;
}
void * MaxAWDImporter::readMeshInstanceBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readMaterialBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readTextureBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readSkeletonBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    
    BitmapTex *bmt = NewDefaultBitmapTex();
    return NULL;
}
void * MaxAWDImporter::readSkeletonPoseBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readSkeletonAnimBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readPrimGeomBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readSkyBoxBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readCameraBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readLightBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readLightPickerBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readCubeTextureBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readCubeTextureATFBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readEffectMethodBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readShadowtMethodBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readVertexPoseBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readVertexAnimBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readAnimationSetBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readAnimatorBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readUVAnimationBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readCommandBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readNameSpaceBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
void * MaxAWDImporter::readMetadataBlock(ImportedAWDBlock * thisBlock, bool mtx_prec, bool geom_prec, bool prop_prec , FILE * fd ){
    return NULL;
}
/*
TriObject * MaxAWDImporter::createTriangleMesh(const std::vector<Point3> &points,
    const std::vector<Point3> &normals,
    const std::vector<Point2> &uvs,
    const std::vector<int> &triangleVertIndices)
    {
    TriObject *triobj = CreateNewTriObject();
    if (triobj == NULL)
    return NULL;

    assert(points.size() == normals.size() && normals.size() == uvs.size());
    assert(triangleVertIndices.size() % 3 == 0);

    int numVertices = (int) points.size();
    int numTriangles = (int) triangleVertIndices.size() / 3;
    Mesh &mesh = triobj->GetMesh();

    // set vertex positions
    mesh.setNumVerts(numVertices);
    for (int i = 0; i < numVertices; i++)
    mesh.setVert(i, points[i]);
    // set vertex normals
    mesh.SpecifyNormals();
    MeshNormalSpec *normalSpec = mesh.GetSpecifiedNormals();
    normalSpec->ClearNormals();
    normalSpec->SetNumNormals(numVertices);
    for (int i = 0; i < numVertices; i++)
    {
    normalSpec->Normal(i) = normals[i].Normalize();
    normalSpec->SetNormalExplicit(i, true);
    }

    // set UVs
    // TODO: multiple map channels?
    // channel 0 is reserved for vertex color, channel 1 is the default texture mapping
    mesh.setNumMaps(2);
    mesh.setMapSupport(1, TRUE); // enable map channel
    MeshMap &map = mesh.Map(1);
    map.setNumVerts(numVertices);
    for (int i = 0; i < numVertices; i++)
    {
    UVVert &texVert = map.tv[i];
    texVert.x = uvs[i].x;
    texVert.y = uvs[i].y;
    texVert.z = 0.0f;
    }

    // set triangles
    mesh.setNumFaces(numTriangles);
    normalSpec->SetNumFaces(numTriangles);
    map.setNumFaces(numTriangles);
    for (int i = 0, j = 0; i < numTriangles; i++, j += 3)
    {
    // three vertex indices of a triangle
    int v0 = triangleVertIndices[j];
    int v1 = triangleVertIndices[j+1];
    int v2 = triangleVertIndices[j+2];

    // vertex positions
    Face &face = mesh.faces[i];
    face.setMatID(1);
    face.setEdgeVisFlags(1, 1, 1);
    face.setVerts(v0, v1, v2);

    // vertex normals
    MeshNormalFace &normalFace = normalSpec->Face(i);
    normalFace.SpecifyAll();
    normalFace.SetNormalID(0, v0);
    normalFace.SetNormalID(1, v1);
    normalFace.SetNormalID(2, v2);

    // vertex UVs
    TVFace &texFace = map.tf[i];
    texFace.setTVerts(v0, v1, v2);
    }

    mesh.InvalidateGeomCache();
    mesh.InvalidateTopologyCache();

    return triobj;
}*/