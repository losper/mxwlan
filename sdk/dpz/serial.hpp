#ifndef _PMU_SERIAL_H_
#define _PMU_SERIAL_H_

#include "windows.h"
template <typename T>
class serial{
public:
	serial(LPTSTR name,DWORD rate){
		this->_work.bind(this);
		_tcscpy(this->_name,name);
		this->_rate=rate;
		this->_inst=INVALID_HANDLE_VALUE;
		this->_trd=INVALID_HANDLE_VALUE;
		this->_exit=0;
		this->bind();
	}
	virtual ~serial(){
		this->end();
	}
	int rate(DWORD rate){
		return 0;
	}
	int begin(){
		_exit=0;
		if(_trd==INVALID_HANDLE_VALUE){
			_trd=CreateThread(NULL,NULL,serial_work,this,NULL,NULL);
		}
		return 0;
	}
	int end(int sync=1){
		_exit=1;
		CloseHandle(_inst);
		if(sync){
			WaitForSingleObject(_trd,INFINITE);
		}
		_trd=INVALID_HANDLE_VALUE;
		_inst=INVALID_HANDLE_VALUE;
		return 0;
	}	
	int write(void* buf,int len){
		DWORD written,error;
		COMSTAT	comstat;

		if(_inst!=INVALID_HANDLE_VALUE){
			if (!WriteFile(_inst, buf, len, &written, NULL))// &m_osWrite))
			{
				if (GetLastError() == ERROR_IO_PENDING)
				{
				}
				else
				{
					written = 0;
					ClearCommError( _inst , &error, &comstat);
				}
			}
		}else{
			return 0;
		}
		return written;
	}
	int read(void* buf,int len){
		DWORD recv,error;
		COMSTAT	comstat;
		if(_inst!=INVALID_HANDLE_VALUE){
			if (!ReadFile(_inst, buf, len, &recv, NULL))// &m_osWrite))
			{
				if (GetLastError() == ERROR_IO_PENDING)
				{
				}
				else
				{
					recv = 0;
					ClearCommError( _inst , &error, &comstat);
				}
			}
		}else{
			return 0;
		}
		return recv;
	}
	static DWORD serial_work(LPVOID arg){
		serial<T>& sp=*(serial<T>*)arg;
		T& work=sp._work;
		if(!sp.bind()){
			while(1){
				switch(sp.wait()){
					case 0:
						work.signaled();
					default:
						break;
				}
				if(sp.exit()){
					break;
				}
			}
		}
		sp.end(0);
		return 0;
	}
	int wait_end(DWORD timeout){
		if(WAIT_OBJECT_0==WaitForSingleObject(_trd,timeout)){
			return 0;
		}else{
			return -1;
		}
	}
private:
	int wait(){
		DWORD ev=0;
		DWORD error;
		COMSTAT	comstat;
		WaitCommEvent(_inst, &ev, NULL);
		if ((ev & EV_RXCHAR) == EV_RXCHAR)
		{
			ClearCommError( _inst , &error, &comstat);
			return 0;
		}
		return -1;
	}
	int exit(){
		return _exit;
	}
	int bind(){
		if(INVALID_HANDLE_VALUE==_inst){
			COMMTIMEOUTS	timeouts;
			DCB				dcb;

			_inst = CreateFile(_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
			//_inst =CreateFile(_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
			//_inst = CreateFile( 
			//	_name,					// Port Name (Unicode compatible)
			//	GENERIC_READ | GENERIC_WRITE,	// Open for Read-Write
			//	0,								// COM port cannot be shared
			//	NULL,							// Always NULL for Windows CE
			//	OPEN_EXISTING,					// For communication resource
			//	0,								// Non-overlapped operation only
			//	NULL							// Always NULL for Windows CE
			//	);

			if (_inst == (HANDLE) -1) 
			{
				DWORD err = GetLastError();
				_tprintf(_T("PMU Dll: ERR Create File for port %s err = %d"),_name,err);
				return -1;
			}

			SetCommMask(_inst, EV_RXCHAR); // Data
			SetupComm(_inst, 4096, 4096);	
			PurgeComm( _inst,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

			timeouts.ReadIntervalTimeout = 20;//500;
			timeouts.ReadTotalTimeoutMultiplier = 0;
			timeouts.ReadTotalTimeoutConstant	= 0;
			timeouts.WriteTotalTimeoutMultiplier= 10;
			timeouts.WriteTotalTimeoutConstant	= 1000;
			if (!SetCommTimeouts( _inst, &timeouts))
				return -2;

			dcb.DCBlength = sizeof(DCB);
			if (!GetCommState( _inst, &dcb))	
				return -3;

			dcb.BaudRate		= _rate;			// BaudRate
			dcb.ByteSize		= 8;
			dcb.Parity			= NOPARITY;			// 0;
			dcb.StopBits		= ONESTOPBIT;		// 0;
			//dcb.fInX			= FALSE;			// Xon 
			//dcb.fOutX			= FALSE;			// Xoff 
			//dcb.fOutxCtsFlow	= TRUE;
			dcb.fRtsControl		= RTS_CONTROL_DISABLE;
			//dcb.fOutxDsrFlow	= FALSE;
			//	dcb.XonChar  = ASCII_XON;
			//	dcb.XoffChar = ASCII_XOFF;
			//	dcb.XonLim	 = 100;
			//	dcb.XoffLim  = 100;
			if (!SetCommState(_inst, &dcb))	
				return -4;


			_tprintf(_T("PMU Dll: Open PMU com port success\r\n"));
		}
		return 0;
	}
private:
	TCHAR _name[10];
	DWORD _rate;
	HANDLE _inst;
	T _work;
	int _exit;
	HANDLE _trd;
};

#endif