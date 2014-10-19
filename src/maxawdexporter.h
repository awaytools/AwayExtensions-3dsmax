#pragma once

#include "3dsmaxsdk_preinclude.h"
#include "Max.h"
#include "maxscript/maxscript.h"
#include "platform.h"
#include "direct.h"

#include "stdmat.h"
#include "IGame.h"
#include "iskin.h"
#include "iSkinPose.h"
#include "custattrib.h"
#include "icustattribcontainer.h"
#include "modstack.h"
#include "MeshNormalSpec.h"
#include "shaders.h"
#include "cs/bipexp.h"
#include "ilayer.h"
#include "ILayerProperties.h"
#include "mnmesh.h"

#include "resourceexporter.h"
#include "maxawdexporteropts.h"
#include "utils.h"

#include "awd.h"
#include "cache.h"

//These are not needed ?

#include "istdplug.h"
//#include "iparamb2.h"

#define MAXAWD_PHASE_PREPROCESS_SCENEGRAPH 1
#define MAXAWD_PHASE_EXPORT_SCENEGRAPH 2
#define MAXAWD_PHASE_PROCESS_GEOMETRY 3
#define MAXAWD_PHASE_PROCESS_ANIMATIONS 4
#define MAXAWD_PHASE_FLUSH 5

// Error handling helper macros
#define RETURN_IF_ERROR if (error) return
#define RETURN_VALUE_IF_ERROR(retval) if (error) return retval
#define DIE_IF_ERROR() if (error) { maxInterface->ProgressEnd(); CleanUp(); return FALSE; }

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

char *W2A( const TCHAR *s );
TCHAR *A2W( const char *s );

typedef struct CustomAttributes_struct {
    bool export_this;
    bool export_this_children;
    struct CustomAttributes_struct *next;
} CustomAttributes_struct;

typedef struct CustomCamera_struct {
    bool export_cam;
    bool export_tex;
    struct CustomCamera_struct *next;
} CustomCamera_struct;

typedef struct ValidTexData_struct {
    int height;
    int width;
    char* filePath;
    struct ValidTexData_struct *next;
} ValidTexData_struct;

typedef struct CustomTexSettings_struct {
    bool replace;
    bool relative;
    char* replaceURL;
    bool useUvAnim;
    char* thisID;
    int thisID_len;
    AWD_tex_type texType;
    struct CustomTexSettings_struct *next;
} CustomTexSettings_struct;
class MaxAWDExporter : public SceneExport {
    private:
        Interface *maxInterface;
        char *awdFullPath;
        bool suppressDialogs;

        int fd;
        AWD *awd;
        AWDNamespace *ns;
        bool hasExcludedLayers;
        bool hasExcludedObjects;
        BlockCache *cubeMatCache;
        BlockCache *nodesToBaseObjects;
        BlockCache *primGeocache;
        BlockCache *INodeToGeoBlockCache;
        BlockCache *sceneBlocks;
        BlockCache *combinedGeosCache;
        StringCache *methodsCache;
        StringCache *shadowMethodsCache;
        StringCache *textureProjectorCache;
        BlockCache *geometryCache;
        BlockCache *animSetsCache;
        BlockCache *animatorCache;
        BlockCache *autoApplyMethodsToINodeCache;
        LightCache *lightCache;
        StringCache *textureCache;
        StringCache *animSetsIDsCache;
        StringCache *animClipsNamesCache;
        StringCache *animClipsIDsCache;
        StringCache *sourceObjsIDsCache;
        StringCache *animSourceForAnimSet;
        StringCache *clipsOnSkelsCache;
        StringCache *sourceModsIDsCache;
        StringCache *geomsForVertexAnim;
        SkeletonCache *skeletonCache;
        ColorMaterialCache *colMtlCache;
        BlockCache *materialCache;
        BoolCache *allExcludedCache;
        BoolCache *allBonesCache;
        IntCache *vetexAnimNeutralPosecache;

        bool exportAll;
        bool error;

        int numNodesTotal;
        int numNodesTraversed;

        MaxAWDExporterOpts *opts;

        void DieWithError(void);
        void DieWithErrorMessage(char *message, char *caption);
        void UpdateProgressBar(int phase, double phaseProgress);

    public:

        void output_debug_string(const char* str);
        static HWND hParams;

        int                ExtCount();                    // Number of extensions supported
        const TCHAR *    Ext(int n);                    // Extension #n (i.e. "3DS")
        const TCHAR *    LongDesc();                    // Long ASCII description (i.e. "Autodesk 3D Studio File")
        const TCHAR *    ShortDesc();                // Short ASCII description (i.e. "3D Studio")
        const TCHAR *    AuthorName();                // ASCII Author name
        const TCHAR *    CopyrightMessage();            // ASCII Copyright message
        const TCHAR *    OtherMessage1();            // Other message #1
        const TCHAR *    OtherMessage2();            // Other message #2
        unsigned int    Version();                    // Version number * 100 (i.e. v3.01 = 301)
        void            ShowAbout(HWND hWnd);        // Show DLL's "About..." box

        BOOL SupportsOptions(int ext, DWORD options);
        int     DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

        int ExecuteExport();

        //Constructor/Destructor
        MaxAWDExporter();
        ~MaxAWDExporter();

        void                PrepareExport(bool, BlockSettings *);
        void                CleanUp();

        //void                CopyViewer(bool launch);
        //void                CopyViewerHTML(char *templatePath, char *outPath, char *name);

        // this functions are the basic export workflow:
        void                PreProcessSceneGraph(INode *node, bool, BlockSettings*);
        void                ProcessSceneGraph(INode *node, AWDSceneBlock *parent, BlockSettings* , bool);
        void                ProcessGeoBlocks();
        void                ExportAnimations(BlockSettings *);

        // all following functions are getting called by the 4 functions above:

        AWDPrimitive *      ExportPrimitiveGeom(Object *, char *);
        void                ExportTriGeom(AWDTriGeom *awdGeom, Object *obj, INode *node, ISkin *skin, IGameMesh * igame_mesh);

        AWDBlockList *        GetMaterialsForMeshInstance(INode *node);
        AWDMaterial    *        ExportOneMaterial(StdMat *mtl);
        AWDBlock    *        getColorMatForObject(INode *node, boolean black=false);
        AWDBitmapTexture *    ExportBitmapTexture(BitmapTex *tex,  AWDMaterial *, AWD_tex_type, AWDTexPurpose_type);
        AWDCubeTexture *    ExportCubeTexture(MultiMtl *tex);
        AWDCubeTexture *    ExportAWDCubeTexure(MultiMtl *tex);

        int                    ExportSkin(INode *node, ISkin *skin, awd_float64 **extWeights, awd_uint32 **extJoints);
        void                ExportSkeletons(INode *node);
        void                ExportSkeleton(INode *rootBone);

        void                ExportUserAttributes(Animatable *obj, AWDAttrElement *elem);
        void                ExportCustomProps(INode *thisNode, AWDAttrElement *elem);
        void                ExportUserAttributesForNode(INode *,Animatable *obj, AWDAttrElement *elem);

        AWDBlock *          ExportCameraAndTextureExporter(INode * node, double *, AWDSceneBlock *, BlockSettings*);
        //CustomCamera_struct CreateCameraBlock(Modifier *node_mod, INode * node);
        void                ReadAWDAnimationClips(IParamBlock2 *, const char *,  AWDBlockList *, AWD_Anim_Type);
        void                ReadAWDEffectMethods(Modifier *node_mod, INode*);
        void                ReadAWDAnimSet(Modifier *node_mod);
        void                ReadAWDSkeletonMod(Modifier *node_mod,INode * );
        void                ReadAWDVertexMod(Modifier *node_mod,INode * );
        int                 IsCombinedGeom(INode * );
        AWDAnimator *       ReadAWDVertexModForMesh(Modifier *node_mod,INode * );
        AWDAnimator *       AutoCreateAnimatorForVertexAnim(INode * );
        AWDAnimator *       AutoCreateAnimatorForUV(INode * ,AWDBlockList * );
        AWDAnimator *       AutoCreateAnimatorForSkeleton(INode * );
        void                ReadAWDAnimSourceCloneMod(Modifier *node_mod,INode * , AWD_Anim_Type);
        AWDBlock *            ExportAWDAnimSet(AWDAnimationSet *, AWDBlockList * , BlockSettings *);
        AWDBlock *            ExportAWDSkeletonClip(SkeletonCacheItem *, AWDSkeletonAnimation *);
        AWDBlock *            ExportAWDVertexClip(AWDTriGeom *, AWDVertexAnimation *, BlockSettings *);
        void                CreateDarkLights();
        void                ExportAnimations();
        AWDAnimator *        GetAWDAnimatorForObject(INode * );

        AWDBlock *          ExportUVClip(AWDUVAnimation *);
        void                ReadAWDShadowsMethods(Modifier *node_mod, AWDBlock * light);
        int                    checkIfUVMapExists(Mesh mesh, int numTris, int mapIdx);

        ValidTexData_struct MakeValidTexture(Bitmap *);
        CustomAttributes_struct    GetCustomAWDObjectSettings(IDerivedObject *, Animatable *);
        CustomTexSettings_struct GetCustomAWDTextureSettings(BitmapTex *);
        void                GetCustomAWDMaterialSettings(StdMat *, AWDMaterial * );
        void CopyViewer(bool);
        void CopyViewerHTML(char *, char *, char *);
};
