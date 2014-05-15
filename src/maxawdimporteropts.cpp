#include "maxawdimporter.h"
#include <custcont.h>

MaxAWDImporterOpts::MaxAWDImporterOpts(void)
{
	// Store pointer to single instance
	INSTANCE = this;

	scale = 1.0;

	// Override defaults using config file, if any exists
	ReadConfigFile();
}

// Static members
MaxAWDImporterOpts *MaxAWDImporterOpts::INSTANCE;// = NULL;
HWND MaxAWDImporterOpts::generalOpts = NULL;

MaxAWDImporterOpts::~MaxAWDImporterOpts(void)
{
}

FILE *MaxAWDImporterOpts::OpenConfigFile(const char *mode)
{
	char buf[1024];

	Interface *ip = GetCOREInterface();
	char *cdir = W2A(ip->GetDir(APP_PLUGCFG_DIR));
	_makepath_s(buf, 1024, NULL, cdir, "MAXAWD", ".CFG");
	free(cdir);
	return fopen(buf, mode);
}

void MaxAWDImporterOpts::ReadConfigFile(void)
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

		else if (ATTREQ(key, "scl")) {
			scale = strtod(val, NULL);
		}
	}

	fclose(cfg);
}

void MaxAWDImporterOpts::WriteConfigFile(void)
{
	// Open config file or abort.
	FILE *cfg = OpenConfigFile("wb");
	if (!cfg) return;

	fprintf(cfg, "scl=%f\n", scale);

	fclose(cfg);
}

bool MaxAWDImporterOpts::ShowDialog(void)
{
	int ret = DialogBoxParam(hInstance,
				MAKEINTRESOURCE(IDD_AWD_OPTIONS),
				GetActiveWindow(),
				DialogProc,
				0);

	return (ret == IDOK);
}

INT_PTR CALLBACK MaxAWDImporterOpts::DialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
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

void MaxAWDImporterOpts::InitDialog(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	int index;

	HWND rh = GetDlgItem(hWnd, IDC_ROLLUP);
	IRollupWindow *rollup = GetIRollup(rh);

	index = rollup->AppendRollup(hInstance, MAKEINTRESOURCE(IDD_AWD_GENERAL_OPTS),
		GeneralOptsDialogProc, TEXT("General"));
	generalOpts = rollup->GetPanelDlg(index);

	// Flip color channels
	//AColor col;
	//col.r = GetBValue(previewBackgroundColor) / 255.0;
	//col.g = GetGValue(previewBackgroundColor) / 255.0;
	//col.b = GetRValue(previewBackgroundColor) / 255.0;
	//GetIColorSwatch(GetDlgItem(viewerOpts, IDC_SWF_COLOR))->SetAColor(col, FALSE);

	// Force redraw all panels
	RedrawGeneralOpts(0);

	rollup->Show();
}

void MaxAWDImporterOpts::SaveOptions(void)
{
	int len;

	// General options
	scale = GetISpinner(GetDlgItem(generalOpts,IDC_SCALE_SPINNER))->GetFVal();

	WriteConfigFile();
}

INT_PTR CALLBACK MaxAWDImporterOpts::GeneralOptsDialogProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	ISpinnerControl *spinnerScale;
	bool enabled;

	switch (message) {
		case WM_INITDIALOG:
			spinnerScale = GetISpinner(GetDlgItem(hWnd,IDC_SCALE_SPINNER));
			spinnerScale->LinkToEdit(GetDlgItem(hWnd,IDC_SCALE_EDIT),EDITTYPE_FLOAT);
			spinnerScale->SetLimits(0.0001, 100000, FALSE);
			spinnerScale->SetValue(INSTANCE->scale, TRUE);
			// TODO: Re-enable once Away3D can actually read LZMA compressed files.
			//ComboBox_AddItemData(GetDlgItem(hWnd, IDC_COMP_COMBO), "LZMA");
			return TRUE;

		case WM_COMMAND:
			return INSTANCE->RedrawGeneralOpts(lParam);
			break;
	}

	return FALSE;
}

bool MaxAWDImporterOpts::RedrawGeneralOpts(LPARAM lParam)
{
	return false;
}

double MaxAWDImporterOpts::Scale(void)
{
	return scale;
}