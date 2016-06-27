#ifndef __XSTRING_HPP__
#define __XSTRING_HPP__
#include <windows.h>

namespace dpz{
	class xstring{
	public:
		unsigned int size() const {
			return _size;
		}
		const char* c_str() const {
			if(_data){
				return _data;
			}else{
				return "";
			}
		}
		xstring():_data(0),_capacity(0),_size(0){
		}
		xstring(unsigned int size):_data(0),_capacity(0),_size(0){
			_capacity=size;
			check_memory(size);
			//_data[0]=0;
		}
		xstring(const char* str):_data(0),_capacity(0),_size(0){
			if(str){
				unsigned int size=strlen(str);
				check_memory(size);
				strcpy(_data,str);
			}
		}
		/*xstring(const xstring str):_data(0),_capacity(0),_size(0){
			unsigned int size=strlen(str.c_str());
			check_memory(size);
			strcpy(_data,str.c_str());
		}*/
		xstring(const xstring& str):_data(0),_capacity(0),_size(0){
			unsigned int size=strlen(str.c_str());
			check_memory(size);
			strcpy(_data,str.c_str());
		}
		int operator==(const char* str) const{
			if(strcmp(str,this->_data)){
				return 0;
			}
			return 1;
		}
		int operator==(const xstring& str) const{
			if(strcmp(str.c_str(),this->_data)){
				return 0;
			}
			return 1;
		}
		xstring &operator=(const char* str){
			if(str){
				unsigned int size=strlen(str);
				check_memory(size);
				strcpy(_data,str);
			}
			return *this;
		}
		xstring &operator=(xstring& str){
			unsigned int size=strlen(str.c_str());
			check_memory(size);
			strcpy(_data,str.c_str());
			return *this;
		}
		xstring &operator+=(xstring& str){
			unsigned int pos=_size;
			unsigned int size=strlen(str.c_str())+_size;
			check_memory(size);
			strcpy(_data+pos,str.c_str());
			return *this;
		}
		xstring &operator+=(const char* str){
			if(str){
				unsigned int pos=_size;
				unsigned int size=strlen(str)+_size;
				check_memory(size);
				strcpy(_data+pos,str);
			}
			return *this;
		}
		///todo 需要再修改
		xstring &operator+(const char* str){
			if(str){
				(*this)+=str;
			}
			return *this;
		}
		xstring &operator+(xstring& str){
			return (*this)+=str;
		}
		virtual ~xstring(){
			if(_data){
				delete _data;
			}
		}
	private:
		void check_memory(unsigned int size){
			_size=size;
			size+=1;
			if(_data){
				if(_capacity<size){
					char* tmp=_data;
					_data=new char[size];
					memcpy(_data,tmp,_size);
					_data[_size]=0;
					delete[] tmp;
					_capacity=size;
				}
			}else{
				_capacity=size;
				_data=new char[size];
			}
		}
	private:
		char* _data;
		unsigned int _capacity;
		unsigned int _size;
	};
}
#endif