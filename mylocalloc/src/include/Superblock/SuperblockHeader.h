#ifndef SUPERBLOCK_HEADER_H_
#define SUPERBLOCK_HEADER_H_

#include <mutex>
#include <cassert>

#include "Utility/Stack.h"

struct BaseHeap;

template<size_t superblock_size>
struct Superblock;

template<size_t superblock_size>
struct SuperblockHeader { 
	typedef std::mutex lock_type;
	typedef Superblock<superblock_size> superblock_type; 

	SuperblockHeader(size_t c_size, size_t buf_size, char *start) 
		: magic(MAGIC_NUMBER)
		, owner(nullptr)
		, prev(nullptr)
		, next(nullptr)
		, mem_start(start)
		, class_size(c_size)
		, buffer_size(buf_size)
		, total_object(buffer_size/ class_size)
		, mutual_exclusuion()		
	{ 
		assert((2 * sizeof(void *)) <= class_size);
		
		size_t j = 0; 
		for(char* ptr = mem_start; j < total_object; ptr += class_size, ++j) {
			free_blocks.push(static_cast<void*>(ptr));
		} 	
		
		//LOG("SuperblockHeader() %p %p\n", (void*)start, (void*)(mem_start + buffer_size));
		assert(total_object == (free_blocks.get_size()));			
	} 

	void *malloc(superblock_type* superblock); 
	void free(void *ptr);

	size_t get_class_size() const { 
		return class_size;
	} 

	long get_total_object() const { 
		return total_object; 
	} 

	long get_count_free_obj() const { 
		return free_blocks.get_size();
	}

	BaseHeap* getOwner() const { 
		return owner; 
	} 	

	superblock_type* getPrev() const { 
		return prev; 
	}  

	superblock_type* getNext() const { 
		return next; 
	} 


	void setOwner(BaseHeap* ow) { 
		owner = ow; 
	} 
 	
	void setPrev(superblock_type* p) { 
		prev = p;
	} 

	void setNext(superblock_type* n) { 
		next = n;
	} 
	 
	bool isValid() const { 
		return MAGIC_NUMBER == magic; 
	} 
	
	void lock() { 
		locker().lock();
	} 

	void unlock() { 	
		locker().unlock();
	} 

	lock_type& locker() { 
		return mutual_exclusuion;
	}  

private: 
	static const size_t MAGIC_NUMBER = 0xdeadbeaf; 
	const size_t magic; 

	BaseHeap* owner; 
		
	superblock_type *prev; //previous suberblock in double linked list
	superblock_type *next; //next suberblock in double linked list

	char *mem_start; //start position of allocation buffer

	Stack free_blocks; 

	const size_t class_size; 
	const size_t buffer_size;

	const unsigned long total_object;
	//long fisrstobject;
	//long sizefreeobject

	lock_type mutual_exclusuion;

	double padding; //force aligment		
};

template<size_t superblock_size>
void* SuperblockHeader<superblock_size>::malloc(superblock_type* superblock) { 
	assert(isValid());	

	void* ptr = static_cast<void*>(free_blocks.pop());
	if (ptr) { 
		void** temp = reinterpret_cast<void**>(ptr);
		*temp = superblock; 
		void* out = static_cast<void*>(static_cast<char*>(ptr) + sizeof(void*));	
#ifdef M_DEBUG
		std::cerr << "SuperblockHeader::malloc(): " << out << " " << ptr << std::endl;
#endif	

		return out;
	} else { 
		assert(free_blocks.get_size() == 0);
#ifdef M_DEBUG
		std::cerr << "SuperblockHeader::malloc(): nuulptr" << std::endl;
#endif	
		return nullptr;
	}  
} 

template<size_t superblock_size> 
void SuperblockHeader<superblock_size>::free(void* ptr) {
	void* out = static_cast<void*>(static_cast<char*>(ptr) - sizeof(void*));

#ifdef M_DEBUG
	std::cerr << "SuperblockHeader::free( " << out << " )" << std::endl;
#endif	

	free_blocks.push(out);
}
	
#endif
