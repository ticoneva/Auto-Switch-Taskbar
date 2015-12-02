#include "stdafx.h"
#include "AST_gui.h"

HWND hWndDetDelay, hWndStatusText;
const BYTE DelayCount = 10;
TCHAR DelayText[DelayCount][15] =
{
	TEXT("Reverse Mode"), TEXT("0 ms"), TEXT("200 ms"), TEXT("300 ms"),
	TEXT("400 ms"), TEXT("500 ms"), TEXT("600 ms"), TEXT("800 ms"),
	TEXT("900 ms"), TEXT("1 s")
};

bool duplicateExist(LPCWSTR lpName) {
	bool AlreadyRunning;


	HANDLE hMutexOneInstance = CreateMutex(NULL, TRUE,lpName);

	AlreadyRunning = (GetLastError() == ERROR_ALREADY_EXISTS);

	if (hMutexOneInstance != NULL)
	{
		ReleaseMutex(hMutexOneInstance);
	}

	return AlreadyRunning;

}

void cbChangeDelay(BYTE index) {
	changeDelay(index);
}

void changeComboSel(BYTE index) {
	// Send the CB_SETCURSEL message to display an item in the selection field  
	SendMessage(hWndDetDelay, CB_SETCURSEL, (WPARAM)index, (LPARAM)0);
}

void addContentToCombo(HWND hWndComboBox) {

	TCHAR A[16];
	int  k = 0;

	memset(&A, 0, sizeof(A));
	for (k = 0; k <= DelayCount-1; k += 1)
	{
		wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)DelayText[k]);

		// Add string to combobox.
		SendMessage(hWndDetDelay, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
	}

	changeComboSel(0);
}

void createChkBoxes(HWND hWnd,LPARAM lParam) {

	//Check boxes
	CreateWindow(TEXT("Static"), TEXT("Auto-hide the taskbar when:"),
		WS_CHILD | WS_VISIBLE, 
		20, 10, 400, 35, 
		hWnd, 0, 0, 0);
	CreateWindow(TEXT("button"), TEXT("in landscape tablet mode."),
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		30, 40, 250, 35,
		hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	CreateWindow(TEXT("button"), TEXT("in portrait tablet mode."),
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		30, 80, 250, 35,
		hWnd, (HMENU)2, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	CreateWindow(TEXT("button"), TEXT("in landscape desktop mode."),
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		30, 120, 250, 35,
		hWnd, (HMENU)3, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	CreateWindow(TEXT("button"), TEXT("in portrait desktop mode."),
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		30, 160, 250, 35,
		hWnd, (HMENU)4, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	//Dropdown menu
	CreateWindow(TEXT("Static"), TEXT("Tablet Mode detection delay:"),
		WS_CHILD | WS_VISIBLE,
		20, 220, 400, 35,
		hWnd, 0, 0, 0);
	hWndDetDelay = CreateWindow(L"ComboBox", TEXT(""),
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		30, 250, 150, 350, 
		hWnd, (HMENU)5, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	addContentToCombo(hWndDetDelay);

	//Buttons
	CreateWindow(TEXT("button"), TEXT("Minimize"),
		WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		30, 300, 100, 35,
		hWnd, (HMENU)6, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	CreateWindow(TEXT("button"), TEXT("Shutdown"),
		WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		140, 300, 100, 35,
		hWnd, (HMENU)7, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	//Status text
	hWndStatusText = CreateWindow(TEXT("Static"), TEXT("Status is displayed here."),
				WS_CHILD | WS_VISIBLE,
				30, 350, 400, 300,
				hWnd, 0, 0, 0);

	CheckDlgButton(hWnd, 1, BST_CHECKED);
}

void changeStatusText(LPCTSTR lpString) {
	SetWindowText(hWndStatusText, lpString);
}

void chkBoxChange(HWND hWnd, int wmId) {
	BOOL checked;
	checked = IsDlgButtonChecked(hWnd, wmId);

	changeStatusText(TEXT("Changing settings."));

	if (checked) {
		changeChkBox(hWnd, wmId, FALSE, TRUE);
	}
	else {
		changeChkBox(hWnd, wmId, TRUE, TRUE);
	}
}

void changeChkBox(HWND hWnd, int wmId, BOOL checked, BOOL writeToReg) {
	
	int myVal;
	wchar_t buffer[2];
	_itow_s(wmId, buffer, 10);

	//Save settings change
	if (checked) {
		myVal = ABS_AUTOHIDE;
		CheckDlgButton(hWnd, wmId, BST_CHECKED);
	}
	else {
		myVal = 0;
		CheckDlgButton(hWnd, wmId, BST_UNCHECKED);
	}

	setHideSetting(wmId-1, myVal);
	if (writeToReg) {
		if (ERROR_SUCCESS != writeDWORDToReg(HKEY_CURRENT_USER, buffer, myVal)) {
			changeStatusText(TEXT("Failed to change settings."));
		}
		else {
			changeStatusText(TEXT("Settings saved."));
		}
	}

	checkTabletMode();
}