#pragma once

#include "3dsmaxsdk_preinclude.h"
#include "max.h"
#include "resourceexporter.h"

#include "awd.h"

// Config attribute macros
#define ATTR_BUF_LEN 1024
#define ATTREQ(str0,str1) strncmp(str0,str1,ATTR_BUF_LEN)==0

class MaxAWDExporterOpts
{
private:

    char *targetFilePath;
    int targetFilePath_len;

    int compression_def;
    float scale_def;
    bool exportAttributes_def;
    char *attributeNamespace_def;
    int attributeNamespace_len_def;
    bool exportMeta_def;

    bool saveSettingsFile_def;
    int storageMatrix_def;
    int storageGeo_def;
    int storageProps_def;
    int storageAttr_def;

    bool exportScene_def;
    bool exportEmptyContainers_def;
    bool exportPrimitives_def;
    bool exportGeometry_def;
    bool exportUVs_def;
    bool exportNormals_def;
    bool exportSkin_def;
    bool splitByMatID_def;
    bool exportCameras_def;
    bool exportSkyboxes_def;

    bool exportMaterials_def;
    int textureMode_def;
    bool includeShadings_def;
    bool includeMethods_def;
    bool includeLights_def;
    bool applyDarkLight_def;
    bool includeShadows_def;
    bool setMultiPass_def;

    bool excludeUnselectedLayers_def;
    bool excludeInvisibleLayers_def;

    bool exportSkeletons_def;
    bool exportSkelAnim_def;
    bool exportVertexAnim_def;

    int launchPreview_def;//can be 0: nothing, 1: awayBuilder 2: html-viewer
    int previewBackgroundColor_def;

    int compression;
    float scale;
    bool exportAttributes;
    char *attributeNamespace;
    int attributeNamespace_len;
    int storageMatrix;
    int storageGeo;
    int storageProps;
    int storageAttr;
    bool saveSettingsFile;
    bool exportMeta;

    bool exportScene;
    bool exportEmptyContainers;
    bool exportPrimitives;
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

    bool excludeUnselectedLayers;
    bool excludeInvisibleLayers;

    bool exportSkeletons;
    bool exportSkelAnim;
    bool exportVertexAnim;

    int launchPreview;//can be 0: nothing, 1: awayBuilder 2: html-viewer
    int previewBackgroundColor;

    int compression_bkp;
    float scale_bkp;
    bool exportAttributes_bkp;
    char *attributeNamespace_bkp;
    int attributeNamespace_len_bkp;
    bool exportMeta_bkp;

    bool saveSettingsFile_bkp;
    int storageMatrix_bkp;
    int storageGeo_bkp;
    int storageProps_bkp;
    int storageAttr_bkp;

    bool exportScene_bkp;
    bool exportEmptyContainers_bkp;
    bool exportPrimitives_bkp;
    bool exportGeometry_bkp;
    bool exportUVs_bkp;
    bool exportNormals_bkp;
    bool exportSkin_bkp;
    bool splitByMatID_bkp;
    bool exportCameras_bkp;
    bool exportSkyboxes_bkp;

    bool exportMaterials_bkp;
    int textureMode_bkp;
    bool includeShadings_bkp;
    bool includeMethods_bkp;
    bool includeLights_bkp;
    bool applyDarkLight_bkp;
    bool includeShadows_bkp;
    bool setMultiPass_bkp;

    bool excludeUnselectedLayers_bkp;
    bool excludeInvisibleLayers_bkp;

    bool exportSkeletons_bkp;
    bool exportSkelAnim_bkp;
    bool exportVertexAnim_bkp;

    int launchPreview_bkp;//can be 0: nothing, 1: awayBuilder 2: html-viewer
    int previewBackgroundColor_bkp;

    static MaxAWDExporterOpts *INSTANCE;

    static HWND generalOpts;
    static HWND sceneOpts;
    static HWND mtlOpts;
    static HWND animOpts;
    static HWND viewerOpts;
    // Config file
    FILE *OpenConfigFile(const char *mode, const char *mode2);
    void ReadConfigFile(const char *);
    void WriteConfigFile(const char *);
    void SetDefaults(bool);
    void UpdateUI(void);
    void SafeFile(void);
    void LoadFile(void);
    void SetBackup(void);
    void SetOptionsFromUI(void);
    bool CompareBackup(void);
    bool CompareDefault(void);

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
    MaxAWDExporterOpts(const char *);
    ~MaxAWDExporterOpts(void);

    bool ShowDialog(void);

    // Getters for options
    int Compression(void);
    bool ExportAttributes(void);
    char *AttributeNamespace(void);
    double Scale();
    bool ExportMeta(void);

    int StorageMatrix(void);
    int StorageGeometry(void);
    int StorageProperties(void);
    int StorageAttributes(void);

    int SaveSettingsFile(void);
    bool ExportEmptyContainers(void);
    bool ExportPrimitives(void);
    bool ExportScene(void);
    bool ExportGeometry(void);
    bool ExportUVs(void);
    bool ExportNormals(void);
    bool ExportSkin(void);
    bool SplitByMatID(void);
    bool ExportCameras(void);
    bool ExportSkyboxes(void);

    bool ExcludeUnselectedLayers(void);
    bool ExcludeInvisibleLayers(void);

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
    int LaunchPreview(void);
    int PreviewBackgroundColor(void);
};
