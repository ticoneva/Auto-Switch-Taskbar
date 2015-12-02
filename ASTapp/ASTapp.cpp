// ASTapp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ASTapp.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
BOOL canQuit = false;
UINT ASTM_QUIT;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ASTAPP, szWindowClass, MAX_LOADSTRING);

	//Check for existing running instance
	//The name for the mutex is not GUID right now. Change it in a future version.
	if (duplicateExist(_T("AutoSwitchTaskbar-3tg2g23g81ksf12tfgb2b"))) {
		//Another instance exist. Show it and quit self
		HWND anotherMe = FindWindow(szWindowClass, 0);
		ShowWindow(anotherMe, SW_SHOW);
		return FALSE;
	} else {
		//New instance

		MyRegisterClass(hInstance);
		ASTM_QUIT = RegisterWindowMessage(TEXT("ASTM_quit"));

		// Perform application initialization:
		if (!InitInstance(hInstance, nCmdShow))
		{
			return FALSE;
		}

		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ASTAPP));

		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return (int)msg.wParam;
	}
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASTAPP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ASTAPP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 300, 420, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   SetMenu(hWnd, NULL);

   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent, ItemIndex;
	PAINTSTRUCT ps;
	HDC hdc;
	static HBRUSH hBrush;

	if (message == ASTM_QUIT) {
		//Quit message from installer
		canQuit = true;
		PostMessage(hWnd, WM_CLOSE, 0, 0);
	}
	else {

		switch (message)
		{
		case WM_POWERBROADCAST:
			if (wParam == PBT_APMRESUMEAUTOMATIC) {
				checkTabletMode(0);
			}
			else {
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		case WM_SETTINGCHANGE:
			onSettingsChange(lParam);
			break;
		case WM_DISPLAYCHANGE:
			onRotation();
			break;
		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);

			// Parse the menu selections:
			switch (wmId)
			{
			case 1:
			case 2:
			case 3:
			case 4:
				//These are the checkboxes events
				chkBoxChange(hWnd, wmId);
				break;

			case 5:
				//Delay-time combobox
				ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
					(WPARAM)0, (LPARAM)0);
				cbChangeDelay(ItemIndex);
				break;

			case 6:
				//Minimize button
				ShowWindow(hWnd, SW_HIDE);
				break;

			case 7:
				//Shutdown button
				canQuit = true;
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			EndPaint(hWnd, &ps);
			break;
		case WM_SYSCOMMAND:
			if (wParam == SC_CLOSE)
				if (canQuit) {
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
				else {
					ShowWindow(hWnd, SW_HIDE);
				}
				break;
		case WM_DESTROY:
			restoreTaskbarState();
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			//Create GUI checkboxes
			createChkBoxes(hWnd, lParam);
			//Initialize AST features
			initAST(hWnd);
			break;
		case WM_CTLCOLORSTATIC:
			//Set check boxes background to white
			hdc = (HDC)wParam;
			SetBkMode(hdc, TRANSPARENT);
			hBrush = CreateSolidBrush(RGB(255, 255, 255));
			return (LRESULT)hBrush;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
