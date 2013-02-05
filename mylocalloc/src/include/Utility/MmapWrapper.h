#ifndef MMAPWRAPPER_H_
#define MMAPWRAPPER_H_

#include <sys/mman.h>
#include <cstdlib> 
#include <unistd.h>

#include "const.h"

struct MmapWrapper {
	//static const size_t page_size = PAGE_SIZE; 

	static void *wrap_map(size_t size) { 
		if (size == 0) { 
			return nullptr;
		} 

		void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 

		if (MAP_FAILED == ptr) { 
			char message[] = "Virtual memeory exhausted.\n"; 
			ssize_t sz = write(STDERR_FILENO, message, sizeof(message) - 1); 
			sz = sz;
			return nullptr; 
		} 

#ifdef M_DEBUG
		std::cerr << "mmap( " << size << "): " << ptr << std::endl;
#endif

		return ptr; 
	} 

	static void unmap(void *ptr, size_t size) { 
#ifdef M_DEBUG
		std::cerr << "munmap( " << ptr << " " << size << " )" << std::endl;
#endif
		munmap(ptr, size); 
	} 
};

#endif 
