#ifndef _DPZ_MSGQUEUE_H_
#define _DPZ_MSGQUEUE_H_

#include "windows.h"
namespace dpz{
	template<typename T>
	class msg{
	public:
		msg():
		  _len(0),
		  _exit(0),
		  _hq(INVALID_HANDLE_VALUE){
			_name[0]=0;
			_len=0;
			_exit=0;
		}
		int bind(LPCTSTR name,DWORD len,DWORD max=0){
			if(_hq==INVALID_HANDLE_VALUE){
				_tcscpy(_name,name);
				_len=len;
				_exit=0;
				_work.bind(this);
				_hq=connect(name,len,1,max);
				CloseHandle(CreateThread(NULL,NULL,msg_work,this,NULL,NULL));
				return 0;
			}
			return -1;
		}
		msg(LPCTSTR name,DWORD len,DWORD max=0){
			_tcscpy(_name,name);
			_len=len;
			_exit=0;
			_work.bind(this);
			_hq=connect(name,len,1,max);
			CloseHandle(CreateThread(NULL,NULL,msg_work,this,NULL,NULL));
		}
		~msg(){
			_exit=0;
			this->send(_name,"",1);
			printf("msg:????");
		}
		T& getwork(){
			return _work;
		}
		static int send(LPCTSTR name,void* data,unsigned int len,DWORD max=0){
			HANDLE hq=connect(name,len,0,max);
			//printf("[PMU]send msg start:%d\r\n",GetTickCount());
			if(hq){
				if(WriteMsgQueue(hq,data,len,0,0)){
					CloseMsgQueue(hq);
				}else{
					CloseMsgQueue(hq);
					printf("[PMU]send msg fails :%d\r\n",GetLastError());
					return -2;
				}
			}else{
				printf("[PMU]send msg fails :%d\r\n",GetLastError());
				return -1;
			}
			//printf("[PMU]send msg end:%d\r\n",GetTickCount());
			return 0;
		}
		static DWORD msg_work(LPVOID lp){
			msg* mg=(msg*)lp;
			T& work=mg->_work;
			while(1){
				if(!mg->exit()){
					work.signaled();	
				}else{
					break;
				}
			}
			return 0;
		}
		int read(LPVOID data,DWORD len,DWORD dwTimeout){
			DWORD dwRead=0,dwFlag=0;
			if(!ReadMsgQueue(_hq,data,len,&dwRead,dwTimeout,&dwFlag)){
				return -1;
			}else{
				return dwRead;
			}
		}
		static HANDLE connect(LPCTSTR name,DWORD len,BOOL flag,DWORD max){
			MSGQUEUEOPTIONS options;  
			options.dwSize = sizeof(options);  
			options.dwFlags = MSGQUEUE_NOPRECOMMIT|MSGQUEUE_ALLOW_BROKEN;  
			options.dwMaxMessages = max;  
			options.cbMaxMessage = len;  
			options.bReadAccess = flag ? TRUE : FALSE;  
			return CreateMsgQueue(name,&options);
			
		}
		LPCTSTR name(){
			return _name;
		}
	private:
		int exit(){
			return _exit;
		}
	private:
		TCHAR _name[100];
		DWORD _len;
		HANDLE _hq;
		int _exit;
		T _work;
	};
typedef msg<int> mgr;
}
#endif