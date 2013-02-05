#ifndef SUPERBLOCK_H_
#define SUPERBLOCK_H_

#include "Superblock/Block.h"
#include "Superblock/SuperblockHeader.h"

#include "const.h"

template<size_t superblock_size>
struct Superblock: public Block { 
	typedef SuperblockHeader<superblock_size> header_type;
	typedef typename header_type::lock_type lock_type;

	Superblock(size_t class_size)
		:header(class_size, buffer_size, buffer) 
	{ 	
	}   

	void* malloc();
	void free(void *ptr);
	
	Superblock<superblock_size>* get_prev() const { 
		return header.getPrev();
	} 

	Superblock<superblock_size>* get_next() const { 
		return header.getNext();
	} 

	BaseHeap* get_owner() const { 
		return header.getOwner();
	} 

	void set_owner(BaseHeap* ow) { 
		header.setOwner(ow);
	} 

	void set_next(Superblock<superblock_size>* n) {
		header.setNext(n);		
	} 

	void set_prev(Superblock<superblock_size>* p) {
		header.setPrev(p); 
	} 

	size_t get_class_size() const { 
		return header.get_class_size();
	} 

	long get_total_object() const { 
		return header.get_total_object(); 
	} 

	long get_count_free_obj() const { 
		return header.get_count_free_obj();
	}

	bool isValid() const { 
		return header.isValid();
	}  

	bool inRange(void *ptr) const { 
		assert(isValid());
		return ((ptr >= buffer) && (ptr < &buffer[buffer_size]));
	} 

	virtual size_t get_size() { 
		return header.get_class_size();
	} 

	lock_type& locker() {
		assert(isValid());
		return header.locker();
	}

	static size_t get_buffer_size() { 
		return buffer_size;
	} 
  
private: 
	static const size_t buffer_size = superblock_size - sizeof(header_type); 
	char buffer[buffer_size]; 
	header_type header; 
};  

template<size_t superblock_size>
void* Superblock<superblock_size>::malloc() { 
	assert(isValid());
	void* ptr = header.malloc(this); 	
	if (ptr) { 
		assert(inRange(ptr));
	} 
	return ptr; 
} 

template<size_t superblock_size>
void Superblock<superblock_size>::free(void *ptr) { 
	assert(isValid());
	assert(inRange(ptr));
	header.free(ptr);
} 


#endif
