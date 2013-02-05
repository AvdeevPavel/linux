#ifndef MAINHEAP_H_
#define MAINHEAP_H_

#include "Heaps/BigObjectHeap.h"
#include "Heaps/PerProcessorHeap.h"
#include "Utility/MmapWrapper.h"

struct MainHeap {	
	typedef std::mutex lock_type; 
	typedef Superblock<SUPERBLOCK_SIZE> superblock_type;
	typedef SuperblockHeader<SUPERBLOCK_SIZE> superblockheader_type;
	typedef Bin_traits<superblockheader_type, SUPERBLOCK_SIZE> bins;
	
	MainHeap(size_t count_heap)
		: big_heap()
		, count_local_heaps(count_heap)
	{
		local_heaps = static_cast<PerProcessorHeap*> (MmapWrapper::wrap_map(count_local_heaps * sizeof(PerProcessorHeap)));

		for(PerProcessorHeap *t = local_heaps; t != local_heaps + count_local_heaps; ++t) { 
			new (t) PerProcessorHeap;
		} 
	}  

	void* malloc(size_t sz);
	void* memalign(size_t sz, size_t aligment);
	void free(void *ptr); 

	size_t get_size(void* ptr);
private: 
	void* internal_malloc(size_t sz);
	PerProcessorHeap& get_small_heap() const;
	void* align_pointer(char* start, size_t alignment) const;
private: 	
	BigObjectHeap big_heap;

	size_t count_local_heaps;
		
	PerProcessorHeap *local_heaps;  
}; 


void* MainHeap::malloc(size_t sz) {
#ifdef M_DEBUG
	std::cerr << "mainheap::malloc( " << sz << " ): " << std::endl;
#endif

	void* ptr = internal_malloc(sz + sizeof(void*));
	
	void* mem_start = static_cast<void*>(static_cast<char*>(ptr) + sizeof(void*)); 

	void** t = static_cast<void**>(ptr);
	*(t) = ptr;

#ifdef M_DEBUG
	std::cerr << "mainheap::malloc(): " << mem_start << std::endl;
#endif
	return mem_start;
} 


void* MainHeap::memalign(size_t sz, size_t alignment) { 
	void* ptr = internal_malloc(sz + alignment + sizeof(void*) - 1);
	void* start_memory = align_pointer(static_cast<char*>(ptr) + sizeof(void*), alignment); 
	
	void** t = reinterpret_cast<void**>(static_cast<char*>(start_memory) - sizeof(void*));
	*(t) = ptr; 

#ifdef M_DEBUG
	std::cerr << "mainheap::memalign( " << sz << " " << alignment << " ): " << start_memory << std::endl;
#endif

	return start_memory;
} 

void* MainHeap::internal_malloc(size_t sz) { 
	if ((sz + sizeof(void*)) > bins::biggest_object_size) { 
		return big_heap.malloc(sz + sizeof(void*));
	} else { 
		return get_small_heap().malloc(sz + sizeof(void*));
	}  
} 

//memory view superblock_pointer|...|start memory|memory... 
void MainHeap::free(void* ptr) { 
	//beginner memory
	char* p = static_cast<char*>(*(reinterpret_cast<void**>(static_cast<char*>(ptr) - sizeof(void*))));

#ifdef M_DEBUG
	std::cerr << "mainheap::free( " << ptr << " ): start_memory = " << (void*) p << " start_read_block = " << *(reinterpret_cast<void**>(p - sizeof(void*))) << std::endl;
#endif

	//get block	
	Block* block = reinterpret_cast<Block*>(*(reinterpret_cast<void**>(p - sizeof(void*))));

	if (block->get_size() > bins::biggest_object_size) { 
		BigObjectblock* superblock = dynamic_cast<BigObjectblock*>(block);
		big_heap.free(superblock);
	} else { 
		superblock_type* superblock = dynamic_cast<superblock_type*>(block);
		assert(superblock->isValid());
		std::lock_guard<lock_type> block_lock(superblock->locker());	
		superblock->get_owner()->free(static_cast<void*>(p), superblock);
	} 
} 

size_t MainHeap::get_size(void* ptr) { 
	char* p = static_cast<char*>(*(reinterpret_cast<void**>(static_cast<char*>(ptr) - sizeof(void*))));

#ifdef M_DEBUG
	std::cerr << "mainheap::get_size(): start_memory = " << (void*)p << " start_read_block = " << *(reinterpret_cast<void**>(p - sizeof(void*))) << std::endl;
#endif
	
	Block* block = reinterpret_cast<Block*>(*(reinterpret_cast<void**>(p - sizeof(void*))));
	return block->get_size();	
} 

PerProcessorHeap& MainHeap::get_small_heap() const { 
	pthread_t tid = pthread_self();
	size_t id = tid % count_local_heaps;
#ifdef M_DEBUG
	std::cerr << "mainheap::get_small_heap(): tid = " << tid << " id = " << id << std::endl;
#endif
	return local_heaps[id];
} 

void* MainHeap::align_pointer(char* start, size_t alignment) const {
	return reinterpret_cast<void *>((reinterpret_cast<intptr_t>(start) + alignment - 1) & ~(alignment - 1));
} 
#endif
