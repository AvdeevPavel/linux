#ifndef BIGOBJECTBLOCK_H_
#define BIGOBJECTBLOCK_H_

#include "Superblock/Block.h"

struct BigObjectHeap; 

struct BigObjectblock: public Block {
	BigObjectblock(BigObjectHeap* ow, BigObjectblock* n, size_t sz, char* start)
		: magic(MAGIC_NUMBER)
		, owner(ow)
		, prev(NULL)
		, next(n)
		, size(sz)
		, mem_start(start)
	{ 
	} 
	

	void set_prev(BigObjectblock* p) { 
		prev = p; 
	} 

	void set_next(BigObjectblock* n) { 
		next = n; 
	} 


	BigObjectblock* get_prev() { 
		return prev; 
	} 

	BigObjectblock* get_next() { 
		return next; 
	} 

	virtual size_t get_size() { 
		return size;
	} 

	BigObjectHeap* get_owner() {
		return owner;
	}

	bool isValid() const { 
		return MAGIC_NUMBER == magic;
	} 
private: 
	static const size_t MAGIC_NUMBER = 0xdeadbeaf; 
	const size_t magic; 
	
	BigObjectHeap *owner; 
	
	BigObjectblock* prev; 
	BigObjectblock* next; 
	
	size_t size; 
	char* mem_start; 

	double padding;
};

#endif
