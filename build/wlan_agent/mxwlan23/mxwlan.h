// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MXWLAN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MXWLAN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MXWLAN_EXPORTS
#define MXWLAN_API __declspec(dllexport)
#else
#define MXWLAN_API __declspec(dllimport)
#endif

// This class is exported from the mxwlan.dll
class MXWLAN_API Cmxwlan {
public:
	Cmxwlan(void);
	// TODO: add your methods here.
};

extern MXWLAN_API int nmxwlan;

MXWLAN_API int fnmxwlan(void);
