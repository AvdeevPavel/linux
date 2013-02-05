#ifndef BIGOBJECTHEAP_H_
#define BIGOBJECTHEAP_H_

#include <mutex>
#include <cassert>

#include "const.h"
#include "Superblock/BigObjectblock.h"
#include "Utility/MmapWrapper.h"

struct BigObjectHeap { 
	typedef std::mutex lock_type; 
	
	BigObjectHeap()
		: head(NULL)
		, mutual_exclusion() 
	{ 
	} 

	void* malloc(size_t sz); 

	void free(BigObjectblock* ptr);

	lock_type& locker() { 
		return mutual_exclusion;
	} 
private:
	BigObjectblock* head;
	lock_type mutual_exclusion;	
};

void* BigObjectHeap::malloc(size_t sz) { 
#ifdef M_DEBUG
	std::cerr << "BigObjectHeap::malloc( " << sz << " )" << std::endl;
#endif	
	size_t result = sz + sizeof(void*) + sizeof(BigObjectblock);
	void* ptr = MmapWrapper::wrap_map(result);
	
	if (ptr) { 
		char* mem_start = static_cast<char*>(ptr) + sizeof(BigObjectblock);

		new (ptr) BigObjectblock(this, head, sz + sizeof(void*), mem_start);

		if (head) { 
			head->set_prev(static_cast<BigObjectblock*>(ptr));
		}
  
		head = static_cast<BigObjectblock*>(ptr);

		void** pointer = reinterpret_cast<void**>(mem_start);
		*pointer = ptr;

		mem_start += sizeof(void*);

#ifdef M_DEBUG
		std::cerr << "BigObjectHeap::malloc(): " << static_cast<void*>(mem_start) << std::endl;
#endif	
		return static_cast<void*>(mem_start);		
	} else {  
		char message[] = "BigObjectHeap::malloc() error: no memory\n";
		ssize_t sz = write(STDERR_FILENO, message, sizeof(message) - 1);			
		sz = sz;
		return nullptr;
	} 
} 

void BigObjectHeap::free(BigObjectblock* ptr) { 
#ifdef M_DEBUG
	std::cerr << "BigObjectHeap::free( " << static_cast<void*>(ptr) << " )" << std::endl;
#endif	
	
	assert(ptr);
	assert(ptr->isValid());
	assert(ptr->get_owner() == this);

	std::lock_guard<lock_type> heap_lock(locker());

	size_t sz = ptr->get_size() + sizeof(BigObjectblock);
#ifdef M_DEBUG
	std::cerr << "BigObjectHeap::free(): " << sz << std::endl;
#endif	

 	if (ptr->get_prev()) {
		ptr->get_prev()->set_next(ptr->get_next()); 
	} else { 
		head = ptr->get_next();
	}

#ifdef M_DEBUG
	std::cerr << "BigObjectHeap::free(): " << head << " " << ptr << std::endl;
#endif	

	if (ptr->get_next()) {
		ptr->get_next()->set_prev(ptr->get_prev()); 
	}

	ptr->set_prev(nullptr);
	ptr->set_next(nullptr);

	MmapWrapper::unmap(static_cast<void*>(ptr), sz);
} 
 
#endif
