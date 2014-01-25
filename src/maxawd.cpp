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

#include <Windows.h>
#include <icustattribcontainer.h>
#include <custattrib.h>
#include <iparamb2.h>
#include <MeshNormalSpec.h>

#include "awd.h"
#include "util.h"
#include "platform.h"
#include "geomutil.h"
#include "maxawd.h"
#include "utils.h"

#define MaxAWDExporter_CLASS_ID	Class_ID(0xa8e047f2, 0x81e112c0)

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
	free(awdFullPath);
}

int MaxAWDExporter::ExtCount()
{
	return 1;
}

const TCHAR *MaxAWDExporter::Ext(int n)
{		
	return _T("AWD");
}

const TCHAR *MaxAWDExporter::LongDesc()
{
	return _T("Away3D AWD File");
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
	return _T("Copyright 2012 The Away3D Team");
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


int	MaxAWDExporter::DoExport(const TCHAR *path,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	awdFullPath = W2A(path);
	maxInterface = i;
	suppressDialogs = suppressPrompts;

	// Open the dialog (provided that prompts are not suppressed) and
	// if it returns false, return to cancel the export.
	opts = new MaxAWDExporterOpts();
	if (!suppressPrompts && !opts->ShowDialog()) {
		delete opts;
		return true;
	}

	// Open file and check for success
	fd = open(awdFullPath, _O_TRUNC | _O_CREAT | _O_BINARY | _O_RDWR, _S_IWRITE);
	if (fd == -1)
		return FALSE;
	
	if (options==SCENE_EXPORT_SELECTED){
		exportAll=FALSE;
	}
	// Execute export while showing a progress bar. Send this as argument
	// to the execute callback, which will invoke MaxAWDExporter::ExecuteExport();
	maxInterface->ProgressStart(TEXT("Exporting AWD file"), TRUE, ExecuteExportCallback, this);

	// Export worked
	return TRUE;
}

int MaxAWDExporter::ExecuteExport()
{
	// AwayBuilder has a bug that doesnt allow for importing capital fileextension (AWD instead of awd)
	// i couldnt figure out where the file-extension is set to "AWD" instead of "awd"
	// thats why i change the file extension to "awd" instead of "AWD" here:
	char awdDrive[4];
	char awdPath[1024];
	char awdName[256];
	char outAWDPath[1024];
	_splitpath_s(awdFullPath, awdDrive, 4, awdPath, 1024, awdName, 256, NULL, 0);
	_makepath_s(outAWDPath, 1024, awdDrive, awdPath, awdName, "awd");

	PrepareExport();

	// Get total number of nodes for progress calculation
	INode *root = maxInterface->GetRootNode();
	numNodesTotal = CalcNumDescendants(root);

	// Traverse node tree for skeletons
	numNodesTraversed = 0;
	ExportSkeletons(root);

	// Die if error occurred.
	DIE_IF_ERROR();

	// Traverse node tree again for scene objects
	// (including their geometry, materials, et c)
	numNodesTraversed = 0;
	ExportNode(root, NULL);

	// Die if error occurred.
	DIE_IF_ERROR();

	// Export animation if enabled and if a sequences.txt file was found
	if (opts->ExportSkelAnim()) {
		SequenceMetaData *sequences = LoadSequenceFile(awdFullPath, opts->SequencesTxtPath());
		if (sequences != NULL)
			ExportAnimation(sequences);
	}

	// Die if error occurred.
	DIE_IF_ERROR();

	// Flush serialized AWD structure to file
	awd->flush(fd);
	close(fd);

	maxInterface->ProgressEnd();

	// Copy viewer HTML and SWF template to output directory
	//if (opts->CreatePreview()) {
	//	CopyViewer(launch);
	//}
	
	
	bool launch = (!suppressDialogs && opts->LaunchPreview());
	if (launch){
		//open the file with the default appliction (e.g. Awaybuilder)
		ShellExecute(NULL, "open", outAWDPath, NULL, NULL, SW_SHOWNORMAL);
	}

	// Free used memory
	CleanUp();

	return TRUE;
}

void MaxAWDExporter::PrepareExport()
{
	cache = new BlockCache();
	colMtlCache = new ColorMaterialCache();
	skeletonCache = new SkeletonCache();
	awd = new AWD((AWD_compression)opts->Compression(), 0);
	ns = NULL;
}


void MaxAWDExporter::CleanUp()
{
	delete awd;
	delete opts;
	delete cache;
	delete colMtlCache;
	delete skeletonCache;

	if (ns) {
		delete ns;
	}
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
	int phaseStart;
	int phaseFinish;
	TCHAR *title;

	switch (phase) {
		case MAXAWD_PHASE_SKEL:
			phaseStart = 0;
			phaseFinish = 20;
			title = TEXT("Skeletons");
			break;
		case MAXAWD_PHASE_SCENE:
			phaseStart = 20;
			phaseFinish = 60;
			title = TEXT("Scene & geometry");
			break;
		case MAXAWD_PHASE_ANIM:
			phaseStart = 60;
			phaseFinish = 80;
			title = TEXT("Animation");
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


void MaxAWDExporter::CopyViewerHTML(char *templatePath, char *outPath, char *name)
{
	char *buf;
	int bufLen;

	bufLen = 0xffff;
	buf = (char *)malloc(bufLen);

    FILE *in = fopen(templatePath, "r");
    bufLen = fread((void *)buf, sizeof(char), bufLen, in);
	memset((void *)(buf + bufLen), 0, 1);
    fclose(in);

	char bgcolor[8];
	snprintf(bgcolor, 8, "%x", opts->PreviewBackgroundColor());

    ReplaceString(buf, &bufLen, "%NAME%", name);
	ReplaceString(buf, &bufLen, "%COLOR%", bgcolor);

    FILE *out = fopen(outPath, "w");
    fwrite(buf, sizeof(char), bufLen, out);
    fclose(out);

	free(buf);
}


//void MaxAWDExporter::CopyViewer(bool launch)
//{
//	char awdDrive[4];
//	char awdPath[1024];
//	char awdName[256];
//	char dleFullPath[1024];
//	char dleDrive[4];
//	char dlePath[1024];
//	char tplHtmlPath[1024];
//	char tplSwfPath[1024];
//	char tplJsPath[1024];
//	char outHtmlPath[1024];
//	char outSwfPath[1024];
//	char outJsPath[1024];
//
//	// Get paths of plug-in DLE file and output AWD file and split into
//	// components to be used to concatenate input and output paths.
//	GetModuleFileName(hInstance, dleFullPath, 1024);
//	_splitpath_s(dleFullPath, dleDrive, 4, dlePath, 1024, NULL, 0, NULL, 0);
//	_splitpath_s(awdFullPath, awdDrive, 4, awdPath, 1024, awdName, 256, NULL, 0);
//
//	// Select which viewer SWF file to copy depending on which sandbox
//	// it should be compiled for (network or local.)
//	const char *viewerName = (opts->PreviewForDeployment())? 
//		"maxawd\\viewer_n" : "maxawd\\viewer_l";
//
//	// Assemble paths for inputs (templates)
//	_makepath_s(tplHtmlPath, 1024, dleDrive, dlePath, "maxawd\\template", "html");
//	_makepath_s(tplSwfPath, 1024, dleDrive, dlePath, viewerName, "swf");
//	_makepath_s(tplJsPath, 1024, dleDrive, dlePath, "maxawd\\swfobject", "js");
//
//	// Assemble paths for outputs
//	_makepath_s(outHtmlPath, 1024, awdDrive, awdPath, awdName, "html");
//	_makepath_s(outSwfPath, 1024, awdDrive, awdPath, "viewer", "swf");
//	_makepath_s(outJsPath, 1024, awdDrive, awdPath, "swfobject", "js");
//
//	// Copy HTML, and evaluate any variables in the template
//	CopyViewerHTML(tplHtmlPath, outHtmlPath, awdName);
//
//	// Copy SWF and JS files as-is
//	CopyFile(tplSwfPath, outSwfPath, false);
//	CopyFile(tplJsPath, outJsPath, true);
//
//	if (launch) {
//		ShellExecute(NULL, "open", outHtmlPath, NULL, NULL, SW_SHOWNORMAL);
//	}
//}


void MaxAWDExporter::ExportNode(INode *node, AWDSceneBlock *parent)
{
	//output_debug_string(node->GetName());
	Object *obj;
	bool goDeeper;

	AWDSceneBlock *awdParent = NULL;

	// By default, also parse children of this node
	goDeeper = true;

	obj = node->GetObjectRef();
	//if (obj && obj->ClassID()==BONE_OBJ_CLASSID) {
	//	// This will have already been exported by the initial sweep
	//	// for bones/skeletons, so there is no need to recurse deeper
	//	goDeeper = false;
	//	output_debug_string("is bone");
	//}
	//else {
		//output_debug_string("is not bone");
	if ( (node->Selected()!=0) || (parent!=NULL) || (exportAll) ){
		int skinIdx;
		ObjectState os;

		IDerivedObject *derivedObject = NULL;
		skinIdx = IndexOfSkinMod(node->GetObjectRef(), &derivedObject);
		if (skinIdx >= 0) {
			// Flatten all modifiers up to but not including
			// the skin modifier.
			os = derivedObject->Eval(0, skinIdx + 1);
		}
		else {
			// Flatten entire modifier stack
			os = node->EvalWorldState(0);
		}
	
		obj = os.obj;
		if (obj) {
			if (obj->CanConvertToType(triObjectClassID)) {
				output_debug_string("try to export this node");
				AWDMeshInst *awdMesh;
			
				// Check if there is a skin, that can be
				// exported as part of the geometry.
				ISkin *skin = NULL;
				if (derivedObject != NULL && skinIdx >= 0) {
					Modifier *mod = derivedObject->GetModifier(skinIdx);
					skin = (ISkin *)mod->GetInterface(I_SKIN);
				}

				awdMesh = ExportTriObject(obj, node, skin);
				RETURN_IF_ERROR;

				// Add generated mesh instance to AWD scene graph.
				// This can be null, if exporter was configured not
				// to export scene graph objects.
				if (awdMesh) {
					if (parent) {
						parent->add_child(awdMesh);
					}
					else {
						awd->add_scene_block(awdMesh);
					}
				}

				// Store the new block (if any) as parent to be used for
				// blocks that represent children of this Max node.
				awdParent = awdMesh;
				output_debug_string("node exported");
			}
			else
			{
				output_debug_string("can not convert to triObjectClassID");
				char* name = W2A(node->GetName());
				Matrix3 bindMtx;
				// Default at no bind transform.
				bindMtx.IdentityMatrix();
				// Get local matrix by "un-multiplying" the parent matrix, as well as the 
				// bind shape matrix which will have already been applied to the geometry.
				Matrix3 mtx = node->GetNodeTM(0) * Inverse(node->GetParentTM(0)) * Inverse(bindMtx);

				double *mtxData = (double *)malloc(12*sizeof(double));
				SerializeMatrix3(mtx, mtxData);
				awdParent=new AWDMeshInst(name, strlen(name), 
					new AWDTriGeom(name, strlen(name)), mtxData);
				if (parent) {
					parent->add_child(awdParent);
				}
				else {
					awd->add_scene_block(awdParent);
				}
				free(name);
			}
		}
		else
		{
			output_debug_string("no obj in state");
		}
	}
	//}

	numNodesTraversed++;

	if (goDeeper) {
		output_debug_string("export children");
		int i;
		int numChildren = node->NumberOfChildren();

		// Update progress bar before recursing
		UpdateProgressBar(MAXAWD_PHASE_SCENE, (double)numNodesTraversed/(double)numNodesTotal);

		for (i=0; i<numChildren; i++) {
			s_depth++;
			ExportNode(node->GetChildNode(i), awdParent);
			s_depth--;
			RETURN_IF_ERROR;
		}
	}
	else {
		// No need to traverse this branch further. Count all
		// descendants as traversed and update progress bar.
		numNodesTraversed += CalcNumDescendants(node);
		UpdateProgressBar(MAXAWD_PHASE_SCENE, (double)numNodesTraversed/(double)numNodesTotal);
	}
}


AWDMeshInst * MaxAWDExporter::ExportTriObject(Object *obj, INode *node, ISkin *skin)
{
	Matrix3 bindMtx;
	AWDMaterial *awdMtl = NULL;
	AWDTriGeom *awdGeom = NULL;

	// Default at no bind transform.
	bindMtx.IdentityMatrix();

	if (opts->ExportGeometry()) {
		awdGeom = ExportTriGeom(obj, node, skin, &bindMtx);

		// No geometry could be exported
		if (awdGeom == NULL)
			return NULL;
	}

	// Export material
	if (opts->ExportMaterials()) {
		awdMtl = ExportNodeMaterial(node);
		RETURN_VALUE_IF_ERROR(NULL);
	}

	// Export instance
	if (opts->ExportScene()) {
		// Get local matrix by "un-multiplying" the parent matrix, as well as the 
		// bind shape matrix which will have already been applied to the geometry.
		Matrix3 mtx = node->GetNodeTM(0) * Inverse(node->GetParentTM(0)) * Inverse(bindMtx);

		double *mtxData = (double *)malloc(12*sizeof(double));
		SerializeMatrix3(mtx, mtxData);

		char *name = W2A(node->GetName());
		AWDMeshInst *inst = new AWDMeshInst(name, strlen(name), awdGeom, mtxData);
		free(name);

		ExportUserAttributes(obj, inst);

		if (awdMtl)
			inst->add_material(awdMtl);
	
		return inst;
	}

	return NULL;
}


AWDTriGeom *MaxAWDExporter::ExportTriGeom(Object *obj, INode *node, ISkin *skin, Matrix3 *bindMtx)
{
	AWDTriGeom *awdGeom;

	awdGeom = (AWDTriGeom *)cache->Get(obj);
	if (awdGeom == NULL) {
		int t;
		int jpv=0;
		awd_float64 *weights;
		awd_uint32 *joints;

		TriObject *triObject = (TriObject*)obj->ConvertToType(0, triObjectClassID);	

		Mesh mesh = triObject->GetMesh();
		MeshNormalSpec *normals = mesh.GetSpecifiedNormals();

		// Extract skinning information (returns number of joints per vertex)
		jpv = ExportSkin(node, skin, &weights, &joints);

		// Calculate offset matrix from the object TM (which includes geometry
		// offset) and the node TM (which doesn't.) This will be used to transform
		// all vertices into node space.
		Matrix3 offsMtx = node->GetObjectTM(0) * Inverse(node->GetNodeTM(0));

		// AWD does not support bind shape matrices, so if a geometry is bound
		// to a skeleton, any mesh using it has to be "untransformed", and in 
		// turn the geometry needs to have that transformation applied to match.
		//if (skin && jpv) {
		//	Matrix3 bm;
		//	skin->GetSkinInitTM(node, bm, true);
		//	offsMtx *= bm;

		//	if (bindMtx) {
		//		// Transfer to bind shape matrix transform that should be "removed"
		//		// from the mesh instance transformation.
		//		bindMtx->Set(bm.GetRow(0), bm.GetRow(1), bm.GetRow(2), bm.GetRow(3));
		//	}
		//}

		AWDGeomUtil geomUtil;
		geomUtil.joints_per_vertex = jpv;
		geomUtil.include_uv = (opts->ExportUVs() && mesh.tvFace != NULL);
		geomUtil.include_normals = (opts->ExportNormals() && normals && normals->GetNumNormals()>0);

		int numTris = mesh.getNumFaces();

		// This could happen for example with splines
		if (numTris==0)
			return NULL;

		for (t=0; t<numTris; t++) {
			int v;
			TVFace tvface;
			Face face = mesh.faces[t];
			DWORD *inds = face.getAllVerts();

			if (geomUtil.include_uv)
				tvface = mesh.tvFace[t];

			for (v=2; v>=0; v--) {
				int vIdx = face.getVert(v);
				Point3 vtx = offsMtx * mesh.getVert(vIdx);

				vdata *vd = (vdata *)malloc(sizeof(vdata));
				vd->orig_idx = vIdx;
				vd->x = vtx.x;
				vd->y = vtx.z;
				vd->z = vtx.y;
				
				// Might not have UV coords
				if (geomUtil.include_uv) {
					int tvIdx = tvface.getTVert(v);
					Point3 tvtx = mesh.getTVert(tvIdx);

					vd->u = tvtx.x;
					vd->v = 1.0-tvtx.y;
				}

				if (geomUtil.include_normals) {
					Point3 normal = normals->GetNormal(t, v);
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

				vd->mtlid = 0; // TODO: Implement sub-meshing
				vd->force_hard = false;

				geomUtil.append_vdata_struct(vd);
			}
		}
		
		// Generate geometry name by concatenating the name 
		// of the mesh/node with the suffix "_geom"
		char *bname = W2A(node->GetName());
		char *name = (char*)malloc(strlen(bname)+6);
		strcpy(name, bname);
		strcat(name, "_geom");

		awdGeom = new AWDTriGeom(name, strlen(name));
		geomUtil.build_geom(awdGeom);
		free(bname);
		free(name);

		awd->add_mesh_data(awdGeom);
		cache->Set(obj, awdGeom);

		// If conversion created a new object, dispose it
		if (triObject != obj) 
			triObject->DeleteMe();
	}

	return awdGeom;
}


AWDMaterial *MaxAWDExporter::ExportNodeMaterial(INode *node) 
{
	AWDMaterial *awdMtl;
	Mtl *mtl = node->GetMtl();

	if (mtl == NULL) {
		awd_color color = node->GetWireColor();

		// Look in the cache for an existing "default" color material
		// that matches the color of this object. If none exists,
		// create a new one and store it in the cache.
		awdMtl = colMtlCache->Get(color);
		if (awdMtl == NULL) {
			awdMtl = new AWDMaterial(AWD_MATTYPE_COLOR, "", 0);
			awdMtl->color = color;
			awd->add_material(awdMtl);

			colMtlCache->Set(color, awdMtl);
		}
	}
	else {
		awdMtl = (AWDMaterial *)cache->Get(mtl);
		if (awdMtl == NULL) {
			int i;
			const MSTR &name = mtl->GetName();

			if (mtl->IsSubClassOf(Class_ID(DMTL_CLASS_ID, 0))) {
				StdMat *stdMtl = (StdMat *)mtl;
			}

			for (i=0; i<mtl->NumSubTexmaps(); i++) {
				Texmap *tex = mtl->GetSubTexmap(i);

				// If there is a texture, AND that texture is a plain bitmap
				if (tex != NULL && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
					MSTR slotName = mtl->GetSubTexmapSlotName(i);
					const MSTR diff = _M("Diffuse Color");

					if (slotName == diff) {
						AWDBitmapTexture *awdDiffTex;
					
						awdDiffTex = ExportBitmapTexture((BitmapTex *)tex);

						// Stop if an error occurred during texture
						// export (e.g. that a file was missing.)
						RETURN_VALUE_IF_ERROR(NULL);

						char *cname = W2A(name.data());
						awdMtl = new AWDMaterial(AWD_MATTYPE_TEXTURE, cname, strlen(cname));
						free(cname);
						awdMtl->set_texture(awdDiffTex);
					}
				}
			}

			// If no material was created during the texture search loop, this
			// is a plain color material.
			if (awdMtl == NULL) {
				char *cname = W2A(name.data());
				awdMtl = new AWDMaterial(AWD_MATTYPE_COLOR, cname, strlen(cname));
				free(cname);
			}

			awd->add_material(awdMtl);
			cache->Set(mtl, awdMtl);
		}
	}

	return awdMtl;
}


AWDBitmapTexture * MaxAWDExporter::ExportBitmapTexture(BitmapTex *tex)
{
	AWDBitmapTexture *awdTex;
	MSTR name;
	char *fullPath;

	name = tex->GetName();
	fullPath = W2A(tex->GetMapName());

	// Get absolute path for open/copy operations. The path used by Max may
	// be relative, which will not work since CWD is not the project folder.
	MaxSDK::AssetManagement::AssetUser asset = tex->GetMap();
	MSTR absTexPath = asset.GetFullFilePath();

	if (opts->EmbedTextures()) {
		char *apath = W2A(absTexPath);
		int fd = open(apath, _O_BINARY | _O_RDONLY);
		free(apath);
		
		if (fd >= 0) {
			struct stat fst;
			fstat(fd, &fst);

			awd_uint8 *buf = (awd_uint8*)malloc(fst.st_size);
			read(fd, buf, fst.st_size);
			close(fd);

			char *cname = W2A(name);
			awdTex = new AWDBitmapTexture(EMBEDDED, cname, strlen(cname));
			free(cname);
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
		char *cname = W2A(name);
		awdTex = new AWDBitmapTexture(EXTERNAL, cname, strlen(cname));
		free(cname);

		if (opts->ForceBasenameTextures()) {
			char fileName[256];
			char fileExt[16];
			char *url;

			// Split path to retrieve name and concatenate to form base name
			_splitpath_s(fullPath, NULL, 0, NULL, 0, fileName, 240, fileExt, 16);
			url = (char*)malloc(strlen(fileName)+strlen(fileExt)+1);
			strcpy(url, fileName);
			strcat(url, fileExt);
			awdTex->set_url(url, strlen(fileName)+strlen(fileExt));

			if (opts->CopyTextures()) {
				char awdDrive[4];
				char awdPath[1024];
				char outPath[1024];

				// Concatenate output path using base path of AWD file and basename of
				// texture file, and copy texture file to output directory.
				_splitpath_s(awdFullPath, awdDrive, 4, awdPath, 1024, NULL, 0, NULL, 0);
				_makepath_s(outPath, 1024, awdDrive, awdPath, fileName, fileExt);
				char *apath = W2A(absTexPath);
				CopyFileA(apath, outPath, true);
				free(apath);
			}
		}
		else {
			awdTex->set_url(fullPath, strlen(fullPath));
		}
	}

	awd->add_texture(awdTex);

	return awdTex;
}


int MaxAWDExporter::ExportSkin(INode *node, ISkin *skin, awd_float64 **extWeights, awd_uint32 **extJoints)
{
	if (opts->ExportSkin() && skin && skin->GetNumBones()) {
		int iVtx;
		awd_float64 *weights;
		awd_uint32 *indices;

		const int jointsPerVertex = opts->JointsPerVertex();

		// Get skeleton information from cache and geometry information
		// through an ISkinContextData interface.
		SkeletonCacheItem *skel = skeletonCache->GetFromBone(skin->GetBone(0));
		ISkinContextData *context = skin->GetContextInterface(node);

		// If the skeleton used for this skin could not be found,
		// break now or the code below will crash
		if (skel == NULL)
			return 0;

		// Configure skeleton (i.e. update bind matrices) for the 
		// binding defined by this particular skin.
		skel->ConfigureForSkin(skin);

		int numVerts = context->GetNumPoints();
		weights = (awd_float64*)malloc(jointsPerVertex * numVerts * sizeof(awd_float64));
		indices = (awd_uint32*)malloc(jointsPerVertex * numVerts * sizeof(awd_uint32));

		for (iVtx=0; iVtx<numVerts; iVtx++) {
			int iBone;
			int iWeight;
			int numBones;
			double weightSum = 0;

			numBones = context->GetNumAssignedBones(iVtx);

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

			free(tmpWeights);
			free(tmpIndices);

			// Normalize weights (sum must be 1.0)
			double scale = 1/weightSum;
			for (iBone=0; iBone<jointsPerVertex; iBone++) {
				weights[iVtx*jointsPerVertex + iBone] *= scale;
			}
		}

		*extWeights = weights;
		*extJoints = indices;

		return jointsPerVertex;
	}

	return 0;
}


void MaxAWDExporter::ExportSkeletons(INode *node)
{
	numNodesTraversed++;

	Object *obj = node->GetObjectRef();
	if (obj)		//here no need to traverse its children, because ExportSkeleton() will do this in cache
	{
		Class_ID class_id=obj->ClassID();
		if(class_id == BONE_OBJ_CLASSID || class_id.PartA()==DUMMY_CLASS_ID) 
		{
			ExportSkeleton(node);

			// No need to traverse this branch further. Count
			// the entire branch as traversed.
			numNodesTraversed += CalcNumDescendants(node);
			UpdateProgressBar(MAXAWD_PHASE_SKEL, (double)numNodesTraversed/(double)numNodesTotal);
		}
	}
	else {
		 //This wasn't a bone, but there might be bones
		 //further down the hierarchy from this one
		int i;

		// Update progress bar before recursing
		UpdateProgressBar(MAXAWD_PHASE_SKEL, (double)numNodesTraversed/(double)numNodesTotal);

		for (i=0; i<node->NumberOfChildren(); i++) {
			ExportSkeletons(node->GetChildNode(i));
		}
	}
}


void MaxAWDExporter::ExportSkeleton(INode *rootBone)
{
	// Add to skeleton cache so that animation export can find
	// this skeleton and sample it's animation. This will also
	// construct an intermediate structure that can be used to
	// look-up joint indices and more.
	AWDSkeleton *awdSkel = skeletonCache->Add(rootBone);
	awd->add_skeleton(awdSkel);
}


void MaxAWDExporter::ExportAnimation(SequenceMetaData *sequences)
{
	if (skeletonCache->HasItems() && sequences != NULL) {
		int ticksPerFrame;
		int frameDur;
		SkeletonCacheItem *curSkel;

		ticksPerFrame = GetTicksPerFrame();
		frameDur = floor(TicksToSec(ticksPerFrame) * 1000.0 + 0.5); // ms

		skeletonCache->IterReset();
		while ((curSkel = skeletonCache->IterNext()) != NULL) {
			SequenceMetaData *curSeq = sequences;

			while (curSeq) {
				int f;
				AWDSkeletonAnimation *awdAnim;

				// TODO: Consider concatenating names if >1 skeleton
				awdAnim = new AWDSkeletonAnimation(curSeq->name, strlen(curSeq->name));
				awd->add_skeleton_anim(awdAnim);

				// Loop through frames for this sequence and create poses
				for (f=curSeq->start; f<curSeq->stop; f++) {
					SkeletonCacheJoint *curJoint;
					AWDSkeletonPose *pose;

					TimeValue t = f * ticksPerFrame;

					// TODO: Consider coming  up with a proper name
					pose = new AWDSkeletonPose("", 0);

					curSkel->IterReset();
					while ((curJoint = curSkel->IterNext()) != NULL) {
						INode *bone = curJoint->maxBone;
						Matrix3 tm = bone->GetNodeTM(t) * Inverse(bone->GetParentTM(t));

						awd_float64 *mtx = (awd_float64*)malloc(sizeof(awd_float64)*12);
						SerializeMatrix3(tm, mtx);

						pose->set_next_transform(mtx);
					}

					// Store pose in AWD document
					awdAnim->set_next_frame_pose(pose, frameDur);
					awd->add_skeleton_pose(pose);
				}

				// Proceed to next sequence
				curSeq = curSeq->next;
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
		int a;
		int numAttribs;

		numAttribs = attributes->GetNumCustAttribs();
		for (a=0; a<numAttribs; a++) {
			int p;

			CustAttrib *attr = attributes->GetCustAttrib(a);
			IParamBlock2 *block = attr->GetParamBlock(0);

			for (p=0; p<block->NumParams(); p++) {
				ParamID pid = block->IndextoID(p);
				Color col;
				AColor acol;

				Interval valid = FOREVER;

				awd_uint16 len;
				AWD_field_type type;
				AWD_field_ptr ptr;
				ptr.v = NULL;

				switch (block->GetParameterType(pid)) {
					case TYPE_ANGLE:
					case TYPE_PCNT_FRAC:
					case TYPE_WORLD:
					case TYPE_FLOAT:
						type = AWD_FIELD_FLOAT32;
						len = sizeof(awd_float32);
						ptr.v = malloc(len);
						*ptr.f32 = block->GetFloat(pid);
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
						ns = new AWDNamespace(opts->AttributeNamespace(), strlen(opts->AttributeNamespace()));
						awd->add_namespace(ns);
					}
					
					char *cname = W2A(def.int_name);
					elem->set_attr(ns, cname, strlen(cname), ptr, len, type);
					free(cname);
				}
			}
		}
	}
}
