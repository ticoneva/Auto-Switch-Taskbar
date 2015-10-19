#pragma once

#include "AST_gui.h"

void initAST(HWND hWnd);
void changeDelay(int delay,BOOL saveToReg = TRUE);
void changeComboSel(BYTE index);
void setHideSetting(int id, DWORD value);
int getMyAppBarState();
void getTaskbarState();
void restoreTaskbarState();
void OnSettingsChange(LPARAM lParam);
void checkTabletMode(int delay = 0);
DWORD readDWORDFromReg(HKEY hKey, LPTSTR subkey, DWORD * vp);
LONG writeDWORDToReg(HKEY inKey, LPTSTR valname, DWORD vp);

