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

/*
#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif
   */

#include <cs/bipexp.h>
#include <Windows.h>
#include <icustattribcontainer.h>
#include <custattrib.h>
#include <iparamb2.h>
#include <MeshNormalSpec.h>
#include <direct.h>

#include "awd.h"
#include "method.h"
#include "util.h"
#include "platform.h"
#include "geomUtil.h"
#include "maxawdexporter.h"
#include "utils.h"

#include "msxml2.h"
#include "decomp.h"

#include "IGame.h"
#include "IGameObject.h"
#include "IGameProperty.h"
#include "IGameControl.h"
#include "IGameModifier.h"
#include "IConversionManager.h"
#include "IGameError.h"
#include "IGameFX.h"
#include "iSkinPose.h"
#include "maxscript/maxscript.h"
//#include "maxscrpt/maxscrpt.h"

#define MaxAWDExporter_CLASS_ID	Class_ID(0xa8e047f2, 0x81e112c0)

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

static unsigned char s_depth=0;
static void output_debug_string(const char* str)
{

    TSTR varName3 = _T("print \"");
    // append the sc-depth (tab for each scenegraph level...)
    for(unsigned char uc=0;uc<s_depth;uc++)
    {
        OutputDebugStringA("    ");
        varName3.Append(_T("    "));
    }

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

class MaxAWDExporterClassDesc : public ClassDesc2 
{
public:
    virtual int IsPublic() 							{ return TRUE; }
    virtual void* Create(BOOL /*loading = FALSE*/) 	{ return new MaxAWDExporter(); }
    virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
    virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
    virtual Class_ID ClassID() 						{ return MaxAWDExporter_CLASS_ID; }
    virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

    virtual const TCHAR* InternalName() 			{ return _T("MaxAWDExporter"); }		// returns fixed parsable name (scripter-visible name)
    virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
    

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

int	MaxAWDExporter::DoExport(const TCHAR *path,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)

{
    //_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    output_debug_string("\n\n\nStart AWD 2.1 Export");
    awdFullPath = W2A(path);
    maxInterface = i;
    suppressDialogs = suppressPrompts;
    // make shure the file extension is not written in capital letters
    // a bug in AwayBuilder prevents to open awd-files where extension = "*.AWD"
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
    
    output_debug_string("output path:");
    output_debug_string(awdFullPath);
    // Open the dialog (provided that prompts are not suppressed) and
    // if it returns false, return to cancel the export.
    opts = new MaxAWDExporterOpts();
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
    output_debug_string("-> Start Prepare Export (create AWD-Object and Cache-Objects)");
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
    //		reasons to be excluded: 
    //			- AWDObjectSettings.exclude is set to false (Custom-object-settings)
    //			- obj is used as joint. (joints can be exported only as skeletons, not as scenegraph objects)
    //			- obj is bone or part of a biped.
    // - check for CustomAWDModifiers on all SceneObjects 
    //		since some CustomAWDModifiers can be used by other objects than the ones they are applied to, 
    //		we check for CustomAWDModifiers even on excluded objects,
    //		so we are shure we can access all CustomAWDModifiers needed for included objects.
    // - create a cache for all included light-objects.
    //		the scenegraph-information for lights (matrix + parent) will be added later when parsing the scenegraph.
    //		By preparing this cache, we can calculate the lightpicker for objects, while exporting the scenegraph later.
    PreProcessSceneGraph(root, false);
    output_debug_string("-> End Preprocess Scenegraph (Step1)\n");
    output_debug_string("-> Total Scene-Objects:");
    char numNodesTotal_buf [10];
    output_debug_string(itoa (int(numNodesTotal),numNodesTotal_buf,10));
    output_debug_string("-> Excluded Scene-Objects:");
    char numNodesExlcuded_buf [10];
    output_debug_string(itoa (int(allExcludedCache->GetItemCount()),numNodesExlcuded_buf,10));
    // Die if error occurred
    DIE_IF_ERROR();	
    
    output_debug_string("\n-> Start Process Scenegraph (Step2)");
    // Traverse node tree again for scene objects
    // (including their geometry, materials, etc)
    numNodesTraversed = 0;
    ProcessSceneGraph(root, NULL);
    output_debug_string("-> End Process Scenegraph (Step2)\n");
    DIE_IF_ERROR();
    
    output_debug_string("-> Start Process Geometries (Step3)");
    ProcessGeoBlocks();
    output_debug_string("-> End Process Geometries (Step3)");
    DIE_IF_ERROR();	
    
    CreateDarkLights();

    output_debug_string("-> Start Process Animations (Step4)");
    UpdateProgressBar(MAXAWD_PHASE_PROCESS_ANIMATIONS, 0);
    ExportAnimations();
    UpdateProgressBar(MAXAWD_PHASE_PROCESS_ANIMATIONS, 1);
    output_debug_string("-> End Process Animations (Step4)");
    DIE_IF_ERROR();	

    //create the metadatablock
    AWDMetaData *newMetaData=new AWDMetaData();
    char * thisName=W2A(_T("Autodesk 3dsMax"));
    char buffer [10];
    char * generatorVersion=itoa (int(MAX_PRODUCT_YEAR_NUMBER),buffer,10);
    newMetaData->set_generator_metadata(thisName, generatorVersion);
    free(thisName);
    awd->set_metadata(newMetaData);
    
    UpdateProgressBar(MAXAWD_PHASE_FLUSH, 0);
    output_debug_string("-> Start flush (Step5)");
    awd->flush(fd);
    output_debug_string("-> End flush (Step5)");
    close(fd);
    UpdateProgressBar(MAXAWD_PHASE_FLUSH, 1);
    
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
    

    output_debug_string("-> Start Clean Up (Step6)");
    maxInterface->ProgressEnd();
    output_debug_string("-> End Clean Up (Step6)");
    CleanUp();

    return TRUE;
}

// ---> functions called by ExecuteExport():

void MaxAWDExporter::PrepareExport(bool splitByRoot, BlockSettings * thisBlockSettings)
{
    sceneBlocks = new BlockCache(); // cache INode to AWDSceneBlock
    primGeocache = new BlockCache(); // cache 3dsmax-object to AWDPrimitive
    INodeToGeoBlockCache = new BlockCache(); // cache AWDTriGeom to INode
    geometryCache = new BlockCache(); // cache 3dsmax-object to AWDTriGeom

    materialCache = new BlockCache(); // cache 3dsmax material to AWDMaterial-Block
    animSetsCache = new BlockCache(); // cache AWDAnimationSet-Modifier to AWDAnimationSet-Block
    animatorCache = new BlockCache(); // cache AWDAnimator-Modifier or AWDSkeleton-Block to AWDAnimator-Block
    cubeMatCache = new BlockCache(); // cache AWDCubeMaterial to AWDCubeTexture-block

    allExcludedCache = new BoolCache(); // cache INode to bool. 

    lightCache = new LightCache(); // cache INode + excludedList to AWDLight-Block - also provides function to calculate Lightpickers for a mesh
    colMtlCache = new ColorMaterialCache(); // cache color to AWDMaterial-Block
    skeletonCache = new SkeletonCache(); // cache color to AWDMaterial-Block

    textureCache = new StringCache();	
    methodsCache = new StringCache();
    clipsOnSkelsCache = new StringCache();
    animSourceForAnimSet = new StringCache();
    animSetsIDsCache = new StringCache();
    animClipsIDsCache = new StringCache();
    sourceObjsIDsCache = new StringCache();

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
    delete primGeocache;
    delete INodeToGeoBlockCache;
    delete allExcludedCache;
    delete geometryCache;

    delete materialCache;
    delete animSetsCache;
    delete animatorCache;
    delete cubeMatCache;
    
    delete lightCache;
    delete colMtlCache;
    delete skeletonCache;

    delete textureCache;	
    methodsCache->DeleteVals();
    delete methodsCache;
    delete clipsOnSkelsCache;
    animSourceForAnimSet->DeleteVals();
    delete animSourceForAnimSet;
    delete animSetsIDsCache;
    delete animClipsIDsCache;
    delete sourceObjsIDsCache;

    delete vetexAnimNeutralPosecache;
}

void MaxAWDExporter::PreProcessSceneGraph(INode *node, bool parentExcluded)
{	
    Object *obj;
    obj = node->GetObjectRef();
    SClass_ID sid;
    getBaseObjectAndID(obj, sid);
    bool excludeThis=parentExcluded;
    bool excludeChild=parentExcluded;
    if (obj){	
        // no matter if set to export or  "selectedObject"-export, 
        // we need to collect the AWDAnimationNodes, 
        // if we want Animation to be savly exported for a selected object. 
        BaseObject* node_bo = node->GetObjectRef();
        if(node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID)
        {
            // prevent reading AWD-modifier from the same instance
            bool isAllreadyCached=allExcludedCache->Get(obj);
            if (!isAllreadyCached){
                IDerivedObject* node_der = (IDerivedObject*)(node_bo);
                if (node_der!=NULL){
                    int nMods = node_der->NumModifiers();
                    for (int m = 0; m<nMods; m++){
                        Modifier* node_mod = node_der->GetModifier(m);
                        if (node_mod->IsEnabled()){
                            MSTR className;
                            node_mod->GetClassName(className);
                            char * className_ptr=W2A(className);
                            if (ATTREQ(className_ptr,"AWDEffectMethod")){
                                if (opts->IncludeMethods())
                                    ReadAWDEffectMethods(node_mod);
                            }
                            else if (ATTREQ(className_ptr,"AWDAnimationSet")){
                                if ((opts->ExportSkelAnim())||(opts->ExportVertexAnim()))
                                    ReadAWDAnimSet(node_mod);
                            }
                            else if (ATTREQ(className_ptr,"AWDSkeleton")){
                                allExcludedCache->Set(obj, true);
                                if (opts->ExportSkeletons())
                                    ReadAWDSkeletonMod(node_mod, node);
                            }
                            else if (ATTREQ(className_ptr,"AWDVertexAnimSource")){
                                if (opts->ExportVertexAnim())
                                    ReadAWDVertexMod(node_mod, node);
                            }
                            else if (ATTREQ(className_ptr,"AWDSkeletonClone")){
                                allExcludedCache->Set(obj, true);
                                if (opts->ExportSkelAnim())
                                    ReadAWDAnimSourceCloneMod(node_mod, node, ANIMTYPESKELETON);							
                            }
                            else if (ATTREQ(className_ptr,"AWDVertexAnimSourceClone")){
                                allExcludedCache->Set(obj, true);
                                if (opts->ExportVertexAnim())
                                    ReadAWDAnimSourceCloneMod(node_mod, node, ANIMTYPEVERTEX);
                            }

                            free(className_ptr);
                        }
                    }
                }
            }
        }
        // if the node is allready excluded from export by its parent, we dont need to read its custom AWDObjectSettings
        // and we dont need to check if its a light either...		
        if (excludeThis)
            allExcludedCache->Set(obj, true);
        else{
            BaseObject * node_bo = node->GetObjectRef();
            CustomAttributes_struct custAWDSettings;
            if(node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID){
                IDerivedObject * node_der = (IDerivedObject *)(node_bo);
                Object * newObj = node_der->GetObjRef();
                getBaseObjectAndID(newObj, sid);
                custAWDSettings = GetCustomAWDObjectSettings(newObj);
            }
            else{
                custAWDSettings = GetCustomAWDObjectSettings(obj);
            }
            if (!custAWDSettings.export_this){
                allExcludedCache->Set(obj, true);
            }
            excludeChild=false;
            if (!custAWDSettings.export_this_children){
                excludeChild=true;
            }
            // check if the node is a light, and if so, create a entry in the lightCache.
            if (sid==LIGHT_CLASS_ID){
                if (opts->IncludeLights()){
                    Interval valid;
                    AWD_light_type light_type= AWD_LIGHT_UNDEFINED;
                    if (obj->ClassID()==Class_ID(OMNI_LIGHT_CLASS_ID,0)){
                        light_type=AWD_LIGHT_POINT;
                    }
                    else if (obj->ClassID()==Class_ID(DIR_LIGHT_CLASS_ID,0)) {
                        light_type=AWD_LIGHT_DIR;
                    }
                    else if (obj->ClassID()==Class_ID(SPOT_LIGHT_CLASS_ID,0)) {
                        //ERROR - light will be converted to omni-light(pointlight)
                        light_type=AWD_LIGHT_POINT;
                    }
                    else if (obj->ClassID()==Class_ID(FSPOT_LIGHT_CLASS_ID,0)) {
                        //ERROR - light will be converted to omni-light(pointlight)
                        light_type=AWD_LIGHT_POINT;
                    }
                    if (light_type!=AWD_LIGHT_UNDEFINED){
                        char * lightName_ptr=W2A(node->GetName());
                        AWDLight * awdLight=new AWDLight(lightName_ptr, strlen(lightName_ptr));
                        free(lightName_ptr);
                        awdLight->set_light_type(light_type);
                        LightObject * lt= (LightObject *)obj;
                        GenLight *gl = (GenLight *)obj;
                        LightState ls;
                        lt->EvalLightState(0, valid, &ls);
                        ExclList * excludeList=lt->GetExclList();
                        bool includeObjs=lt->Include();					
                        double diffuse=lt->GetIntensity(0);
                        Point3 color = lt->GetRGBColor(0);
                        awd_color awdColor = createARGB(255, color.x*255, color.y*255, color.z*255);

                        awdLight->set_color(awdColor);
                        awdLight->set_diffuse(diffuse);
                        lightCache->Set(node,awdLight,excludeList, includeObjs);
                    }
                }
                
            }
        }
                
        // We want to exclude all objects that are used as bones. 
        // So we check for skin-modifier, no matter if the skinned object will be exported or not.
        // Get the skin - find all bones for the skin - add all the bones to the excluded cache:
        IDerivedObject *derivedObject = NULL;
        int skinIdx;
        ObjectState os;
        skinIdx = IndexOfSkinMod(node->GetObjectRef(), &derivedObject);
        ISkin *skin = NULL;
        if (derivedObject != NULL && skinIdx >= 0) {
            Modifier *mod = derivedObject->GetModifier(skinIdx);
            skin = (ISkin *)mod->GetInterface(I_SKIN);
        }
        if (skin && skin->GetNumBones()){
            int numBones=skin->GetNumBones();
            for (int i=0;i<numBones;i++){
                INode* curBone=skin->GetBone(i);
                SClass_ID sid;
                Object* thisobj=curBone->GetObjectRef();
                getBaseObjectAndID(thisobj, sid);
                allExcludedCache->Set(thisobj, true);	
            }
        }

        // This still can be a bone object that is not used by any skin. 
        // We dont want to export a sceneObject for that!
        if (obj->ClassID()==BONE_OBJ_CLASSID)
            allExcludedCache->Set(obj, true);
        
        // The object can be part of a biped.
        // We dont want to export a sceneObject for that!
        Control* control = node->GetTMController();
        if ( control )
        {
            Class_ID controllerClassId = control->ClassID();
            if (   controllerClassId == BIPSLAVE_CONTROL_CLASS_ID 
                || controllerClassId == BIPBODY_CONTROL_CLASS_ID 
                || controllerClassId == FOOTPRINT_CLASS_ID 
                || controllerClassId == BIPED_CLASS_ID )
                {
                allExcludedCache->Set(obj, true);
            }
        }
    }

    // update ProgressBar and call this function on all children of this obj (recursive)
    int i;
    int numChildren = node->NumberOfChildren();
    UpdateProgressBar(MAXAWD_PHASE_PREPROCESS_SCENEGRAPH, (double)numNodesTraversed/(double)numNodesTotal);
    for (i=0; i<numChildren; i++) {
        s_depth++;
        PreProcessSceneGraph(node->GetChildNode(i), excludeChild);
        s_depth--;
        RETURN_IF_ERROR;
    }
    
}

void MaxAWDExporter::ProcessSceneGraph(INode *node, AWDSceneBlock *parent)
{
    Object *obj;
    bool goDeeper;

    AWDSceneBlock *awdParent = NULL;

    // By default, also parse children of this node
    goDeeper = true;

    obj = node->GetObjectRef();				
    SClass_ID sid;
    getBaseObjectAndID(obj, sid);
    
    // if the obj can be found in the excluded cache, we can just return (no childs of this will be exported )
    bool isExcluded=allExcludedCache->Get(obj);
    if (isExcluded)
        return;
    else{
        if ( (node->Selected()!=0) || (parent!=NULL) || (exportAll) ){		
            if (obj) {	
                Matrix3 mtx = node->GetNodeTM(0) * Inverse(node->GetParentTM(0));
                double *mtxData = (double *)malloc(12*sizeof(double));
                SerializeMatrix3(mtx, mtxData);
                bool isExported=false;
                if (sid==CAMERA_CLASS_ID){
                    if ((opts->ExportScene())&&(opts->ExportCameras())){
                        output_debug_string("   -->Object will be exported as a Camera");
                        // hack: switch and negate axis of matrix because otherwise the camera looks up instead of front
                        awd_float64 store1 = mtxData[6];
                        awd_float64 store2 = mtxData[7];
                        awd_float64 store3 = mtxData[8];
                        mtxData[6]=-1* mtxData[3];
                        mtxData[7]=-1* mtxData[4];
                        mtxData[8]=-1* mtxData[5];
                        mtxData[3]=store1;
                        mtxData[4]=store2;
                        mtxData[5]=store3;

                        CameraObject *camObject= (CameraObject *)obj;

                        double fov=camObject->GetFOV(0);
                        //TODO: export camera properties
                        // check camera lens type and export
                        char * camName_ptr=W2A(node->GetName());
                        AWDCamera * awdCamera = new AWDCamera(camName_ptr, strlen(camName_ptr), AWD_LENS_PERSPECTIVE, mtxData);
                        free(camName_ptr);
                        if (parent) {
                            parent->add_child(awdCamera);
                        }
                        else {
                            awd->add_scene_block(awdCamera);
                        }
                        awdParent=awdCamera;
                    }
                    isExported=true;
                    //export camera
                }
                else if (sid==SHAPE_CLASS_ID){
                    isExported=true;
                    //shapes are not supported, so skip  this
                }
                else if (sid==HELPER_CLASS_ID){
                    //do nothing, export as ObjectContainer3D
                }
                else if (sid==MATERIAL_CLASS_ID){
                    isExported=true;
                    //ERROR material should not occur here....
                }
                else if (sid==LIGHT_CLASS_ID){
                    // lights should allready been created, but not included in the scene graph
                    // so we get the light from the cache and insert it into the scen graph
                    if ((opts->ExportScene())&&(opts->IncludeLights())){
                        output_debug_string("   -->Object will be exported as a Light");
                        AWDLight * awdLight=lightCache->Get(node);
                        if (awdLight==NULL){
                            int test=0;//ERROR SHOULD Always found the light !
                        }
                        else{
                            if (awdLight->get_light_type()==AWD_LIGHT_DIR){
                                awdLight->set_directionVec(-1*mtxData[3], -1*mtxData[4], -1*mtxData[5]);
                            }
                            awdLight->set_transform(mtxData);
                            if (parent) {
                                parent->add_child(awdLight);
                            }
                            else {
                                awd->add_scene_block(awdLight);
                            }
                            awdParent=awdLight;
                        }
                    }
                    isExported=true;
                }
                else if (sid==GEOMOBJECT_CLASS_ID){	
                    AWDAnimator * animatorBlock=NULL;					
                    BaseObject* node_bo = node->GetObjectRef();
                    // if the node has modifier applied, it might give back the wrong class_id, so get the correct obj:
                    if(node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID) {
                        IDerivedObject* node_der = (IDerivedObject*)(node_bo);								
                        node_bo = node_der->GetObjRef();
                        // get the first AWD-Animator-Modifier thats applied to this mesh (if this is no mehs, the animator will not be considered later)
                        animatorBlock=GetAWDAnimatorForObject(node);
                    }
                    MSTR className;
                    // check if this is a AWDSkyBox
                    node_bo->GetClassName(className);
                    char * className_ptr=W2A(className);
                    if (ATTREQ(className_ptr, "AWDSkyBox")) {
                        output_debug_string("   -->Object will be exported as a SkyBox");
                        char * skyBoxName_ptr=W2A(node->GetName());
                        AWDSkyBox * newSkyBox = new AWDSkyBox(skyBoxName_ptr, strlen(skyBoxName_ptr));
                        free(skyBoxName_ptr);
                        Mtl *mtl = node->GetMtl();
                        if (mtl!=NULL){
                            MSTR matClassName;
                            mtl->GetClassName(matClassName);
                            char * classNameCubeTex_ptr=W2A(matClassName);
                            if (ATTREQ(classNameCubeTex_ptr,"AWDCubeMaterial")){
                                output_debug_string("      -->Exporting Cubetexture for SkyBox");
                                AWDCubeTexture* newCubetex = GetCustomAWDCubeTexSettings((MultiMtl *)mtl);
                                if (newCubetex!=NULL){
                                    newSkyBox->set_cube_tex(newCubetex);
                                }
                            }
                            free(classNameCubeTex_ptr);
                        }
                        if (parent) {
                            //ERROR cannot Skybox cannot be a child (and skybox cannot be a parent, so we insert it as root object)
                            awd->add_scene_block(newSkyBox);
                        }
                        awd->add_scene_block(newSkyBox);
                        isExported=true;
                    }
                    else{	
                        // check if the object contains any triangles/faces. otherwise we can cancel here.
                        TriObject *triObject = (TriObject*)obj->ConvertToType(maxInterface->GetTime(), Class_ID(TRIOBJ_CLASS_ID, 0));
                        if (triObject!=NULL){
                            Mesh mesh = triObject->mesh;
                            if (mesh.getNumFaces()>0){
                                output_debug_string("   -->Object is a Geometry");
                                AWDBlockList * matBlocks=NULL;
                                if ((opts->ExportScene())&&(opts->ExportMaterials())) {
                                    output_debug_string("      -->Search for materials on object");
                                    matBlocks=GetMaterialsForMeshInstance(node);
                                }
                                if (opts->ExportGeometry()) {
                                    // if the mesh has a AWD-animator-modifier applied, we do not consider exporing it as primitive
                                    // if it has no AWD-animator-modifier applied, we check for AWD-AnimSources, that are set to auto-create AWDAnimators
                                        
                                    AWDTriGeom *awdGeom=NULL;	
                                    AWDPrimitive *awdPrimGeom=NULL;						
                                    // check if a Vertex-AnimSource-Modifier is applied to mesh (with settings: simpleMode=true)
                                    // if one is found, we create a AWDAnimator and Animationset for this.
                                    if (animatorBlock==NULL){
                                        if(opts->ExportVertexAnim()){
                                            output_debug_string("      -->Try to auto create Animation-Setup for VertexAnimation");
                                            animatorBlock=AutoCreateAnimatorForVertexAnim(node);
                                        }
                                        if (animatorBlock==NULL){
                                            if(opts->ExportSkelAnim()){
                                                output_debug_string("      -->Try to auto create Animation-Setup for SkeletonAnimation");
                                                animatorBlock=AutoCreateAnimatorForSkeleton(node);
                                            }
                                            // if still no AWD-animator has been created for this mesh, we check if we can export it as primitive
                                            if ((animatorBlock==NULL)&&((matBlocks==NULL)||((matBlocks!=NULL)&&(matBlocks->get_num_blocks()==1)))){
                                                if (opts->ExportPrimitives()){
                                                    output_debug_string("      -->Check if Object can be exported as Primitve");
                                                    char *bname = W2A(node->GetName());
                                                    char *primName_ptr = (char*)malloc(strlen(bname)+6);
                                                    strcpy(primName_ptr, bname);
                                                    strcat(primName_ptr, "_prim");
                                                    free(bname);
                                                    awdPrimGeom=ExportPrimitiveGeom(node_bo, primName_ptr);
                                                    free(primName_ptr);
                                                }
                                            }
                                        }
                                    }
                                    if (awdPrimGeom==NULL){
                                        awdGeom = (AWDTriGeom *)geometryCache->Get(obj);
                                        if (awdGeom == NULL){
                                            output_debug_string("      -->Create a TriGeom for the Object");
                                            char *bname = W2A(node->GetName());
                                            char *triGeoName_ptr = (char*)malloc(strlen(bname)+6);
                                            strcpy(triGeoName_ptr, bname);
                                            strcat(triGeoName_ptr, "_geom");
                                            free(bname);
                                            awdGeom = new AWDTriGeom(triGeoName_ptr, strlen(triGeoName_ptr));
                                            free(triGeoName_ptr);
                                            geometryCache->Set(obj, awdGeom);
                                            awd->add_mesh_data(awdGeom);
                                            INodeToGeoBlockCache->Set(awdGeom, node);
                                        }
                                        if (matBlocks != NULL){
                                            if (matBlocks->get_num_blocks()>1){
                                                awdGeom->set_split_faces(true);
                                            }
                                        }
                                    }
                                    if (opts->ExportScene()) {
                                        AWDLightPicker * lightPicker=NULL;
                                        if ((opts->ExportScene())&&(opts->ExportMaterials())&&(matBlocks != NULL)) {
                                            output_debug_string("      -->get the Lightpicker for the materials applied to the Object");
                                            lightPicker=lightCache->GetLightPickerForMesh(node);
                                            if(lightPicker!=NULL)
                                                awd->add_light_picker_block(lightPicker);
                                        }
                                        AWDMeshInst *awdMesh=NULL;
                                        if ((awdPrimGeom==NULL)&&(awdGeom==NULL)){
                                            //output_debug_string("print ('-->empty mesh')");
                                        }
                                        
                                        char * meshName_ptr=W2A(node->GetName());
                                        if (awdPrimGeom!=NULL){
                                            output_debug_string("      -->Create a MeshInstance for the Primitive");
                                            mtxData[10]+=awdPrimGeom->get_Yoffset();
                                            awdMesh = new AWDMeshInst(meshName_ptr, strlen(meshName_ptr), awdPrimGeom, mtxData);
                                            if (matBlocks!=NULL){
                                                AWDMaterial * newPrimMat=(AWDMaterial *)matBlocks->getByIndex(0);
                                                if(lightPicker!=NULL){
                                                    AWDMaterial * newPrimMatLP=newPrimMat->get_material_for_lightPicker(lightPicker);
                                                    if(opts->SetMultiPass()){
                                                        // multipass using the number of lights, that the lightpicker uses
                                                        newPrimMatLP->set_multiPass(false);
                                                        if (lightPicker->get_lights()->get_num_blocks()>3)
                                                            newPrimMatLP->set_multiPass(true);
                                                    }
                                                    awdMesh->add_material(newPrimMatLP);
                                                }
                                                else{
                                                    awdMesh->add_material(newPrimMat);
                                                }
                                                delete matBlocks;
                                            }
                                        }
                                        else if (awdGeom!=NULL){
                                            output_debug_string("      -->Create a MeshInstance for the TriGeom");
                                            awdMesh = new AWDMeshInst(meshName_ptr, strlen(meshName_ptr), awdGeom, mtxData);
                                            if (awdGeom != NULL){
                                                if (matBlocks!=NULL){
                                                    if (matBlocks->get_num_blocks()==1)
                                                        awdMesh->set_defaultMat(matBlocks->getByIndex(0));
                                                    else{
                                                        awdMesh->set_defaultMat(getColorMatForObject(node, true));

                                                    }
                                                    awdMesh->set_pre_materials(matBlocks);
                                                }
                                                awdGeom->get_mesh_instance_list()->append(awdMesh);
                                            }
                                            if (awdMesh) {
                                                if (lightPicker!=NULL)
                                                    awdMesh->set_lightPicker(lightPicker);
                                                if (animatorBlock!=NULL)
                                                    animatorBlock->add_target(awdMesh);
                                            }
                                        }										
                                        free(meshName_ptr);
                                        if (awdMesh!=NULL){
                                            output_debug_string("      -->Read Custom Attributes of the object");
                                            ExportUserAttributes(obj, awdMesh);
                                            output_debug_string("      -->Place object into scenegraph");
                                            if (parent) 
                                                parent->add_child(awdMesh);
                                            else
                                                awd->add_scene_block(awdMesh);
                                            awdParent = awdMesh;
                                        }
                                    }
                                    else{
                                    }
                                }
                                isExported=true;
                            }
                            else{
                            }
                        }
                    }
                    free(className_ptr);
                }
                if (!isExported){
                    output_debug_string("   -->NO OBJECT Created - create container");
                    if (opts->ExportScene()) {
                        char * containerName_ptr=W2A(node->GetName());
                        awdParent=new AWDContainer(containerName_ptr, strlen(containerName_ptr), mtxData);
                        ExportUserAttributes(obj, awdParent);
                        free(containerName_ptr);
                        if (parent)
                            parent->add_child(awdParent);
                        else
                            awd->add_scene_block(awdParent);
                    }
                    else{
                    }
                }
                free(mtxData);
            }
            else
            {
                // TODO: ERROR because no (3dsmax)obj was found (?)
            }
        }
    }
    numNodesTraversed++;

    if (goDeeper) {
        output_debug_string("");
        output_debug_string("   -->PARSE CHILDREN:");
        int i;
        int numChildren = node->NumberOfChildren();
        // Update progress bar before recursing
        UpdateProgressBar(MAXAWD_PHASE_EXPORT_SCENEGRAPH, (double)numNodesTraversed/(double)numNodesTotal);

        for (i=0; i<numChildren; i++) {
            s_depth++;
            ProcessSceneGraph(node->GetChildNode(i), awdParent);
            s_depth--;
            RETURN_IF_ERROR;
        }
    }
    else {
        // No need to traverse this branch further. Count all
        // descendants as traversed and update progress bar.
        numNodesTraversed += CalcNumDescendants(node);
        UpdateProgressBar(MAXAWD_PHASE_EXPORT_SCENEGRAPH, (double)numNodesTraversed/(double)numNodesTotal);
    }
}

void MaxAWDExporter::ProcessGeoBlocks()
{
    AWDTriGeom *geoBlock;
    AWDBlockIterator *it;
    int proccessed=0;
    it = new AWDBlockIterator(awd->get_mesh_data_blocks());
    UpdateProgressBar(MAXAWD_PHASE_PROCESS_GEOMETRY, (double)proccessed/(double)awd->get_mesh_data_blocks()->get_num_blocks());
    INodeTab lNodes;
    while ((geoBlock = (AWDTriGeom * ) it->next()) != NULL){
        INode * node = (INode *)INodeToGeoBlockCache->Get(geoBlock);
        if (node==NULL){
            //ERROR - THIS SHOULD NEVER HAPPEN
            return;
        }
        lNodes.AppendNode( node );
    }
                        
    IGameScene* _pIgame = NULL;	
    _pIgame = GetIGameInterface(); 
    _pIgame->InitialiseIGame( lNodes );
    it->reset();
    while ((geoBlock = (AWDTriGeom * ) it->next()) != NULL){
        INode * node = (INode *)INodeToGeoBlockCache->Get(geoBlock);
        if (node==NULL){
            //ERROR - THIS SHOULD NEVER HAPPEN
        }
        else{
            int exportThis=false;
            IGameObject * gobj = NULL;	
            IGameMesh * igame_mesh = NULL;	
            gobj = GetIGameInterface()->GetIGameNode(node)->GetIGameObject();	
            if(gobj->GetIGameType()==IGameObject::IGAME_MESH){
                igame_mesh = (IGameMesh*)gobj;
                if (igame_mesh!=NULL){
                    igame_mesh->InitializeData();
                    if(igame_mesh->GetNumberOfFaces()>0){
                        exportThis=true;
                    }
                }
            }
            if (exportThis){
                Object *obj;
                obj = node->GetObjectRef();			
                SClass_ID sid;
                getBaseObjectAndID(obj, sid);
                int skinIdx;
                ObjectState os;
                IDerivedObject *derivedObject = NULL;
                skinIdx = IndexOfSkinMod(node->GetObjectRef(), &derivedObject);
                if (skinIdx >= 0) {				
                    // Flatten all modifiers up to but not including
                    // the skin modifier.
                    // to do: get the correct time for the neutral-pose
                    os = derivedObject->Eval(0, skinIdx + 1);
                }
                else {
                    // Flatten entire modifier stack
                    // to do: get the correct time for the neutral-pose
                    os = node->EvalWorldState(maxInterface->GetTime());
                }
                obj = os.obj;
                ISkin *skin = NULL;				
                if (derivedObject != NULL && skinIdx >= 0) {
                    Modifier *mod = derivedObject->GetModifier(skinIdx);
                    skin = (ISkin *)mod->GetInterface(I_SKIN);
                }
                ExportTriGeom(geoBlock,obj,node,skin, igame_mesh);
                RETURN_IF_ERROR;
            }
        }
        proccessed++;
        UpdateProgressBar(MAXAWD_PHASE_PROCESS_GEOMETRY, (double)proccessed/(double)awd->get_mesh_data_blocks()->get_num_blocks());
    }	
    delete it;
    _pIgame->ReleaseIGame();
}

void MaxAWDExporter::ExportAnimations(){
    
    AWDAnimator *block;
    AWDBlockIterator *it;
    AWDBlockList * animatorBlocks = (AWDBlockList *)awd->get_animator_blocks();		
    if (animatorBlocks!=NULL){
        if (animatorBlocks->get_num_blocks()>0){
            int animCnt=0;
            it = new AWDBlockIterator(animatorBlocks);
            while ((block = (AWDAnimator * ) it->next()) != NULL) {
                AWDAnimationSet * animSet = block->get_animationSet();
                if (animSet!=NULL){
                    ExportAWDAnimSet(animSet, block->get_targets());
                }
                animCnt++;
                UpdateProgressBar(MAXAWD_PHASE_PROCESS_ANIMATIONS, (double)animCnt/(double)animatorBlocks->get_num_blocks());
            }
            delete it;
        }
    }
}


// ---> functions called by PreProcessSceneGraph():

CustomAttributes_struct MaxAWDExporter::GetCustomAWDObjectSettings(Animatable *obj)
{
    CustomAttributes_struct returnData;
    returnData.export_this=true;
    returnData.export_this_children=true;
    ICustAttribContainer *attributes = obj->GetCustAttribContainer();
    if (attributes) {
        int a=0;
        int numAttribs=0;
        numAttribs = attributes->GetNumCustAttribs();
        for (a=0; a<numAttribs; a++) {
            int p=0;
            int t=0;
            CustAttrib *attr = attributes->GetCustAttrib(a);
            for (t=0; t<attr->NumParamBlocks(); t++) {
                IParamBlock2 *block = attr->GetParamBlock(t);
                char * localName_ptr=W2A(block->GetLocalName());
                if (ATTREQ(localName_ptr,"AWD_Export") ){
                    for (p=0; p<block->NumParams(); p++) {
                        ParamID pid = block->IndextoID(p);
                        ParamDef def = block->GetParamDef(pid);
                        char * paramName_ptr=W2A(def.int_name);
                        if (block->GetParameterType(pid)==TYPE_BOOL){
                            if (ATTREQ(paramName_ptr,"Export") )
                                returnData.export_this= (0 != block->GetInt(pid));
                            else if (ATTREQ(paramName_ptr,"ExportChildren") )
                                returnData.export_this_children= (0 != block->GetInt(pid));
                        }
                        free(paramName_ptr);
                    }
                }
                free(localName_ptr);
            }
        }
    }	
    return returnData;
}

void MaxAWDExporter::ReadAWDSkeletonMod(Modifier *node_mod, INode * node){

    int num_params = node_mod->NumParamBlocks();						
    char * skeletonMod_ptr=NULL;						
    int p=0;
    int numBlockparams=0;
    int jpv=0;
    int neutralPose=0;
    bool simpleMode=true;
    IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
    if(pb!=NULL){
        numBlockparams=pb->NumParams();
        for (p=0; p<numBlockparams; p++) {
            ParamID pid = pb->IndextoID(p);
            ParamDef def = pb->GetParamDef(pid);
            ParamType2 paramtype = pb->GetParameterType(pid);	
            char * paramName_ptr=W2A(def.int_name);
            if (ATTREQ(paramName_ptr, "thisAWDID")){
                if (paramtype==TYPE_STRING)				
                    skeletonMod_ptr=W2A(pb->GetStr(pid));		
            }
            if (ATTREQ(paramName_ptr, "jointPerVert")){
                if (paramtype==TYPE_INT)				
                    jpv=pb->GetInt(pid);
            }	
            if (ATTREQ(paramName_ptr, "neutralPose")){
                if (paramtype==TYPE_INT)				
                    neutralPose=pb->GetInt(pid);
            }			
            if (ATTREQ(paramName_ptr, "simpleMode")){
                if (paramtype==TYPE_BOOL)				
                    simpleMode=(0 != pb->GetInt(pid));
            }	
            free(paramName_ptr);
        }
    }
    else{
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }		
    AWDSkeleton *awdSkel = skeletonCache->Add(node, skeletonMod_ptr, neutralPose*GetTicksPerFrame());
    awdSkel->set_joints_per_vert(jpv);
    awdSkel->set_simpleMode(simpleMode);
    awd->add_skeleton(awdSkel);
    char * skelName_ptr=W2A(node_mod->GetName());
    awdSkel->set_name(skelName_ptr, strlen(skelName_ptr));
    free(skelName_ptr);
    sourceObjsIDsCache->Set(skeletonMod_ptr,node);//using the custom-AWD-IDs of the custom-nodes as key in this cache, way we can access the correct source object (3dsmax-node) for each animClip later...

    AWDBlockList * thisSourceClips=(AWDBlockList *)animSourceForAnimSet->Get(skeletonMod_ptr);
    if (thisSourceClips==NULL){
        thisSourceClips = new AWDBlockList();
        animSourceForAnimSet->Set(skeletonMod_ptr, thisSourceClips);
    }
    pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 1);//second params contain the AnimationClips !
    if(pb!=NULL){
        ReadAWDClipsFromAWDModifier(pb, skeletonMod_ptr, thisSourceClips, ANIMTYPESKELETON);
    }
    else{
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }
    if (skeletonMod_ptr!=NULL)
        free(skeletonMod_ptr);
    // no need to read in the SkeletonClones-rollout  and the anim-set-rollout...
}

void MaxAWDExporter::ReadAWDClipsFromAWDModifier(IParamBlock2 *pb, const char * settingsNodeID_ptr, AWDBlockList * clipList, AWD_Anim_Type animType)
{
    int numBlockparams=pb->NumParams();
    int p=0;
    ParamID pid_names = NULL;
    ParamID pid_ids = NULL;
    ParamID pid_starts = NULL;
    ParamID pid_ends = NULL;
    ParamID pid_skips = NULL;
    ParamID pid_stitchs = NULL;
    int minCount=100000;
    for (p=0; p<numBlockparams; p++) {
        ParamID pid = pb->IndextoID(p);
        ParamDef def = pb->GetParamDef(pid);
        char * paramName = W2A(def.int_name);
        if (ATTREQ(paramName, "saveAnimClipNames")){
            pid_names=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveAnimSourceIDs")){
            pid_ids=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveStartFrames")){
            pid_starts=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveEndFrames")){
            pid_ends=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveSkipFrames")){
            pid_skips=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }
        if (ATTREQ(paramName, "saveStitchFinals")){
            pid_stitchs=pid;
            if (pb->Count(pid)<minCount){minCount=pb->Count(pid);}
        }	
        free(paramName);
    }
    if ((pid_names==NULL)||(pid_ids==NULL)||(pid_starts==NULL)||(pid_ends==NULL)||(pid_skips==NULL)||(pid_stitchs==NULL)){
        minCount=0;
    }
    if (minCount!=0){
        int clipCnt=0;
        for (clipCnt=0;clipCnt<minCount;clipCnt++){		
            char * thisName_ptr=W2A(pb->GetStr(pid_names, 0, clipCnt));
            char * awdID_ptr=W2A(pb->GetStr(pid_ids, 0, clipCnt));
            int startFrame = pb->GetInt(pid_starts, 0, clipCnt);
            int endFrame = pb->GetInt(pid_ends, 0, clipCnt);
            int skipFrames = pb->GetInt(pid_skips, 0, clipCnt);
            bool stitchFrames = (0 != pb->GetInt(pid_skips, 0, clipCnt));
            if (animType==ANIMTYPEVERTEX){
                AWDVertexAnimation * newVertexAnimClip = new AWDVertexAnimation(thisName_ptr, strlen(thisName_ptr), startFrame, endFrame, skipFrames, stitchFrames, settingsNodeID_ptr);
                clipList->append(newVertexAnimClip);
                output_debug_string("created vertexClip for vertexsource with id=");
                output_debug_string(settingsNodeID_ptr);
                animClipsIDsCache->Set(awdID_ptr, newVertexAnimClip);
                awd->add_vertex_anim_block(newVertexAnimClip);
            }
            else if (animType==ANIMTYPESKELETON){
                AWDSkeletonAnimation * newSkeletonAnimClip = new AWDSkeletonAnimation(thisName_ptr, strlen(thisName_ptr), startFrame, endFrame, skipFrames, stitchFrames, settingsNodeID_ptr);
                clipList->append(newSkeletonAnimClip);
                animClipsIDsCache->Set(awdID_ptr, newSkeletonAnimClip);
                awd->add_skeleton_anim(newSkeletonAnimClip);
            }
            free(thisName_ptr);
            free(awdID_ptr);
        }
    }
    pid_names = NULL;
    pid_ids = NULL;
    pid_starts = NULL;
    pid_ends = NULL;
    pid_skips = NULL;
    pid_stitchs = NULL;
}

void MaxAWDExporter::ReadAWDVertexMod(Modifier *node_mod, INode * node)
{
    output_debug_string("-> Found a active AWDVertexModifier");
    int num_params = node_mod->NumParamBlocks();						
    char * vertexAnimMod_ptr = NULL;				
    int p=0;
    int numBlockparams=0;
    bool simpleMode=true;
    IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
    if(pb!=NULL){
        numBlockparams=pb->NumParams();
        for (p=0; p<numBlockparams; p++) {
            ParamID pid = pb->IndextoID(p);
            ParamDef def = pb->GetParamDef(pid);	
            ParamType2 paramtype = pb->GetParameterType(pid);	
            char * paramName_ptr=W2A(def.int_name);
            if (ATTREQ(paramName_ptr, "thisAWDID")){
                if (paramtype==TYPE_STRING)				
                    vertexAnimMod_ptr=W2A(pb->GetStr(pid));		
            }	
            if (ATTREQ(paramName_ptr, "neutralPose")){
                if (paramtype==TYPE_INT){				
                    int neutralPose=pb->GetInt(pid);
                    vetexAnimNeutralPosecache->Set(node, neutralPose);
                }
            }			
            if (ATTREQ(paramName_ptr, "simpleMode")){
                if (paramtype==TYPE_BOOL)				
                    simpleMode=(0 != pb->GetInt(pid));
            }	
            free(paramName_ptr);
        }
    }
    else{
        output_debug_string("!!!!! ERROR COULD NOT READ THE PARAMBLOCK FOR THE AWDVERTEXMODIFIER-MAIN_PARAMS!!!!!");
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }	
    if(vertexAnimMod_ptr!=NULL){
        sourceObjsIDsCache->Set(vertexAnimMod_ptr,node);//using the custom-AWD-IDs of the custom-nodes as key in this cache, way we can access the correct source object (3dsmax-node) for each animClip later...
        AWDBlockList * thisSourceClips=(AWDBlockList *)animSourceForAnimSet->Get(vertexAnimMod_ptr);
        output_debug_string(vertexAnimMod_ptr);
        if (thisSourceClips==NULL){
            thisSourceClips = new AWDBlockList();
            animSourceForAnimSet->Set(vertexAnimMod_ptr, thisSourceClips);
        }
        pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 1);//second params contain the AnimationClips !
        if(pb!=NULL){
            ReadAWDClipsFromAWDModifier(pb, vertexAnimMod_ptr, thisSourceClips, ANIMTYPEVERTEX);
        }
        else{
            output_debug_string("!!!!! ERROR COULD NOT READ THE PARAMBLOCK FOR THE AWDVERTEXMODIFIER-ANIMATIONCLIPS!!!!!");
            //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
        }
        free(vertexAnimMod_ptr);
    }
    else
        output_debug_string("!!!!! ERROR COULD NOT READ THE ID FOR THE AWDVERTEXMODIFIER!!!!!");
}

void MaxAWDExporter::ReadAWDAnimSourceCloneMod(Modifier *node_mod, INode * node, AWD_Anim_Type animType){

    int num_params = node_mod->NumParamBlocks();						
    char * settingsNodeID_ptr=NULL;					
    char * targetID_ptr=NULL;						
    int p=0;
    int numBlockparams=0;
    IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
    if(pb!=NULL){
        numBlockparams=pb->NumParams();
        for (p=0; p<numBlockparams; p++) {
            ParamID pid = pb->IndextoID(p);
            ParamDef def = pb->GetParamDef(pid);	
            ParamType2 paramtype = pb->GetParameterType(pid);	
            if (paramtype==TYPE_STRING)	{
                char * paramName=W2A(def.int_name);
                if (ATTREQ(paramName, "thisAWDID"))							
                    settingsNodeID_ptr=W2A(pb->GetStr(pid));		
                if (ATTREQ(paramName, "target_ID"))
                    targetID_ptr=W2A(pb->GetStr(pid));	
                free(paramName);
            }									
        }
    }
    else{
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }							
    sourceObjsIDsCache->Set(settingsNodeID_ptr,node);//using the custom-AWD-IDs of the custom-nodes as key in this cache, way we can access the correct source object (3dsmax-node) for each animClip later...

    AWDBlockList * thisSourceClips=(AWDBlockList *)animSourceForAnimSet->Get(targetID_ptr);
    if (thisSourceClips==NULL){
        thisSourceClips = new AWDBlockList();
        animSourceForAnimSet->Set(targetID_ptr, thisSourceClips);
    }
    pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 1);//second params contain the AnimationClips !
    if(pb!=NULL){
        ReadAWDClipsFromAWDModifier(pb, settingsNodeID_ptr, thisSourceClips, animType);
    }
    else{
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }
    if(settingsNodeID_ptr!=NULL)
        free(settingsNodeID_ptr);
    if(targetID_ptr!=NULL)
        free(targetID_ptr);
}

void MaxAWDExporter::ReadAWDAnimSet(Modifier *node_mod){
    AWDAnimationSet * animSet = (AWDAnimationSet *)animSetsCache->Get(node_mod);
    if (animSet==NULL){
        // create a AnimationSet Block
        // if it is not in "simple mode", we need to get all the stored animclips-IDs and the corresponding sourceModifier-IDs
        int num_params = node_mod->NumParamBlocks();
        char * settingsNodeID_ptr=NULL;
        char * sourceSkeletonID_ptr=NULL;
        char * sourceVertexID_ptr=NULL;
        bool simpleMode=true;
        int animType=0;
        int p=0;
        IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
        if(pb!=NULL){
            int numBlockparams=pb->NumParams();
            for (p=0; p<numBlockparams; p++) {
                ParamID pid = pb->IndextoID(p);
                ParamDef def = pb->GetParamDef(pid);
                ParamType2 paramtype = pb->GetParameterType(pid);
                char * paramName=W2A(def.int_name);
                if (paramtype==TYPE_STRING) {
                    if (ATTREQ(paramName, "thisAWDID"))
                        settingsNodeID_ptr = W2A(pb->GetStr(pid));
                    if (ATTREQ(paramName, "sourceVertexID"))
                        sourceVertexID_ptr = W2A(pb->GetStr(pid));
                    if (ATTREQ(paramName, "sourceSkeletonID"))
                        sourceSkeletonID_ptr= W2A(pb->GetStr(pid));
                }
                if (ATTREQ(paramName, "animType")){
                    if (paramtype==TYPE_INT)
                        animType = pb->GetInt(pid);
                }
                if (ATTREQ(paramName, "simpleMode")){
                    if (paramtype==TYPE_BOOL)
                        simpleMode= (0 != pb->GetInt(pid));
                }
                free(paramName);
            }
                
            if ((animType==1)||(animType==2)){
                char * animSetname=W2A(node_mod->GetName());
                if (animType==1)
                    animSet = new AWDAnimationSet(animSetname, strlen(animSetname),ANIMTYPESKELETON, sourceSkeletonID_ptr, strlen(sourceSkeletonID_ptr));
                if (animType==2)
                    animSet = new AWDAnimationSet(animSetname, strlen(animSetname),ANIMTYPEVERTEX, sourceVertexID_ptr, strlen(sourceVertexID_ptr));
                animSetsIDsCache->Set(settingsNodeID_ptr, animSet);
                animSetsCache->Set(node_mod, animSet);
                awd->add_amin_set_block(animSet);
                free(animSetname);
            }
        }
        else{
                        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
        }
        if (settingsNodeID_ptr!=NULL)
            free(settingsNodeID_ptr);
        if (sourceVertexID_ptr!=NULL)
            free(sourceVertexID_ptr);
        if (sourceSkeletonID_ptr!=NULL)
            free(sourceSkeletonID_ptr);

    }
}

void MaxAWDExporter::ReadAWDEffectMethods(Modifier *node_mod){

    int num_params = node_mod->NumParamBlocks();
    int cntBlocks=0;
    IParamBlock2* pb ;
    char * settingsNodeID_ptr = NULL;
    bool simpleMode=true;
    int animType=0;
    int p=0;
    bool isColorMatrix=false;
    bool isColorTransform=false;
    bool isEnvMap=false;
    bool isLightMapMethod=false;
    bool isProjectiveTextureMethod=false;
    bool isRimLightMethod=false;
    bool isAlphaMaskMethod=false;
    bool isRefractionEnvMapMethod=false;
    bool isOutlineMethod=false;
    bool isFresnelEnvMapMethod=false;
    bool isFogMethod=false;
    int idxColorMatrix=1;
    int idxColorTransform=2;
    int idxEnvMap=3;
    int idxLightMapMethod=4;
    int idxProjectiveTextureMethod=5;
    int idxRimLightMethod=6;
    int idxAlphaMaskMethod=7;
    int idxRefractionEnvMapMethod=8;
    int idxOutlineMethod=9;
    int idxFresnelEnvMapMethod=10;
    int idxFogMethod=11;
    
    pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 11);
    if(pb!=NULL){
        int numBlockparams=pb->NumParams();
        for (p=0; p<numBlockparams; p++) {
            ParamID pid = pb->IndextoID(p);
            ParamDef def = pb->GetParamDef(pid);
            ParamType2 paramtype = pb->GetParameterType(pid);
            char * paramName=W2A(def.int_name);
            if (paramtype==TYPE_BOOL){
                if (ATTREQ(paramName, "isColorMatrix"))
                    isColorMatrix = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isColorTransform"))
                    isColorTransform = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isEnvMap"))
                    isEnvMap = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isLightMapMethod"))
                    isLightMapMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isProjectiveTextureMethod"))
                    isProjectiveTextureMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isRimLightMethod"))
                    isRimLightMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isAlphaMaskMethod"))
                    isAlphaMaskMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isRefractionEnvMapMethod"))
                    isRefractionEnvMapMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isOutlineMethod"))
                    isOutlineMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isFresnelEnvMapMethod"))
                    isFresnelEnvMapMethod = (0 != pb->GetInt(pid));
                if (ATTREQ(paramName, "isFogMethod"))
                    isFogMethod = (0 != pb->GetInt(pid));
            }
            if (paramtype==TYPE_INT){
                if (ATTREQ(paramName, "idxColorMatrix"))
                    idxColorMatrix = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxColorTransform"))
                    idxColorTransform = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxEnvMap"))
                    idxEnvMap = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxLightMapMethod"))
                    idxLightMapMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxProjectiveTextureMethod"))
                    idxProjectiveTextureMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxRimLightMethod"))
                    idxRimLightMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxAlphaMaskMethod"))
                    idxAlphaMaskMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxRefractionEnvMapMethod"))
                    idxRefractionEnvMapMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxOutlineMethod"))
                    idxOutlineMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxFresnelEnvMapMethod"))
                    idxFresnelEnvMapMethod = pb->GetInt(pid);
                if (ATTREQ(paramName, "idxFogMethod"))
                    idxFogMethod = pb->GetInt(pid);
            }
            if (paramtype==TYPE_STRING){
                if (ATTREQ(paramName, "thisAWDID"))
                    settingsNodeID_ptr = W2A(pb->GetStr(pid));
            }
            free(paramName);
        }
    }
    else{
        return;
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }
    AWDBlockList * awdEffectBlocks=(AWDBlockList *)methodsCache->Get(settingsNodeID_ptr);
    if (awdEffectBlocks==NULL){
        IntCache * orderEffectMethodsCache=new IntCache();
        if (isColorMatrix){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+13);
            strcpy(thisName, bname);
            strcat(thisName, "_colormatrix");
            thisName[strlen(bname)+12]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_COLORMATRIX);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxColorMatrix);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                AWD_field_ptr colorMatrix_val;
                colorMatrix_val.v = malloc(sizeof(awd_float64)*20);
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"colorMatrixVal1"))
                            colorMatrix_val.f64[0]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal2"))
                            colorMatrix_val.f64[1]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal3"))
                            colorMatrix_val.f64[2]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal4"))
                            colorMatrix_val.f64[3]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal5"))
                            colorMatrix_val.f64[4]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal6"))
                            colorMatrix_val.f64[5]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal7"))
                            colorMatrix_val.f64[6]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal8"))
                            colorMatrix_val.f64[7]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal9"))
                            colorMatrix_val.f64[8]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal10"))
                            colorMatrix_val.f64[9]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal11"))
                            colorMatrix_val.f64[10]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal12"))
                            colorMatrix_val.f64[11]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal13"))
                            colorMatrix_val.f64[12]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal14"))
                            colorMatrix_val.f64[13]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal15"))
                            colorMatrix_val.f64[14]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal16"))
                            colorMatrix_val.f64[15]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal17"))
                            colorMatrix_val.f64[16]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal18"))
                            colorMatrix_val.f64[17]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal19"))
                            colorMatrix_val.f64[18]=pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorMatrixVal20"))
                            colorMatrix_val.f64[19]=pb->GetFloat(pid);
                    }
                    free(paramName_ptr);
                }
                //TODO: check if the matrix is default...
                awdFXMethod->get_effect_props()->set(PROPS_NUMBER1, colorMatrix_val, sizeof(awd_float64)*20, AWD_FIELD_FLOAT64);
                                
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isColorTransform){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+16);
            strcpy(thisName, bname);
            strcat(thisName, "_colortransform");
            thisName[strlen(bname)+15]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_COLORTRANSFORM);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxColorTransform);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 1);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                Point3 color;
                bool hasColor=false;
                double alpha=0;
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_RGBA){
                        if (ATTREQ(paramName_ptr,"colorTransformColor")){
                            color = pb->GetColor(pid);
                            hasColor=true;
                        }
                    }
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"colorTransformVal1"))
                            alpha = pb->GetFloat(pid);
                        if (ATTREQ(paramName_ptr,"colorTransformVal2"))
                            awdFXMethod->add_number_property(PROPS_NUMBER2, pb->GetFloat(pid), 1.0);
                        if (ATTREQ(paramName_ptr,"colorTransformVal3"))
                            awdFXMethod->add_number_property(PROPS_NUMBER3, pb->GetFloat(pid), 1.0);
                        if (ATTREQ(paramName_ptr,"colorTransformVal4"))
                            awdFXMethod->add_number_property(PROPS_NUMBER4, pb->GetFloat(pid), 1.0);
                        if (ATTREQ(paramName_ptr,"colorTransformVal5"))
                            awdFXMethod->add_number_property(PROPS_NUMBER1, pb->GetFloat(pid), 1.0);
                    }
                    free(paramName_ptr);
                }
                if (hasColor)
                    awdFXMethod->add_color_property(PROPS_COLOR1, createARGB(alpha*255, color.x*255, color.y*255, color.z*255), 0xffffff);
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isEnvMap){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+8);
            strcpy(thisName, bname);
            strcat(thisName, "_envmap");
            thisName[strlen(bname)+7]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_ENVMAP);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxEnvMap);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 2);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);	
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"envMapAlpha"))
                            awdFXMethod->add_number_property(PROPS_NUMBER1, pb->GetFloat(pid), 1.0);
                    }
                    if (paramtype==TYPE_TEXMAP){
                        if (ATTREQ(paramName_ptr,"envMapMask")){
                            Texmap *tex = pb->GetTexmap(pid);
                            AWDBitmapTexture * newtex=NULL;
                            if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE);
                                if ((newtex!=NULL)&&(newtex->get_isValid()))
                                    awdFXMethod->set_awdBlock2(newtex);
                            }
                        }
                    }
                    if (paramtype==TYPE_REFTARG){
                        if (ATTREQ(paramName_ptr,"envMap")){
                            //TODO: THERE IS NO OPTION TO ADD THE ENVMAP IN THE INTERFACE YET, so this is not tested:
                            Mtl *thisMtl = (Mtl *)pb->GetReferenceTarget(pid);
                            if (thisMtl!=NULL){
                                MSTR matClassName;
                                thisMtl->GetClassName(matClassName);
                                char * matClassName_ptr=W2A(matClassName);
                                if (ATTREQ(matClassName_ptr,"AWDCubeMaterial")){
                                    AWDCubeTexture* newCubetex = GetCustomAWDCubeTexSettings((MultiMtl *)thisMtl);
                                    if (newCubetex!=NULL){
                                        awdFXMethod->set_awdBlock1(newCubetex);
                                    }
                                }
                                free(matClassName_ptr);
                            }
                        }
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isLightMapMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+10);
            strcpy(thisName, bname);
            strcat(thisName, "_lightmap");
            thisName[strlen(bname)+9]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_LIGHTMAP);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxLightMapMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 3);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);	
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_TEXMAP){
                        if (ATTREQ(paramName_ptr,"thislightMap") ){
                            Texmap *tex = pb->GetTexmap(pid);
                            AWDBitmapTexture * newtex=NULL;
                            if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE);
                                if ((newtex!=NULL)&&(newtex->get_isValid()))
                                    awdFXMethod->set_awdBlock1(newtex);
                            }
                        }
                    }
                    if (paramtype==TYPE_BOOL){
                        if (ATTREQ(paramName_ptr,"useSecUV") )
                            awdFXMethod->add_bool_property(PROPS_BOOL1, (0 != pb->GetInt(pid)), false);
                    }
                    if (paramtype==TYPE_INT){
                        if (ATTREQ(paramName_ptr,"blendMode") ){
                            AWD_field_ptr lightMapBlendMode_val;
                            //TODO: translate to correct blendmode int for AWD
                            int blendMode=10;
                            if(pb->GetInt(pid)==2)
                                blendMode=1;
                            lightMapBlendMode_val.v = malloc(sizeof(awd_uint8));
                            *lightMapBlendMode_val.ui8 = (awd_uint8)blendMode;
                            awdFXMethod->get_effect_props()->set(PROPS_INT8_1, lightMapBlendMode_val, sizeof(awd_uint8), AWD_FIELD_UINT8);
                        }
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isProjectiveTextureMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+19);
            strcpy(thisName, bname);
            strcat(thisName, "_projectivetexture");
            thisName[strlen(bname)+18]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_PROJECTIVE_TEXTURE);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxProjectiveTextureMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 4);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    free(paramName_ptr);
                    //TODO: REAT PROJECTIVE TEXTURE METHOD (ITS DISABLED FOR NOW)
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isRimLightMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+10);
            strcpy(thisName, bname);
            strcat(thisName, "_rimlight");
            thisName[strlen(bname)+9]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_RIMLIGHT);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxRimLightMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 5);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_RGBA){
                        if (ATTREQ(paramName_ptr,"thisColor") ){
                            Point3 color = pb->GetColor(pid);
                            awdFXMethod->add_color_property(PROPS_COLOR1, createARGB(255, color.x*255, color.y*255, color.z*255), 0xffffff);
                        }
                    }
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"thisStrength") )
                            awdFXMethod->add_number_property(PROPS_NUMBER1, pb->GetFloat(pid), 0.4);
                        if (ATTREQ(paramName_ptr,"thispower") )
                            awdFXMethod->add_number_property(PROPS_NUMBER2, pb->GetFloat(pid), 2.0);
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isAlphaMaskMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+11);
            strcpy(thisName, bname);
            strcat(thisName, "_alphamask");
            thisName[strlen(bname)+10]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_ALPHA_MASK);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxAlphaMaskMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 6);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_TEXMAP){
                        if (ATTREQ(paramName_ptr,"mask_map") ){
                            Texmap *tex = pb->GetTexmap(pid);
                            AWDBitmapTexture * newtex=NULL;
                            if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE);
                                if ((newtex!=NULL)&&(newtex->get_isValid()))
                                    awdFXMethod->set_awdBlock1(newtex);
                            }
                        }
                    }
                    if (paramtype==TYPE_BOOL){
                        if (ATTREQ(paramName_ptr,"mask_useSecUV"))
                            awdFXMethod->add_bool_property(PROPS_BOOL1, (0 != pb->GetInt(pid)), false);
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isRefractionEnvMapMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+18);
            strcpy(thisName, bname);
            strcat(thisName, "_refractionenvmap");
            thisName[strlen(bname)+17]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_REFRACTION_ENVMAP);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxRefractionEnvMapMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 7);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"refract_index"))
                            awdFXMethod->add_number_property(PROPS_NUMBER1, pb->GetFloat(pid), 0.1);
                        if (ATTREQ(paramName_ptr,"refract_dispersion1"))
                            awdFXMethod->add_number_property(PROPS_NUMBER2, pb->GetFloat(pid), 0.01);
                        if (ATTREQ(paramName_ptr,"refract_dispersion2"))
                            awdFXMethod->add_number_property(PROPS_NUMBER3, pb->GetFloat(pid), 0.01);
                        if (ATTREQ(paramName_ptr,"refract_dispersion3"))
                            awdFXMethod->add_number_property(PROPS_NUMBER4, pb->GetFloat(pid), 0.01);
                        if (ATTREQ(paramName_ptr,"refract_alpha"))
                            awdFXMethod->add_number_property(PROPS_NUMBER5, pb->GetFloat(pid), 1.0);
                    }
                    if (paramtype==TYPE_REFTARG){
                        if (ATTREQ(paramName_ptr,"refract_envMap")){
                            Mtl *thisMtl = (Mtl *)pb->GetReference(pid);
                            if (thisMtl!=NULL){
                                MSTR matClassName;
                                thisMtl->GetClassName(matClassName);
                                char * matClassName_ptr=W2A(matClassName);
                                if (ATTREQ(matClassName_ptr,"AWDCubeMaterial")){
                                    AWDCubeTexture* newCubetex = GetCustomAWDCubeTexSettings((MultiMtl *)thisMtl);
                                    if (newCubetex!=NULL){
                                        awdFXMethod->set_awdBlock1(newCubetex);
                                    }
                                }
                                free(matClassName_ptr);
                            }
                        }
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod - should never happn
            }
        }
        if (isOutlineMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+9);
            strcpy(thisName, bname);
            strcat(thisName, "_outline");
            thisName[strlen(bname)+8]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_OUTLINE);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxOutlineMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 8);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_RGBA){
                        if (ATTREQ(paramName_ptr,"outlineColor") ){
                            Point3 color = pb->GetColor(pid);
                            awdFXMethod->add_color_property(PROPS_COLOR1, createARGB(255, color.x*255, color.y*255, color.z*255), 0x000000);
                        }
                    }
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"outlineSize") )
                            awdFXMethod->add_number_property(PROPS_NUMBER1, pb->GetFloat(pid), 1.0);
                    }
                    if (paramtype==TYPE_BOOL){
                        if (ATTREQ(paramName_ptr,"innerLines") )
                            awdFXMethod->add_bool_property(PROPS_BOOL1, (0 != pb->GetInt(pid)), true);
                        if (ATTREQ(paramName_ptr,"dedicadedMesh") )
                            awdFXMethod->add_bool_property(PROPS_BOOL2, (0 != pb->GetInt(pid)), false);
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isFresnelEnvMapMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+15);
            strcpy(thisName, bname);
            strcat(thisName, "_fresnelenvmap");
            thisName[strlen(bname)+14]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_FRESNEL_ENVMAP);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxFresnelEnvMapMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 9);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_REFTARG){
                        if (ATTREQ(paramName_ptr,"fresnel_EnvMap") ){
                            Mtl *thisMtl = (Mtl *)pb->GetReference(pid);
                            if (thisMtl!=NULL){
                                MSTR matClassName;
                                thisMtl->GetClassName(matClassName);
                                char * matClassName_ptr=W2A(matClassName);
                                if (ATTREQ(matClassName_ptr,"AWDCubeMaterial") ){
                                    AWDCubeTexture* newCubetex = GetCustomAWDCubeTexSettings((MultiMtl *)thisMtl);
                                    if (newCubetex!=NULL){
                                        awdFXMethod->set_awdBlock1(newCubetex);
                                    }
                                }
                                free(matClassName_ptr);
                            }
                        }
                    }
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"fresnel_alpha"))
                            awdFXMethod->add_number_property(PROPS_NUMBER1, pb->GetFloat(pid), 1.0);
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        if (isFogMethod){
            char *bname = W2A(node_mod->GetName());
            char * thisName = (char*)malloc(strlen(bname)+5);
            strcpy(thisName, bname);
            strcat(thisName, "_fog");
            thisName[strlen(bname)+4]=0;
            free(bname);
            AWDEffectMethod * awdFXMethod = new AWDEffectMethod (thisName, strlen(thisName), AWD_FXMETHOD_FOG);
            free(thisName);
            orderEffectMethodsCache->Set(awdFXMethod, idxFogMethod);
            awd->add_effect_method_block(awdFXMethod);
            pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 10);
            if(pb!=NULL){
                int numBlockparams=pb->NumParams();
                for (p=0; p<numBlockparams; p++) {
                    ParamID pid = pb->IndextoID(p);
                    ParamDef def = pb->GetParamDef(pid);
                    ParamType2 paramtype = pb->GetParameterType(pid);
                    char * paramName_ptr=W2A(def.int_name);
                    if (paramtype==TYPE_RGBA){
                        if (ATTREQ(paramName_ptr,"fogColor")){
                            Point3 color = pb->GetColor(pid);
                            awdFXMethod->add_color_property(PROPS_COLOR1, createARGB(255, color.x*255, color.y*255, color.z*255), 0x808080);
                        }
                    }
                    if (paramtype==TYPE_FLOAT){
                        if (ATTREQ(paramName_ptr,"fogDistanceMin"))
                            awdFXMethod->add_number_property(PROPS_NUMBER1, pb->GetFloat(pid), 0);
                        if (ATTREQ(paramName_ptr,"fogDistanceMax"))
                            awdFXMethod->add_number_property(PROPS_NUMBER2, pb->GetFloat(pid), 1000.0);
                    }
                    free(paramName_ptr);
                }
            }
            else{
                //error: could not read paramBlock for effectmethod
            }
        }
        awdEffectBlocks=orderEffectMethodsCache->GetKeysOrderedByVal();

        methodsCache->Set(settingsNodeID_ptr, awdEffectBlocks);
        delete orderEffectMethodsCache;
    }
    if (settingsNodeID_ptr!=NULL)
        free(settingsNodeID_ptr);
}
    

// ---> functions called by ProcessSceneGraph():

AWDBlock *MaxAWDExporter::getColorMatForObject(INode *node, boolean black){
    awd_color color = NULL;	
    if (!black){
        color = convertColor(node->GetWireColor());	}	
    else{
        color=convertColor((DWORD)0x000000 );
    }
    // Look in the cache for an existing "default" color material
    // that matches the color of this object. If none exists,
    // create a new one and store it in the cache.
    AWDMaterial *awdMtl;
    awdMtl = colMtlCache->Get(color);
    if (awdMtl == NULL) {		
        char * matName_ptr = W2A(node->GetName());
        char * thisName = (char*)malloc(strlen(matName_ptr)+10);
        strcpy(thisName, matName_ptr);
        strcat(thisName, "_colorMat");
        thisName[strlen(matName_ptr)+9]=0;
        free(matName_ptr);
        awdMtl = new AWDMaterial(thisName, strlen(thisName));
        free(thisName);
        awdMtl->set_type(AWD_MATTYPE_COLOR);
        awdMtl->set_color(color);
        colMtlCache->Set(color, awdMtl);
        awd->add_material(awdMtl);
    }	
    return awdMtl;
}

AWDBlockList *MaxAWDExporter::GetMaterialsForMeshInstance(INode *node) 
{
    output_debug_string("         -->Get Materials For MeshInstance");
    AWDBlockList * awdBlockList= new AWDBlockList();
    Mtl *mtl = node->GetMtl();
    if (mtl == NULL) {		
        awdBlockList->append(getColorMatForObject(node, false));
        return awdBlockList;
    }
    else {
        int i=0;
        if (mtl->IsSubClassOf(Class_ID(DMTL_CLASS_ID, 0))) {
            StdMat *stdMtl = (StdMat *)mtl;
            AWDMaterial *awdmat;
            awdmat = ExportOneMaterial(stdMtl);
            awdBlockList->force_append(awdmat);
            return awdBlockList;
        }
        else if (mtl->IsSubClassOf(Class_ID(MULTI_CLASS_ID, 0))) {
            MultiMtl *multiMtl = (MultiMtl *)mtl;
            int numMaterials=multiMtl->NumSubMtls();
            for (i=0; i<numMaterials; i++) {
                Mtl* new_material=multiMtl->GetSubMtl(i);
                bool matExported=false;
                if (new_material!=NULL){
                    if (new_material->IsSubClassOf(Class_ID(DMTL_CLASS_ID, 0))) {
                        StdMat *stdMtl = (StdMat *)new_material;
                        AWDMaterial *awdmat;
                        awdmat = ExportOneMaterial(stdMtl);
                        awdBlockList->force_append(awdmat);	
                        matExported=true;		
                    }
                }
                if (!matExported){
                    awdBlockList->force_append(getColorMatForObject(node, true));
                }
            }
        }
        else{
            // not a multimaterial or standart material -> create a color-material with object-color
            AWDMaterial *awdmat=(AWDMaterial*)getColorMatForObject(node, false);
            awdBlockList->append(awdmat);
        }
    }
    if (awdBlockList->get_num_blocks()==0)
        awdBlockList->append(getColorMatForObject(node, false));
    return awdBlockList;
}

AWDMaterial *MaxAWDExporter::ExportOneMaterial(StdMat *mtl) 
{
    output_debug_string("         -->ExportOneMaterial");
    AWDMaterial *awdMtl=NULL;
    if (mtl != NULL) {
        awdMtl = (AWDMaterial *)materialCache->Get(mtl);
        if (awdMtl == NULL) {
            int i=0;
            char * matName_ptr = W2A(mtl->GetName());
            awdMtl = new AWDMaterial(matName_ptr, strlen(matName_ptr));
            free(matName_ptr);
            bool hasDifftex=false;
            bool hasAmbTex=false;
            bool hasSpecTex=false;
            bool hasNormalTex=false;
            AWDBitmapTexture *awdNormalTex=NULL;
            AWDBitmapTexture *awdDiffTex=NULL;
            AWDBitmapTexture *awdAmbTex=NULL;
            AWDBitmapTexture *awdSpecTex=NULL;
            StdMat2 * stdMat2=(StdMat2 *)mtl;
            if (stdMat2!=NULL){
                if (stdMat2->IsFaceted()){
                    awdMtl->set_is_faceted(true);
                }
                Shader * thisShader=stdMat2->GetShader();
                if (thisShader!=NULL){
                    MSTR className;
                    thisShader->GetClassName(className);
                    char * thisShader_ptr=W2A(className);
                    if (ATTREQ(thisShader_ptr,"Blinn")){
                        // standart material blinn
                    }
                    else if (ATTREQ(thisShader_ptr,"Anisotropic")){
                        // Anisotropic spec shading
                        awdMtl->add_method(new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_ANISOTROPIC));
                    }
                    else if (ATTREQ(thisShader_ptr,"Metal")){
                        // Metal material
                    }
                    else if (ATTREQ(thisShader_ptr,"Multi-Layer")){
                        // Multi-Layer material
                    }
                    else if (ATTREQ(thisShader_ptr,"Oren-Nayar-Blinn")){
                        // Oren-Nayar-Blinn material
                    }
                    else if (ATTREQ(thisShader_ptr,"Phong")){
                        awdMtl->add_method(new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_PHONG));
                        // Phong material
                    }
                    else if (ATTREQ(thisShader_ptr,"Strauss")){
                        // Strauss material
                    }
                    else if (ATTREQ(thisShader_ptr,"Translucent Shader")){
                        // Strauss material
                    }
                    free(thisShader_ptr);
                }
                //int thisShading=stdMat2->GetShading();
            }
            
            int saveType=opts->TextureMode();
            if (saveType!=0){
                for (i=0; i<mtl->NumSubTexmaps(); i++) {
                    Texmap *tex = mtl->GetSubTexmap(i);
                    // If there is a texture, AND that texture is a plain bitmap
                    if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                        char * slotName_ptr=W2A(mtl->GetSubTexmapSlotName(i));
                        if (ATTREQ(slotName_ptr,"Diffuse Color")){
                            awdDiffTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE);
                            hasDifftex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        if (ATTREQ(slotName_ptr,"Ambient Color")){
                            awdAmbTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE);
                            hasAmbTex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        if (ATTREQ(slotName_ptr,"Specular Color")){
                            awdSpecTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE);
                            hasSpecTex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        if (ATTREQ(slotName_ptr,"Specular Level")){
                            awdSpecTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE);
                            hasSpecTex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        if (ATTREQ(slotName_ptr,"Glossiness")){
                            awdSpecTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE);
                            hasSpecTex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        if (ATTREQ(slotName_ptr,"Bump")){
                            awdNormalTex = ExportBitmapTexture((BitmapTex *)tex, awdMtl, UNDEFINEDTEXTYPE);
                            hasNormalTex=true;
                            RETURN_VALUE_IF_ERROR(NULL);
                        }
                        free(slotName_ptr);
                    }
                    
                    if (tex != NULL && tex->ClassID() == Class_ID(0x243e22c6, 0x63f6a014)) { // GNORMAL_CLASS_ID not supüported in 2011
                        char * slotName_ptr=W2A(mtl->GetSubTexmapSlotName(i));
                        if (ATTREQ(slotName_ptr,"Bump")){
                            // we consider all maps found in the bumb_node as normal-maps, without checking the mode of the node. 
                            IParamBlock2 * pb = tex->GetParamBlockByID(0); //gnormal_params = 0  not supüported in 2011
                            int p=0;
                            for (p=0; p<pb->NumParams(); p++) {
                                ParamID pid = pb->IndextoID(p);
                                ParamDef def = pb->GetParamDef(pid);
                                if (pb->GetParameterType(pid)==TYPE_TEXMAP){
                                    char * defName_ptr=W2A(def.int_name);
                                    if (ATTREQ(defName_ptr,"normal_map")){
                                        if (!hasNormalTex){
                                            Texmap * newNormalTexMap=(Texmap *)pb->GetTexmap(pid);
                                            if (newNormalTexMap != NULL && newNormalTexMap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)){
                                                awdNormalTex = ExportBitmapTexture((BitmapTex *)newNormalTexMap, awdMtl, UNDEFINEDTEXTYPE);												
                                                hasNormalTex=true;
                                            }
                                        }
                                        RETURN_VALUE_IF_ERROR(NULL);
                                    }
                                    free(defName_ptr);
                                }
                            }
                        }
                        free(slotName_ptr);
                    }
                    
                }
                if (hasDifftex || hasAmbTex){
                    awdMtl->set_type(AWD_MATTYPE_TEXTURE);
                    if (hasDifftex){
                        awdMtl->set_texture(awdDiffTex);}
                    if (hasAmbTex){
                        awdMtl->set_ambientTexture(awdAmbTex);}
                }
                else{
                    awdMtl->set_type(AWD_MATTYPE_COLOR);
                }	
            }
            else{
                awdMtl->set_type(AWD_MATTYPE_COLOR);
            }

            awdMtl->set_color(convertColor(mtl->GetDiffuse(0).toRGB()));
            awdMtl->set_ambientColor(convertColor(mtl->GetAmbient(0).toRGB()));
            awdMtl->set_specularColor(convertColor(mtl->GetSpecular(0).toRGB()));
            awdMtl->set_both_sides(mtl->GetTwoSided());

            // this can optionally overwrite previous defined material-settings
            GetCustomAWDMaterialSettings(mtl, awdMtl);
            if (hasSpecTex){
                awdMtl->set_specTexture(awdSpecTex);}		
            if (hasNormalTex){
                awdMtl->set_normalTexture(awdNormalTex);}
            awd->add_material(awdMtl);
            materialCache->Set(mtl, awdMtl);
        }
    }
    output_debug_string("         -->finished ExportOneMaterial");
    return awdMtl;
}

AWDBitmapTexture * MaxAWDExporter::ExportBitmapTexture(BitmapTex *tex, AWDMaterial * curMat, AWD_tex_type cubeTexType)
{
    output_debug_string("            -->ExportBitmapTexture");
    AWDBitmapTexture *awdTex;
    if(tex!=NULL){
        CustomTexSettings_struct customTexSettings=GetCustomAWDTextureSettings(tex);
        bool replaceBitmap=false;
        MaxSDK::AssetManagement::AssetUser asset = tex->GetMap();
        char * absTexPath_ptr = W2A(asset.GetFullFilePath());
        if (customTexSettings.replace){
            if (strlen(customTexSettings.replaceURL)==0){
                //TODO: handle error if replace string is empty (uncheck 'replace' in the AWDTextureSettings if replaceURL is empty ? ...)
            }
            else{
                free(absTexPath_ptr);
                output_debug_string(customTexSettings.replaceURL);
                absTexPath_ptr = customTexSettings.replaceURL;
                output_debug_string(absTexPath_ptr);
                replaceBitmap=true;
            }
        }
        /*
        TODO:
        IMPLEMENT UV ROTATION
        float angleRadians=uvGen->GetAng(0);
        float angleUradians=uvGen->GetUAng(0);
        float angleVradians=uvGen->GetVAng(0);
        float angleWradians=uvGen->GetWAng(0);
        int tillingFlag=uvGen->GetTextureTiling();
        */
        
        StdUVGen * uvGen=tex->GetUVGen();
        if ((curMat!=NULL)&&(uvGen!=NULL)){
            if (curMat->get_uv_transform_mtx()==NULL){
                double *uvmtxData = (double *)malloc(6*sizeof(double));
                uvmtxData[0]=uvGen->GetUScl(0);//a:scalex
                uvmtxData[1]=0.0;//b:rot1
                uvmtxData[2]=0.0;//d:rot2
                uvmtxData[3]=uvGen->GetVScl(0);//c:scaley
                uvmtxData[4]=uvGen->GetUOffs(0); //0.0;//tx:offsetx
                uvmtxData[5]=uvGen->GetVOffs(0); //0.0;//ty:offsety
                curMat->set_uv_transform_mtx(uvmtxData);
                free(uvmtxData);
            }
            int mapChannel=uvGen->GetMapChannel();
            if (curMat->get_mappingChannel()==0){
                curMat->set_mappingChannel(mapChannel);
            }
            else if (curMat->get_mappingChannel()==mapChannel){
                // WARNING: All textures should use the same mapping channel
            }
        }
    
        AWD_tex_type texType=UNDEFINEDTEXTYPE;
        bool copyTxt=true;
        bool forceBaseName=true;
        
        // if no saveType is requested (for cubeTex), we check for the saveType
        if (cubeTexType==UNDEFINEDTEXTYPE){
            if (customTexSettings.texType!=UNDEFINEDTEXTYPE){
                texType=customTexSettings.texType;
                copyTxt=true;
                forceBaseName=customTexSettings.relative;		}
            else{
                int saveType=opts->TextureMode();
                if (saveType==1)
                    texType=EMBEDDED;
                if (saveType==2){
                    copyTxt=false;
                    forceBaseName=false;
                    texType=EXTERNAL;
                }
                if (saveType==3){
                    copyTxt=true;
                    forceBaseName=true;
                    texType=EXTERNAL;
                }
            }
        }
        else{
            texType=cubeTexType;
        }
        // now we have the abosute pathname of the file to export.
        // we check if a block exists, and if a saveMode was requested, we check if it fits
        awdTex = (AWDBitmapTexture *)textureCache->Get(absTexPath_ptr);
        bool isWrongMode=false;
        if (cubeTexType!=UNDEFINEDTEXTYPE){
            if (awdTex != NULL){
                if(awdTex->get_tex_type()!=texType){
                    isWrongMode=true;
                }
            }
        }
        if ((awdTex == NULL) || (isWrongMode)) {
            if (!isWrongMode){				
                char * texname_ptr=W2A(tex->GetName());
                awdTex = new AWDBitmapTexture(texname_ptr, strlen(texname_ptr));
                free(texname_ptr);
            }
            bool bitMapValid=true;

            // only check if the bitmap is valid, if the path was not replaced earlier
            if (!replaceBitmap){
                Bitmap *thisbitmap;
                try{
                    thisbitmap=tex->GetBitmap(0);
                    int width=thisbitmap->Width();
                    int height=thisbitmap->Height();
                    if ((width!=2)&&(width!=4)&&(width!=8)&&(width!=16)&&(width!=32)&&(width!=64)&&(width!=128)&&(width!=256)&&(width!=512)&&(width!=1024)&&(width!=2048)&&(width!=4096)){
                        awdTex->set_isValid(false);
                        bitMapValid=false;
                        // todo: error
                    }
                    if ((height!=2)&&(height!=4)&&(height!=8)&&(height!=16)&&(height!=32)&&(height!=64)&&(height!=128)&&(height!=256)&&(height!=512)&&(height!=1024)&&(height!=2048)&&(height!=4096)){
                        awdTex->set_isValid(false);
                        bitMapValid=false;
                        // todo: error
                    }
                }
                catch(...){bitMapValid=false;}
            }
            //TODO: get the type of the bitmap (is it jpg or png ?) 
            // if the bitmap is not JPG or PNG, either say its invalid, or find a way to convert the bitmap by the 3dsmax sdk....

            if (bitMapValid){
                if (texType==EMBEDDED) {
                    int fd = open(absTexPath_ptr, _O_BINARY | _O_RDONLY);
        
                    if (fd >= 0) {
                        struct stat fst;
                        fstat(fd, &fst);

                        awd_uint8 *buf = (awd_uint8*)malloc(fst.st_size);
                        read(fd, buf, fst.st_size);
                        close(fd);

                        awdTex->set_tex_type(EMBEDDED);
                        awdTex->set_embed_data(buf, fst.st_size);
                    }
                    else {
                        char buf[1024];
                        snprintf(buf, 1024, 
                            "Texture \"%s\" could not be opened for embedding. "
                            "The file might be missing. Correct the path and try exporting again.",
                            tex->GetName());

                        // Show error message and return to stop texture export.
                        DieWithErrorMessage(buf, "Texture embedding error");
                        return NULL;
                    }
                }
                else {
                    awdTex->set_tex_type(EXTERNAL);

                    if (forceBaseName) {
                        char fileName[256];
                        char fileExt[16];
                        char *url;

                        // Split path to retrieve name and concatenate to form base name
                        _splitpath_s(absTexPath_ptr, NULL, 0, NULL, 0, fileName, 240, fileExt, 16);
                        
                        if (!copyTxt)  {
                            url = (char*)malloc(strlen(fileName)+strlen(fileExt)+1);
                            strcpy(url, fileName);
                            strcat(url, fileExt);
                            awdTex->set_url(url, strlen(fileName)+strlen(fileExt));
                        }

                        else{
                            url = (char*)malloc(strlen("textures/")+strlen(fileName)+strlen(fileExt)+1);
                            strcpy(url, "textures/");
                            strcat(url, fileName);
                            strcat(url, fileExt);
                            awdTex->set_url(url, strlen("textures/")+strlen(fileName)+strlen(fileExt));
                            char awdDrive[4];
                            char awdPath[1024];
                            char outPath[1024];
                            char outPathDir[1024];
                            try{
                                // Concatenate output path using base path of AWD file and basename of
                                // texture file, and copy texture file to output directory.
                                _splitpath_s(awdFullPath, awdDrive, 4, awdPath, 1024, NULL, 0, NULL, 0);

                                //add the "textures" folder to the output directory
                                strcat(awdPath, "textures");
                                _makepath_s(outPathDir, 1024, awdDrive, awdPath, NULL, NULL);
                                mkdir(outPathDir);

                                //copy the bitmap to the "textures" folder
                                _makepath_s(outPath, 1024, awdDrive, awdPath, fileName, fileExt);
                                CopyFileA(absTexPath_ptr, outPath, true);
                            }
                            catch(...){
                                awdTex->set_tex_type(UNDEFINEDTEXTYPE);
                                //error: something went wrong when copying the file 
                            }
                        }
                    }
                    else {
                        awdTex->set_url(absTexPath_ptr, strlen(absTexPath_ptr));
                    }
                }
                
            }
            //create and add the texture-block no matter if the bitmap is valid or not. 
            //this way we prevent the exporter to try to validate the same (invalid) bitmap multiple times
            textureCache->Set(absTexPath_ptr,awdTex);
            //if (cubeTexType==UNDEFINEDTEXTYPE)
            awd->add_texture(awdTex);
        }
        free(absTexPath_ptr);
        return awdTex;
    }
    return NULL;
}

AWDPrimitive * MaxAWDExporter::ExportPrimitiveGeom(BaseObject * obj, char * name){
    Class_ID classId = obj->ClassID();
    if (classId.PartA() == EDITTRIOBJ_CLASS_ID || classId.PartA() == TRIOBJ_CLASS_ID ){
        return NULL;
    }
    AWD_primitive_type isPrimitve=AWD_PRIMITIVE_UNDEFINED;
    if (classId == Class_ID( BOXOBJ_CLASS_ID, 0 ))
        isPrimitve=AWD_PRIMITIVE_CUBE;
    if (classId == Class_ID( SPHERE_CLASS_ID, 0 ))
        isPrimitve=AWD_PRIMITIVE_SPHERE;
    if (classId == Class_ID( CYLINDER_CLASS_ID, 0 ))
        isPrimitve=AWD_PRIMITIVE_CYLINDER;
    if (classId == PLANE_CLASS_ID)
        isPrimitve=AWD_PRIMITIVE_PLANE;
    if (classId == Class_ID( CONE_CLASS_ID, 0 ))
        isPrimitve=AWD_PRIMITIVE_CONE;
    if (classId == Class_ID( TORUS_CLASS_ID, 0 ))
        isPrimitve=AWD_PRIMITIVE_TORUS;
    if (classId ==PYRAMID_CLASS_ID){}
    if (classId == GSPHERE_CLASS_ID){}
    if (classId == Class_ID( TUBE_CLASS_ID, 0 )){}
    if (classId == Class_ID( HEDRA_CLASS_ID, 0 )){}
    if (classId == Class_ID( BOOLOBJ_CLASS_ID, 0 )){}
    if (isPrimitve!=AWD_PRIMITIVE_UNDEFINED){
        AWDPrimitive *awdGeom = (AWDPrimitive *)primGeocache->Get(obj);				
        if (awdGeom == NULL) {			
            IParamBlock* pblk = GetParamBlockByIndex((ReferenceMaker* )obj, 0);
            if (pblk){
                if (isPrimitve==AWD_PRIMITIVE_PLANE){
                    awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_PLANE);	
                    //todo: get the prop-ids for the plane-properties by using the IParamBlock, or how to get them else ?
                }
                if (isPrimitve==AWD_PRIMITIVE_CUBE){
                    awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_CUBE);	
                    awdGeom->set_Yoffset(pblk->GetFloat(BOXOBJ_HEIGHT) /2);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(BOXOBJ_WIDTH) * opts->Scale());
                    awdGeom->add_number_property(PROP_PRIM_NUMBER2, pblk->GetFloat(BOXOBJ_HEIGHT) * opts->Scale());
                    awdGeom->add_number_property(PROP_PRIM_NUMBER3, pblk->GetFloat(BOXOBJ_LENGTH) * opts->Scale());
                    int cubeSegX=pblk->GetInt(BOXOBJ_WSEGS);
                    if(cubeSegX>100) cubeSegX=100;
                    int cubeSegY=pblk->GetInt(BOXOBJ_HSEGS);
                    if(cubeSegY>100) cubeSegY=100;
                    int cubeSegZ=pblk->GetInt(BOXOBJ_LSEGS);
                    if(cubeSegZ>100) cubeSegZ=100;
                    awdGeom->add_int_property(PROP_PRIM_INT1, cubeSegX);
                    awdGeom->add_int_property(PROP_PRIM_INT2, cubeSegY);
                    awdGeom->add_int_property(PROP_PRIM_INT3, cubeSegZ);
                }
                if (isPrimitve==AWD_PRIMITIVE_SPHERE){
                    awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_SPHERE);	
                    awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(SPHERE_RADIUS) * opts->Scale());
                    awdGeom->add_int_property(PROP_PRIM_INT1, pblk->GetInt(SPHERE_SEGS));
                    //bool sphere_smooth=pblk->GetInt(SPHERE_SMOOTH);
                    //float sphere_hemisphere=pblk->GetFloat(SPHERE_HEMI);
                    //int sphere_radius=pblk->GetInt(SPHERE_SQUASH);
                    //int sphere_radius=pblk->GetInt(SPHERE_RECENTER);
                }
                if (isPrimitve==AWD_PRIMITIVE_CYLINDER){
                    awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_CYLINDER);
                    awdGeom->set_Yoffset(pblk->GetFloat(CYLINDER_HEIGHT) /2);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(CYLINDER_RADIUS) * opts->Scale());
                    awdGeom->add_number_property(PROP_PRIM_NUMBER2, pblk->GetFloat(CYLINDER_RADIUS) * opts->Scale());
                    awdGeom->add_number_property(PROP_PRIM_NUMBER3, pblk->GetFloat(CYLINDER_HEIGHT) * opts->Scale());
                    awdGeom->add_int_property(PROP_PRIM_INT1, pblk->GetInt(CYLINDER_SIDES));
                    awdGeom->add_int_property(PROP_PRIM_INT2, pblk->GetInt(CYLINDER_SEGMENTS));
                    //int cylinder_capsegments=pblk->GetInt(CYLINDER_CAPSEGMENTS);
                    //bool cylinder_slice_on=pblk->GetInt(CYLINDER_SLICEON);
                    //int box_depth_segs=pblk->GetInt(CYLINDER_SMOOTH);
                }
                if (isPrimitve==AWD_PRIMITIVE_CONE){
                    float cone_radius2=pblk->GetFloat(CONE_RADIUS2);
                    if (cone_radius2==0.0){
                        awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_CONE);
                        awdGeom->set_Yoffset(pblk->GetFloat(CONE_HEIGHT) /2);
                        awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(CONE_RADIUS1) * opts->Scale());
                        awdGeom->add_number_property(PROP_PRIM_NUMBER2, pblk->GetFloat(CONE_HEIGHT) * opts->Scale());
                        awdGeom->add_int_property(PROP_PRIM_INT1, pblk->GetInt(CONE_SIDES));
                        awdGeom->add_int_property(PROP_PRIM_INT2, pblk->GetFloat(CONE_SEGMENTS));	
                    }
                    else{
                        awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_CYLINDER);
                        awdGeom->set_Yoffset(pblk->GetFloat(CONE_HEIGHT) /2);
                        awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(CONE_RADIUS2) * opts->Scale());
                        awdGeom->add_number_property(PROP_PRIM_NUMBER2, pblk->GetFloat(CONE_RADIUS1) * opts->Scale());
                        awdGeom->add_number_property(PROP_PRIM_NUMBER3, pblk->GetFloat(CONE_HEIGHT) * opts->Scale());
                        awdGeom->add_int_property(PROP_PRIM_INT1, pblk->GetInt(CONE_SIDES));
                        awdGeom->add_int_property(PROP_PRIM_INT2, pblk->GetInt(CONE_SEGMENTS));
                    }
                    //int cone_capSegments=pblk->GetInt(CONE_CAPSEGMENTS);
                    //int cone_smooth=pblk->GetInt(CONE_SMOOTH);
                    //int cone_slice_on=pblk->GetInt(CONE_SLICEON);
                }
                if (isPrimitve==AWD_PRIMITIVE_CAPSULE){//not in 3dsmax supported...
                    awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_CAPSULE);
                }
                if (isPrimitve==AWD_PRIMITIVE_TORUS){
                    awdGeom=new AWDPrimitive(name, strlen(name), AWD_PRIMITIVE_TORUS);
                    awdGeom->add_number_property(PROP_PRIM_NUMBER1, pblk->GetFloat(TORUS_RADIUS) * opts->Scale());
                    awdGeom->add_number_property(PROP_PRIM_NUMBER2, pblk->GetFloat(TORUS_RADIUS2) * opts->Scale());
                    awdGeom->add_int_property(PROP_PRIM_INT1, pblk->GetInt(TORUS_SEGMENTS));
                    //float torus_rotation=pblk->GetFloat(TORUS_ROTATION);
                    //float torus_twist=pblk->GetFloat(TORUS_TWIST);
                    //int torus_sides=pblk->GetInt(TORUS_SIDES);
                    //int torus_smooth=pblk->GetInt(TORUS_SMOOTH);
                    //int torus_slice_on=pblk->GetInt(TORUS_SLICEON);
                }
                awd->add_prim_block(awdGeom);
                primGeocache->Set(obj, awdGeom);
                return awdGeom;
            }	
        }
        return awdGeom;
    }	
    return NULL;							
}

AWDAnimator * MaxAWDExporter::AutoCreateAnimatorForSkeleton(INode * node){	
    AWDAnimator * animatorBlock=NULL;			
    Object *obj;
    obj = node->GetObjectRef();			
    SClass_ID sid;
    getBaseObjectAndID(obj, sid);
    int skinIdx=0;
    ObjectState os;
    IDerivedObject *derivedObject = NULL;
    skinIdx = IndexOfSkinMod(node->GetObjectRef(), &derivedObject);
    if (skinIdx >= 0) {				
        // Flatten all modifiers up to but not including
        // the skin modifier.
        // to do: get the correct time for the neutral-pose
        os = derivedObject->Eval(0, skinIdx + 1);
    }
    else {
        // no skin = no skeleton animation
        return NULL;
    }
    obj = os.obj;
    ISkin *skin = NULL;				
    if (derivedObject != NULL && skinIdx >= 0) {
        Modifier *mod = derivedObject->GetModifier(skinIdx);
        skin = (ISkin *)mod->GetInterface(I_SKIN);
    }
    else {
        // no skin = no skeleton animation
        return NULL;
    }
    if (opts->ExportSkin() && skin && skin->GetNumBones()) {
        SkeletonCacheItem *skel = skeletonCache->GetFromBone(skin->GetBone(0));
        if (skel!=NULL){
            if (skel->awdSkel->get_simpleMode()){
                animatorBlock = (AWDAnimator *)animatorCache->Get(skel->awdSkel);
                if (animatorBlock==NULL){
                    char *nodename = W2A(node->GetName());
                    char *animSetName_ptr = (char*)malloc(strlen(nodename)+9);
                    strcpy(animSetName_ptr, nodename);
                    strcat(animSetName_ptr, "_animSet");
                    animSetName_ptr[strlen(nodename)+8]=0;
                    AWDAnimationSet *animSet=new AWDAnimationSet(animSetName_ptr, strlen(animSetName_ptr),ANIMTYPESKELETON, skel->awdID, strlen(skel->awdID));
                    awd->add_amin_set_block(animSet);
                    free(animSetName_ptr);
                    char *animatorName_ptr = (char*)malloc(strlen(nodename)+10);
                    strcpy(animatorName_ptr, nodename);
                    strcat(animatorName_ptr, "_animator");
                    animatorName_ptr[strlen(nodename)+9]=0;
                    animatorBlock = new AWDAnimator(animatorName_ptr, strlen(animatorName_ptr),animSet);
                    awd->add_animator(animatorBlock);
                    animatorCache->Set(skel->awdSkel, animatorBlock);
                    free(animatorName_ptr);
                    free(nodename);
                    return animatorBlock;
                }
                else{
                    // allready has animator for this skeleton
                    return animatorBlock;
                }
            }
            else {
                // not in simple mode -  no skeleton animation
                return NULL;
            }
        }
        else {
            // no skeleton found in cache!!! -  no skeleton animation
            return NULL;
        }
    }
    else {
        // no skin = no skeleton animation
        return NULL;
    }
    return animatorBlock;
}

AWDAnimator * MaxAWDExporter::AutoCreateAnimatorForVertexAnim(INode * node){	
    AWDAnimator * animatorBlock=NULL;
    BaseObject* node_bo = node->GetObjectRef();
    if(node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID) {
        IDerivedObject* node_der = (IDerivedObject*)(node_bo);
        node_bo = node_der->GetObjRef();
        if (node_der!=NULL){
            int nMods = node_der->NumModifiers();
            for (int m = 0; m<nMods; m++){
                Modifier* node_mod = node_der->GetModifier(m);
                MSTR className;
                node_mod->GetClassName(className);
                char * className_ptr=W2A(className);
                if (ATTREQ(className_ptr,"AWDVertexAnimSource")){
                    free(className_ptr);
                    animatorBlock = (AWDAnimator *)animatorCache->Get(node_mod);
                    if (animatorBlock==NULL){
                        animatorBlock=ReadAWDVertexModForMesh(node_mod, node);
                        animatorCache->Set(node_mod, animatorBlock);
                        if (animatorBlock!=NULL)
                            return animatorBlock;
                    }
                }
                else{
                    free(className_ptr);
                }
            }
        }
    }
    return animatorBlock;
}

AWDAnimator * MaxAWDExporter::ReadAWDVertexModForMesh(Modifier *node_mod, INode * node){
    
    AWDAnimator * animatorBlock=NULL;					
    int num_params = node_mod->NumParamBlocks();						
    char * settingsNodeID=NULL;		
    int p=0;
    bool simpleMode=false;
    int numBlockparams=0;
    IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
    if(pb!=NULL){
        numBlockparams=pb->NumParams();
        for (p=0; p<numBlockparams; p++) {
            ParamID pid = pb->IndextoID(p);
            ParamDef def = pb->GetParamDef(pid);	
            ParamType2 paramtype = pb->GetParameterType(pid);	
            char * paramName = W2A(def.int_name);
            if (ATTREQ(paramName, "thisAWDID")){
                if (paramtype==TYPE_STRING)				
                    settingsNodeID=W2A(pb->GetStr(pid));		
            }			
            if (ATTREQ(paramName, "simpleMode")){
                if (paramtype==TYPE_BOOL)			
                    simpleMode=(0 != pb->GetInt(pid));	
            }
            free(paramName);
        }		
        if (simpleMode){
            char *nodename = W2A(node->GetName());
            char *animSetName_ptr = (char*)malloc(strlen(nodename)+9);
            strcpy(animSetName_ptr, nodename);
            strcat(animSetName_ptr, "_animSet");
            animSetName_ptr[strlen(nodename)+8]=0;
            AWDAnimationSet *animSet=new AWDAnimationSet(animSetName_ptr, strlen(animSetName_ptr),ANIMTYPEVERTEX, settingsNodeID, strlen(settingsNodeID));
            awd->add_amin_set_block(animSet);
            free(animSetName_ptr);
            char *animatorName_ptr = (char*)malloc(strlen(nodename)+10);
            strcpy(animatorName_ptr, nodename);
            strcat(animatorName_ptr, "_animator");
            animatorName_ptr[strlen(nodename)+9]=0;
            animatorBlock = new AWDAnimator(animatorName_ptr, strlen(animatorName_ptr), animSet);
            awd->add_animator(animatorBlock);
            free(animatorName_ptr);
            free(nodename);
        }
    }
    else{
        //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
    }
    if(settingsNodeID!=NULL)
        free(settingsNodeID);
    return animatorBlock;
}

AWDAnimator * MaxAWDExporter::GetAWDAnimatorForObject(INode *node){
    AWDAnimator * animatorBlock=NULL;
    BaseObject* node_bo = node->GetObjectRef();
    if(node_bo->SuperClassID() == GEN_DERIVOB_CLASS_ID) {
        IDerivedObject* node_der = (IDerivedObject*)(node_bo);
        node_bo = node_der->GetObjRef();
        if (node_der!=NULL){
            int nMods = node_der->NumModifiers();
            for (int m = 0; m<nMods; m++){
                Modifier* node_mod = node_der->GetModifier(m);
                //DebugPrint("node_mod.IsEnabled() = "+node_mod->IsEnabled());
                //MSTR name=node_mod->GetName();
                MSTR className;
                node_mod->GetClassName(className);
                char * className_ptr=W2A(className);
                if (ATTREQ(className_ptr,"AWDAnimator") ){
                    animatorBlock = (AWDAnimator *)animatorCache->Get(node_mod);
                    if (animatorBlock==NULL){
                        int num_params = node_mod->NumParamBlocks();						
                        char * settingsNodeID_ptr=NULL;	//TODOFREE					
                        char * animSetID_ptr=NULL;	//TODOFREE						
                        int p=0;
                        IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)node_mod, 0);
                        if(pb!=NULL){
                            int numBlockparams=pb->NumParams();
                            for (p=0; p<numBlockparams; p++) {
                                ParamID pid = pb->IndextoID(p);
                                ParamDef def = pb->GetParamDef(pid);	
                                ParamType2 paramtype = pb->GetParameterType(pid);	
                                char * paramName_ptr=W2A(def.int_name);
                                if (ATTREQ(paramName_ptr, "thisAWDID") ){
                                    if (paramtype==TYPE_STRING)
                                        settingsNodeID_ptr = W2A(pb->GetStr(pid));		
                                }			
                                if (ATTREQ(paramName_ptr, "AnimationSetID") ){
                                    if (paramtype==TYPE_STRING)
                                        animSetID_ptr = W2A(pb->GetStr(pid));		
                                }	
                                free(paramName_ptr);
                            }
                            AWDAnimationSet *animSet=(AWDAnimationSet *)animSetsIDsCache->Get(animSetID_ptr);
                            char * animatorName=W2A(node_mod->GetName());
                            animatorBlock = new AWDAnimator(animatorName, strlen(animatorName),animSet);
                            free(animatorName);
                            animatorCache->Set(node_mod, animatorBlock);
                            awd->add_animator(animatorBlock);
                        }
                        else{
                            //TO DO: RAISE EROOR because of IPAramBlock not found in custom AWDModifier
                        }
                    }				
                }
                free(className_ptr);	
            }
        }	
    }
    //if the anitamorBlock is still NULL, we check if we should create Auto-AnomatorBlocks for this meshes
    if (animatorBlock==NULL){
    }

    return animatorBlock;
}

CustomTexSettings_struct MaxAWDExporter::GetCustomAWDTextureSettings(BitmapTex *tex)
{
    CustomTexSettings_struct returnData;
    returnData.replace=false;
    returnData.replaceURL="";
    returnData.texType=UNDEFINEDTEXTYPE;
    ICustAttribContainer *attributes = tex->GetCustAttribContainer();
    if (attributes) {
        int a=0;
        int numAttribs;
        numAttribs = attributes->GetNumCustAttribs();
        for (a=0; a<numAttribs; a++) {
            int p=0;
            CustAttrib *attr = attributes->GetCustAttrib(a);
            IParamBlock2 *block = attr->GetParamBlock(0);
            for (p=0; p<block->NumParams(); p++) {
                ParamID pid = block->IndextoID(p);					
                ParamDef def = block->GetParamDef(pid);	
                char * paramName_ptr=W2A(def.int_name);
                if (block->GetParameterType(pid)==TYPE_BOOL){
                    if (ATTREQ(paramName_ptr,"saveReplace") )
                        returnData.replace= (0 != block->GetInt(pid));
                }		
                if ((block->GetParameterType(pid)==TYPE_STRING)||(block->GetParameterType(pid)==TYPE_FILENAME)){
                    if (ATTREQ(paramName_ptr,"saveReplaceURL") )
                        returnData.replaceURL = W2A(block->GetStr(pid));
                }		
                if (block->GetParameterType(pid)==TYPE_INT){
                    if (ATTREQ(paramName_ptr,"saveTexType") ){
                        int typeInt=block->GetInt(pid);
                        if (typeInt==2){
                            returnData.relative = true;
                            returnData.texType = EXTERNAL;
                        }
                        if (typeInt==3){
                            returnData.relative = false;
                            returnData.texType = EXTERNAL;
                        }
                        if (typeInt==4)
                            returnData.texType = EMBEDDED;
                    }
                }	
                free(paramName_ptr);
            }
        }
    }	
    return returnData;
}

void MaxAWDExporter::GetCustomAWDMaterialSettings(StdMat *mtl, AWDMaterial * awdMat)
{
    ICustAttribContainer *attributes = mtl->GetCustAttribContainer();
    AWDShadingMethod * newEnvMapMethod=NULL;
    AWDShadingMethod * newBaseDiffuseMethod=NULL;
    AWDShadingMethod * newBaseSpecularMethod=NULL;
    AWDShadingMethod * newDiffuseMethod=NULL;
    AWDShadingMethod * newSpecularMethod=NULL;
    AWDShadingMethod * newNormalMethod=NULL;
    if (attributes) {
        int a=0;
        int p=0;
        int numAttribs=0;
        numAttribs = attributes->GetNumCustAttribs();
        for (a=0; a<numAttribs; a++) {
            int t=0;
            bool useShading=false;
            CustAttrib *attr = attributes->GetCustAttrib(a);
            for (t=0; t<attr->NumParamBlocks(); t++) {
                IParamBlock2 *block = attr->GetParamBlock(t);
                char * localName_ptr=W2A(block->GetLocalName());
                if (ATTREQ(localName_ptr,"AWD_MaterialSettingsparams") ){
                    bool multiPassMatenabled=false;
                    bool multiPassMat=false;
                    bool repeatMatenabled=false;
                    bool repeatMat=false;
                    bool smoothMatenabled=false;
                    bool smoothMat=false;
                    bool mipmapMatenabled=false;
                    bool mipmapMat=false;
                    bool alphaBlendingMatenabled=false;
                    bool alphaBlendingMat=false;
                    bool alphaTresholdMatenabled=false;
                    float alphaTresholdMat=0;
                    bool premultipliedMatenabled=false;
                    bool premultipliedMat=false;
                    bool blendModeenabled=false;
                    int blendMode=0;
                    bool MainUVenabled=false;
                    int mainUVChannel=0;
                    bool SecUVenabled=false;
                    int secUVChannel=0;
                    bool AmbientLevelenabled=false;
                    float ambientLevel=0;
                    bool SpecularLevelenabled=false;
                    float specularLevel=0;
                    bool GlossLevelenabled=false;
                    int glossLevel=0;
                    for (p=0; p<block->NumParams(); p++) {
                        ParamID pid = block->IndextoID(p);
                        ParamDef def = block->GetParamDef(pid);
                        ParamType2 typeID=block->GetParameterType(pid);
                        // TO DO: READ IN ALL THE MATERIAL PROPERTIES
                        char * paramName_ptr=W2A(def.int_name);
                        if (block->GetParameterType(pid)==TYPE_BOOL){
                            if (ATTREQ(paramName_ptr,"shadingenabled"))
                                useShading=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"multiPassMatenabled"))
                                multiPassMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"multiPassMat"))
                                multiPassMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"repeatMatenabled"))
                                repeatMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"repeatMat"))
                                repeatMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"smoothMatenabled"))
                                smoothMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"smoothMat"))
                                smoothMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"mipmapMatenabled"))
                                mipmapMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"mipmapMat"))
                                mipmapMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"alphaBlendingMatenabled"))
                                alphaBlendingMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"alphaBlendingMat"))
                                alphaBlendingMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"alphaTresholdMatenabled"))
                                alphaTresholdMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"premultipliedMatenabled"))
                                premultipliedMatenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"premultipliedMat"))
                                premultipliedMat=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"blendModeenabled"))
                                blendModeenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"MainUVenabled"))
                                MainUVenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"SecUVenabled"))
                                SecUVenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"AmbientLevelenabled"))
                                AmbientLevelenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"SpecularLevelenabled"))
                                SpecularLevelenabled=(0 != block->GetInt(pid));
                            if (ATTREQ(paramName_ptr,"GlossLevelenabled"))
                                GlossLevelenabled=(0 != block->GetInt(pid));
                        }
                        if (block->GetParameterType(pid)==TYPE_FLOAT){
                            if (ATTREQ(paramName_ptr,"alphaTresholdMat"))
                                alphaTresholdMat= block->GetFloat(pid);
                            if (ATTREQ(paramName_ptr,"ambientLevel"))
                                ambientLevel= block->GetFloat(pid);
                            if (ATTREQ(paramName_ptr,"specularLevel"))
                                specularLevel= block->GetFloat(pid);
                        }
                        if (block->GetParameterType(pid)==TYPE_INT){
                            if (ATTREQ(paramName_ptr,"blendMode"))
                                blendMode= block->GetInt(pid);
                            if (ATTREQ(paramName_ptr,"mainUVChannel"))
                                mainUVChannel= block->GetInt(pid);
                            if (ATTREQ(paramName_ptr,"secUVChannel"))
                                secUVChannel= block->GetInt(pid);
                            if (ATTREQ(paramName_ptr,"glossLevel"))
                                glossLevel= block->GetInt(pid);
                        }
                        free(paramName_ptr);
                    }
                    if(multiPassMatenabled)
                        awdMat->set_multiPass(multiPassMat);
                    if(repeatMatenabled)
                        awdMat->set_repeat(repeatMat);
                    if(smoothMatenabled)
                        awdMat->set_smooth(smoothMat);
                    if(mipmapMatenabled)
                        awdMat->set_mipmap(mipmapMat);
                    if(alphaBlendingMatenabled)
                        awdMat->set_alpha_blending(alphaBlendingMat);
                    if(alphaTresholdMatenabled)
                        awdMat->set_alpha_threshold(alphaTresholdMat);
                    if(premultipliedMatenabled)
                        awdMat->set_premultiplied(premultipliedMat);
                    if(blendModeenabled){
                        if(blendMode==1)awdMat->set_blendMode(0);//Normal
                        else if(blendMode==2)awdMat->set_blendMode(8);//Layer
                        else if(blendMode==3)awdMat->set_blendMode(10);//Multiply
                        else if(blendMode==4)awdMat->set_blendMode(1);//ADD
                        else if(blendMode==5)awdMat->set_blendMode(2);//Alpha						
                    }
                    if(MainUVenabled)
                        awdMat->set_mappingChannel(mainUVChannel);
                    if(SecUVenabled)
                        awdMat->set_secondMappingChannel(secUVChannel);	
                    if(AmbientLevelenabled)
                        awdMat->set_ambientStrength(ambientLevel);	
                    if(SpecularLevelenabled)
                        awdMat->set_specularStrength(specularLevel);	
                    if(GlossLevelenabled)
                        awdMat->set_glossStrength(glossLevel);	
                }
                free(localName_ptr);
            }
            if(!opts->IncludeShadings())
                useShading=false;
            for (t=0; t<attr->NumParamBlocks(); t++) {
                IParamBlock2 *block = attr->GetParamBlock(t);
                char * localName_ptr=W2A(block->GetLocalName());
                if ((ATTREQ(localName_ptr,"AWD_EffectMethodsparams"))&&(opts->IncludeMethods())){
                    for (p=0; p<block->NumParams(); p++) {
                        ParamID pid = block->IndextoID(p);
                        ParamDef def = block->GetParamDef(pid);
                        ParamType2 typeID=block->GetParameterType(pid);
                        char * paramName_ptr=W2A(def.int_name);
                        if (ATTREQ(paramName_ptr,"saveMethodIds") ){
                            int cntp=0;
                            AWDBlockList * newEffectList= new AWDBlockList();
                            for (cntp=0;cntp<block->Count(pid);cntp++){
                                char * effectlistID=W2A(block->GetStr(pid, 0, cntp));
                                AWDBlockList * foundEffectList=(AWDBlockList *)methodsCache->Get(effectlistID);
                                if (foundEffectList!=NULL){
                                    AWDBlock * awdBlock=NULL;
                                    AWDBlockIterator *it;
                                    it = new AWDBlockIterator(foundEffectList);
                                    while ((awdBlock = (AWDBlock*)it->next()) != NULL) {
                                        newEffectList->append(awdBlock);
                                    }
                                    delete it;
                                }
                                free(effectlistID);
                            }
                            awdMat->set_effectMethods(newEffectList);
                        }
                        free(paramName_ptr);
                    }
                }
                else if (ATTREQ(localName_ptr,"AWDShadingParams") ){
                    if (useShading){
                        awdMat->resetShadingMethods();
                        for (p=0; p<block->NumParams(); p++) {
                            ParamID pid = block->IndextoID(p);
                            ParamDef def = block->GetParamDef(pid);
                            ParamType2 typeID=block->GetParameterType(pid);
                            char * paramName_ptr=W2A(def.int_name);
                            if (block->GetParameterType(pid)==TYPE_INT){
                                if (ATTREQ(paramName_ptr,"ambientMethod") ){
                                    int ambientMethod=block->GetInt(pid);
                                    if (ambientMethod==2){
                                        newEnvMapMethod=new AWDShadingMethod(AWD_SHADEMETHOD_ENV_AMBIENT);
                                    }
                                }
                                if (ATTREQ(paramName_ptr,"diffuseMethod") ){
                                    int diffuseMethod=block->GetInt(pid);
                                    if (diffuseMethod==2)
                                        newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_GRADIENT);
                                    else if (diffuseMethod==3)
                                        newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_WRAP);
                                    else if (diffuseMethod==4)
                                        newDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_LIGHTMAP);
                                    else if (diffuseMethod==5)
                                        newDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_CELL);
                                
                                }
                                if (ATTREQ(paramName_ptr,"specMethod") ){
                                    int specularMethod=block->GetInt(pid);
                                    if (specularMethod==2)
                                        newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_ANISOTROPIC);
                                    if (specularMethod==3)
                                        newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_PHONG);
                                    if (specularMethod==4)
                                        newSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_CELL);
                                    if (specularMethod==5)
                                        newSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_FRESNEL);
                                
                                }
                                if (ATTREQ(paramName_ptr,"normalMethod") ){
                                    int normalMethod=block->GetInt(pid);
                                    if (normalMethod==2){
                                        newNormalMethod=new AWDShadingMethod(AWD_SHADEMETHOD_NORMAL_SIMPLE_WATER);
                                    }
                                }
                            }
                            free(paramName_ptr);
                        }
                    }
                }
                free(localName_ptr);
            }
            if (useShading){
                if (newDiffuseMethod!=NULL || newSpecularMethod!=NULL){
                    for (t=0; t<attr->NumParamBlocks(); t++) {
                        IParamBlock2 *block = attr->GetParamBlock(t);
                        char * localName_ptr=W2A(block->GetLocalName());
                        if (ATTREQ(localName_ptr,"LightMapDiffuseParams") ){
                            if (newDiffuseMethod!=NULL && newDiffuseMethod->get_shading_type()==AWD_SHADEMETHOD_DIFFUSE_LIGHTMAP){
                                for (p=0; p<block->NumParams(); p++) {
                                    ParamID pid = block->IndextoID(p);
                                    ParamDef def = block->GetParamDef(pid);
                                    ParamType2 typeID=block->GetParameterType(pid);
                                    char * paramName_ptr=W2A(def.int_name);
                                    if (block->GetParameterType(pid)==TYPE_INT){
                                        if (ATTREQ(paramName_ptr,"lightMap_baseMethod") ){
                                            int baseMethod=block->GetInt(pid);
                                            if (baseMethod==2)
                                                newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_GRADIENT);
                                            if (baseMethod==3)
                                                newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_WRAP);
                                        }
                                        if (ATTREQ(paramName_ptr,"light_map_blendMode") ){
                                            int blendMode=block->GetInt(pid);//TODO take care that the blendmode is correct
                                            newDiffuseMethod->add_int8_property(PROPS_INT8_1, blendMode, 10);
                                        }
                                    }
                                    if (block->GetParameterType(pid)==TYPE_TEXMAP){
                                        if (ATTREQ(paramName_ptr,"light_map") ){
                                            Texmap *tex = block->GetTexmap(pid);
                                            AWDBitmapTexture * newtex=NULL;
                                            if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                                newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE);
                                                if (newtex!=NULL)
                                                    newDiffuseMethod->set_awdBlock1(newtex);
                                            }
                                        }
                                    }
                                    free(paramName_ptr);
                                }
                            }
                        }
                        if (ATTREQ(localName_ptr,"CellDiffuseParams") ){
                            if (newDiffuseMethod!=NULL && newDiffuseMethod->get_shading_type()==AWD_SHADEMETHOD_DIFFUSE_CELL){
                                for (p=0; p<block->NumParams(); p++) {
                                    ParamID pid = block->IndextoID(p);
                                    ParamDef def = block->GetParamDef(pid);
                                    ParamType2 typeID=block->GetParameterType(pid);
                                    char * paramName_ptr=W2A(def.int_name);
                                    if (block->GetParameterType(pid)==TYPE_FLOAT){
                                        if (ATTREQ(paramName_ptr,"celldiffuse_smooth") ){
                                            double smooth=block->GetFloat(pid);//TODO take care that the blendmode is correct
                                            newDiffuseMethod->add_number_property(PROPS_NUMBER1, smooth, 0.1);
                                        }
                                    }
                                    if (block->GetParameterType(pid)==TYPE_INT){
                                        if (ATTREQ(paramName_ptr,"celldiffuse_level") ){
                                            int level=block->GetInt(pid);
                                            newDiffuseMethod->add_int8_property(PROPS_INT8_1, level, 3);
                                        }
                                        if (ATTREQ(paramName_ptr,"celldiffuse_baseMethod") ){
                                            int baseMethod=block->GetInt(pid);
                                            if (baseMethod==2)
                                                newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_GRADIENT);
                                            if (baseMethod==3)
                                                newBaseDiffuseMethod=new AWDShadingMethod(AWD_SHADEMETHOD_DIFFUSE_WRAP);
                                        }
                                    }
                                    free(paramName_ptr);
                                }
                            }
                        }
                        if (ATTREQ(localName_ptr,"CellSpecularParams") ){
                            if (newSpecularMethod!=NULL && newSpecularMethod->get_shading_type()==AWD_SHADEMETHOD_SPECULAR_CELL){
                                for (p=0; p<block->NumParams(); p++) {
                                    ParamID pid = block->IndextoID(p);
                                    ParamDef def = block->GetParamDef(pid);
                                    ParamType2 typeID=block->GetParameterType(pid);
                                    char * paramName_ptr=W2A(def.int_name);
                                    if (block->GetParameterType(pid)==TYPE_FLOAT){
                                        if (ATTREQ(paramName_ptr,"cellspec_smooth") ){
                                            double smooth=block->GetFloat(pid);
                                            newSpecularMethod->add_number_property(PROPS_NUMBER2, smooth, 0.1);
                                        }
                                        if (ATTREQ(paramName_ptr,"cellspec_cutoff") ){
                                            double cutOff=block->GetFloat(pid);
                                            newSpecularMethod->add_number_property(PROPS_NUMBER1, cutOff, 0.5);
                                        }
                                    }
                                    if (block->GetParameterType(pid)==TYPE_INT){
                                        if (ATTREQ(paramName_ptr,"cellspec_baseMethod") ){
                                            int baseMethod=block->GetInt(pid);
                                            if (baseMethod==2)
                                                newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_ANISOTROPIC);
                                            if (baseMethod==3)
                                                newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_PHONG);
                                        }
                                    }
                                    free(paramName_ptr);
                                }
                            }
                        }
                        if (ATTREQ(localName_ptr,"FresnelSpecularParams") ){
                            if (newSpecularMethod!=NULL && newSpecularMethod->get_shading_type()==AWD_SHADEMETHOD_SPECULAR_FRESNEL){
                                for (p=0; p<block->NumParams(); p++) {
                                    ParamID pid = block->IndextoID(p);
                                    ParamDef def = block->GetParamDef(pid);
                                    ParamType2 typeID=block->GetParameterType(pid);
                                    char * paramName_ptr=W2A(def.int_name);
                                    if (block->GetParameterType(pid)==TYPE_FLOAT){
                                        if (ATTREQ(paramName_ptr,"fresnelspec_reflect") ){
                                            double reflect=block->GetFloat(pid);
                                            newSpecularMethod->add_number_property(PROPS_NUMBER2, reflect, 0.1);
                                        }
                                    }
                                    if (block->GetParameterType(pid)==TYPE_BOOL){
                                        if (ATTREQ(paramName_ptr,"fresnelspec_basedOnSurface") ){
                                            bool basedOnSurface = (0 != block->GetInt(pid));
                                            newSpecularMethod->add_bool_property(PROPS_BOOL1, basedOnSurface, true);
                                        }
                                    }
                                    if (block->GetParameterType(pid)==TYPE_INT){
                                        if (ATTREQ(paramName_ptr,"fresnelspec_power") ){
                                            int power=block->GetInt(pid);
                                            newSpecularMethod->add_int_property(PROPS_INT1, power, 5);
                                        }
                                        if (ATTREQ(paramName_ptr,"fresnelspec_baseMethod") ){
                                            int baseMethod=block->GetInt(pid);
                                            if (baseMethod==2)
                                                newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_ANISOTROPIC);
                                            if (baseMethod==3)
                                                newBaseSpecularMethod=new AWDShadingMethod(AWD_SHADEMETHOD_SPECULAR_PHONG);
                                        }
                                    }
                                    free(paramName_ptr);
                                }
                            }
                        }
                        free(localName_ptr);
                    }
                }
                for (t=0; t<attr->NumParamBlocks(); t++) {
                    int p=0;
                    IParamBlock2 *block = attr->GetParamBlock(t);
                    char * localName_ptr=W2A(block->GetLocalName());
                    if (ATTREQ(localName_ptr,"EnvMapAmbientParams") ){
                        if (newEnvMapMethod!=NULL){
                            for (p=0; p<block->NumParams(); p++) {
                                ParamID pid = block->IndextoID(p);
                                ParamDef def = block->GetParamDef(pid);
                                ParamType2 typeID=block->GetParameterType(pid);
                                char * paramName_ptr=W2A(def.int_name);
                                if (typeID==TYPE_REFTARG){
                                    if (ATTREQ(paramName_ptr,"ambient_envMap")){
                                        //TODO: THERE IS NO OPTION TO ADD THE ENVMAP IN THE INTERFACE YET, so this is not tested:
                                        Mtl *thisMtl = (Mtl *)block->GetReferenceTarget(pid);
                                        if (thisMtl!=NULL){
                                            MSTR matClassName;
                                            thisMtl->GetClassName(matClassName);
                                            char * matClassName_ptr=W2A(matClassName);
                                            if (ATTREQ(matClassName_ptr,"AWDCubeMaterial")){
                                                AWDCubeTexture* newCubetex = GetCustomAWDCubeTexSettings((MultiMtl *)thisMtl);
                                                if (newCubetex!=NULL){
                                                    newEnvMapMethod->set_awdBlock1(newCubetex);
                                                }
                                            }
                                            free(matClassName_ptr);
                                        }
                                    }
                                }
                                free(paramName_ptr);
                            }
                        }
                    }
                    else if (ATTREQ(localName_ptr,"GradientDiffuseParams") ){
                        if (newBaseDiffuseMethod!=NULL && newBaseDiffuseMethod->get_shading_type()==AWD_SHADEMETHOD_DIFFUSE_GRADIENT){
                            for (p=0; p<block->NumParams(); p++) {
                                ParamID pid = block->IndextoID(p);
                                ParamDef def = block->GetParamDef(pid);
                                ParamType2 typeID=block->GetParameterType(pid);
                                char * paramName_ptr=W2A(def.int_name);
                                if (ATTREQ(paramName_ptr,"gradientdiffuse_map") ){
                                    if (block->GetParameterType(pid)==TYPE_TEXMAP){
                                        Texmap *tex = block->GetTexmap(pid);
                                        AWDBitmapTexture * newtex=NULL;
                                        if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                            newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE);
                                            if (newtex!=NULL)
                                                newBaseDiffuseMethod->set_awdBlock1(newtex);
                                        }
                                    }
                                }
                                free(paramName_ptr);
                            }
                        }
                    }
                    else if (ATTREQ(localName_ptr,"WarpDiffuseParams") ){
                        if (newBaseDiffuseMethod!=NULL && newBaseDiffuseMethod->get_shading_type()==AWD_SHADEMETHOD_DIFFUSE_WRAP){
                            for (p=0; p<block->NumParams(); p++) {
                                ParamID pid = block->IndextoID(p);
                                ParamDef def = block->GetParamDef(pid);
                                ParamType2 typeID=block->GetParameterType(pid);
                                char * paramName_ptr=W2A(def.int_name);
                                if (block->GetParameterType(pid)==TYPE_FLOAT){
                                    if (ATTREQ(paramName_ptr,"warpdiffuse_factor") ){
                                        double factor=block->GetFloat(pid);
                                        newBaseDiffuseMethod->add_number_property(PROPS_NUMBER1, factor, 5);
                                    }
                                }
                                free(paramName_ptr);
                            }
                        }
                    }
                    else if (ATTREQ(localName_ptr,"SimpleWaterNormalParams") ){
                        if (newNormalMethod!=NULL){
                            for (p=0; p<block->NumParams(); p++) {
                                ParamID pid = block->IndextoID(p);
                                ParamDef def = block->GetParamDef(pid);
                                ParamType2 typeID=block->GetParameterType(pid);
                                char * paramName_ptr=W2A(def.int_name);
                                if (ATTREQ(paramName_ptr,"waternormal_map") ){
                                    if (block->GetParameterType(pid)==TYPE_TEXMAP){
                                        Texmap *tex = block->GetTexmap(pid);
                                        AWDBitmapTexture * newtex=NULL;
                                        if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                                            newtex=ExportBitmapTexture((BitmapTex *)tex, NULL, UNDEFINEDTEXTYPE);
                                            if (newtex!=NULL)
                                                newNormalMethod->set_awdBlock1(newtex);
                                        }
                                    }
                                }
                                free(paramName_ptr);
                            }
                        }
                    }
                    free(localName_ptr);
                }
                if(newEnvMapMethod!=NULL)
                    awdMat->add_method(newEnvMapMethod);

                if(newBaseDiffuseMethod!=NULL)
                    awdMat->add_method(newBaseDiffuseMethod);
                if(newDiffuseMethod!=NULL)
                    awdMat->add_method(newDiffuseMethod);
                
                if(newBaseSpecularMethod!=NULL)
                    awdMat->add_method(newBaseSpecularMethod);
                if(newSpecularMethod!=NULL)
                    awdMat->add_method(newSpecularMethod);
                
                if(newNormalMethod!=NULL)
                    awdMat->add_method(newNormalMethod);
            }
        }
    }	
}

void MaxAWDExporter::ExportUserAttributes(Animatable *obj, AWDAttrElement *elem)
{
    if (!opts->ExportAttributes())
        return;

    ICustAttribContainer *attributes = obj->GetCustAttribContainer();
    if (attributes) {
        int a=0;
        int numAttribs=0;
        numAttribs = attributes->GetNumCustAttribs();
        for (a=0; a<numAttribs; a++) {
            int k=0;
            CustAttrib *attr = attributes->GetCustAttrib(a);
            
            for (k=0; k<attr->NumParamBlocks(); k++) {
                int p=0;
                IParamBlock2 *block = attr->GetParamBlock(k);
                if (block!=NULL){
                    char * blockName_ptr=W2A(block->GetLocalName());
                    if (ATTREQ(blockName_ptr,"AWDObjectSettingsParams") ){	}
                    // the next three should not occur yet, as we do not read Custom-properties on materials yet
                    else if (ATTREQ(blockName_ptr,"AWD_MaterialSettingsparams") ){	}
                    else if (ATTREQ(blockName_ptr,"AWD_EffectMethodsparams") ){	}
                    else if (ATTREQ(blockName_ptr,"AWDShadingParams") ){	}
                    else{
                        for (p=0; p<block->NumParams(); p++) {
                            ParamID pid = block->IndextoID(p);
                            Color col;
                            AColor acol;

                            Interval valid = FOREVER;

                            awd_uint16 len = 0;
                            AWD_field_type type = AWD_FIELD_FLOAT32;
                            AWD_field_ptr ptr;
                            ptr.v = NULL;

                            switch (block->GetParameterType(pid)) {
                                case TYPE_ANGLE:
                                case TYPE_PCNT_FRAC:
                                case TYPE_WORLD:
                                case TYPE_FLOAT:
                                    type = AWD_FIELD_FLOAT64;
                                    len = sizeof(awd_float64);
                                    ptr.v = malloc(len);
                                    *ptr.f64 = block->GetFloat(pid);
                                    break;

                                case TYPE_TIMEVALUE:
                                case TYPE_INT:
                                    type = AWD_FIELD_INT32;
                                    len = sizeof(awd_int32);
                                    ptr.v = malloc(len);
                                    *ptr.i32 = block->GetInt(pid);
                                    break;

                                case TYPE_BOOL:
                                    type = AWD_FIELD_BOOL;
                                    len = sizeof(awd_bool);
                                    ptr.v = malloc(len);
                                    *ptr.b = (0 != block->GetInt(pid));
                                    break;

                                case TYPE_FILENAME:
                                case TYPE_STRING:
                                    type = AWD_FIELD_STRING;
                                    ptr.str = (char*)block->GetStr(pid);
                                    len = strlen(ptr.str);
                                    break;

                                case TYPE_RGBA:
                                    type = AWD_FIELD_COLOR;
                                    len = sizeof(awd_color);
                                    col = block->GetColor(pid);
                                    ptr.v = malloc(len);
                                    *ptr.col = awdutil_float_color(col.r, col.g, col.b, 1.0);
                                    break;

                                case TYPE_FRGBA:
                                    type = AWD_FIELD_COLOR;
                                    len = sizeof(awd_color);
                                    acol = block->GetAColor(pid);
                                    ptr.v = malloc(len);
                                    *ptr.col = awdutil_float_color(acol.r, acol.g, acol.b, acol.a);
                                    break;
                            }

                            if (ptr.v != NULL) {
                                ParamDef def = block->GetParamDef(pid);
                                if (ns == NULL) {
                                    // Namespace has not yet been created; ns is a class
                                    // variable that will be created only once and then
                                    // reused for all user attributes.
                                    char * ns_ptr=opts->AttributeNamespace();//dont free, as this will get freed in the opts delete
                                    ns = new AWDNamespace(ns_ptr, strlen(ns_ptr));
                                    awd->add_namespace(ns);
                                }		
                                char * thisName=W2A(def.int_name);
                                elem->set_attr(ns, thisName, strlen(thisName), ptr, len, type);
                                free(thisName);
                            }
                        }
                    }
                    free(blockName_ptr);
                }
            }
        }
    }
}

AWDCubeTexture * MaxAWDExporter::GetCustomAWDCubeTexSettings(MultiMtl * awdCubeMat)
{
    
    AWDCubeTexture * cubeTex = (AWDCubeTexture * )cubeMatCache->Get(awdCubeMat);
    if (cubeTex==NULL){
        char * cubetexName_ptr=W2A(awdCubeMat->GetName());
        cubeTex = new AWDCubeTexture(cubetexName_ptr, strlen(cubetexName_ptr));
        free(cubetexName_ptr);
        awd->add_cube_texture(cubeTex);
        cubeMatCache->Set(awdCubeMat , cubeTex);
        try{
        }
        catch(...){

        }
        int num_params = awdCubeMat->NumParamBlocks();
        int p=0;
        int x=0;
        BitmapTex * upTex = NULL;
        BitmapTex * downTex = NULL;
        BitmapTex * leftTex = NULL;
        BitmapTex * rightTex = NULL;
        BitmapTex * frontTex = NULL;
        BitmapTex * backTex = NULL;
        bool replacetex = false;
        char * replaceURL_ptr=NULL;
        int saveType=0;
        for (x=0; x<awdCubeMat->NumParamBlocks(); x++) {
            IParamBlock2* pb = GetParamBlock2ByIndex((ReferenceMaker*)awdCubeMat, x);
            for (p=0; p<pb->NumParams(); p++) {
                ParamID pid = pb->IndextoID(p);
                ParamDef def = pb->GetParamDef(pid);
                char * paramName_ptr=W2A(def.int_name);
                if (pb->GetParameterType(pid)==TYPE_BOOL){
                    if (ATTREQ(paramName_ptr,"saveReplace"))
                        replacetex = (0 != pb->GetInt(pid));
                }
                if (pb->GetParameterType(pid)==TYPE_STRING){
                    if (ATTREQ(paramName_ptr,"saveReplaceURL"))
                        replaceURL_ptr = W2A(pb->GetStr(pid));
                }
                if (pb->GetParameterType(pid)==TYPE_INT){
                    if (ATTREQ(paramName_ptr,"saveTexType")){
                        int texType=pb->GetInt(pid);
                        if (texType==2)
                            cubeTex->set_tex_type(EXTERNAL);
                        if (texType==3)
                            cubeTex->set_tex_type(EMBEDDED);
                    }
                }
                if (pb->GetParameterType(pid)==TYPE_TEXMAP){
                    if (ATTREQ(paramName_ptr,"upSave"))
                        upTex = (BitmapTex*)pb->GetTexmap(pid);
                    if (ATTREQ(paramName_ptr,"downSave"))
                        downTex = (BitmapTex*)pb->GetTexmap(pid);
                    if (ATTREQ(paramName_ptr,"leftSave"))
                        leftTex = (BitmapTex*)pb->GetTexmap(pid);
                    if (ATTREQ(paramName_ptr,"rightSave"))
                        rightTex = (BitmapTex*)pb->GetTexmap(pid);
                    if (ATTREQ(paramName_ptr,"frontSave"))
                        frontTex = (BitmapTex*)pb->GetTexmap(pid);
                    if (ATTREQ(paramName_ptr,"backSave"))
                        backTex = (BitmapTex*)pb->GetTexmap(pid);
                }
                free(paramName_ptr);
            }
        }
        if (cubeTex->get_tex_type()==UNDEFINEDTEXTYPE){
            int saveType=opts->TextureMode();
            if (saveType==1)
                cubeTex->set_tex_type(EMBEDDED);
            if (saveType==2)
                cubeTex->set_tex_type(EXTERNAL);
            if (saveType==3)
                cubeTex->set_tex_type(EXTERNAL);
        }

        if (!replacetex){
            if((upTex==NULL)||(downTex==NULL)||(leftTex==NULL)||(rightTex==NULL)||(frontTex==NULL)||(backTex==NULL)){
                //error no bitmap set
            }
            else{
                cubeTex->get_texture_blocks()->force_append(ExportBitmapTexture(leftTex,  NULL, cubeTex->get_tex_type()));
                cubeTex->get_texture_blocks()->force_append(ExportBitmapTexture(rightTex, NULL, cubeTex->get_tex_type()));
                cubeTex->get_texture_blocks()->force_append(ExportBitmapTexture(upTex,    NULL, cubeTex->get_tex_type()));
                cubeTex->get_texture_blocks()->force_append(ExportBitmapTexture(downTex,  NULL, cubeTex->get_tex_type()));
                cubeTex->get_texture_blocks()->force_append(ExportBitmapTexture(frontTex, NULL, cubeTex->get_tex_type()));
                cubeTex->get_texture_blocks()->force_append(ExportBitmapTexture(backTex,  NULL, cubeTex->get_tex_type()));
            }
        }
        else{
            if (strlen(replaceURL_ptr)==0){
                //Error = no URL SET
            }
            else{
                //TODO: implement ATFCubetexture in AWD 
            }
        }
        if (replaceURL_ptr!=NULL)
            free(replaceURL_ptr);
    }
    return cubeTex;
}

void MaxAWDExporter::CreateDarkLights(){
    if((!opts->IncludeLights())||(!opts->ApplyDarkLight()))
        return;
    AWDMaterial *block;
    AWDBlockIterator *it;
    AWDBlockList * matBlockList = (AWDBlockList *)awd->get_material_blocks();
    if (matBlockList!=NULL){
        if (matBlockList->get_num_blocks()>0){
            it = new AWDBlockIterator(matBlockList);
            while ((block = (AWDMaterial * ) it->next()) != NULL) {
                if(block->get_lightPicker()==NULL){
                    AWDLightPicker * darkLightPicker = awd->CreateDarkLightPicker();
                    if (darkLightPicker!=NULL)
                        block->set_lightPicker(darkLightPicker);
                }
                AWDBlockIterator *itClones;
                AWDMaterial *cloneBlock;
                AWDBlockList * thisBlockList = (AWDBlockList *)block->get_materialClones();
                if((thisBlockList!=NULL)&&(thisBlockList->get_num_blocks()>0)){
                    itClones = new AWDBlockIterator(thisBlockList);
                    while ((cloneBlock = (AWDMaterial * ) it->next()) != NULL) {
                        if(cloneBlock->get_lightPicker()==NULL){
                            AWDLightPicker * darkLightPickerClone = awd->CreateDarkLightPicker();
                            if (darkLightPickerClone!=NULL)
                                cloneBlock->set_lightPicker(darkLightPickerClone);
                        }
                    }
                    delete itClones;
                }
            }
            delete it;
        }
    }
}


// ---> functions called by ProcessGeoBlocks():

void MaxAWDExporter::ExportTriGeom(AWDTriGeom *awdGeom, Object *obj, INode *node, ISkin *skin, IGameMesh * igame_mesh)
{
    if (awdGeom != NULL) {
        if (awdGeom->get_is_created())
            return;
        else {
            awdGeom->set_is_created(true);
            
            // Extract skinning information (returns number of joints per vertex),
            // and writes the weights and joints array according to the jpv
            int jpv=0;
            awd_float64 *weights=NULL;
            awd_uint32 *joints=NULL;
            jpv = ExportSkin(node, skin, &weights, &joints);
            // Calculate offset matrix from the object TM (which includes geometry offset)
            // this will be used to transform all vertices into node space.
            int time=maxInterface->GetTime();
            
            // get the neutral pose time from the vertex (using a didcated cache for this)
            bool hasVertexAnim=vetexAnimNeutralPosecache->hasKey(node);
            if(hasVertexAnim)
                time=vetexAnimNeutralPosecache->Get(node)*GetTicksPerFrame();
            else{
                // get the neutral pose time from the skeleton, if any is used
                if (opts->ExportSkin() && skin && jpv>0) {
                    SkeletonCacheItem *skel = skeletonCache->GetFromBone(skin->GetBone(0));
                    time=skel->awdSkel->get_neutralPose();
                }
            }

            Matrix3 offsMtx = node->GetObjectTM(time) * Inverse(node->GetNodeTM(time));
            bool isInSkinPoseMode=true;
            double *mtxData = (double *)malloc(12*sizeof(double));
            if (skin && jpv>0) {
                // if the mesh is not in "Skin Pose Mode", 
                // we add the bind-matrix to the offset matrix (so that all points are moved accoridingly)
                // and later we set the transform matrix of the mesh to the identity-matrix (no transform)
                ISkinPose * skinPose;
                skinPose=skinPose->GetISkinPose(*node);
                isInSkinPoseMode=skinPose->SkinPoseMode();
                Matrix3 mtx;
                mtx.IdentityMatrix();
                if (!isInSkinPoseMode){
                    Matrix3 bm;
                    bm.IdentityMatrix();
                    skin->GetSkinInitTM(node, bm, true);
                    offsMtx *= bm;
                    SerializeMatrix3(mtx, mtxData);
                }	
                skinPose=NULL;
            }
            
            ObjectState os;
            // Flatten entire modifier stack
            os = node->EvalWorldState(time);
            obj = os.obj;
            // its allready been taken care that the correct obj is submitted to this function, so we can directly convert to TriObject (?)
            TriObject *triObject = (TriObject*)obj->ConvertToType(time, Class_ID(TRIOBJ_CLASS_ID, 0));	

            Mesh mesh = triObject->mesh;

            int numTris = mesh.getNumFaces();
            int numVerts = mesh.getNumVerts();
            awdGeom->set_originalPointCnt(numVerts);

            // This could happen for example with splines (No!, this should never happen, because we check for this earlier (?))
            if (numTris==0)
                return;
            
            bool force_split=opts->SplitByMatID();// if true, the GeomUtils will create a SubGeo for each MaterialID used by a face, no matter if they share materials or not
            bool useUV=opts->ExportUVs();// TODO: check if uvs exists
            bool useSecUVs=useUV; // TODO: check if second UVs exists (and are requested)
            bool useNormals=opts->ExportNormals();

            // ATTENTION: 
            // we have collected all meshintsances that are using this geometry.
            // but some material-settings ( UV / SecondUV / explode) can force us to create multiple geometries...

            // the IGAmeMesh gives acces to some handy functions for exporting meshes
            // we still need to use the mesh from the standart api, to have access to the correct UV (?)

            MeshNormalSpec *specificNormals = NULL;
            if(igame_mesh!=NULL){
                int numTrisGameMesh = igame_mesh->GetNumberOfFaces();
                if (numTrisGameMesh!=numTris){
                    return; //ERROR: faceCount of game-mesh is not facecount of api-mesh - should not happen
                }
                AWDBlockList * meshInstanceList = awdGeom->get_mesh_instance_list();
                if (meshInstanceList==NULL){
                    return; //ERROR: faceCount of game-mesh is not facecount of api-mesh - should not happen
                }
                int numMeshInstances=meshInstanceList->get_num_blocks();
                if ((meshInstanceList!=NULL)&&(numMeshInstances==0)){
                    return; //ERROR: faceCount of game-mesh is not facecount of api-mesh - should not happen
                }
                // check if the first UVChannel is available for this mesh ( numTVFaces must be equal to numTris)
                if (useUV) {
                    try{
                        if (mesh.mapSupport(1)){
                            MeshMap * mesh_map;
                            mesh_map = &(mesh.Map(1));
                            int numTrisMap = mesh_map->getNumFaces();
                            if (numTrisMap!=numTris){
                                useUV=false;
                                useSecUVs=false;
                            }
                        }
                        else{
                            useUV=false;
                            useSecUVs=false;
                        }

                    }
                    catch(...){
                        useUV=false;
                        useSecUVs=false;
                    }
                }
                // check if any normals are available for the mesh
                if (useNormals) {
                    mesh.SpecifyNormals();
                    specificNormals = mesh.GetSpecifiedNormals();
                    int  specificNormalCount = specificNormals->GetNumNormals();// for me, this is allways been 0
                    if (specificNormalCount==0){
                        specificNormals=NULL;
                        int numNorms = igame_mesh->GetNumberOfNormals();
                        if (numNorms==0){
                            useNormals=false;
                        }
                    }
                }
                
                AWD_field_type precision_geo=AWD_FIELD_FLOAT32;
                if (opts->StorageGeometry()==1)
                    precision_geo=AWD_FIELD_FLOAT64;
                AWDGeomUtil * geomUtil=new AWDGeomUtil(awdGeom->get_split_faces(), force_split, useUV, useSecUVs, useNormals, 0.0, jpv, precision_geo);
                // create a list of GUGeom for each Mesh instance. 
                // before collecting the actual geom-data,
                // we will reduce the number of GUGeoms to the minimum needed to display all mesh-instances correctly
                geomUtil->createPreGeometries(meshInstanceList);
                Tab<int> MatIDList=igame_mesh->GetActiveMatIDs();
                int matIDCnt;
                Tab<FaceEx *> facelist;
                // for each submesh do:
                for (matIDCnt=0; matIDCnt< MatIDList.Count(); matIDCnt++){
                    facelist=igame_mesh->GetFacesFromMatID(MatIDList[matIDCnt]);
                    int idx;
                    int faceCnt=facelist.Count();
                    if (faceCnt>0){
                        // if the submesh will be used (if the matID is used by any face):
                        AWDBlockList * subMaterialList = new AWDBlockList();
                        int meshInstCnt=0;
                        // for each mesh instance, apply the material 
                        for (meshInstCnt=0;meshInstCnt<numMeshInstances; meshInstCnt++){
                            AWDMeshInst * awdMesh=(AWDMeshInst *)meshInstanceList->getByIndex(meshInstCnt);
                            if (awdMesh==NULL){
                                return;
                            }
                            else{
                                AWDBlockList * preMaterials = awdMesh->get_pre_materials();
                                if (preMaterials==NULL){
                                    return;
                                }
                                else{
                                    bool createDefault = false;
                                    AWDMaterial * thisMatBlock = (AWDMaterial *)preMaterials->getByIndex(MatIDList[matIDCnt]);
                                    if (thisMatBlock==NULL){
                                        thisMatBlock=(AWDMaterial *)awdMesh->get_defaultMat();
                                    }
                                    if (useUV){
                                        if (thisMatBlock->get_mappingChannel()>0)
                                            thisMatBlock->set_mappingChannel(checkIfUVMapExists(mesh, numTris, thisMatBlock->get_mappingChannel()));
                                    }
                                    if (useSecUVs){
                                        if (thisMatBlock->get_secondMappingChannel()>0)
                                            thisMatBlock->set_secondMappingChannel(checkIfUVMapExists(mesh, numTris, thisMatBlock->get_secondMappingChannel()));
                                    }
                                    subMaterialList->force_append(thisMatBlock);
                                }
                            }
                        }
                        geomUtil->add_new_sub_geo_to_preGUgeoms(subMaterialList, MatIDList[matIDCnt]);
                        delete subMaterialList;
                    }
                }
                geomUtil->createGeometries();
                
                for (matIDCnt=0; matIDCnt<MatIDList.Count(); matIDCnt++){
                    facelist=igame_mesh->GetFacesFromMatID(MatIDList[matIDCnt]);
                    int idx;
                    int faceCnt=facelist.Count();
                    if (faceCnt>0){
                        int numGeoms=geomUtil->get_geoList()->get_num_blocks();
                        int geoCnt;
                        for(geoCnt=0; geoCnt<numGeoms; geoCnt++){
                            GUGeo * thisGUGeo = geomUtil->get_geoList()->get_by_idx(geoCnt);
                            // get the uvs using the index stored in the geomUtil
                            // get the secondUV using the index stored in the geomUtil
                            // get the explode (normals) using the index stored in the geomUtil

                            // ATTENTION: its not a Subgeo but a SubgeoGroup, so it contain several Subgeos if the face or vert lists are to big for one subgeo
                            int thisSubGeoIdx=geomUtil->getSubGeoIdxForMatIdx(MatIDList[matIDCnt]);
                            GUSubGeoGroup *thisSubGeoGroup = thisGUGeo->get_subGeomList()->get_by_idx(thisSubGeoIdx);
                            if (thisSubGeoGroup==NULL){
                                return;
                            }
                            AWDMaterial * thisAWDMat=(AWDMaterial *)thisSubGeoGroup->materials->getByIndex(0);
                            bool explode=thisAWDMat->get_is_faceted();

                            MeshMap * mainUVMeshMap=NULL;
                            MeshMap * secondUVMeshMap=NULL;
                            
                            if (thisSubGeoGroup->include_uv){
                                if (thisAWDMat->get_mappingChannel()>0)
                                    mainUVMeshMap = &(mesh.Map(thisAWDMat->get_mappingChannel()));
                            }
                            if (thisSubGeoGroup->include_suv){
                                if (thisAWDMat->get_secondMappingChannel()>0)
                                    secondUVMeshMap = &(mesh.Map(thisAWDMat->get_secondMappingChannel()));
                            }
                            // for each face in the list do:
                            bool hasMultipleUV=false;
                            for (idx=0;idx<faceCnt;idx++){
                                // this will create a new SubGeo inside the SubgeoGroup, if the limits are reached
                                thisSubGeoGroup->check_limits();
                                // create a new vert
                                FaceEx * f=facelist[idx];
                                int apiFaceIdx=f->meshFaceIndex;
                                TVFace tvface;
                                TVFace tvFaceSecond; 
                                Face face = mesh.faces[apiFaceIdx];
                                DWORD *inds = face.getAllVerts();
                                if (thisSubGeoGroup->include_uv){
                                    if (mainUVMeshMap!=NULL)
                                        tvface = mainUVMeshMap->tf[apiFaceIdx];
                                    else
                                        tvface = mesh.tvFace[apiFaceIdx];
                                }
                                if (thisSubGeoGroup->include_suv){
                                    if (secondUVMeshMap!=NULL)
                                        tvFaceSecond = secondUVMeshMap->tf[apiFaceIdx];
                                    else
                                        tvFaceSecond = mesh.tvFace[apiFaceIdx];
                                }	
                                Point3 faceNormal;
                                if (geomUtil->include_normals) {
                                    // if we want to export normals, but no normals was read, than we need to calculate ourself,
                                    // using the face-normal for the angle-calulation
                                    if ((igame_mesh==NULL && specificNormals==NULL)||(explode)){	
                                        // faceNormal = mesh.getFaceNormal(t); // this crashes 3dsmax (why?), so calulate the facenormal manually:	
                                         Point3 v0, v1, v2;
                                         Tab<Point3> fnorms;
                                         v0 = mesh.getVert(face.getVert(0));
                                         v1 = mesh.getVert(face.getVert(1));
                                         v2 = mesh.getVert(face.getVert(2));
                                         faceNormal = (v1-v0)^(v2-v1);
                                         faceNormal = Normalize(faceNormal);		
                                    }				
                                }
                                int v;
                                for (v=2; v>=0; v--) {
                    
                                    int vIdx = face.getVert(v);
                                    Point3 vtx = offsMtx * mesh.getVert(vIdx);

                                    vdata *vd = (vdata *)malloc(sizeof(vdata));
                                    vd->orig_idx = vIdx;
                                    vd->x = vtx.x;
                                    vd->y = vtx.z;
                                    vd->z = vtx.y;
                                    // Might not have UV coords
                                    if (geomUtil->include_uv) {
                                        int tvIdx;
                                        Point3 tvtx;
                                        Point3 stvtx;
                                        if (mainUVMeshMap!=NULL)
                                            tvtx=mainUVMeshMap->tv[tvface.t[v]];
                                        else{
                                            tvIdx = tvface.getTVert(v);
                                            tvtx = mesh.getTVert(tvIdx);
                                        }
                                    
                                        if (secondUVMeshMap!=NULL)
                                            stvtx=secondUVMeshMap->tv[tvFaceSecond.t[v]];
                                        else{
                                            tvIdx = tvface.getTVert(v);
                                            stvtx = mesh.getTVert(tvIdx);
                                        }
                                        vd->u = tvtx.x;
                                        vd->v = 1.0-tvtx.y;
                                        vd->su = stvtx.x;
                                        vd->sv = 1.0-stvtx.y;
                                        
                                    }
                    
                                    if (geomUtil->include_normals) {
                                        Point3 normal;
                                        // if specific vertex-normals was found, we use it, if the subgeo is not set to explode
                                        if ((specificNormals!=NULL) && (!explode)){
                                            normal = specificNormals->GetNormal(apiFaceIdx, v);
                                        }
                                        // else if a (not specific) vertex-normals was found (on the igame-mesh), we use it, if the subgeo is not set to explode
                                        else if ((igame_mesh!=NULL) && (!explode)){
                                            igame_mesh->GetNormal(f->norm[v], normal, true);
                                        }
                                        // else: since we still want normals exported, we use the face-normal (using facenormal with threshold of 0 will explode the mesh
                                        else{
                                            // i dont think this should really get executed anymore (since the igame-object allways should give access to the normals)
                                            normal=faceNormal;
                                        }
                                        // if the object is skinned, we get the global normals
                                        if (jpv>0){
                                            if (normal)
                                                normal=offsMtx*normal;
                                        }
                                        
                                        vd->nx = normal.x;
                                        vd->ny = normal.z;
                                        vd->nz = normal.y;
                                    }

                                    // If there is skinning information, copy it from the weight
                                    // and joint index arrays returned by ExportSkin() above.
                                    vd->num_bindings = jpv;
                                    if (jpv > 0) {
                                        vd->weights = (awd_float64*)malloc(jpv*sizeof(awd_float64));
                                        vd->joints = (awd_uint32*)malloc(jpv*sizeof(awd_uint32));

                                        int memoffs = jpv*vIdx;
                                        memcpy(vd->weights, weights+memoffs, jpv*sizeof(awd_float64));
                                        memcpy(vd->joints, joints+memoffs, jpv*sizeof(awd_uint32));
                                    }

                                    vd->force_hard = false;
                                    
                                    // add the new vertex to the subgeo
                                    thisSubGeoGroup->append_vdata(vd);
                                }
                            }
                        }
                    }
                }
                AWDBlockList * returned_geoms =  geomUtil->build_geom(awdGeom);
                
                AWDBlockIterator * it=NULL;
                AWDMeshInst * block;			
                int maxCount=0;
                int geomCnt=0;
                for (geomCnt=0; geomCnt<returned_geoms->get_num_blocks();geomCnt++){
                    AWDTriGeom * thisAWDGeom=(AWDTriGeom *)returned_geoms->getByIndex(geomCnt);
                    it = new AWDBlockIterator(thisAWDGeom->get_mesh_instance_list());
                    AWDSubGeom *sub;
                    sub = thisAWDGeom->get_first_sub();
                    while (sub) {
                        AWDBlockList *subGeoGroupMatList=sub->get_materials();
                        int thisIdx=0;
                        it->reset();
                        while ((block = (AWDMeshInst*)it->next()) != NULL) {
                            block->set_geom(thisAWDGeom);
                            if (!isInSkinPoseMode){
                                block->set_transform(mtxData);
                            }
                            AWDMaterial * thisMat=(AWDMaterial *)subGeoGroupMatList->getByIndex(thisIdx);
                            if (thisMat==NULL){
                                int test=0;
                                //ERROR - this should never happen
                            }
                            else{
                                AWDLightPicker * lightPicker=(AWDLightPicker *)block->get_lightPicker();
                                if (lightPicker!=NULL){
                                    thisMat=thisMat->get_material_for_lightPicker(lightPicker);
                                    if(opts->SetMultiPass()){
                                        // multipass using the number of lights, that the lightpicker uses
                                        if (lightPicker->get_lights()->get_num_blocks()>3){
                                            thisMat->set_multiPass(true);
                                        }
                                        else{
                                            thisMat->set_multiPass(false);
                                        }
                                    }
                                }
                                block->add_material((AWDMaterial*)thisMat);
                            }
                            thisIdx++;
                        }
                        sub = sub->next;
                    }
                    delete it;
                }
                delete returned_geoms;
                // If conversion created a new object, dispose it
                if (triObject != obj) 
                    triObject->DeleteMe();
                delete geomUtil;
            }
            else{
            }
            free(mtxData);
            if (weights!=NULL)
                free(weights);
            if (joints!=NULL)
                free(joints);
        }
    }
    return;
}

int MaxAWDExporter::checkIfUVMapExists(Mesh mesh, int numTris, int mapIdx){
    int numUVMaps = mesh.getNumMaps();	
    int t=0;
    if (numUVMaps>2){	
        if (numUVMaps>=mapIdx){	
            if (mesh.mapSupport(mapIdx)){
                MeshMap * mesh_map;
                mesh_map = &(mesh.Map(mapIdx));
                int numTrisMap = mesh_map->getNumFaces();
                if (numTrisMap==numTris){
                    return mapIdx;
                }
            }
        }
    }
    return 0;
}

int MaxAWDExporter::ExportSkin(INode *node, ISkin *skin, awd_float64 **extWeights, awd_uint32 **extJoints)
{
    if (opts->ExportSkin() && skin && skin->GetNumBones()) {
        int iVtx=0;
        awd_float64 *weights;
        awd_uint32 *indices;

        // Get skeleton information from cache and geometry information
        // through an ISkinContextData interface.
        SkeletonCacheItem *skel = skeletonCache->GetFromBone(skin->GetBone(0));
        ISkinContextData *context = skin->GetContextInterface(node);

        // If the skeleton used for this skin could not be found,
        // break now or the code below will crash
        if (skel == NULL){
            return 0;
        }
        int jointsPerVertex = 0;
        if (skel->awdSkel->get_joints_per_vert()!=0){
            jointsPerVertex = skel->awdSkel->get_joints_per_vert();
        }
        
        int numVerts = context->GetNumPoints();
        weights = (awd_float64*)malloc(jointsPerVertex * numVerts * sizeof(awd_float64));
        indices = (awd_uint32*)malloc(jointsPerVertex * numVerts * sizeof(awd_uint32));
        
        for (iVtx=0; iVtx<numVerts; iVtx++) {
            int iBone=0;
            int iWeight=0;
            int numBones=0;
            double weightSum = 0;
            numBones = context->GetNumAssignedBones(iVtx);
            if (numBones>0){
                double *tmpWeights = (double*)malloc(numBones*sizeof(double));
                int *tmpIndices = (int*)malloc(numBones*sizeof(int));
                // Retrieve weight/index for all joints in skin.
                for (iBone=0; iBone<numBones; iBone++) {
                    int boneId = context->GetAssignedBone(iVtx, iBone);
                    INode *bone = skin->GetBone(boneId);
                    tmpWeights[iBone] = context->GetBoneWeight(iVtx, iBone);
                    tmpIndices[iBone] = skel->IndexOfBone(bone);
                }
                // Retrieve most significant joint weights from temporary buffers
                // or after having run out of assigned bones for a vertex, set
                // weight to zero.
                for (iWeight=0; iWeight < jointsPerVertex; iWeight++) {
                    int strIdx = iVtx*jointsPerVertex + iWeight;

                    if (iWeight < numBones) {
                        int maxIBone = -1;
                        double max = 0.0;

                        // Loop through temporary buffer to find most significant
                        // joint (highest weight) and store it.
                        for (iBone=0; iBone<numBones; iBone++) {
                            if (tmpWeights[iBone] > max) {
                                max = tmpWeights[iBone];
                                maxIBone = iBone;
                            }
                        }

                        // Retrieve most significant weight/index pair
                        weights[strIdx] = tmpWeights[maxIBone];
                        indices[strIdx] = tmpIndices[maxIBone];

                        weightSum += weights[strIdx];

                        // Set to zero to mark as already used.
                        tmpWeights[maxIBone] = 0.0;
                    }
                    else {
                        weights[strIdx] = 0.0;
                        indices[strIdx] = 0;
                    }
                }

                // if the sum of the weights was 0.0, than  skin the vert to the root-joint:
                if (weightSum<=0){
                    weights[iVtx*jointsPerVertex + 0] = 1.0;
                    indices[iVtx*jointsPerVertex + 0] = 0;
                }
                else{
                    // Normalize weights (sum must be 1.0)
                    double scale = 1/weightSum;
                    for (iBone=0; iBone<jointsPerVertex; iBone++) {
                        weights[iVtx*jointsPerVertex + iBone] *= scale;
                    }
                }
                free(tmpWeights);
                free(tmpIndices);
            }
            else{
                weights[iVtx*jointsPerVertex + 0] = 1.0;
                indices[iVtx*jointsPerVertex + 0] = 0;
                for (iWeight=1; iWeight < jointsPerVertex; iWeight++) {
                    weights[iVtx*jointsPerVertex + iWeight] = 0.0;
                    indices[iVtx*jointsPerVertex + iWeight] = 0;
                }
            }
        }

        *extWeights = weights;
        *extJoints = indices;

        return jointsPerVertex;
    }

    return 0;
}


// ---> functions called by ExportAnimations():


AWDBlock * MaxAWDExporter::ExportAWDVertexClip( AWDTriGeom  * targetGeom, AWDVertexAnimation * vertClip)
{
    // each VertexClip must know its target-Geo
    if (vertClip==NULL)
        return NULL;
    if (vertClip->get_is_processed())
        return vertClip;
    vertClip->set_is_processed(true);
    char * thisName=vertClip->get_sourceID();
    if(thisName==NULL)
        return NULL;
    output_debug_string(thisName);
    INode * sourceObj = (INode *) sourceObjsIDsCache->Get(thisName);
    if (sourceObj==NULL)
        return NULL;
    vertClip->set_targetGeo(targetGeom);
    int ticksPerFrame=0;
    int frameDur=0;
    int thisframeDur=0;
    ticksPerFrame = GetTicksPerFrame();
    frameDur = floor(TicksToSec(ticksPerFrame) * 1000.0 + 0.5); // ms
    int start = vertClip->get_start_frame();
    int end = vertClip->get_end_frame();
    int skip = vertClip->get_skip_frame();
    // Loop through frames for this sequence and create poses
    int f=0;
    int skipf=0;
    for (f=start; f<=end; f++) {
        
        skipf=f+skip;
        thisframeDur=frameDur;
        while (f<(end-1)&&(f<=skipf)){
            f++;
            thisframeDur+=frameDur;
        }
        AWDVertexGeom *pose;
        TimeValue t = f * ticksPerFrame;
        pose = new AWDVertexGeom();
        Object * obj = sourceObj->GetObjectRef();				
        SClass_ID sid;
        getBaseObjectAndID(obj, sid);
        ObjectState os;
        // Flatten entire modifier stack
        os = sourceObj->EvalWorldState(t);
        obj = os.obj;
        TriObject *triObject = (TriObject*)obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0));	
        Mesh mesh = triObject->mesh;
        AWDSubGeom *sub;
        
        // Write all sub-meshes 
        // for each verticle in a submesh, 
        // get the original vertex-index, than get the current position...)
        sub = targetGeom->get_first_sub();
        Matrix3 offsMtx = sourceObj->GetObjectTM(t) * Inverse(sourceObj->GetNodeTM(t));
        while (sub) {
            AWD_str_ptr v_str;
            AWD_str_ptr originalIdxStream = sub->get_original_idx_data();
            awd_uint32 orgIdxLength = sub->get_original_idx_data_len()  *3;	

            v_str.v = malloc(sizeof(awd_float64) * orgIdxLength);	
            for (int e=0; e<orgIdxLength/3; e++) {
                    awd_uint32 *p = (originalIdxStream.ui32 + (e));
                    awd_uint16 elem = UI16((awd_uint16)*p);
                    Point3 newPoint =  offsMtx * mesh.getVert(elem);
                    v_str.f64[e*3+0] = newPoint.x;
                    v_str.f64[e*3+1] = newPoint.z;
                    v_str.f64[e*3+2] = newPoint.y;				
            }
            AWDSubGeom * newVertexSub=new AWDSubGeom(NULL);
            newVertexSub->add_stream(VERTICES, AWD_FIELD_FLOAT32, v_str, orgIdxLength);
            pose->add_sub_mesh(newVertexSub);
            sub = sub->next;
        }
        vertClip->set_next_frame_pose(pose, thisframeDur);
    }
    return vertClip;
}

AWDBlock * MaxAWDExporter::ExportAWDSkeletonClip(SkeletonCacheItem * skelCacheItem, AWDSkeletonAnimation * skelClip)
{
    if (skelCacheItem==NULL)
        return NULL;//ERROR NO skelCacheItem AVAILABLE
    if (skelClip==NULL)
        return NULL;//ERROR NO CLIP AVAILABLE
    if (skelClip->get_is_processed())
        return skelClip;
    skelClip->set_is_processed(true);
    char * sourceID=skelClip->get_sourceID();
    if(sourceID==NULL)
        return NULL;
    INode * sourceObj = (INode *) sourceObjsIDsCache->Get(skelClip->get_sourceID());
    if (sourceObj==NULL)
        return NULL;
    
    int ticksPerFrame=0;
    int frameDur=0;
    int thisframeDur=0;
    ticksPerFrame = GetTicksPerFrame();
    frameDur = floor(TicksToSec(ticksPerFrame) * 1000.0 + 0.5); // ms
    int start = skelClip->get_start_frame();
    int end = skelClip->get_end_frame();
    int skip = 1 + skelClip->get_skip_frame();

    skelCacheItem->IterReset();
    int numJoints=skelCacheItem->get_num_joints();
    INode * rootBone = skelCacheItem->IterNext()->maxBone;
    if(rootBone==NULL)
        return NULL; //ERROR
    skelCacheItem->IterReset();
    if (sourceObj==rootBone){
        skelCacheItem->IterReset();
        // Loop through frames for this sequence and create poses
        int f=0;
        int skipf=0;
        for (f=start; f<=end; f++) {
            skipf=f+skip;
            thisframeDur=frameDur;
            while (f<(end-1)&&(f<=skipf)){
                f++;
                thisframeDur+=frameDur;
            }
            SkeletonCacheJoint *curJoint;
            AWDSkeletonPose *pose;
            TimeValue t = f * ticksPerFrame;
            char * posename=(char *)malloc(13);
            strcpy(posename, "SkeletonPose");
            posename[12]=0;
            pose = new AWDSkeletonPose(posename, strlen(posename));
            free(posename);
            skelCacheItem->IterReset();
            while ((curJoint = skelCacheItem->IterNext()) != NULL) {
                INode * bone = curJoint->maxBone;
                Matrix3 parentMtx=bone->GetParentTM(t);
                parentMtx.NoScale(); // get rid of the scale part of the parent matrix
                Matrix3 tm = bone->GetNodeTM(t) * Inverse(parentMtx);
                awd_float64 *mtx = (awd_float64*)malloc(sizeof(awd_float64)*12);
                SerializeMatrix3(tm, mtx);
                pose->set_next_transform(mtx);
            }	
            skelClip->set_next_frame_pose(pose, thisframeDur);
            awd->add_skeleton_pose(pose);

        }
        return skelClip;
    }
    else{
        int numChilds=CalcNumDescendants(sourceObj);
        if (numChilds!=numJoints){
            return NULL; //ERROR - SourceObject not cambatible to sourceSkeleton
        }		
        // Loop through frames for this sequence and create poses
        int f=0;
        int skipf=0;
        for (f=start; f<=end; f++) {
            skipf=f+skip;
            thisframeDur=frameDur;
            while (f<(end-1)&&(f<=skipf)){
                f++;
                thisframeDur+=frameDur;
            }
            AWDSkeletonPose *pose;
            TimeValue t = f * ticksPerFrame;
            char * posename=(char *)malloc(13);
            strcpy(posename, "SkeletonPose");
            posename[12]=0;
            pose = new AWDSkeletonPose(posename, strlen(posename));
            free(posename);
            read_transform_position_into_Pose(sourceObj, t, pose);
            skelClip->set_next_frame_pose(pose, thisframeDur);
            awd->add_skeleton_pose(pose);
        }
        return skelClip;
    }
    
    return NULL;
}

AWDBlock * MaxAWDExporter::ExportAWDAnimSet(AWDAnimationSet * animSet, AWDBlockList * targetGeos)
{
    if (animSet==NULL)
        return NULL;

    AWDBlockList * targetGeosList = new AWDBlockList();
    AWDMeshInst *block;
    AWDBlockIterator *itMeshes;
    itMeshes = new AWDBlockIterator(targetGeos);
    while ((block = (AWDMeshInst * )itMeshes->next()) != NULL) {
        targetGeosList->append(block->get_geom());
    }
    delete itMeshes;
    if (targetGeosList->get_num_blocks()==0)
        return NULL;
    
    AWD_Anim_Type animType=animSet->get_anim_type(); 
    // if the clips for this animset have allready been created, we still check if it is compatible to the given list of mesh-instances
    if (animSet->get_is_processed()){
        /*AWDTriGeom *block;
        itMeshes = new AWDBlockIterator(targetGeosList);
        while ((block = (AWDTriGeom * )itMeshes->next()) != NULL) {
            if (animType==ANIMTYPESKELETON){
            }
            else if (animType==ANIMTYPEVERTEX){
                if (block->get_originalPointCnt()!=0){
                    if (animSet->get_originalPointCnt() != block->get_originalPointCnt()){
                        //ERROR, animated mesh has different pointcount than vertexAnimAtionSet
                    }
                }
                else{
                    //ERROR, geometry has no pointCnt set
                }
            }
        }
        delete targetGeosList;
        delete itMeshes;
        return NULL;*/
    }
    else{
        delete targetGeosList;
        animSet->set_is_processed(true);
        // in simple mode we just get all awdclips, that are found on the source-obj (TODO: incl awdClips that are found on clones of sourceOBJ)
        if (animSet->get_simple_mode()){
            char * sourceID = animSet->get_sourcePreID();//no need to delete
            AWDBlockIterator *it;
            AWDBlockList * newClipList = new AWDBlockList();
            AWDBlockList * sourceClipList = (AWDBlockList *)animSourceForAnimSet->Get(sourceID);		
            if (animType==ANIMTYPESKELETON){
                SkeletonCacheItem * skeletonCacheItem=skeletonCache->GetByAWDID(sourceID);
                if (skeletonCacheItem!=NULL){
                    AWDBlock *block;
                    animSet->set_skeleton(skeletonCacheItem->awdSkel);
                    it = new AWDBlockIterator(sourceClipList);
                    while ((block = it->next()) != NULL) {
                        AWDBlock * newClipBlock;
                        newClipBlock = ExportAWDSkeletonClip(skeletonCacheItem, (AWDSkeletonAnimation*)block);	
                        if (newClipBlock!=NULL)
                            newClipList->append(newClipBlock);
                    }
                    delete it;
                }
            }	
            else if (animType==ANIMTYPEVERTEX){
                INode * sourceINode = (INode *) sourceObjsIDsCache->Get(sourceID);
                if (sourceINode != NULL){
                    output_debug_string("export a  animset for vertex for sourceID=");
                    output_debug_string(sourceID);
                    Object * obj = sourceINode->GetObjectRef();				
                    SClass_ID sid;
                    getBaseObjectAndID(obj, sid);
                    AWDTriGeom * awdGeom = (AWDTriGeom *)geometryCache->Get(obj);
                    if (awdGeom!=NULL) {
                        AWDVertexAnimation *block;
                        it = new AWDBlockIterator(sourceClipList);
                        while ((block = (AWDVertexAnimation *)it->next()) != NULL) {
                            AWDBlock * newClipBlock;
                            output_debug_string("export a  clip for vertex");
                            //output_debug_string(block->get_sourceID());
                            newClipBlock = ExportAWDVertexClip(awdGeom, block);	
                            if (newClipBlock!=NULL)
                                newClipList->append(newClipBlock);
                        }
                        delete it;
                    }
                
                }	
            }
            if (newClipList->get_num_blocks()>0){
                animSet->set_animationClipNodes(newClipList);
                return animSet;
            }
            else
                delete newClipList;
        }
    }
    return NULL;
}

void MaxAWDExporter::CopyViewerHTML(char *templatePath, char *outPath, char *name)
{
    char *buf;
    int bufLen;

    bufLen = 0xffff;
    buf = (char *)malloc(bufLen);
    
    FILE *in = fopen(templatePath, "r");
    if (in!=NULL){
        bufLen = fread((void *)buf, sizeof(char), bufLen, in);
        memset((void *)(buf + bufLen), 0, 1);
        fclose(in);


        ReplaceString(buf, &bufLen, "%NAME%", name);
    
        char bgcolor[8];
        snprintf(bgcolor, 8, "%x", opts->PreviewBackgroundColor());
        ReplaceString(buf, &bufLen, "%COLOR%", bgcolor);
    
        
        FILE *out = fopen(outPath, "w");
        fwrite(buf, sizeof(char), bufLen, out);
        fclose(out);
    }
    

    free(buf);
}



void MaxAWDExporter::CopyViewer(bool network)
{
   
    char awdDrive[4];
    char awdPath[1024];
    char awdName[256];
    char dleFullPath[1024];
    char dleDrive[4];
    char dlePath[1024];
    char tplHtmlPath[1024];
    char tplSwfPath[1024];
    char tplJsPath[1024];
    char outHtmlPath[1024];
    char outSwfPath[1024];
    char outJsPath[1024];

    // Get paths of plug-in DLE file and output AWD file and split into
    // components to be used to concatenate input and output paths.
    //TCHAR * dleFullPath_tchar=A2W(_T(""));
    TCHAR dleFullPath_tchar[1024];
    GetModuleFileName(hInstance, dleFullPath_tchar, 1024);
    char * dleFullPath_ptr=W2A(dleFullPath_tchar);
    _splitpath_s(dleFullPath_ptr, dleDrive, 4, dlePath, 1024, NULL, 0, NULL, 0);
    _splitpath_s(awdFullPath, awdDrive, 4, awdPath, 1024, awdName, 256, NULL, 0);
    // Select which viewer SWF file to copy depending on which sandbox
    // it should be compiled for (network or local.)
    char *viewerName = network?"AwayExtensions3dsMax\\AWDHTMLViewer\\viewer_n" : "AwayExtensions3dsMax\\AWDHTMLViewer\\viewer_l";

    // Assemble paths for inputs (templates)
    _makepath_s(tplHtmlPath, 1024, dleDrive, dlePath, "AwayExtensions3dsMax\\AWDHTMLViewer\\template", "html");
    _makepath_s(tplSwfPath, 1024, dleDrive, dlePath, viewerName, "swf");
    _makepath_s(tplJsPath, 1024, dleDrive, dlePath, "AwayExtensions3dsMax\\AWDHTMLViewer\\swfobject", "js");

    // Assemble paths for outputs
    _makepath_s(outHtmlPath, 1024, awdDrive, awdPath, awdName, "html");
    _makepath_s(outSwfPath, 1024, awdDrive, awdPath, "viewer", "swf");
    _makepath_s(outJsPath, 1024, awdDrive, awdPath, "swfobject", "js");

    // Copy HTML, and evaluate any variables in the template
    CopyViewerHTML(tplHtmlPath, outHtmlPath, awdName);

    // Copy SWF and JS files as-is
    TCHAR * tplSwfPath_tchar=A2W(tplSwfPath);
    TCHAR * outSwfPath_tchar=A2W(outSwfPath);
    TCHAR * tplJsPath_tchar=A2W(tplJsPath);
    TCHAR * outJsPath_tchar=A2W(outJsPath);
    CopyFile(tplSwfPath_tchar, outSwfPath_tchar, false);
    CopyFile(tplJsPath_tchar, outJsPath_tchar, true);
    
    TCHAR * outHtmlPath_tchar=A2W(outHtmlPath);
    ShellExecute(NULL, _T("open"), outHtmlPath_tchar, NULL, NULL, SW_SHOWNORMAL);
    free (tplSwfPath_tchar);
    free (outSwfPath_tchar);
    free (tplJsPath_tchar);
    free (outJsPath_tchar);
    free (dleFullPath_ptr);
    free (outHtmlPath_tchar);
}



