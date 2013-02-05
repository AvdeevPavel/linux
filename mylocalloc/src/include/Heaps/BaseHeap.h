#ifndef BASEHEAP_H_
#define BASEHEAP_H_

#include <mutex>

#include "Superblock/Superblock.h"

struct BaseHeap { 
	typedef std::recursive_mutex lock_type;		
	typedef Superblock<SUPERBLOCK_SIZE> superblock_type; 
		
	BaseHeap()
		: magic(MAGIC_NUMBER) 
	{ 
	} 
	
	bool isValid() { 
		return MAGIC_NUMBER == magic; 
	} 

	virtual ~BaseHeap() {
	}

	virtual void free(void* ptr, superblock_type* sb) = 0;
	virtual lock_type& locker() = 0; 
private:
	static const size_t MAGIC_NUMBER = 0x01234567; 
	const size_t magic; 		
}; 

#endif 
