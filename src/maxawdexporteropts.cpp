#include "maxawdexporter.h"
#include <custcont.h>
#include "maxawdexporter.h"
#include <custcont.h>
#include <windows.h>

MaxAWDExporterOpts::MaxAWDExporterOpts(const char *thisTargetFilePath)
{
    // Store pointer to single instance
    INSTANCE = this;

    targetFilePath_len = strlen(thisTargetFilePath)+1;
    targetFilePath = (char*)malloc(targetFilePath_len);
    strcpy(targetFilePath, thisTargetFilePath);
    targetFilePath[targetFilePath_len-1]=0;

    attributeNamespace_len_bkp=0;
    attributeNamespace_len=0;
    // Default values
    compression_def = (int)DEFLATE;
    scale_def = 1.0;
    exportAttributes_def = true;
    exportMeta_def = true;

    attributeNamespace_len_def = 25;
    attributeNamespace_def = (char*)malloc(attributeNamespace_len_def);
    strcpy(attributeNamespace_def, "http://example.com/awdns");
    attributeNamespace_def[attributeNamespace_len_def-1]=0;
    saveSettingsFile_def = true;
    storageMatrix_def = 0;
    storageGeo_def = 0;
    storageProps_def = 0;
    storageAttr_def = 0;

    exportScene_def = true;
    exportGeometry_def = true;
    exportUVs_def = true;
    exportNormals_def = true;
    splitByMatID_def = false;
    exportCameras_def = true;
    exportSkyboxes_def = true;
    exportPrimitives_def = true;
    exportEmptyContainers_def = false;
    exportMaterials_def = true;
    textureMode_def = 1;
    includeShadings_def = true;
    includeMethods_def = true;
    includeLights_def = true;
    applyDarkLight_def = true;
    includeShadows_def = true;
    setMultiPass_def = true;

    excludeUnselectedLayers_def = true;
    excludeInvisibleLayers_def = true;

    exportSkeletons_def = true;
    exportSkelAnim_def = true;
    exportVertexAnim_def = true;

    launchPreview_def = 1;
    previewBackgroundColor_def = 0;

    SetDefaults(false);
    ReadConfigFile(NULL);
}

// Static members
MaxAWDExporterOpts *MaxAWDExporterOpts::INSTANCE;// = NULL;
HWND MaxAWDExporterOpts::generalOpts = NULL;
HWND MaxAWDExporterOpts::sceneOpts = NULL;
HWND MaxAWDExporterOpts::mtlOpts = NULL;
HWND MaxAWDExporterOpts::animOpts = NULL;
HWND MaxAWDExporterOpts::viewerOpts = NULL;

MaxAWDExporterOpts::~MaxAWDExporterOpts(void)
{
    if(targetFilePath_len>0){
        free(targetFilePath);
        targetFilePath=NULL;
        targetFilePath_len=0;
    }
    if(attributeNamespace_len_bkp>0){
        free(attributeNamespace_bkp);
        attributeNamespace_bkp=NULL;
        attributeNamespace_len_bkp=0;
    }
    if(attributeNamespace_len>0){
        free(attributeNamespace);
        attributeNamespace=NULL;
        attributeNamespace_len=0;
    }
    if(attributeNamespace_len_def>0){
        free(attributeNamespace_def);
        attributeNamespace_def=NULL;
        attributeNamespace_len_def=0;
    }
}

FILE *MaxAWDExporterOpts::OpenConfigFile(const char *mode, const char *filepath)
{
    char buf[1024];

    Interface *ip = GetCOREInterface();
    if(filepath==NULL){
        char *cdir = W2A(ip->GetDir(APP_PLUGCFG_DIR));
        _makepath_s(buf, 1024, NULL, cdir, "MAXAWD", ".CFG");
        free(cdir);
        return fopen(buf, mode);
    }
    return fopen(filepath, mode);
}

void MaxAWDExporterOpts::SetBackup(void)
{
    // Default values
    compression_bkp = compression;
    scale_bkp = scale;
    exportAttributes_bkp = exportAttributes;
    exportMeta_bkp = exportMeta;

    if(attributeNamespace_len_bkp>0){
        free(attributeNamespace_bkp);
        attributeNamespace_bkp=NULL;
        attributeNamespace_len_bkp=0;
    }
    attributeNamespace_bkp = (char*)malloc(attributeNamespace_len);
    strcpy(attributeNamespace_bkp, attributeNamespace);
    attributeNamespace_len_bkp=attributeNamespace_len;
    attributeNamespace_bkp[attributeNamespace_len_bkp-1]=0;

    saveSettingsFile_bkp = saveSettingsFile;
    storageMatrix_bkp = storageMatrix;
    storageGeo_bkp = storageGeo;
    storageProps_bkp = storageProps;
    storageAttr_bkp = storageAttr;

    exportScene_bkp = exportScene;
    exportGeometry_bkp = exportGeometry;
    exportUVs_bkp = exportUVs;
    exportNormals_bkp = exportNormals;
    splitByMatID_bkp = splitByMatID;
    exportCameras_bkp = exportCameras;
    exportSkyboxes_bkp = exportSkyboxes;
    exportPrimitives_bkp = exportPrimitives;
    exportEmptyContainers_bkp = exportEmptyContainers;
    exportMaterials_bkp = exportMaterials;
    textureMode_bkp = textureMode;
    includeShadings_bkp = includeShadings;
    includeMethods_bkp = includeMethods;
    includeLights_bkp = includeLights;
    applyDarkLight_bkp = applyDarkLight;
    includeShadows_bkp = includeShadows;
    setMultiPass_bkp = setMultiPass;

    excludeUnselectedLayers_bkp = excludeUnselectedLayers;
    excludeInvisibleLayers_bkp = excludeInvisibleLayers;

    exportSkeletons_bkp = exportSkeletons;
    exportSkelAnim_bkp = exportSkelAnim;
    exportVertexAnim_bkp = exportVertexAnim;

    launchPreview_bkp = launchPreview;
    previewBackgroundColor_bkp = previewBackgroundColor;
}
bool MaxAWDExporterOpts::CompareBackup(void)
{
    if(compression_bkp != compression) return false;
    if(scale_bkp != scale) return false;
    if(exportAttributes_bkp != exportAttributes) return false;
    if(exportMeta_bkp != exportMeta) return false;
    if (!ATTREQ(attributeNamespace_bkp, attributeNamespace) )return false;
    if(storageMatrix_bkp != storageMatrix) return false;
    if(storageGeo_bkp != storageGeo) return false;
    if(storageProps_bkp != storageProps) return false;
    if(storageAttr_bkp != storageAttr) return false;
    if(saveSettingsFile_bkp != saveSettingsFile) return false;

    if(exportScene_bkp != exportScene) return false;
    if(exportGeometry_bkp != exportGeometry) return false;
    if(exportUVs_bkp != exportUVs) return false;
    if(exportNormals_bkp != exportNormals) return false;
    if(splitByMatID_bkp != splitByMatID) return false;
    if(exportCameras_bkp != exportCameras) return false;
    if(exportSkyboxes_bkp != exportSkyboxes) return false;
    if(exportPrimitives_bkp != exportPrimitives) return false;
    if(exportEmptyContainers_bkp != exportEmptyContainers) return false;
    if(exportMaterials_bkp != exportMaterials) return false;
    if(textureMode_bkp != textureMode) return false;
    if(includeShadings_bkp != includeShadings) return false;
    if(includeMethods_bkp != includeMethods) return false;
    if(includeLights_bkp != includeLights) return false;
    if(applyDarkLight_bkp != applyDarkLight) return false;
    if(includeShadows_bkp != includeShadows) return false;
    if(setMultiPass_bkp != setMultiPass) return false;

    if(excludeUnselectedLayers_bkp != excludeUnselectedLayers) return false;
    if(excludeInvisibleLayers_bkp != excludeInvisibleLayers) return false;

    if(exportSkeletons_bkp != exportSkeletons) return false;
    if(exportSkelAnim_bkp != exportSkelAnim) return false;
    if(exportVertexAnim_bkp != exportVertexAnim) return false;

    if(launchPreview_bkp != launchPreview) return false;
    if(previewBackgroundColor_bkp != previewBackgroundColor) return false;
    return true;
}
bool MaxAWDExporterOpts::CompareDefault(void)
{
    if(compression_def != compression) return false;
    if(scale_def != scale) return false;
    if(exportAttributes_def != exportAttributes) return false;
    if(exportMeta_def != exportMeta) return false;
    if (!ATTREQ(attributeNamespace_bkp, attributeNamespace) )return false;
    if(storageMatrix_def != storageMatrix) return false;
    if(storageGeo_def != storageGeo) return false;
    if(storageProps_def != storageProps) return false;
    if(storageAttr_def != storageAttr) return false;
    if(saveSettingsFile_def != saveSettingsFile) return false;

    if(exportScene_def != exportScene) return false;
    if(exportGeometry_def != exportGeometry) return false;
    if(exportUVs_def != exportUVs) return false;
    if(exportNormals_def != exportNormals) return false;
    if(splitByMatID_def != splitByMatID) return false;
    if(exportCameras_def != exportCameras) return false;
    if(exportSkyboxes_def != exportSkyboxes) return false;
    if(exportPrimitives_def != exportPrimitives) return false;
    if(exportEmptyContainers_def != exportEmptyContainers) return false;
    if(exportMaterials_def != exportMaterials) return false;
    if(textureMode_def != textureMode) return false;
    if(includeShadings_def != includeShadings) return false;
    if(includeMethods_def != includeMethods) return false;
    if(includeLights_def != includeLights) return false;
    if(applyDarkLight_def != applyDarkLight) return false;
    if(includeShadows_def != includeShadows) return false;
    if(setMultiPass_def != setMultiPass) return false;

    if(excludeUnselectedLayers_def != excludeUnselectedLayers) return false;
    if(excludeInvisibleLayers_def != excludeInvisibleLayers) return false;

    if(exportSkeletons_def != exportSkeletons) return false;
    if(exportSkelAnim_def != exportSkelAnim) return false;
    if(exportVertexAnim_def != exportVertexAnim) return false;

    if(launchPreview_def != launchPreview) return false;
    if(previewBackgroundColor_def != previewBackgroundColor) return false;
    return true;
}
void MaxAWDExporterOpts::SetDefaults(bool updatethisUI=true)
{
    // Default values
    compression = compression_def;
    scale = scale_def;
    exportAttributes = exportAttributes_def;
    exportMeta = exportMeta_def;

    if(attributeNamespace_len>0){
        free(attributeNamespace);
        attributeNamespace=NULL;
        attributeNamespace_len=0;
    }
    attributeNamespace = (char*)malloc(attributeNamespace_len_def);
    strcpy(attributeNamespace, attributeNamespace_def);
    attributeNamespace_len=attributeNamespace_len_def;
    attributeNamespace[attributeNamespace_len-1]=0;

    saveSettingsFile = saveSettingsFile_def;
    storageMatrix = storageMatrix_def;
    storageGeo = storageGeo_def;
    storageProps = storageProps_def;
    storageAttr = storageAttr_def;

    exportScene = exportScene_def;
    exportGeometry = exportGeometry_def;
    exportUVs = exportUVs_def;
    exportNormals = exportNormals_def;
    splitByMatID = splitByMatID_def;
    exportCameras = exportCameras_def;
    exportSkyboxes = exportSkyboxes_def;
    exportPrimitives = exportPrimitives_def;
    exportEmptyContainers = exportEmptyContainers_def;
    exportMaterials = exportMaterials_def;
    textureMode = textureMode_def;
    includeShadings = includeShadings_def;
    includeMethods = includeMethods_def;
    includeLights = includeLights_def;
    applyDarkLight = applyDarkLight_def;
    includeShadows = includeShadows_def;
    setMultiPass = setMultiPass_def;

    excludeUnselectedLayers = excludeUnselectedLayers_def;
    excludeInvisibleLayers = excludeInvisibleLayers_def;

    exportSkeletons = exportSkeletons_def;
    exportSkelAnim = exportSkelAnim_def;
    exportVertexAnim = exportVertexAnim_def;

    launchPreview = launchPreview_def;
    previewBackgroundColor = previewBackgroundColor_def;
    SetBackup();
    if(updatethisUI)
        UpdateUI();
}

void MaxAWDExporterOpts::ReadConfigFile(const char *filepath)
{
    // Open config file or abort.
    FILE *cfg = OpenConfigFile("rb", filepath);
    if (!cfg) return;

    while (!feof(cfg)) {
        char buf[ATTR_BUF_LEN];
        fgets(buf, 1024, cfg);

        // Read key and value and skip if any is missing
        char *key = strtok(buf, "=");
        char *val = strtok(NULL, "\n");
        if (!key || !val)
            continue;

        if (ATTREQ(key,"compression")) {
            compression = strtol(val, NULL, 10);
        }
        if (ATTREQ(key,"storageMatrix")) {
            storageMatrix = strtol(val, NULL, 10);
        }
        if (ATTREQ(key,"storageGeo")) {
            storageGeo = strtol(val, NULL, 10);
        }
        if (ATTREQ(key,"storageProps")) {
            storageProps = strtol(val, NULL, 10);
        }
        if (ATTREQ(key,"storageAttr")) {
            storageAttr = strtol(val, NULL, 10);
        }
        else if (ATTREQ(key, "scl")) {
            scale = strtod(val, NULL);
        }
        else if (ATTREQ(key,"saveSettingsFile")) {
            saveSettingsFile = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key,"attributes")) {
            exportAttributes = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key,"exportMeta")) {
            exportMeta = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key,"namespace")) {
            attributeNamespace = (char*)realloc(attributeNamespace, strlen(val)+1);
            attributeNamespace_len = strlen(val)+1;
            strcpy(attributeNamespace, val);
            attributeNamespace[attributeNamespace_len-1]=0;
        }
        else if (ATTREQ(key, "scene")) {
            exportScene = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "geometry")) {
            exportGeometry = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "uvs")) {
            exportUVs = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "normals")) {
            exportNormals = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "skin")) {
            exportSkin = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "splitByMatID")) {
            splitByMatID = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "exportPrimitives")) {
            exportPrimitives = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "exportEmptyContainers")) {
            exportEmptyContainers = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "exportCameras")) {
            exportCameras = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "exportSkyboxes")) {
            exportSkyboxes = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "materials")) {
            exportMaterials = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "includeShadings")) {
            includeShadings = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "includeMethods")) {
            includeMethods = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "includeLights")) {
            includeLights = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "applyDarkLight")) {
            applyDarkLight = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "excludeUnselectedLayers")) {
            excludeUnselectedLayers = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "excludeInvisibleLayers")) {
            excludeInvisibleLayers = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "includeShadows")) {
            includeShadows = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "setMultiPass")) {
            setMultiPass = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "textureMode")) {
            textureMode = strtol(val, NULL, 10);
        }
        else if (ATTREQ(key, "skeletons")) {
            exportSkeletons = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "skelanim")) {
            exportSkelAnim = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "exportVertexAnim")) {
            exportVertexAnim = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key, "launch")) {
            launchPreview = strtol(val, NULL, 10);
        }
        else if (ATTREQ(key, "bgcolor")) {
            previewBackgroundColor = strtol(val, NULL, 16);
        }
    }

    fclose(cfg);
}

void MaxAWDExporterOpts::WriteConfigFile(const char *filepath)
{
    // Open config file or abort.
    FILE *cfg = OpenConfigFile("wb", filepath);
    if (!cfg) return;
    fprintf(cfg, "compression=%d\n", compression);
    fprintf(cfg, "storageMatrix=%d\n", storageMatrix);
    fprintf(cfg, "storageGeo=%d\n", storageGeo);
    fprintf(cfg, "storageProps=%d\n", storageProps);
    fprintf(cfg, "storageAttr=%d\n", storageAttr);
    fprintf(cfg, "scl=%f\n", scale);
    fprintf(cfg, "saveSettingsFile=%d\n", saveSettingsFile);
    fprintf(cfg, "attributes=%d\n", exportAttributes);
    fprintf(cfg, "exportMeta=%d\n", exportMeta);
    fprintf(cfg, "namespace=%s\n", attributeNamespace);

    fprintf(cfg, "scene=%d\n", exportScene);
    fprintf(cfg, "geometry=%d\n", exportGeometry);
    fprintf(cfg, "uvs=%d\n", exportUVs);
    fprintf(cfg, "normals=%d\n", exportNormals);
    fprintf(cfg, "skin=%d\n", exportSkin);
    fprintf(cfg, "splitByMatID=%d\n", splitByMatID);
    fprintf(cfg, "exportPrimitives=%d\n", exportPrimitives);
    fprintf(cfg, "exportEmptyContainers=%d\n", exportEmptyContainers);
    fprintf(cfg, "exportCameras=%d\n", exportCameras);
    fprintf(cfg, "exportSkyboxes=%d\n", exportSkyboxes);

    fprintf(cfg, "materials=%d\n", exportMaterials);
    fprintf(cfg, "includeShadings=%d\n", includeShadings);
    fprintf(cfg, "includeMethods=%d\n", includeMethods);
    fprintf(cfg, "includeLights=%d\n", includeLights);
    fprintf(cfg, "applyDarkLight=%d\n", applyDarkLight);
    fprintf(cfg, "excludeUnselectedLayers=%d\n", excludeUnselectedLayers);
    fprintf(cfg, "excludeInvisibleLayers=%d\n", excludeInvisibleLayers);
    fprintf(cfg, "includeShadows=%d\n", includeShadows);
    fprintf(cfg, "setMultiPass=%d\n", setMultiPass);
    fprintf(cfg, "textureMode=%d\n", textureMode);

    fprintf(cfg, "skeletons=%d\n", exportSkeletons);
    fprintf(cfg, "skelanim=%d\n", exportSkelAnim);
    fprintf(cfg, "exportVertexAnim=%d\n", exportVertexAnim);

    fprintf(cfg, "launch=%d\n", launchPreview);
    fprintf(cfg, "bgcolor=%06x\n", previewBackgroundColor);

    fclose(cfg);
}

void MaxAWDExporterOpts::SafeFile(void)
{
    SetOptionsFromUI();
    TSTR filename, dir;
    FilterList filter;
    filter.Append( _M("Text(*.awdPreset)") );
    filter.Append( _M("*.awdPreset") );
    Interface8* imax = GetCOREInterface8();
    if (imax->DoMaxSaveAsDialog( generalOpts, _T("Save AWD Preset"), filename, dir, filter )) {
        char * fileName_ptr = W2A(filename);
        WriteConfigFile(fileName_ptr);
        free(fileName_ptr);
        //int test=0;// do something with filename
    }
}
void MaxAWDExporterOpts::LoadFile(void)
{
    TSTR filename, dir;
    FilterList filter;
    filter.Append( _M("Text(*.awdPreset)") );
    filter.Append( _M("*.awdPreset") );
    Interface8* imax = GetCOREInterface8();
    if (imax->DoMaxOpenDialog( generalOpts, _T("Open AWD Preset"), filename, dir, filter )) {
        char * fileName_ptr = W2A(filename);
        ReadConfigFile(fileName_ptr);
        free(fileName_ptr);
        UpdateUI();
    }
}
bool MaxAWDExporterOpts::ShowDialog(void)
{
    int ret = DialogBoxParam(hInstance,
                MAKEINTRESOURCE(IDD_AWD_OPTIONS),
                GetActiveWindow(),
                DialogProc,
                0);
    return (ret == IDOK);
}

INT_PTR CALLBACK MaxAWDExporterOpts::DialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
            INSTANCE->InitDialog(hWnd, message, wParam, lParam);
            CenterWindow(hWnd,GetParent(hWnd));
            return TRUE;

        case WM_COMMAND:
            switch (wParam) {
                case IDC_OK:
                    INSTANCE->SaveOptions();
                    EndDialog(hWnd, IDOK);
                    break;

                case IDC_CANCEL:
                    EndDialog(hWnd, IDCANCEL);
                    break;
            }
            return TRUE;

        case WM_CLOSE:
            EndDialog(hWnd, 0);
            return TRUE;
    }

    return FALSE;
}

void MaxAWDExporterOpts::InitDialog(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    int index;

    HWND rh = GetDlgItem(hWnd, IDC_ROLLUP);
    IRollupWindow *rollup = GetIRollup(rh);

    index = rollup->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_AWD_GENERAL_OPTS),
        GeneralOptsDialogProc, TEXT("General"));
    generalOpts = rollup->GetPanelDlg(index);

    index = rollup->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_AWD_SCENE_OPTS),
        SceneOptsDialogProc, TEXT("Scene & geometry"), 0, APPENDROLL_CLOSED);
    sceneOpts = rollup->GetPanelDlg(index);

    index = rollup->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_AWD_MTL_OPTS),
        MtlOptsDialogProc, TEXT("Materials & Lights"), 0, APPENDROLL_CLOSED);
    mtlOpts = rollup->GetPanelDlg(index);

    index = rollup->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_AWD_ANIM_OPTS),
        AnimOptsDialogProc, TEXT("Animation"), 0, APPENDROLL_CLOSED);
    animOpts = rollup->GetPanelDlg(index);

    index = rollup->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_AWD_VIEWER_OPTS),
        ViewerOptsDialogProc, TEXT("Preview options"), 0, APPENDROLL_CLOSED);
    viewerOpts = rollup->GetPanelDlg(index);
    UpdateUI();
    rollup->Show();
}

void MaxAWDExporterOpts::UpdateUI()
{
    // Set default (or loaded if cfg file existed) options
    ComboBox_SetCurSel(GetDlgItem(generalOpts, IDC_COMP_COMBO), compression);
    ComboBox_SetCurSel(GetDlgItem(generalOpts, IDC_COMBO_MTX), storageMatrix);
    ComboBox_SetCurSel(GetDlgItem(generalOpts, IDC_COMBO_GEOM), storageGeo);
    ComboBox_SetCurSel(GetDlgItem(generalOpts, IDC_COMBO_PROPS), storageProps);
    ComboBox_SetCurSel(GetDlgItem(generalOpts, IDC_COMBO_ATTR), storageAttr);

    ISpinnerControl *spinnerScale = GetISpinner(GetDlgItem(generalOpts,IDC_SCALE_SPINNER));
    spinnerScale->SetValue(scale, FALSE);
    SetCheckBox(generalOpts, IDC_INC_ATTR, exportAttributes);
    SetCheckBox(generalOpts, IDC_EXPORTMETA, exportMeta);

    TCHAR *ans = A2W(attributeNamespace);
    Edit_SetText(GetDlgItem(generalOpts, IDC_ATTRNS_TEXT), ans);
    free(ans);
    SetCheckBox(sceneOpts, IDC_INC_SCENE, exportScene);
    SetCheckBox(sceneOpts, IDC_INC_GEOM, exportGeometry);
    SetCheckBox(sceneOpts, IDC_INC_UVS, exportUVs);
    SetCheckBox(sceneOpts, IDC_INC_NORMALS, exportNormals);
    SetCheckBox(sceneOpts, IDC_INC_SKIN, exportSkin);
    SetCheckBox(sceneOpts, IDC_SPLITBYMATID, splitByMatID);
    SetCheckBox(sceneOpts, IDC_INC_PRIMS, exportPrimitives);
    SetCheckBox(sceneOpts, IDC_INC_EXC_EMPTY, exportEmptyContainers);
    SetCheckBox(sceneOpts, IDC_EXL_INVIS_LAYERS, excludeInvisibleLayers);
    SetCheckBox(sceneOpts, IDC_EXCL_SELECT_LAYER, excludeUnselectedLayers);

    SetCheckBox(sceneOpts, IDC_INC_CAMERAS, exportCameras);
    SetCheckBox(sceneOpts, IDC_INC_SKYBOXES, exportSkyboxes);
    SetCheckBox(mtlOpts, IDC_INC_MTL, exportMaterials);
    SetCheckBox(mtlOpts, IDC_INC_SHADING, includeShadings);
    SetCheckBox(mtlOpts, IDC_INC_EFFECT, includeMethods);
    SetCheckBox(mtlOpts, IDC_INC_LIGHTS, includeLights);
    SetCheckBox(mtlOpts, IDC_INC_DARKLIGHT, applyDarkLight);
    SetCheckBox(mtlOpts, IDC_INC_SHADOWS, includeShadows);
    SetCheckBox(mtlOpts, IDC_INC_MULTIPASS, setMultiPass);
    ComboBox_SetCurSel(GetDlgItem(mtlOpts, IDC_COMBOTEXTURES), textureMode);
    SetCheckBox(animOpts, IDC_INC_SKEL, exportSkeletons);
    SetCheckBox(animOpts, IDC_INC_SKELANIM, exportSkelAnim);
    SetCheckBox(animOpts, IDC_INC_VERTEXANIM, exportVertexAnim);

    ComboBox_SetCurSel(GetDlgItem(viewerOpts, IDC_SWF_LAUNCH), launchPreview);

    // Flip color channels
    AColor col;
    col.r = GetBValue(previewBackgroundColor) / 255.0;
    col.g = GetGValue(previewBackgroundColor) / 255.0;
    col.b = GetRValue(previewBackgroundColor) / 255.0;
    GetIColorSwatch(GetDlgItem(viewerOpts, IDC_SWF_COLOR))->SetAColor(col, FALSE);

    // Force redraw all panels
    RedrawGeneralOpts(0);
    RedrawSceneOpts(0);
    RedrawMtlOpts(0);
    RedrawAnimOpts(0);
    RedrawViewerOpts(0);
}

void MaxAWDExporterOpts::SetOptionsFromUI(void)
{
    int len;

    // General options
    compression = ComboBox_GetCurSel(GetDlgItem(generalOpts, IDC_COMP_COMBO));
    storageMatrix = ComboBox_GetCurSel(GetDlgItem(generalOpts, IDC_COMBO_MTX));
    storageGeo = ComboBox_GetCurSel(GetDlgItem(generalOpts, IDC_COMBO_GEOM));
    storageProps = ComboBox_GetCurSel(GetDlgItem(generalOpts, IDC_COMBO_PROPS));
    storageAttr = ComboBox_GetCurSel(GetDlgItem(generalOpts, IDC_COMBO_ATTR));
    exportAttributes = (IsDlgButtonChecked(generalOpts, IDC_INC_ATTR) == BST_CHECKED);
    exportMeta = (IsDlgButtonChecked(generalOpts, IDC_EXPORTMETA) == BST_CHECKED);
    saveSettingsFile = (IsDlgButtonChecked(generalOpts, IDC_CBX_SAVEFORFILE) == BST_CHECKED);

    len = Edit_GetTextLength(GetDlgItem(generalOpts, IDC_ATTRNS_TEXT));
    TCHAR *tmp = (TCHAR*)malloc((len+1) * sizeof(TCHAR));
    Edit_GetText(GetDlgItem(generalOpts, IDC_ATTRNS_TEXT), tmp, len+1);
    if(attributeNamespace_len>0){
        free(attributeNamespace);
        attributeNamespace_len=0;
        attributeNamespace=NULL;
    }
    attributeNamespace = W2A(tmp);
    free(tmp);
    scale = GetISpinner(GetDlgItem(generalOpts,IDC_SCALE_SPINNER))->GetFVal();

    // Scene & geometry options
    exportScene = (IsDlgButtonChecked(sceneOpts, IDC_INC_SCENE) == BST_CHECKED);
    exportGeometry = (IsDlgButtonChecked(sceneOpts, IDC_INC_GEOM) == BST_CHECKED);
    exportUVs = (IsDlgButtonChecked(sceneOpts, IDC_INC_UVS) == BST_CHECKED);
    exportNormals = (IsDlgButtonChecked(sceneOpts, IDC_INC_NORMALS) == BST_CHECKED);
    exportSkin = (IsDlgButtonChecked(sceneOpts, IDC_INC_SKIN) == BST_CHECKED);
    splitByMatID = (IsDlgButtonChecked(sceneOpts, IDC_SPLITBYMATID) == BST_CHECKED);
    exportPrimitives = (IsDlgButtonChecked(sceneOpts, IDC_INC_PRIMS) == BST_CHECKED);
    exportEmptyContainers = (IsDlgButtonChecked(sceneOpts, IDC_INC_EXC_EMPTY) == BST_CHECKED);
    excludeInvisibleLayers = (IsDlgButtonChecked(sceneOpts, IDC_EXL_INVIS_LAYERS) == BST_CHECKED);
    excludeUnselectedLayers = (IsDlgButtonChecked(sceneOpts, IDC_EXCL_SELECT_LAYER) == BST_CHECKED);

    exportCameras = (IsDlgButtonChecked(sceneOpts, IDC_INC_CAMERAS) == BST_CHECKED);
    exportSkyboxes = (IsDlgButtonChecked(sceneOpts, IDC_INC_SKYBOXES) == BST_CHECKED);
    // Material options
    exportMaterials = (IsDlgButtonChecked(mtlOpts, IDC_INC_MTL) == BST_CHECKED);
    includeShadings = (IsDlgButtonChecked(mtlOpts, IDC_INC_EFFECT) == BST_CHECKED);
    includeMethods = (IsDlgButtonChecked(mtlOpts, IDC_INC_EFFECT) == BST_CHECKED);
    includeLights = (IsDlgButtonChecked(mtlOpts, IDC_INC_LIGHTS) == BST_CHECKED);
    applyDarkLight = (IsDlgButtonChecked(mtlOpts, IDC_INC_DARKLIGHT) == BST_CHECKED);
    includeShadows = (IsDlgButtonChecked(mtlOpts, IDC_INC_SHADOWS) == BST_CHECKED);
    setMultiPass = (IsDlgButtonChecked(mtlOpts, IDC_INC_MULTIPASS) == BST_CHECKED);
    textureMode = ComboBox_GetCurSel(GetDlgItem(mtlOpts, IDC_COMBOTEXTURES));

    // Animation options
    exportSkeletons = (IsDlgButtonChecked(animOpts, IDC_INC_SKEL) == BST_CHECKED);
    exportSkelAnim = (IsDlgButtonChecked(animOpts, IDC_INC_SKELANIM) == BST_CHECKED);
    exportVertexAnim = (IsDlgButtonChecked(animOpts, IDC_INC_VERTEXANIM) == BST_CHECKED);

    // Preview options
    launchPreview = ComboBox_GetCurSel(GetDlgItem(viewerOpts, IDC_SWF_LAUNCH));

    int col = GetIColorSwatch(GetDlgItem(viewerOpts, IDC_SWF_COLOR))->GetColor();
    previewBackgroundColor = (GetRValue(col)<<16) | (GetGValue(col)<<8) | GetBValue(col);
}
void MaxAWDExporterOpts::SaveOptions(void)
{
    SetOptionsFromUI();
    if(saveSettingsFile){
        char awdDrive[4];
        char awdPath[1024];
        char awdName[256];
        char * outAWDPresetPath = (char *)malloc(sizeof(char)*1024);
        _splitpath_s(targetFilePath, awdDrive, 4, awdPath, 1024, awdName, 256, NULL, 0);
        _makepath_s(outAWDPresetPath, 1024, awdDrive, awdPath, awdName, "awdPreset");
        WriteConfigFile(outAWDPresetPath);
    }
    WriteConfigFile(NULL);
}

INT_PTR CALLBACK MaxAWDExporterOpts::GeneralOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    ISpinnerControl *spinnerScale;
    bool enabled;

    switch (message) {
        case WM_INITDIALOG:
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMP_COMBO), _T("None"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMP_COMBO), _T("GZIP"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBO_MTX), _T("FileSize"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBO_MTX), _T("Precision"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBO_GEOM), _T("FileSize"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBO_GEOM), _T("Precision"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBO_PROPS), _T("FileSize"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBO_PROPS), _T("Precision"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBO_ATTR), _T("FileSize"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBO_ATTR), _T("Precision"));
            // TODO: LZMA is not working
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMP_COMBO), _T("LZMA"));
            spinnerScale = GetISpinner(GetDlgItem(hWnd,IDC_SCALE_SPINNER));
            spinnerScale->LinkToEdit(GetDlgItem(hWnd,IDC_SCALE_EDIT),EDITTYPE_FLOAT);
            spinnerScale->SetLimits(0.0001, 100000, FALSE);
            spinnerScale->SetValue(INSTANCE->scale, TRUE);
            return TRUE;

        case WM_COMMAND:
            return INSTANCE->RedrawGeneralOpts(lParam);
            break;
    }

    return FALSE;
}

bool MaxAWDExporterOpts::RedrawGeneralOpts(LPARAM lParam)
{
    bool force = (lParam == 0);
    if ((HWND)lParam == GetDlgItem(generalOpts,IDC_SAVE)) {
        SafeFile();
    }
    if ((HWND)lParam == GetDlgItem(generalOpts,IDC_LOAD)) {
        LoadFile();
    }
    if ((HWND)lParam == GetDlgItem(generalOpts,IDC_SET_DEFAULTS)) {
        SetDefaults();
    }
    if (force || (HWND)lParam == GetDlgItem(generalOpts,IDC_INC_ATTR)) {
        bool enabled = (IsDlgButtonChecked(generalOpts,IDC_INC_ATTR) == BST_CHECKED);
        Static_Enable(GetDlgItem(generalOpts,IDC_ATTRNS_STATIC), enabled);
        Edit_Enable(GetDlgItem(generalOpts,IDC_ATTRNS_TEXT), enabled);
        return true;
    }
    return false;
}

INT_PTR CALLBACK MaxAWDExporterOpts::SceneOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    ISpinnerControl *spinner;
    bool enabled;

    switch (message) {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            return INSTANCE->RedrawSceneOpts(lParam);
            break;
    }

    return FALSE;
}

bool MaxAWDExporterOpts::RedrawSceneOpts(LPARAM lParam)
{
    bool ret = false;
    bool force = (lParam == 0);

    if (force || (HWND)lParam == GetDlgItem(sceneOpts, IDC_INC_GEOM)) {
        bool enabled = (IsDlgButtonChecked(sceneOpts, IDC_INC_GEOM) == BST_CHECKED);
        Button_Enable(GetDlgItem(sceneOpts,IDC_INC_NORMALS), enabled);
        Button_Enable(GetDlgItem(sceneOpts,IDC_INC_UVS), enabled);
        Button_Enable(GetDlgItem(sceneOpts,IDC_INC_SKIN), enabled);
        Button_Enable(GetDlgItem(sceneOpts,IDC_SPLITBYMATID), enabled);
        Button_Enable(GetDlgItem(sceneOpts,IDC_INC_PRIMS), enabled);
        ret = true;
    }
    if (force || (HWND)lParam == GetDlgItem(sceneOpts, IDC_INC_SCENE)) {
        bool enabled = (IsDlgButtonChecked(sceneOpts, IDC_INC_SCENE) == BST_CHECKED);
        Button_Enable(GetDlgItem(sceneOpts,IDC_INC_CAMERAS), enabled);
        Button_Enable(GetDlgItem(sceneOpts,IDC_INC_SKYBOXES), enabled);
        ret = true;
    }
    /*if (force || (HWND)lParam == GetDlgItem(sceneOpts, IDC_EXCL_SELECT_LAYER)) {
        bool enabled = (IsDlgButtonChecked(sceneOpts, IDC_EXCL_SELECT_LAYER) == BST_CHECKED);
        Edit_Enable(GetDlgItem(sceneOpts,IDC_EXL_INVIS_LAYERS), !enabled);
        ret = true;
    }*/

    return ret;
}

INT_PTR CALLBACK MaxAWDExporterOpts::MtlOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch (message) {
        case WM_INITDIALOG:
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBOTEXTURES), _T("Do not export textures"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBOTEXTURES), _T("Embbed textures into AWD-file"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBOTEXTURES), _T("External (relative path)"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMBOTEXTURES), _T("External (absolute path)"));
            return TRUE;
        case WM_COMMAND:return INSTANCE->RedrawMtlOpts(lParam);
            break;
    }

    return FALSE;
}

bool MaxAWDExporterOpts::RedrawMtlOpts(LPARAM lParam)
{
    bool force = (lParam == 0);
    bool enabled;

    if (force || (HWND)lParam == GetDlgItem(mtlOpts, IDC_INC_MTL)) {
        enabled = (IsDlgButtonChecked(mtlOpts, IDC_INC_MTL) == BST_CHECKED);
        Button_Enable(GetDlgItem(mtlOpts,IDC_COMBOTEXTURES), enabled);
        Button_Enable(GetDlgItem(mtlOpts,IDC_STATICTEXTURES), enabled);
        Button_Enable(GetDlgItem(mtlOpts,IDC_INC_SHADING), enabled);
        Button_Enable(GetDlgItem(mtlOpts,IDC_INC_EFFECT), enabled);
        Button_Enable(GetDlgItem(mtlOpts,IDC_INC_DARKLIGHT), enabled);
        Button_Enable(GetDlgItem(mtlOpts,IDC_INC_MULTIPASS), enabled);
    }
    if (force || (HWND)lParam == GetDlgItem(mtlOpts, IDC_INC_LIGHTS)) {
        enabled = (IsDlgButtonChecked(mtlOpts, IDC_INC_LIGHTS) == BST_CHECKED);
        Button_Enable(GetDlgItem(mtlOpts,IDC_INC_SHADOWS), enabled);
        if (!IsDlgButtonChecked(mtlOpts, IDC_INC_MTL) == BST_CHECKED)
            enabled=false;
        Button_Enable(GetDlgItem(mtlOpts,IDC_INC_DARKLIGHT), enabled);
        Button_Enable(GetDlgItem(mtlOpts,IDC_INC_MULTIPASS), enabled);
    }

    return true;
}

INT_PTR CALLBACK MaxAWDExporterOpts::AnimOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch (message) {
        case WM_COMMAND:
            return INSTANCE->RedrawAnimOpts(lParam);
            break;
    }

    return FALSE;
}

bool MaxAWDExporterOpts::RedrawAnimOpts(LPARAM lParam)
{
    bool force = (lParam == 0);

    if (force || (HWND)lParam == GetDlgItem(animOpts,IDC_INC_SKELANIM)) {
        return true;
    }

    return false;
}

INT_PTR CALLBACK MaxAWDExporterOpts::ViewerOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    bool enabled;

    switch (message) {
        case WM_INITDIALOG:
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_SWF_LAUNCH), _T("1. Do nothing"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_SWF_LAUNCH), _T("2. Open AWD with default app (AwayBuilder)"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_SWF_LAUNCH), _T("3. Open AWD in Browser (viewer.swf) - LOCAL"));
            ComboBox_AddItemData(GetDlgItem(hWnd, IDC_SWF_LAUNCH), _T("4. Open AWD in Browser (viewer.swf) - NETWORK"));
            return TRUE;
        case WM_COMMAND:
            return INSTANCE->RedrawViewerOpts(lParam);
        break;
    }

    return FALSE;
}

bool MaxAWDExporterOpts::RedrawViewerOpts(LPARAM lParam)
{
    bool force = (lParam == 0);

    if (force || (HWND)lParam == GetDlgItem(viewerOpts,IDC_SWF_LAUNCH)) {
        bool enabled = (ComboBox_GetCurSel(GetDlgItem(viewerOpts, IDC_SWF_LAUNCH)) >= 2);
        //Button_Enable(GetDlgItem(viewerOpts,IDC_SWF_LAUNCH), enabled);
        Static_Enable(GetDlgItem(viewerOpts,IDC_BGCOLOR_STATIC), enabled);
        GetIColorSwatch(GetDlgItem(viewerOpts,IDC_SWF_COLOR))->Enable(enabled);
        return true;
    }

    return false;
}

int MaxAWDExporterOpts::Compression(void)
{
    return compression;
}
int MaxAWDExporterOpts::StorageMatrix(void)
{
    return storageMatrix;
}
int MaxAWDExporterOpts::StorageGeometry(void)
{
    return storageGeo;
}
int MaxAWDExporterOpts::StorageProperties(void)
{
    return storageProps;
}
int MaxAWDExporterOpts::StorageAttributes(void)
{
    return storageAttr;
}

bool MaxAWDExporterOpts::ExportAttributes(void)
{
    return exportAttributes;
}
bool MaxAWDExporterOpts::ExportMeta(void)
{
    return exportMeta;
}

char *MaxAWDExporterOpts::AttributeNamespace(void)
{
    return attributeNamespace;
}

bool MaxAWDExporterOpts::ExportScene(void)
{
    return exportScene;
}

bool MaxAWDExporterOpts::ExportGeometry(void)
{
    return exportGeometry;
}

bool MaxAWDExporterOpts::ExportUVs(void)
{
    return exportUVs;
}

bool MaxAWDExporterOpts::ExportNormals(void)
{
    return exportNormals;
}

bool MaxAWDExporterOpts::ExportSkin(void)
{
    return exportSkin;
}
bool MaxAWDExporterOpts::SplitByMatID(void)
{
    return splitByMatID;
}
bool MaxAWDExporterOpts::ExportPrimitives(void)
{
    return exportPrimitives;
}
bool MaxAWDExporterOpts::ExportEmptyContainers(void)
{
    return exportEmptyContainers;
}

bool MaxAWDExporterOpts::ExportCameras(void)
{
    return exportCameras;
}
bool MaxAWDExporterOpts::ExportSkyboxes(void)
{
    return exportSkyboxes;
}

bool MaxAWDExporterOpts::ExcludeInvisibleLayers(void)
{
    return excludeInvisibleLayers;
}
bool MaxAWDExporterOpts::ExcludeUnselectedLayers(void)
{
    return excludeUnselectedLayers;
}
double MaxAWDExporterOpts::Scale(void)
{
    return scale;
}

bool MaxAWDExporterOpts::ExportMaterials(void)
{
    return exportMaterials;
}
int MaxAWDExporterOpts::TextureMode(void)
{
    return textureMode;
}

bool MaxAWDExporterOpts::IncludeShadings(void)
{
    return includeShadings;
}
bool MaxAWDExporterOpts::IncludeMethods(void)
{
    return includeMethods;
}
bool MaxAWDExporterOpts::IncludeLights(void)
{
    return includeLights;
}
bool MaxAWDExporterOpts::ApplyDarkLight(void)
{
    return applyDarkLight;
}
bool MaxAWDExporterOpts::IncludeShadows(void)
{
    return includeShadows;
}
bool MaxAWDExporterOpts::SetMultiPass(void)
{
    return setMultiPass;
}

bool MaxAWDExporterOpts::ExportSkeletons(void)
{
    return exportSkeletons;
}

bool MaxAWDExporterOpts::ExportSkelAnim(void)
{
    return exportSkelAnim;
}

bool MaxAWDExporterOpts::ExportVertexAnim(void)
{
    return exportVertexAnim;
}

int MaxAWDExporterOpts::LaunchPreview(void)
{
    return launchPreview;
}

int MaxAWDExporterOpts::PreviewBackgroundColor(void)
{
return previewBackgroundColor;
}