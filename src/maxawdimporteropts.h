#pragma once

#include "3dsmaxsdk_preinclude.h"
#include "max.h"
#include "resourceimporter.h"

#include "awd.h"

// Config attribute macros
#define ATTR_BUF_LEN 1024
#define ATTREQ(str0,str1) strncmp(str0,str1,ATTR_BUF_LEN)==0

class MaxAWDImporterOpts
{
private:
	int compression;
	float scale;
	bool exportAttributes;
	char *attributeNamespace;

	bool exportScene;
	bool exportGeometry;
	bool exportUVs;
	bool exportNormals;
	bool exportSkin;
	bool splitByMatID;
	bool exportCameras;
	bool exportSkyboxes;

	bool exportMaterials;
	int textureMode;
	bool includeShadings;
	bool includeMethods;
	bool includeLights;
	bool applyDarkLight;
	bool includeShadows;
	bool setMultiPass;

	bool exportSkeletons;
	bool exportSkelAnim;
	bool exportVertexAnim;

	bool launchPreview;

	static MaxAWDImporterOpts *INSTANCE;

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
	MaxAWDImporterOpts(void);
	~MaxAWDImporterOpts(void);

	bool ShowDialog(void);

	// Getters for options
	int Compression(void);
	bool ExportAttributes(void);
	char *AttributeNamespace(void);
	double Scale();

	bool ExportScene(void);
	bool ExportGeometry(void);
	bool ExportUVs(void);
	bool ExportNormals(void);
	bool ExportSkin(void);
	bool SplitByMatID(void);
	bool ExportCameras(void);
	bool ExportSkyboxes(void);

	bool ExportMaterials(void);
	int TextureMode(void);
	bool IncludeShadings(void);
	bool IncludeMethods(void);
	bool IncludeLights(void);
	bool ApplyDarkLight(void);
	bool IncludeShadows(void);
	bool SetMultiPass(void);

	bool ExportSkeletons(void);
	bool ExportSkelAnim(void);
	bool ExportVertexAnim(void);

	bool LaunchPreview(void);
};
