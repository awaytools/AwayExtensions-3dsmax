#pragma once

#include "3dsmaxsdk_preinclude.h"
#include "max.h"
#include "resource.h"

#include "awd.h"

// Config attribute macros
#define ATTR_BUF_LEN 1024
#define ATTREQ(str0,str1) strncmp(str0,str1,ATTR_BUF_LEN)==0

class MaxAWDExporterOpts
{
private:
	int compression;
	bool exportAttributes;
	char *attributeNamespace;

	bool exportScene;
	bool exportGeometry;
	bool exportUVs;
	bool exportNormals;
	bool exportSkin;
	int jointsPerVertex;
	
	bool exportMaterials;
	bool forceBasenameTextures;
	bool copyTextures;
	bool embedTextures;

	bool exportSkeletons;
	bool exportSkelAnim;
	char *sequencesTxtPath;

	bool createPreview;
	bool launchPreview;
	bool networkPreview;
	int previewBackgroundColor;

	static MaxAWDExporterOpts *INSTANCE;

	static HWND generalOpts;
	static HWND sceneOpts;
	static HWND mtlOpts;
	static HWND animOpts;
	static HWND viewerOpts;

	// Config file
	FILE *OpenConfigFile(const char *mode);
	void ReadConfigFile(void);
	void WriteConfigFile(void);

	// Main proc function and helpers
	static INT_PTR CALLBACK DialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	void InitDialog(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void SaveOptions(void);

	// Tab panel proc functions
	static INT_PTR CALLBACK GeneralOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	bool RedrawGeneralOpts(LPARAM lParam);
	static INT_PTR CALLBACK SceneOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	bool RedrawSceneOpts(LPARAM lParam);
	static INT_PTR CALLBACK MtlOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	bool RedrawMtlOpts(LPARAM lParam);
	static INT_PTR CALLBACK AnimOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	bool RedrawAnimOpts(LPARAM lParam);
	static INT_PTR CALLBACK ViewerOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	bool RedrawViewerOpts(LPARAM lParam);

public:
	MaxAWDExporterOpts(void);
	~MaxAWDExporterOpts(void);

	bool ShowDialog(void);

	// Getters for options
	int Compression(void);
	bool ExportAttributes(void);
	char *AttributeNamespace(void);

	bool ExportScene(void);
	bool ExportGeometry(void);
	bool ExportUVs(void);
	bool ExportNormals(void);
	bool ExportSkin(void);
	int JointsPerVertex();

	bool ExportMaterials(void);
	bool ForceBasenameTextures(void);
	bool CopyTextures(void);
	bool EmbedTextures(void);

	bool ExportSkeletons(void);
	bool ExportSkelAnim(void);
	char *SequencesTxtPath(void);
	
	bool CreatePreview(void);
	bool LaunchPreview(void);
	bool PreviewForDeployment(void);
	int PreviewBackgroundColor(void);
};

