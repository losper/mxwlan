#include "windows.h"
//#include "wifiwork.hpp"
HINSTANCE			g_hPMUInst;			// current instance
ATOM			MyPMURegisterClass(HINSTANCE, LPTSTR);
BOOL			InitPMUInstance(HINSTANCE, int);
LRESULT CALLBACK	WndPMUProc(HWND, UINT, WPARAM, LPARAM);


//HWND				agent;
//
//  FUNCTION: MyPMURegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
ATOM MyPMURegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndPMUProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, RT_ICON);
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
}

//
//   FUNCTION: InitPMUInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitPMUInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	TCHAR szTitle[100];		// title bar text
	TCHAR szWindowClass[100];	// main window class name

	g_hPMUInst = hInstance; // Store instance handle in our global variable

	_tcscpy(szWindowClass,L"ra_agent");
	_tcscpy(szTitle,L"ra_agent");

	if (!MyPMURegisterClass(hInstance, szWindowClass))
	{
		return FALSE;
	}

	hWnd = CreateWindow(szWindowClass, szTitle, WS_DLGFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 400, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}
	ShowWindow(hWnd,SW_SHOW);
	return TRUE;
}


//
//  FUNCTION: WndPMUProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndPMUProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;


	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:	
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_CREATE:
		{
			SetWindowPos(hWnd,NULL,0,0,0,0,SWP_HIDEWINDOW|SWP_NOSIZE);//}
		}
		break;
	case WM_LBUTTONUP:
		{

			//wifihelper::get_inst().connect(L"ÁõÎÄ¾ü",L"12345678");
			
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		// TODO: Add any drawing code here...

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		
		PostQuitMessage(0);
		break;
	case WM_COPYDATA:
		
		break;
	default:
		
		return DefWindowProc(hWnd, message, wParam, lParam);
		
	}
	return 0;
}
