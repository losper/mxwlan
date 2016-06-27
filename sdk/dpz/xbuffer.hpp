#ifndef __DPZ_BUFFER_HPP__
#define __DPZ_BUFFER_HPP__

#include "xmemory.hpp"
namespace dpz{

template<typename T>
class buffer{
public:
	buffer(unsigned int capacity):_size(0),_head(0),_tail(0),_capacity(capacity){
		_buf=new allocator<T>[capacity];
		InitializeCriticalSection(&_cs);
	}
	virtual ~buffer(){
		delete [] _buf;
	}
	unsigned int size(){
		return _size;
	}
	int push(const T* data,unsigned int len){
		if(_size<_capacity){
			//printf("[info]:size:%d-_tail:%d-_head:%d!!\r\n",_size,_head,_tail);
			_tail=_tail%_capacity;
			//if(_tail<_capacity){
				_buf[_tail++].copy(data,len);
			/*	_tail++;
			}else{
				_tail=0;
				_buf[_tail].copy(data,len);
			}*/
			
			_size++;
		}else{
			printf("[info]:size:%d-_tail:%d-_head:%d!!\r\n",_size,_head,_tail);
			//printf("[info]:radio data is alread full!!\r\n");
			return -1;
		}
		return 0;
	}
	T* top(){
		return _buf[_head].data();
	}
	unsigned int top_size(){
		return _buf[_head].size();
	}
	int pop(){
		if(_size){
			//printf("[info]:size:%d-_tail:%d-_head:%d!!\r\n",_size,_head,_tail);
			_head=(_head%_capacity);
			_head++;
			_size--;
		}else{
			//printf("[info]:size:%d-_tail:%d-_head:%d!!\r\n",_size,_head,_tail);
			return -1;
		}
		return 0;
	}
private:
	unsigned int _size;
	unsigned int _capacity;
	allocator<T> *_buf;
	unsigned int _head;
	unsigned int _tail;
	CRITICAL_SECTION _cs;
};

}
#endif
