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
#include "resourceimporter.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "maxawdimporterblocks.h"
//SIMPLE TYPE

#include "awd.h"
#include "cacheimporter.h"
#include "maxawdimporteropts.h"

#include <stdmat.h>
#include <iskin.h>
#include <modstack.h>

#define MAXAWD_PHASE_PREPROCESS_SCENEGRAPH 1
#define MAXAWD_PHASE_EXPORT_SCENEGRAPH 2
#define MAXAWD_PHASE_PROCESS_GEOMETRY 3
#define MAXAWD_PHASE_PROCESS_ANIMATIONS 4
#define MAXAWD_PHASE_FLUSH 5

#define RDERR(ptr,count) { if(!fread(ptr,count,1,stream)) return 0; }
// Error handling helper macros
#define RETURN_IF_ERROR if (error) return
#define RETURN_VALUE_IF_ERROR(retval) if (error) return retval
#define DIE_IF_ERROR() if (error) { maxInterface->ProgressEnd(); CleanUp(); return FALSE; }

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

char *W2A( const TCHAR *s );
TCHAR *A2W( const char *s );



class MaxAWDImporter : public SceneImport {
	private:
		Interface *maxInterface;
		char *awdFullPath;
		bool showPrompts;

		int fd;
		AWD *awd;
		AWDNamespace *ns;
		ImporterBlockCache *cache;			
		
		bool exportAll;
		bool error;

		int numNodesTotal;
		int numNodesTraversed;

		MaxAWDImporterOpts *opts;

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
		int	DoImport(const TCHAR *name,ImpInterface *i,Interface *gi, BOOL suppressPrompts=FALSE);	// Import file

		int ExecuteImport();
		ImportedAWDBlock *	read_new_block(FILE *, bool);

		//Constructor/Destructor
		MaxAWDImporter();
		~MaxAWDImporter();
		
		void read_sceneblock_common(ImportedAWDBlock *, Animatable *, bool);
		void * readTriGeomBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readContainerBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readMeshInstanceBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readMaterialBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readTextureBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readSkeletonBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readSkeletonPoseBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readSkeletonAnimBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readPrimGeomBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readSkyBoxBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readCameraBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readLightBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readLightPickerBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readCubeTextureBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readCubeTextureATFBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readEffectMethodBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readShadowtMethodBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readVertexPoseBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readVertexAnimBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readAnimationSetBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readAnimatorBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readUVAnimationBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readCommandBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readNameSpaceBlock(ImportedAWDBlock *, bool, bool, bool);
		void * readMetadataBlock(ImportedAWDBlock *, bool, bool, bool);
		bool is_bit_set(unsigned value, unsigned bitindex);

};
