// mxwlan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "mxwlan.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

// This is an example of an exported variable
MXWLAN_API int nmxwlan=0;

// This is an example of an exported function.
MXWLAN_API int fnmxwlan(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see mxwlan.h for the class definition
Cmxwlan::Cmxwlan()
{ 
	return; 
}
