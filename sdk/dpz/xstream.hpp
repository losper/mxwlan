#ifndef __DPZ__XSTREAM__HPP__ 
#define __DPZ__XSTREAM__HPP__

#include "xstring.hpp"

namespace dpz{
	class xstream{
	public:
		xstream(xstring filename,xstring mode){
			filename_=filename;
			mode_=mode;
			hfile=NULL;//fopen(filename.c_str(),mode_.c_str());
		}
		xstream(){
			filename_="";
			mode_="a+";
		}
		int bind(xstring filename){
			filename_=filename;
			hfile=fopen(filename.c_str(),mode_.c_str());
			return 0;
		}
		int append(void* buf,unsigned long size){
			if (!hfile)
			{
				hfile=fopen(filename_.c_str(),mode_.c_str());
			}
			if (hfile && !fseek(hfile,0,SEEK_END))
			{
				return fwrite(buf,size,1,hfile);
			}else{
				if (hfile)
				{
					fclose(hfile);
					hfile=NULL;
				}
				return 0;
			}
			
			/*DWORD dwSize=0;
			SetFilePointer(hfile,0,0,FILE_END);
			return WriteFile(hfile,buf,size,&dwSize,NULL);*/
		}
		int write(void* buf,unsigned long size){
			if (!hfile)
			{
				hfile=fopen(filename_.c_str(),mode_.c_str());
			}
			if (hfile && !fseek(hfile,0,SEEK_CUR))
			{
				return fwrite(buf,size,1,hfile);
			}else{
				if (hfile)
				{
					fclose(hfile);
					hfile=NULL;
				}
				return 0;
			}
			/*DWORD dwSize=0;
			return WriteFile(hfile,buf,size,&dwSize,NULL);*/
		}
		~xstream(){
			fclose(hfile);
		}
	private:
		FILE* hfile;
		xstring filename_;
		xstring mode_;
	};

}

#endif