#include "maxawdopts.h"
#include <custcont.h>

MaxAWDExporterOpts::MaxAWDExporterOpts(void)
{
	// Store pointer to single instance
	INSTANCE = this;


	// Default values
	compression = (int)DEFLATE;
	exportAttributes = true;

	attributeNamespace = (char*)malloc(25);
	strcpy(attributeNamespace, "http://example.com/awdns");

	exportScene = true;
	exportGeometry = true;
	exportUVs = true;
	exportNormals = true;
	exportSkin = true;
	jointsPerVertex = 2;

	exportMaterials = true;
	forceBasenameTextures = true;
	copyTextures = true;
	embedTextures = false;

	exportSkeletons = true;
	exportSkelAnim = true;

	sequencesTxtPath = (char*)malloc(14);
	strcpy(sequencesTxtPath, "sequences.txt");

	//createPreview = true;
	//launchPreview = true;
	networkPreview = false;
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
//HWND MaxAWDExporterOpts::viewerOpts = NULL;


MaxAWDExporterOpts::~MaxAWDExporterOpts(void)
{
}


FILE *MaxAWDExporterOpts::OpenConfigFile(const char *mode)
{
	char buf[1024];

	Interface *ip = GetCOREInterface();
	_makepath_s(buf, 1024, NULL, ip->GetDir(APP_PLUGCFG_DIR), "MAXAWD", ".CFG");
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
		else if (ATTREQ(key, "jpv")) {
			jointsPerVertex = strtol(val, NULL, 10);
		}
		else if (ATTREQ(key, "materials")) {
			exportMaterials = (strtol(val, NULL, 10) == 1);
		}
		else if (ATTREQ(key, "basenametex")) {
			forceBasenameTextures = (strtol(val, NULL, 10) == 1);
		}
		else if (ATTREQ(key, "copytex")) {
			copyTextures = (strtol(val, NULL, 10) == 1);
		}
		else if (ATTREQ(key, "embedtex")) {
			embedTextures = (strtol(val, NULL, 10) == 1);
		}
		else if (ATTREQ(key, "skeletons")) {
			exportSkeletons = (strtol(val, NULL, 10) == 1);
		}
		else if (ATTREQ(key, "skelanim")) {
			exportSkelAnim = (strtol(val, NULL, 10) == 1);
		}
		else if (ATTREQ(key, "sequences")) {
			sequencesTxtPath = (char*)realloc(sequencesTxtPath, strlen(val)+1);
			strcpy(sequencesTxtPath, val);
		}
		//else if (ATTREQ(key, "preview")) {
		//	createPreview = (strtol(val, NULL, 10) == 1);
		//}
		//else if (ATTREQ(key, "launch")) {
		//	launchPreview = (strtol(val, NULL, 10) == 1);
		//}
		else if (ATTREQ(key, "deploy")) {
			networkPreview = (strtol(val, NULL, 10) == 1);
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
	fprintf(cfg, "attributes=%d\n", exportAttributes);
	fprintf(cfg, "namespace=%s\n", attributeNamespace);

	fprintf(cfg, "scene=%d\n", exportScene);
	fprintf(cfg, "geometry=%d\n", exportGeometry);
	fprintf(cfg, "uvs=%d\n", exportUVs);
	fprintf(cfg, "normals=%d\n", exportNormals);
	fprintf(cfg, "skin=%d\n", exportSkin);
	fprintf(cfg, "jpv=%d\n", jointsPerVertex);

	fprintf(cfg, "materials=%d\n", exportMaterials);
	fprintf(cfg, "basenametex=%d\n", forceBasenameTextures);
	fprintf(cfg, "copytex=%d\n", copyTextures);
	fprintf(cfg, "embedtex=%d\n", embedTextures);

	fprintf(cfg, "skeletons=%d\n", exportSkeletons);
	fprintf(cfg, "skelanim=%d\n", exportSkelAnim);
	fprintf(cfg, "sequences=%s\n", sequencesTxtPath);

	//fprintf(cfg, "preview=%d\n", createPreview);
	//fprintf(cfg, "launch=%d\n", launchPreview);
	fprintf(cfg, "deploy=%d\n", networkPreview);
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
		GeneralOptsDialogProc, "General");
	generalOpts = rollup->GetPanelDlg(index);

	index = rollup->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_AWD_SCENE_OPTS), 
		SceneOptsDialogProc, "Scene & geometry", 0, APPENDROLL_CLOSED);
	sceneOpts = rollup->GetPanelDlg(index);
	
	index = rollup->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_AWD_MTL_OPTS), 
		MtlOptsDialogProc, "Materials", 0, APPENDROLL_CLOSED);
	mtlOpts = rollup->GetPanelDlg(index);
	
	index = rollup->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_AWD_ANIM_OPTS), 
		AnimOptsDialogProc, "Animation", 0, APPENDROLL_CLOSED);
	animOpts = rollup->GetPanelDlg(index);

	//index = rollup->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_AWD_VIEWER_OPTS), 
	//	ViewerOptsDialogProc, "Flash viewer", 0, APPENDROLL_CLOSED);
	//viewerOpts = rollup->GetPanelDlg(index);

	// Find the correct option for textures
	int texOption = IDC_TEX_FULLPATH;
	if (forceBasenameTextures) texOption = IDC_TEX_BASENAME;
	else if (embedTextures) texOption = IDC_TEX_EMBED;

	// Set default (or loaded if cfg file existed) options
	ComboBox_SetCurSel(GetDlgItem(generalOpts, IDC_COMP_COMBO), compression);
	SetCheckBox(generalOpts, IDC_INC_ATTR, exportAttributes);
	Edit_SetText(GetDlgItem(generalOpts, IDC_ATTRNS_TEXT), attributeNamespace);
	SetCheckBox(sceneOpts, IDC_INC_SCENE, exportScene);
	SetCheckBox(sceneOpts, IDC_INC_GEOM, exportGeometry);
	SetCheckBox(sceneOpts, IDC_INC_UVS, exportUVs);
	SetCheckBox(sceneOpts, IDC_INC_NORMALS, exportNormals);
	SetCheckBox(sceneOpts, IDC_INC_SKIN, exportSkin);
	SetCheckBox(mtlOpts, IDC_INC_MTL, exportMaterials);
	SetCheckBox(mtlOpts, texOption, true);
	SetCheckBox(mtlOpts, IDC_TEX_COPY, copyTextures);
	SetCheckBox(animOpts, IDC_INC_SKEL, exportSkeletons);
	SetCheckBox(animOpts, IDC_INC_SKELANIM, exportSkelAnim);
	Edit_SetText(GetDlgItem(animOpts, IDC_SEQ_TXT), sequencesTxtPath);
	//SetCheckBox(viewerOpts, IDC_SWF_ENABLE, createPreview);
	//SetCheckBox(viewerOpts, IDC_SWF_LAUNCH, launchPreview);
	//SetCheckBox(viewerOpts, IDC_SWFSB_NETWORK, networkPreview);
	//SetCheckBox(viewerOpts, IDC_SWFSB_LOCAL, !networkPreview);

	// Flip color channels
	AColor col;
	col.r = GetBValue(previewBackgroundColor) / 255.0;
	col.g = GetGValue(previewBackgroundColor) / 255.0;
	col.b = GetRValue(previewBackgroundColor) / 255.0;
	//GetIColorSwatch(GetDlgItem(viewerOpts, IDC_SWF_COLOR))->SetAColor(col, FALSE);
		

	// Force redraw all panels
	RedrawGeneralOpts(0);
	RedrawSceneOpts(0);
	RedrawMtlOpts(0);
	RedrawAnimOpts(0);
	//RedrawViewerOpts(0);

	rollup->Show();
}


void MaxAWDExporterOpts::SaveOptions(void)
{
	int len;

	// General options
	compression = ComboBox_GetCurSel(GetDlgItem(generalOpts, IDC_COMP_COMBO));
	exportAttributes = (IsDlgButtonChecked(generalOpts, IDC_INC_ATTR) == BST_CHECKED);
	
	len = Edit_GetTextLength(GetDlgItem(generalOpts, IDC_ATTRNS_TEXT));
	attributeNamespace = (char*)realloc(attributeNamespace, len+1);
	Edit_GetText(GetDlgItem(generalOpts, IDC_ATTRNS_TEXT), attributeNamespace, len+1);

	// Scene & geometry options
	exportScene = (IsDlgButtonChecked(sceneOpts, IDC_INC_SCENE) == BST_CHECKED);
	exportGeometry = (IsDlgButtonChecked(sceneOpts, IDC_INC_GEOM) == BST_CHECKED);
	exportUVs = (IsDlgButtonChecked(sceneOpts, IDC_INC_UVS) == BST_CHECKED);
	exportNormals = (IsDlgButtonChecked(sceneOpts, IDC_INC_NORMALS) == BST_CHECKED);
	exportSkin = (IsDlgButtonChecked(sceneOpts, IDC_INC_SKIN) == BST_CHECKED);
	jointsPerVertex = GetISpinner(GetDlgItem(sceneOpts,IDC_JPV_SPINNER))->GetIVal();
					
	// Material options
	exportMaterials = (IsDlgButtonChecked(mtlOpts, IDC_INC_MTL) == BST_CHECKED);
	forceBasenameTextures = (IsDlgButtonChecked(mtlOpts, IDC_TEX_BASENAME) == BST_CHECKED);
	copyTextures = (IsDlgButtonChecked(mtlOpts, IDC_TEX_COPY) == BST_CHECKED);
	embedTextures = (IsDlgButtonChecked(mtlOpts, IDC_TEX_EMBED) == BST_CHECKED);

	// Animation options
	exportSkeletons = (IsDlgButtonChecked(animOpts, IDC_INC_SKEL) == BST_CHECKED);
	exportSkelAnim = (IsDlgButtonChecked(animOpts, IDC_INC_SKELANIM) == BST_CHECKED);
	
	len = Edit_GetTextLength(GetDlgItem(animOpts, IDC_SEQ_TXT));
	sequencesTxtPath = (char*)realloc(sequencesTxtPath, len+1);
	Edit_GetText(GetDlgItem(animOpts, IDC_SEQ_TXT), sequencesTxtPath, len+1);

	// Preview options
	//createPreview = (IsDlgButtonChecked(viewerOpts, IDC_SWF_ENABLE) == BST_CHECKED);
	//launchPreview = (IsDlgButtonChecked(viewerOpts, IDC_SWF_LAUNCH) == BST_CHECKED);
	//networkPreview = (IsDlgButtonChecked(viewerOpts, IDC_SWFSB_NETWORK) == BST_CHECKED);

	//int col = GetIColorSwatch(GetDlgItem(viewerOpts, IDC_SWF_COLOR))->GetColor();
	//previewBackgroundColor = (GetRValue(col)<<16) | (GetGValue(col)<<8) | GetBValue(col);

	WriteConfigFile();
}



INT_PTR CALLBACK MaxAWDExporterOpts::GeneralOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	bool enabled;

	switch (message) {
		case WM_INITDIALOG:
			ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMP_COMBO), "None");
			ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMP_COMBO), "GZIP");
			// TODO: Re-enable once Away3D can actually read LZMA compressed files.
			//ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMP_COMBO), "LZMA");
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
			spinner = GetISpinner(GetDlgItem(hWnd,IDC_JPV_SPINNER));
			spinner->SetLimits(1, 6, FALSE);
			spinner->SetValue(INSTANCE->jointsPerVertex, TRUE);
			spinner->LinkToEdit(GetDlgItem(hWnd,IDC_JPV_EDIT),EDITTYPE_INT);
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
		GetISpinner(GetDlgItem(sceneOpts,IDC_JPV_SPINNER))->Enable(enabled);
		GetICustEdit(GetDlgItem(sceneOpts,IDC_JPV_EDIT))->Enable(enabled);
		Static_Enable(GetDlgItem(sceneOpts,IDC_JPV_STATIC), enabled);
		ret = true;
	}

	if (force || (HWND)lParam == GetDlgItem(sceneOpts,IDC_INC_SKIN)) {
		bool enabled = ((IsDlgButtonChecked(sceneOpts, IDC_INC_GEOM) == BST_CHECKED)
			&& (IsDlgButtonChecked(sceneOpts, IDC_INC_SKIN) == BST_CHECKED));
		GetISpinner(GetDlgItem(sceneOpts,IDC_JPV_SPINNER))->Enable(enabled);
		GetICustEdit(GetDlgItem(sceneOpts, IDC_JPV_EDIT))->Enable(enabled);
		Static_Enable(GetDlgItem(sceneOpts,IDC_JPV_STATIC), enabled);
		ret = true;
	}

	return ret;
}


INT_PTR CALLBACK MaxAWDExporterOpts::MtlOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch (message) {
		case WM_COMMAND:
			return INSTANCE->RedrawMtlOpts(lParam);
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
		Button_Enable(GetDlgItem(mtlOpts,IDC_TEX_FULLPATH), enabled);
		Button_Enable(GetDlgItem(mtlOpts,IDC_TEX_BASENAME), enabled);
		Button_Enable(GetDlgItem(mtlOpts,IDC_TEX_EMBED), enabled);
	}
			
	// IDC_TEX_COPY should only be enabled if both the IDC_INC_MTL checkbox
	// and the IDC_TEX_BASENAME radio button are checked.
	enabled = ((IsDlgButtonChecked(mtlOpts, IDC_INC_MTL)==BST_CHECKED) 
		&& (IsDlgButtonChecked(mtlOpts, IDC_TEX_BASENAME)==BST_CHECKED));
	Button_Enable(GetDlgItem(mtlOpts,IDC_TEX_COPY), enabled);

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
		bool enabled = (IsDlgButtonChecked(animOpts, IDC_INC_SKELANIM) == BST_CHECKED);
		Edit_Enable(GetDlgItem(animOpts,IDC_SEQ_TXT), enabled);
		Static_Enable(GetDlgItem(animOpts,IDC_SEQ_STATIC1), enabled);
		Static_Enable(GetDlgItem(animOpts,IDC_SEQ_STATIC2), enabled);
		return true;
	}

	return false;
}


//INT_PTR CALLBACK MaxAWDExporterOpts::ViewerOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
//{
//	bool enabled;
//
//	switch (message) {
//		case WM_COMMAND:
//			return INSTANCE->RedrawViewerOpts(lParam);
//			break;
//	}
//
//	return FALSE;
//}

//bool MaxAWDExporterOpts::RedrawViewerOpts(LPARAM lParam)
//{
//	bool force = (lParam == 0);
//
//	if (force || (HWND)lParam == GetDlgItem(viewerOpts,IDC_SWF_ENABLE)) {
//		bool enabled = (IsDlgButtonChecked(viewerOpts,IDC_SWF_ENABLE) == BST_CHECKED);
//		Button_Enable(GetDlgItem(viewerOpts,IDC_SWF_LAUNCH), enabled);
//		Button_Enable(GetDlgItem(viewerOpts,IDC_SWFSB_LOCAL), enabled);
//		Button_Enable(GetDlgItem(viewerOpts,IDC_SWFSB_NETWORK), enabled);
//		Static_Enable(GetDlgItem(viewerOpts,IDC_SWFSB_STATIC1), enabled);
//		Static_Enable(GetDlgItem(viewerOpts,IDC_SWFSB_STATIC2), enabled);
//		Static_Enable(GetDlgItem(viewerOpts,IDC_SWF_COL_STATIC), enabled);
//		GetIColorSwatch(GetDlgItem(viewerOpts,IDC_SWF_COLOR))->Enable(enabled);
//		return true;
//	}
//
//	return false;
//}



int MaxAWDExporterOpts::Compression(void)
{
	return compression;
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

int MaxAWDExporterOpts::JointsPerVertex(void)
{
	return jointsPerVertex;
}

bool MaxAWDExporterOpts::ExportMaterials(void)
{
	return exportMaterials;
}

bool MaxAWDExporterOpts::ForceBasenameTextures(void)
{
	return forceBasenameTextures;
}

bool MaxAWDExporterOpts::CopyTextures(void)
{
	return copyTextures;
}

bool MaxAWDExporterOpts::EmbedTextures(void)
{
	return embedTextures;
}

bool MaxAWDExporterOpts::ExportSkeletons(void)
{
	return exportSkeletons;
}

bool MaxAWDExporterOpts::ExportSkelAnim(void)
{
	return exportSkelAnim;
}

char *MaxAWDExporterOpts::SequencesTxtPath(void)
{
	return sequencesTxtPath;
}
//
//bool MaxAWDExporterOpts::CreatePreview(void)
//{
//	return createPreview;
//}

//bool MaxAWDExporterOpts::LaunchPreview(void)
//{
//	return launchPreview;
//}

bool MaxAWDExporterOpts::PreviewForDeployment(void)
{
	return networkPreview;
}

int MaxAWDExporterOpts::PreviewBackgroundColor(void)
{
	return previewBackgroundColor;
}
