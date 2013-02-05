#ifndef BLOCK_H_
#define BLOCK_H_

#include <mutex>

struct Block { 
	virtual size_t get_size() = 0;
	
	virtual ~Block() { 
	}	 
}; 

#endif
