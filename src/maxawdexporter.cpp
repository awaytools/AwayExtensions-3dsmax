/*#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif*/

#include "maxawdexporter.h"

#define MaxAWDExporter_CLASS_ID    Class_ID(0xa8e047f2, 0x81e112c0)

char * W2A( const TCHAR *s ) {
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

class MaxAWDExporterClassDesc : public ClassDesc2
{
public:
    virtual int IsPublic()                             { return TRUE; }
    virtual void* Create(BOOL /*loading = FALSE*/)     { return new MaxAWDExporter(); }
    virtual const TCHAR *    ClassName()               { return GetString(IDS_CLASS_NAME); }
    virtual SClass_ID SuperClassID()                   { return SCENE_EXPORT_CLASS_ID; }
    virtual Class_ID ClassID()                         { return MaxAWDExporter_CLASS_ID; }
    virtual const TCHAR* Category()                    { return GetString(IDS_CATEGORY); }

    virtual const TCHAR* InternalName()                { return _T("MaxAWDExporter"); }         // returns fixed parsable name (scripter-visible name)
    virtual HINSTANCE HInstance()                      { return hInstance; }                    // returns owning module handle
};

ClassDesc2* GetMaxAWDExporterDesc() {
    static MaxAWDExporterClassDesc MaxAWDExporterDesc;
    return &MaxAWDExporterDesc;
}

/**
 * Invoked by the Interface::ProgressStart() method to execute
 * the export operation while showing the progress bar. The
 * argument is specified by MaxAWDExporter::DoExport() as the
 * instance of the exporter.
*/
static DWORD WINAPI ExecuteExportCallback(LPVOID arg)
{
    MaxAWDExporter *exporter = (MaxAWDExporter*)arg;
    return exporter->ExecuteExport();
}

MaxAWDExporter::MaxAWDExporter()
{
    this->hasExcludedLayers=false;
    this->hasExcludedObjects=false;
    exportAll=TRUE;
    error = false;
    awdFullPath = NULL;
}

MaxAWDExporter::~MaxAWDExporter()
{
}

int MaxAWDExporter::ExtCount()
{
    return 1;
}

void MaxAWDExporter::output_debug_string(const char* str)
{
    TSTR varName3 = _T("print \"");

    // uncommend to test if memory leak-detection is working:
    //AWDBlockList * thisIsAMemoryLeak = new AWDBlockList();

    OutputDebugStringA(str);
    OutputDebugStringA("\r\n");
    TCHAR * maxScriptOutput=A2W(str);
    varName3.Append(maxScriptOutput);
    varName3.Append(_T("\""));
    BOOL res3 = ExecuteMAXScriptScript(varName3, TRUE);
    free(maxScriptOutput);
}

const TCHAR *MaxAWDExporter::Ext(int n)
{
    return _T("awd");
}

const TCHAR *MaxAWDExporter::LongDesc()
{
    return _T("Away3D AWD2.1 File");
}

const TCHAR *MaxAWDExporter::ShortDesc()
{
    return _T("Away3D");
}

const TCHAR *MaxAWDExporter::AuthorName()
{
    return _T("The Away3D Team");
}

const TCHAR *MaxAWDExporter::CopyrightMessage()
{
    return _T("Copyright 2014 The Away3D Team");
}

const TCHAR *MaxAWDExporter::OtherMessage1()
{
    return _T("");
}

const TCHAR *MaxAWDExporter::OtherMessage2()
{
    return _T("");
}

unsigned int MaxAWDExporter::Version()
{
    return 102;
}

void MaxAWDExporter::ShowAbout(HWND hWnd)
{
    // Optional
}

BOOL MaxAWDExporter::SupportsOptions(int ext, DWORD options)
{
    return TRUE;
}

void MaxAWDExporter::DieWithError(void)
{
    error = true;
}

void MaxAWDExporter::DieWithErrorMessage(char *message, char *caption)
{
    if (!suppressDialogs) {
        Interface *i = GetCOREInterface();
        MessageBoxA(i->GetMAXHWnd(), message, caption, MB_OK);
    }

    DieWithError();
}

void MaxAWDExporter::UpdateProgressBar(int phase, double phaseProgress)
{
    int phaseStart=0;
    int phaseFinish=20;
    TCHAR *title=NULL;

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

int MaxAWDExporter::DoExport(const TCHAR *path,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)

{
    output_debug_string("\n\n\nStart AWD 2.1 Export");
    awdFullPath = W2A(path);
    maxInterface = i;
    suppressDialogs = suppressPrompts;

    // make shure the file extension is not written in capital letters
    // i could not figure out how to tell 3dsmax to use non-capital extension.
    // changing the path here is a workarround that seams to work fine
    char awdDrive[4];
    char awdPath[1024];
    char awdName[256];
    char * outAWDPath = (char *)malloc(sizeof(char)*1024);
    _splitpath_s(awdFullPath, awdDrive, 4, awdPath, 1024, awdName, 256, NULL, 0);
    _makepath_s(outAWDPath, 1024, awdDrive, awdPath, awdName, "awd");
    free(awdFullPath);// free original pointer (the "*.AWD")
    awdFullPath=outAWDPath;// set pointer to new path (the "*.awd")

    srand((unsigned)time(0));// init the random seed
    output_debug_string("output path:");
    output_debug_string(awdFullPath);
    // Open the dialog (provided that prompts are not suppressed) and
    // if it returns false, return to cancel the export.
    opts = new MaxAWDExporterOpts(awdFullPath);
    if (!suppressPrompts && !opts->ShowDialog()) {
        delete opts;
        return true;
    }
    // Open file and check for success
    fd = open(awdFullPath, _O_TRUNC | _O_CREAT | _O_BINARY | _O_RDWR, _S_IWRITE);
    if (fd == -1){
        output_debug_string("could not open the output path for writing!!!\nExport cancelled!!!");
        return FALSE;
    }
    // check if only selected objects should be exported
    if (options==SCENE_EXPORT_SELECTED)
        exportAll=FALSE;

    // Execute export while showing a progress bar. Send this as argument
    // to the execute callback, which will invoke MaxAWDExporter::ExecuteExport();
    maxInterface->ProgressStart(TEXT("Exporting AWD file"), TRUE, ExecuteExportCallback, this);

    // Export worked - all memory should be freed by now.
    // except for the awd-output-path. so free it now:
    free(awdFullPath);

    // check for memory-leaks:
    //_CrtDumpMemoryLeaks();

    // exporting is finished.
    return TRUE;
}

int MaxAWDExporter::ExecuteExport()
{
    // if 'splitByRoot' is true, we will create sepperate AWD-Files for sepperate objects.
    // this is disabled for now, and will most likly cause problems.
    // this was working at some time, but since than a lot has changed...
    bool splitByRoot = false;//(!suppressDialogs && opts->SplitByRootObj());

    // create BlockSettings that will be passed to the encode functions.
    // we can have different BlockSettings for AWDBlocks later
    // TODO: Storage-Precision is not implemented yet. for now its allways set to false (FLOAT32)
    bool precision_mtx=false;
    if (opts->StorageMatrix()==1)
        precision_mtx=true;
    bool precision_geo=false;
    if (opts->StorageGeometry()==1)
        precision_geo=true;
    bool precision_props=false;
    if (opts->StorageProperties()==1)
        precision_props=true;
    bool precision_attr=false;
    if (opts->StorageAttributes()==1)
        precision_attr=true;
    BlockSettings * awdBlockSettings=new BlockSettings(precision_mtx,precision_geo,precision_props,precision_attr, opts->Scale());
    output_debug_string("-> Prepare Export (create AWD-Object and Cache-Objects)");
    // prepare export:
    // create all the different cache-objects and the main AWD-object
    // pass the previous decalred properties, and make then available in the AWD-Object
    PrepareExport(splitByRoot, awdBlockSettings);

    // Get total number of nodes for progress calculation
    INode *root = maxInterface->GetRootNode();
    numNodesTotal = CalcNumDescendants(root);
    // Die if error occurred.
    DIE_IF_ERROR();

    numNodesTraversed = 0;
    // Preprocess SceneGraph:
    // - create a cache that contains all objects that should be excluded from objects
    //        reasons to be excluded:
    //            - AWDObjectSettings.exclude is set to false (Custom-object-settings)
    //            - obj is used as joint. (joints can be exported only as skeletons, not as scenegraph objects)
    //            - obj is bone or part of a biped.
    // - check for CustomAWDModifiers on all SceneObjects
    //        since some CustomAWDModifiers can be used by other objects than the ones they are applied to,
    //        we check for CustomAWDModifiers even on excluded objects,
    //        so we are shure we can access all CustomAWDModifiers needed for included objects.
    // - create a cache for all included light-objects.
    //        the scenegraph-information for lights (matrix + parent) will be added later when parsing the scenegraph.
    //        By preparing this cache, we can calculate the lightpicker for objects, while exporting the scenegraph later.

    output_debug_string("-> PreProcess Scenegraph (Step1)");
    PreProcessSceneGraph(root, false, awdBlockSettings);
    output_debug_string("       -> Results");
    char * resultString=NULL;
    resultString = (char *)malloc(255);
    _snprintf_s(resultString, 255, _TRUNCATE, "           -> Total Scene-Objects found:%d",
        int(numNodesTotal));
    output_debug_string(resultString);
    free(resultString);
    resultString = (char *)malloc(255);
    _snprintf_s(resultString, 255, _TRUNCATE, "           -> Scene-Objects excluded from export:%d",
        int(allExcludedCache->GetItemCount()));
    output_debug_string(resultString);
    free(resultString);
    // Die if error occurred
    DIE_IF_ERROR();

    output_debug_string("-> Process Scenegraph (Step2)");
    // Traverse node tree again for scene objects
    // (including their geometry, materials, etc)
    numNodesTraversed = 0;
    ProcessSceneGraph(root, NULL, awdBlockSettings, false);
    output_debug_string("-> End Process Scenegraph (Step2)");
    DIE_IF_ERROR();

    output_debug_string("-> Start Process Geometries (Step3)");
    ProcessGeoBlocks();
    output_debug_string("-> End Process Geometries (Step3)");
    DIE_IF_ERROR();

    CreateDarkLights();

    output_debug_string("-> Start Process Animations (Step4)");
    UpdateProgressBar(MAXAWD_PHASE_PROCESS_ANIMATIONS, 0);
    ExportAnimations(awdBlockSettings);
    UpdateProgressBar(MAXAWD_PHASE_PROCESS_ANIMATIONS, 1);
    output_debug_string("-> End Process Animations (Step4)");
    DIE_IF_ERROR();

    int blockCnt=awd->count_all_valid_blocks();
    if (blockCnt==0){
        AWDBlockList * messageBlocks=awd->get_message_blocks();
        messageBlocks=awd->get_message_blocks();
        AWDMessageBlock *block;
        AWDBlockIterator it(messageBlocks);
        if (!suppressDialogs) {
            if(messageBlocks->get_num_blocks()>0){
                output_debug_string("Warnings:");
                while ((block = (AWDMessageBlock *)it.next()) != NULL) {
                    if (block->get_message_len())
                        output_debug_string(block->get_message());
                }
            }
        }
        Interface *i = GetCOREInterface();
        if(this->hasExcludedLayers){
            MessageBoxA(i->GetMAXHWnd(), "Nothing to export!\nThere are objects on excluded layers!", "AWDWarning", MB_OK);
        }
        else if(this->hasExcludedObjects){
            MessageBoxA(i->GetMAXHWnd(), "Nothing to export!\nThere has been objects excluded from export!", "AWDWarning", MB_OK);
        }
        else if(messageBlocks->get_num_blocks()>0){
            MessageBoxA(i->GetMAXHWnd(), "Nothing to export!\nThere has been Warnings printed to the maxscript exporter!", "AWDWarning", MB_OK);
        }
        else{
            MessageBoxA(i->GetMAXHWnd(), "Nothing to export!\nScene seams to be empty!", "AWDWarning", MB_OK);
        }
    }
    else{
        //create the metadatablock
        AWDMetaData *newMetaData=new AWDMetaData();
        char * thisName=W2A(_T("Autodesk 3dsMax"));
        char buffer [10];
        char * generatorVersion=itoa (int(MAX_PRODUCT_YEAR_NUMBER),buffer,10);
        newMetaData->set_generator_metadata(thisName, generatorVersion);
        free(thisName);
        if(!opts->ExportMeta())
            newMetaData->make_invalide();
        awd->set_metadata(newMetaData);

        if (!opts->ExportSkeletons()){
            AWDBlockList * skeletonBlocks=awd->get_skeleton_blocks();
            AWDBlock *skelBlock;
            AWDBlockIterator it(skeletonBlocks);
            while ((skelBlock = (AWDBlock *)it.next()) != NULL) {
                skelBlock->make_invalide();
            }
        }
        if (!opts->ExportSkelAnim()){
            AWDBlockList * skelAnimSets=awd->get_amin_set_blocks();
            AWDAnimationSet *skelAnimSetBlock;
            AWDBlockIterator it(skelAnimSets);
            while ((skelAnimSetBlock = (AWDAnimationSet *)it.next()) != NULL) {
                if(skelAnimSetBlock->get_anim_type()==ANIMTYPESKELETON){
                    skelAnimSetBlock->make_invalide();
                }
            }
        }
        if (!opts->ExportVertexAnim()){
            AWDBlockList * allAnimSets=awd->get_amin_set_blocks();
            AWDAnimationSet *vertexAnimSetBlock;
            AWDBlockIterator it(allAnimSets);
            while ((vertexAnimSetBlock = (AWDAnimationSet *)it.next()) != NULL) {
                if(vertexAnimSetBlock->get_anim_type()==ANIMTYPEVERTEX){
                    vertexAnimSetBlock->make_invalide();
                }
            }
        }
        if (!opts->ExportScene()){
            AWDBlockList * allsceneBlock=awd->get_scene_blocks();
            AWDSceneBlock *sceneBlock;
            AWDBlockIterator it(allsceneBlock);
            while ((sceneBlock = (AWDSceneBlock *)it.next()) != NULL) {
                sceneBlock->make_invalide();
                sceneBlock->make_children_invalide();
            }
        }
        if (!opts->ExportMaterials()){
            AWDBlockList * allmatBlock=awd->get_material_blocks();
            AWDMaterial *matBlock;
            AWDBlockIterator it(allmatBlock);
            while ((matBlock = (AWDMaterial *)it.next()) != NULL) {
                matBlock->make_invalide();
            }
        }
        UpdateProgressBar(MAXAWD_PHASE_FLUSH, 0);
        output_debug_string("-> Start flush (Step5)");
        awd->flush(fd);
        output_debug_string("-> End flush (Step5)");
        close(fd);
        UpdateProgressBar(MAXAWD_PHASE_FLUSH, 1);
        AWDBlockList * messageBlocks=awd->get_message_blocks();
        AWDMessageBlock *block;
        AWDBlockIterator it(messageBlocks);
        if (!suppressDialogs) {
            if(messageBlocks->get_num_blocks()>0){
                Interface *i = GetCOREInterface();
                MessageBoxA(i->GetMAXHWnd(), "The export was not without issues.\nWarnings have been printed to the maxscript listener", "AWDWarning", MB_OK);
                output_debug_string("Warnings:");
                while ((block = (AWDMessageBlock *)it.next()) != NULL) {
                    if (block->get_message_len())
                        output_debug_string(block->get_message());
                }
            }
        }
        // if launch is true, we will open the AWD-File in AwayBuilder later.
        int launch = opts->LaunchPreview();
        if (launch==1){// open file with awayBuilder
            TCHAR * awdFullPath_tchar=A2W(awdFullPath);
            ShellExecute(NULL, _T("open"), awdFullPath_tchar, NULL, NULL, SW_SHOWNORMAL);
            free(awdFullPath_tchar);
        }

        if (launch>1){// open file with AWD-Viewer
            bool usenetworkViewer=false;
            if (launch==3)
                usenetworkViewer=true;
            CopyViewer(usenetworkViewer);
        }
    }
    maxInterface->ProgressEnd();
    CleanUp();

    return TRUE;
}

// ---> functions called by ExecuteExport():

void MaxAWDExporter::PrepareExport(bool splitByRoot, BlockSettings * thisBlockSettings)
{
    nodesToBaseObjects = new BlockCache(); // cache INode to BaseObject
    sceneBlocks = new BlockCache(); // cache INode to AWDSceneBlock
    primGeocache = new BlockCache(); // cache 3dsmax-object to AWDPrimitive
    INodeToGeoBlockCache = new BlockCache(); // cache AWDTriGeom to INode
    geometryCache = new BlockCache(); // cache 3dsmax-object to AWDTriGeom
    combinedGeosCache = new BlockCache();
    materialCache = new BlockCache(); // cache 3dsmax material to AWDMaterial-Block
    animSetsCache = new BlockCache(); // cache AWDAnimationSet-Modifier to AWDAnimationSet-Block
    animatorCache = new BlockCache(); // cache AWDAnimator-Modifier or AWDSkeleton-Block to AWDAnimator-Block
    cubeMatCache = new BlockCache(); // cache AWDCubeMaterial to AWDCubeTexture-block
    autoApplyMethodsToINodeCache = new BlockCache();

    allExcludedCache = new BoolCache(); // cache INode to bool.
    allBonesCache = new BoolCache(); // cache INode to bool.

    lightCache = new LightCache(); // cache INode + excludedList to AWDLight-Block - also provides function to calculate Lightpickers for a mesh
    colMtlCache = new ColorMaterialCache(); // cache color to AWDMaterial-Block
    skeletonCache = new SkeletonCache(); // cache color to AWDMaterial-Block

    shadowMethodsCache = new StringCache();
    textureProjectorCache = new StringCache();
    textureCache = new StringCache();
    methodsCache = new StringCache();
    clipsOnSkelsCache = new StringCache();
    animSourceForAnimSet = new StringCache();
    animSetsIDsCache = new StringCache();
    animClipsIDsCache = new StringCache();
    animClipsNamesCache = new StringCache();
    sourceModsIDsCache = new StringCache();
    sourceObjsIDsCache = new StringCache();
    geomsForVertexAnim = new StringCache();
    vetexAnimNeutralPosecache = new IntCache();

    int flags = 0;
    //bit1 = streaming
    if (thisBlockSettings->get_wide_matrix())//bit2 = storagePrecision Matrix
        flags |= 0x02;
    if (thisBlockSettings->get_wide_geom())//bit3 = storagePrecision Geo
        flags |= 0x04;
    if (thisBlockSettings->get_wide_props())//bit4 = storagePrecision Props
        flags |= 0x08;
    if (thisBlockSettings->get_wide_attributes())//bit5 = storagePrecision attributes
        flags |= 0x10;
    if (opts->ExportNormals())//bit6 = storeNormals
        flags |= 0x20;
    //bit7 = storeTangents //flags |= 0x40;
    if (opts->TextureMode()==1)//bit8 = embbedTetures
        flags |= 0x80;
    awd = new AWD((AWD_compression)opts->Compression(), flags, awdFullPath, splitByRoot, thisBlockSettings, opts->ExportEmptyContainers());
    ns = NULL;
}

void MaxAWDExporter::CleanUp()
{
    delete awd;
    delete opts;

    delete sceneBlocks;
    delete nodesToBaseObjects;
    delete primGeocache;
    delete INodeToGeoBlockCache;
    delete allExcludedCache;
    delete allBonesCache;
    delete geometryCache;
    combinedGeosCache->DeleteVals();// because the stored lists are not available else where, we need to delete them
    delete combinedGeosCache;
    delete materialCache;
    delete animSetsCache;
    delete animatorCache;
    delete cubeMatCache;
    autoApplyMethodsToINodeCache->DeleteVals();// because the stored lists are not available else where, we need to delete them
    delete autoApplyMethodsToINodeCache;

    delete lightCache;
    delete colMtlCache;
    delete skeletonCache;

    delete shadowMethodsCache;
    delete textureProjectorCache;
    delete textureCache;
    methodsCache->DeleteVals();// because the stored lists are not available else where, we need to delete them
    delete methodsCache;
    delete clipsOnSkelsCache;
    animSourceForAnimSet->DeleteVals();// because the stored lists are not available else where, we need to delete them
    delete animSourceForAnimSet;
    delete animSetsIDsCache;
    delete animClipsIDsCache;
    delete sourceObjsIDsCache;
    delete vetexAnimNeutralPosecache;
    delete animClipsNamesCache;
    delete sourceModsIDsCache;
    delete geomsForVertexAnim;
}