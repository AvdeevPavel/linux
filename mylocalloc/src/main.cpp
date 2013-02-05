//#include <cstdlib>
#include <iostream>

#include "init.h"
#include "Heaps/MainHeap.h"
#include "BinTraits/bin64.h"

/*typedef SuperblockHeader<SUPERBLOCK_SIZE> superblockheader_type;
typedef Bin_traits<superblockheader_type, SUPERBLOCK_SIZE> bins;
	

int main() { 
	/*size_t align = 16; 
	void* ptr = malloc(32 + align + sizeof(void*) - 1);

	void* mem_start = static_cast<void*>(static_cast<char*>(ptr) + sizeof(void*)); 
	void** t = static_cast<void**>(ptr);
	*(t) = ptr;

	std::cout << ptr << " " << mem_start << " " << t << " " << *t << std::endl;

	void* memory_new = *(reinterpret_cast<void**>(static_cast<char*>(mem_start) - sizeof(void*)));
	std::cout << memory_new << std::endl; 
	
	size_t sz = reinterpret_cast<size_t>(ptr);
	size_t sz1 = sz;
	
	for(; sz % align != 0; ++sz) 
		;

	void* out = reinterpret_cast<void*>(sz);  
	void* align_p = reinterpret_cast<void *>((reinterpret_cast<size_t>(ptr) + align - 1) & ~(align - 1));

	std::cout << ptr << " " << sz1 << " " << sz << " " << out << " " << align_p << " " << reinterpret_cast<size_t>(align_p) << std::endl;

	static int count_proc = get_count_processors();
	std::cout << count_proc << " " << std::endl;

	static double buf[sizeof(MainHeap) / sizeof(double) + 1];
	static MainHeap *heap = new (buf) MainHeap(count_proc);
	void* bigptr = heap->malloc(0x11100); 
	void* bigptr1 = heap->malloc(0x11100); 
	void* bigptr2 = heap->malloc(0x11100); 
	void* smallptr = heap->malloc(8);	
	void* smallptr1 = heap->malloc(30); 
	void* bigptr3 = heap->malloc(50000); 
	void* smallptr2 = heap->malloc(8);
	void* smallptr3 = heap->malloc(30000);
	void* smallptr4 = heap->malloc(30000);
	void* smallptr5 =heap->malloc(30000);

	heap->free(bigptr);
	heap->free(bigptr2);
	heap->free(bigptr1);
	heap->free(smallptr);
	heap->free(smallptr1);
	heap->free(bigptr3);
	heap->free(smallptr4);
	heap->free(smallptr3);
	heap->free(smallptr5);
	heap->free(smallptr2);




	//std::cout << bins::superblock_size << " " << bins::superblockheader_size << " " << bins::biggest_object_size <<std::endl;

	return 0;
} */
