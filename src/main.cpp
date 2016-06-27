#include "windows.h"

BOOL	InitPMUInstance(HINSTANCE, int);

int WinMain (HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine, int nShowCmd ){
	MSG msg;
	InitPMUInstance(hInstance,nShowCmd);
	/*SendMessage(FindWindow(L"ra_agent",L"ra_agent"),WM_COMMAND,0,0);
	SendMessage(FindWindow(L"ra_agent",L"ra_agent"),WM_USER+1,0,0);
	SendMessage(FindWindow(L"ra_agent",L"ra_agent"),WM_USER+2,0,0);*/
	while (GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}