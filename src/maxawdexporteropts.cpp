#include "maxawdexporter.h"
#include <custcont.h>

MaxAWDExporterOpts::MaxAWDExporterOpts(void)
{
    // Store pointer to single instance
    INSTANCE = this;


    // Default values
    compression = (int)DEFLATE;
    scale = 1.0;
    exportAttributes = true;
    attributeNamespace = (char*)malloc(25);
    strcpy(attributeNamespace, "http://example.com/awdns");
    
    storageMatrix = 0;
    storageGeo = 0;
    storageProps = 0;
    storageAttr = 0;

    exportScene = true;
    exportGeometry = true;
    exportUVs = true;
    exportNormals = true;
    splitByMatID = false;
    exportCameras = true;
    exportSkyboxes = true;
    exportPrimitives = true;
    exportEmptyContainers = false;
    exportMaterials = true;
    textureMode = 1;
    includeShadings = true;
    includeMethods = true;
    includeLights = true;
    applyDarkLight = true;
    includeShadows = true;
    setMultiPass = true;

    exportSkeletons = true;
    exportSkelAnim = true;
    exportVertexAnim = true;

    launchPreview = 1;
    previewBackgroundColor = 0;

    // Override defaults using config file, if any exists
    ReadConfigFile();
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
    free(attributeNamespace);
}


FILE *MaxAWDExporterOpts::OpenConfigFile(const char *mode)
{
    char buf[1024];

    Interface *ip = GetCOREInterface();
    char *cdir = W2A(ip->GetDir(APP_PLUGCFG_DIR));
    _makepath_s(buf, 1024, NULL, cdir, "MAXAWD", ".CFG");
    free(cdir);
    return fopen(buf, mode);
}

void MaxAWDExporterOpts::ReadConfigFile(void)
{
    // Open config file or abort.
    FILE *cfg = OpenConfigFile("rb");
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
        else if (ATTREQ(key,"attributes")) {
            exportAttributes = (strtol(val, NULL, 10) == 1);
        }
        else if (ATTREQ(key,"namespace")) {
            attributeNamespace = (char*)realloc(attributeNamespace, strlen(val)+1);
            strcpy(attributeNamespace, val);
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

void MaxAWDExporterOpts::WriteConfigFile(void)
{
    // Open config file or abort.
    FILE *cfg = OpenConfigFile("wb");
    if (!cfg) return;
    fprintf(cfg, "compression=%d\n", compression);
    fprintf(cfg, "storageMatrix=%d\n", storageMatrix);
    fprintf(cfg, "storageGeo=%d\n", storageGeo);
    fprintf(cfg, "storageProps=%d\n", storageProps);
    fprintf(cfg, "storageAttr=%d\n", storageAttr);
    fprintf(cfg, "scl=%f\n", scale);
    fprintf(cfg, "attributes=%d\n", exportAttributes);
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

    

    // Set default (or loaded if cfg file existed) options
    ComboBox_SetCurSel(GetDlgItem(generalOpts, IDC_COMP_COMBO), compression);
    ComboBox_SetCurSel(GetDlgItem(generalOpts, IDC_COMBO_MTX), storageMatrix);
    ComboBox_SetCurSel(GetDlgItem(generalOpts, IDC_COMBO_GEOM), storageGeo);
    ComboBox_SetCurSel(GetDlgItem(generalOpts, IDC_COMBO_PROPS), storageProps);
    ComboBox_SetCurSel(GetDlgItem(generalOpts, IDC_COMBO_ATTR), storageAttr);
    
    SetCheckBox(generalOpts, IDC_INC_ATTR, exportAttributes);
    
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

    rollup->Show();
}


void MaxAWDExporterOpts::SaveOptions(void)
{
    int len;
    
    // General options
    compression = ComboBox_GetCurSel(GetDlgItem(generalOpts, IDC_COMP_COMBO));
    storageMatrix = ComboBox_GetCurSel(GetDlgItem(generalOpts, IDC_COMBO_MTX));
    storageGeo = ComboBox_GetCurSel(GetDlgItem(generalOpts, IDC_COMBO_GEOM));
    storageProps = ComboBox_GetCurSel(GetDlgItem(generalOpts, IDC_COMBO_PROPS));
    storageAttr = ComboBox_GetCurSel(GetDlgItem(generalOpts, IDC_COMBO_ATTR));
    exportAttributes = (IsDlgButtonChecked(generalOpts, IDC_INC_ATTR) == BST_CHECKED);
    
    len = Edit_GetTextLength(GetDlgItem(generalOpts, IDC_ATTRNS_TEXT));
    TCHAR *tmp = (TCHAR*)malloc((len+1) * sizeof(TCHAR));
    Edit_GetText(GetDlgItem(generalOpts, IDC_ATTRNS_TEXT), tmp, len+1);
    free(attributeNamespace);
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
    
    WriteConfigFile();
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




