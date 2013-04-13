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
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
//SIMPLE TYPE

#include "awd.h"
#include "cache.h"
#include "maxawdopts.h"

#include <stdmat.h>
#include <iskin.h>
#include <modstack.h>

#define MAXAWD_PHASE_SKEL 1
#define MAXAWD_PHASE_SCENE 2
#define MAXAWD_PHASE_ANIM 3
#define MAXAWD_PHASE_FLUSH 4

// Error handling helper macros
#define RETURN_IF_ERROR if (error) return
#define RETURN_VALUE_IF_ERROR(retval) if (error) return retval
#define DIE_IF_ERROR() if (error) { maxInterface->ProgressEnd(); CleanUp(); return FALSE; }

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

typedef struct SequenceMetaData_struct {
	int start;
	int stop;
	char *name;
	struct SequenceMetaData_struct *next;
} SequenceMetaData;



class MaxAWDExporter : public SceneExport {
	private:
		Interface *maxInterface;
		const char *awdFullPath;
		bool suppressDialogs;

		int fd;
		AWD *awd;
		AWDNamespace *ns;
		BlockCache *cache;
		SkeletonCache *skeletonCache;
		ColorMaterialCache *colMtlCache;

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

		void				PrepareExport();
		void				CleanUp();

		void				CopyViewer(bool launch);
		void				CopyViewerHTML(char *templatePath, char *outPath, char *name);

		void				ExportNode(INode *node, AWDSceneBlock *parent);
		AWDTriGeom *		ExportTriGeom(Object *obj, INode *node, ISkin *skin, Matrix3 *bindMtx);
		AWDMeshInst *		ExportTriObject(Object *obj, INode *node, ISkin *skin);
		AWDMaterial	*		ExportNodeMaterial(INode *node);
		AWDBitmapTexture *	ExportBitmapTexture(BitmapTex *tex);
		int					ExportSkin(INode *node, ISkin *skin, awd_float64 **extWeights, awd_uint32 **extJoints);
		void				ExportSkeletons(INode *node);
		void				ExportSkeleton(INode *rootBone);
		void				ExportAnimation(SequenceMetaData *sequences);
		void				ExportUserAttributes(Animatable *obj, AWDAttrElement *elem);
};
