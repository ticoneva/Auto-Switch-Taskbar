#include "stdafx.h"
#include "AST_handlers.h"

static int AppBarState = 0;
static int queryDelay = 200;
static int hideSettings[4];

void initAST(HWND hWnd) {

	LONG retVal1, retVal2, retVal3;
	HKEY hKey;
	DWORD dwDisp, tbMode;
	BYTE i;
	wchar_t buffer[2];

	//Get the default Taskbar state
	getTaskbarState();

	//Read settings from registry
	retVal1 = RegCreateKeyEx(HKEY_CURRENT_USER,
		TEXT("SOFTWARE\\ASTapp"),
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, &dwDisp);

	//Orientaton and tablet mode
	for (i = 0; i < 4; i++) {
		_itow_s(i+1, buffer, 10);
		retVal2 = readDWORDFromReg(hKey, buffer, (DWORD*)&tbMode);
		if (retVal2 == ERROR_SUCCESS) {
			hideSettings[i] = tbMode;
		} else if (i==0) {
			hideSettings[i] = ABS_AUTOHIDE;
		}
		else {
			hideSettings[i] = AppBarState;
		}
		changeChkBox(hWnd, i+1, (hideSettings[i] == ABS_AUTOHIDE ? true : false), false);
	}

	//Query delay
	retVal2 = readDWORDFromReg(hKey, TEXT("queryDelay"), (DWORD*)&tbMode);
	if (retVal2 == ERROR_SUCCESS) {
		queryDelay = tbMode;
	}

	retVal3 = RegCloseKey(hKey);
}

void setHideSetting(int id, DWORD value) {
	hideSettings[id] = value;
}

int getMyAppBarState() {
	return AppBarState;
}

void getTaskbarState() {
	APPBARDATA tbData = APPBARDATA();
	tbData.cbSize = sizeof(tbData);
	tbData.hWnd = FindWindow(TEXT("System_TrayWnd"), 0);
	SHAppBarMessage(ABM_GETSTATE, &tbData);
	AppBarState = tbData.lParam;

	//For future use (display existing settings in GUI)
	//char buffer[2];
	//_itoa_s(AppBarState, buffer, 2, 10);
}

void SetTaskbarState(int option)
{
	APPBARDATA tbData = APPBARDATA();
	tbData.cbSize = sizeof(tbData);
	tbData.hWnd = FindWindow(TEXT("System_TrayWnd"), 0);
	tbData.lParam = option;
	SHAppBarMessage(ABM_SETSTATE, &tbData);
}

void restoreTaskbarState() {
	SetTaskbarState(AppBarState);
}

bool IsScreenLandscape() {
	return (GetSystemMetrics(SM_CXFULLSCREEN) > GetSystemMetrics(SM_CYFULLSCREEN) ? true : false);
}

DWORD readDWORDFromReg(HKEY hKey, LPTSTR subkey, DWORD * vp) {
	DWORD sz = sizeof(*vp);
	return RegQueryValueEx(hKey, subkey, 0, 0, (BYTE*)vp, &sz);
}

LONG writeDWORDToReg(HKEY inKey, LPTSTR valname, DWORD vp) {

	LONG retVal1, retVal2, retVal3;
	HKEY hKey;
	DWORD dwDisp;

	retVal1 = RegCreateKeyEx(inKey, TEXT("SOFTWARE\\ASTapp"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);
	retVal2 = RegSetValueEx(hKey, valname, 0, REG_DWORD, (const BYTE*)&vp, sizeof(vp));
	retVal3 = RegCloseKey(hKey);

	return max(max(retVal1, retVal2), retVal3);

}

void checkTabletMode(int delay) {
	LONG retVal1, retVal2, retVal3;
	HKEY hKey;
	DWORD dwDisp,tabletMode;

	//Wait a while
	Sleep(delay);

	changeStatusText(TEXT("Reading registry."));

	retVal1 = RegCreateKeyEx(HKEY_CURRENT_USER, 
				TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\ImmersiveShell"), 
				0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, &dwDisp);
	retVal2 = readDWORDFromReg(hKey, TEXT("TabletMode"), (DWORD*)&tabletMode);
	retVal3 = RegCloseKey(hKey);

	if (retVal2 == ERROR_SUCCESS) {
	//Proceed if we have retrieved the tablet mode value

		changeStatusText(TEXT("Tablet Mode data retrieved."));

		if (tabletMode == 1) {
			//Tablet mode
			if (IsScreenLandscape()) {
				SetTaskbarState(hideSettings[0]);
				if (hideSettings[0] == ABS_AUTOHIDE) {
					changeStatusText(TEXT("Tablet landscape - Hide"));
				}
				else {
					changeStatusText(TEXT("Tablet landscape - Show"));
				}
			}
			else {
				SetTaskbarState(hideSettings[1]);
				if (hideSettings[1] == ABS_AUTOHIDE) {
					changeStatusText(TEXT("Tablet portrait - Hide"));
					}
				else {
					changeStatusText(TEXT("Tablet portrait - Show"));
				}
			}
		}
		else {
			//Desktop mode
			if (IsScreenLandscape()) {
				SetTaskbarState(hideSettings[2]);
				if (hideSettings[2] == ABS_AUTOHIDE) {
					changeStatusText(TEXT("Desktop landscape - Hide"));
				}
				else {
					changeStatusText(TEXT("Desktop landscape - Show"));
				}
			}
			else {
				SetTaskbarState(hideSettings[3]);
				if (hideSettings[3] == ABS_AUTOHIDE) {
					changeStatusText(TEXT("Desktop portrait - Hide"));
				}
				else {
					changeStatusText(TEXT("Desktop portrait - Show"));
				}
			}
		}
	}
	
}

void OnSettingsChange(LPARAM lParam)
{
	if (lParam != 0)
	{
		PCWSTR message = reinterpret_cast<PCWSTR>(lParam);
		if (CSTR_EQUAL == CompareStringOrdinal(message, -1, TEXT("UserInteractionMode"), -1, TRUE))
		{
			//Check registry to see if we're in tablet mode
			//It can take up to 200ms for the registry entry on tablet mode to change in atom tablets
			checkTabletMode(queryDelay);
		}
	}
}