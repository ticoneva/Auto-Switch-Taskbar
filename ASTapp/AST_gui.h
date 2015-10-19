#pragma once

#include "AST_handlers.h"

bool duplicateExist(LPCWSTR lpName);
void cbChangeDelay(BYTE index);
void changeComboSel(BYTE index);
void createChkBoxes(HWND hWnd, LPARAM lParam);
void changeStatusText(LPCTSTR lpString);
void chkBoxChange(HWND hWnd, int wmId);
void changeChkBox(HWND hWnd, int wmId, BOOL checked, BOOL writeToReg = true);