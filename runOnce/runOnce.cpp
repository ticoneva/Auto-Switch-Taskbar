// runOnce.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>

/*Work in progress
VOID startup(LPCTSTR lpApplicationName)
{
	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	PHANDLE phToken;
	WTSQueryUserToken(WTS_CURRENT_SESSION, phToken);

	CreateProcessAsUser(
		phToken,
		lpApplicationName,   // the path
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi           // Pointer to PROCESS_INFORMATION structure
		);
		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
*/

BOOL setShowCMD(HWND tHandle, WINDOWPLACEMENT * ptWP, UINT showCMD) {
	if (GetWindowPlacement(tHandle, ptWP)) {				//Get window placement info
		(*ptWP).showCmd = showCMD;							//set showCMD value
		return SetWindowPlacement(tHandle, ptWP);			//Set the new window placement info
	}
	return false;
}		

int main(int argc, char *argv[])
{
	UINT ASTM_QUIT = RegisterWindowMessage(TEXT("ASTM_quit"));
	
	if (argc > 1) {
		if (*argv[1] == 'i' || (strcmp(argv[1], "-i")==0)) {
			MessageBox(0, TEXT("You can now launch Auto-Switch Taskbar from the start menu. Auto-Switch Taskbar also runs automatically at login."), TEXT("Auto-Switch Taskbar"), MB_OK);
			/*
			//work in  progress
			char*  stheParameterFileName = argv[2];
			TCHAR szName[512];
			USES_CONVERSION;
			_tcscpy_s(szName, A2T(stheParameterFileName));
			startup((LPCTSTR)szName);
			*/
		}
		else if (*argv[1] == 'u' || (strcmp(argv[1],"-u")==0)) {
			//Uninstall
			HWND anotherMe = FindWindow(TEXT("AutoSwitchTaskbar"), 0); // TEXT("Auto-Switch Taskbar"));
			PostMessage(anotherMe, ASTM_QUIT, 0, 0);
		}
		else {
			//Unknown command
			MessageBox(0, TEXT("Unknown command."), TEXT("Auto-Switch Taskbar"), MB_OK);
		}
	}

	return 0;
}

