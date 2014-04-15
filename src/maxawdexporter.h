#pragma once

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
// DESCRIPTION: Includes for Plugins
// AUTHOR: 
//***************************************************************************/

#include "3dsmaxsdk_preinclude.h"
#include "Max.h"
#include "resourceexporter.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "shaders.h"
//SIMPLE TYPE

#include "awd.h"
#include "cache.h"
#include "maxawdexporteropts.h"

#include <stdmat.h>
#include <iskin.h>
#include <modstack.h>

#include "IGame.h"
#include "IGameObject.h"
#include "IGameProperty.h"
#include "IGameControl.h"
#include "IGameModifier.h"
#include "IConversionManager.h"
#include "IGameError.h"
#include "IGameFX.h"
#include "iSkinPose.h"
#include "utils.h"
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

typedef struct CustomTexSettings_struct {
	bool replace;
	bool relative;
	char* replaceURL;
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
		BlockCache *cubeMatCache;
		BlockCache *primGeocache;		
		BlockCache *INodeToGeoBlockCache;
		BlockCache *sceneBlocks;
		StringCache *methodsCache;
		BlockCache *geometryCache;
		BlockCache *animSetsCache;
		BlockCache *animatorCache;
		LightCache *lightCache;
		StringCache *textureCache;
		StringCache *animSetsIDsCache;
		StringCache *animClipsIDsCache;
		StringCache *sourceObjsIDsCache;
		StringCache *animSourceForAnimSet;
		StringCache *clipsOnSkelsCache;
		SkeletonCache *skeletonCache;
		ColorMaterialCache *colMtlCache;
		BlockCache *materialCache;
		BoolCache *allExcludedCache;
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
		
		static HWND hParams;
		
		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL SupportsOptions(int ext, DWORD options);
		int	 DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		int ExecuteExport();

		//Constructor/Destructor
		MaxAWDExporter();
		~MaxAWDExporter();

		void				PrepareExport(bool, BlockSettings *);
		void				CleanUp();

		//void				CopyViewer(bool launch);
		//void				CopyViewerHTML(char *templatePath, char *outPath, char *name);
		
		// this functions are the basic export workflow:
		void				PreProcessSceneGraph(INode *node, bool, BlockSettings*);
		void				ProcessSceneGraph(INode *node, AWDSceneBlock *parent, BlockSettings* );
		void				ProcessGeoBlocks();
		void				ExportAnimations(INode *node);
		
		// all following functions are getting called by the 4 functions above:

		AWDPrimitive *		ExportPrimitiveGeom(BaseObject *, char *);
		void				ExportTriGeom(AWDTriGeom *awdGeom, Object *obj, INode *node, ISkin *skin, IGameMesh * igame_mesh);
		
		AWDBlockList *		GetMaterialsForMeshInstance(INode *node);
		AWDMaterial	*		ExportOneMaterial(StdMat *mtl);
		AWDBlock	*		getColorMatForObject(INode *node, boolean black=false);
		AWDBitmapTexture *	ExportBitmapTexture(BitmapTex *tex,  AWDMaterial *, AWD_tex_type);
		AWDCubeTexture *	ExportCubeTexture(MultiMtl *tex);
		AWDCubeTexture *	GetCustomAWDCubeTexSettings(MultiMtl *tex);

		int					ExportSkin(INode *node, ISkin *skin, awd_float64 **extWeights, awd_uint32 **extJoints);
		void				ExportSkeletons(INode *node);
		void				ExportSkeleton(INode *rootBone);

		void				ExportUserAttributes(Animatable *obj, AWDAttrElement *elem);

		void				ReadAWDClipsFromAWDModifier(IParamBlock2 *, const char *,  AWDBlockList *, AWD_Anim_Type);
		void				ReadAWDEffectMethods(Modifier *node_mod);
		void				ReadAWDAnimSet(Modifier *node_mod);
		void				ReadAWDSkeletonMod(Modifier *node_mod,INode * );
		void				ReadAWDVertexMod(Modifier *node_mod,INode * );
		AWDAnimator *		ReadAWDVertexModForMesh(Modifier *node_mod,INode * );
		AWDAnimator *		AutoCreateAnimatorForVertexAnim(INode * );
		AWDAnimator *		AutoCreateAnimatorForSkeleton(INode * );		
		void				ReadAWDAnimSourceCloneMod(Modifier *node_mod,INode * , AWD_Anim_Type);
		AWDBlock *			ExportAWDAnimSet(AWDAnimationSet *, AWDBlockList * );
		AWDBlock *			ExportAWDSkeletonClip(SkeletonCacheItem *, AWDSkeletonAnimation *);
		AWDBlock *			ExportAWDVertexClip(AWDTriGeom *, AWDVertexAnimation *);
		void				CreateDarkLights();
		void				ExportAnimations();
		AWDAnimator *		GetAWDAnimatorForObject(INode * );
		
		
		int					checkIfUVMapExists(Mesh mesh, int numTris, int mapIdx);
		CustomAttributes_struct	GetCustomAWDObjectSettings(Animatable *);
		CustomTexSettings_struct GetCustomAWDTextureSettings(BitmapTex *);
		void				GetCustomAWDMaterialSettings(StdMat *, AWDMaterial * );
        void CopyViewer(bool);
        void CopyViewerHTML(char *, char *, char *);
};
