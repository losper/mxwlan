#ifndef __DPZ__XFILE__HPP__ 
#define __DPZ__XFILE__HPP__

static wchar_t* MultiToUnicode(const char* str,unsigned int code,wchar_t* result,unsigned int len){
	unsigned int n = MultiByteToWideChar(code,0,str,-1,0,0);
	n>len?n=len:n;
	MultiByteToWideChar(code,0,str,-1,result,n);
	return result;
}

static char* UnicodeToMulti(wchar_t* str,unsigned int code,char* result,unsigned int len){
	unsigned int n = WideCharToMultiByte(code,0,str,-1,0,0,0,0);    
	n>len?n=len:n;
	WideCharToMultiByte(code,0,str,-1,result,n,0,0);
	return result;
}

static BOOL copyfile(const char* dst,const char* src,BOOL flag){
	wchar_t dstw[256],srcw[256];
	MultiToUnicode(dst,65001,dstw,256);
	MultiToUnicode(src,65001,srcw,256);
	return CopyFile(srcw,dstw,flag);
}


#endif