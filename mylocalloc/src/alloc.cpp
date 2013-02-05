
#include <cstdlib>
#include <iostream>
#include <malloc.h>
#include <cstring>

#include <dlfcn.h>
#include <pthread.h>

#include "init.h"
#include "Heaps/MainHeap.h"

MainHeap* get_main_heap() {
	static int count_proc = get_count_processors();
	static double buf[sizeof(MainHeap) / sizeof(double) + 1];
	static MainHeap *alloc = new (buf) MainHeap(count_proc);
	return alloc;
}

extern "C" {
	void *malloc(size_t size) throw() {
#ifdef M_DEBUG
		std::cerr << "start::malloc(): " << size << std::endl;
#endif
		void *ptr = nullptr;
		if (size) { 
			ptr = get_main_heap()->malloc(size);
		} 
#ifdef M_DEBUG
		std::cerr << "return ptr::malloc(): " << ptr << std::endl;
#endif

		return ptr;
	}

	void *calloc(size_t nmemb, size_t size) throw() {
		size_t n = nmemb * size;

#ifdef M_DEBUG
		std::cerr << "start::calloc(): " << n << std::endl;
#endif

		void *ptr = nullptr; 
	
		if (n) { 
			ptr = get_main_heap()->malloc(n);
		} 

    	
		if (ptr) {
			memset(ptr, 0, n); 
		}
		
#ifdef M_DEBUG
		std::cerr << "return::calloc(): " << ptr << std::endl;
#endif
		return ptr;
  	}

	void *realloc(void *ptr, size_t size) throw() {
#ifdef M_DEBUG
		std::cerr << "start::realloc(): " << ptr << " " << size << std::endl;
#endif

		if (!ptr) { // if ptr == 0 realoc == malloc
			if (size) { 
				return get_main_heap()->malloc(size);
			} 
		}

		if (!size) { // if size == 0 realoc == free
			free(ptr);
			return nullptr;
		}

		size_t objsize = get_main_heap()->get_size(ptr);
		void *newptr = nullptr; 
		
		if (size) { 
			newptr = get_main_heap()->malloc(size);
		} 

		if (newptr) {
			if (get_main_heap()->get_size(newptr) == objsize) {
				free(newptr);
				return ptr;
			}

			size_t minsize = std::min(objsize, size);
			memcpy(newptr, ptr, minsize);
		}

		free(ptr);

#ifdef M_DEBUG
		std::cerr << "return::calloc(): " << newptr << std::endl;
#endif

		return newptr;
	}

	void free(void *ptr) throw() {
#ifdef M_DEBUG
		std::cerr << "start::free(): " << ptr << std::endl;
#endif

		if (ptr) {
			get_main_heap()->free(ptr);
		}
	}

	void *memalign(size_t boundary, size_t size) throw() {
#ifdef M_DEBUG
		std::cerr << "start::memalign(): " << size << " " << boundary << std::endl;
#endif
		assert(!(boundary & (boundary - 1))); // check boundary power of two
		void *ptr = nullptr;
		if (size) { 
			ptr = get_main_heap()->memalign(size, boundary); 
		} 
		assert((reinterpret_cast<intptr_t>(ptr) & (boundary - 1)) == 0);

#ifdef M_DEBUG
		std::cerr << "return::memalign(): " << ptr << std::endl;
#endif
		return ptr;
	}
  
	int posix_memalign(void **memptr, size_t alignment, size_t size) throw() {
#ifdef M_DEBUG
		std::cerr << "start::posix_memalign(): " << size << " " << alignment << std::endl;
#endif

		if (!alignment || (alignment & (alignment - 1)) || // check power of two
		(alignment & (sizeof(void *) - 1))) { // check multiple of sizeof(void *)
			return EINVAL;
		}
		
		void *ptr = nullptr; 

		if (alignment == 0) { 
			if (size) { 
				ptr = get_main_heap()->malloc(size);
			} 
		} else { 
			ptr = memalign(alignment, size);	
		} 
#ifdef M_DEBUG
		std::cerr << "return::posix_memalign(): " << ptr << std::endl;
#endif
	
		if (!ptr) {
			return ENOMEM;
		} else {
			*memptr = ptr;
			return 0;
		}
	}

  	void *valloc(size_t size) throw() { 
		return memalign(0x1000, size);	
	} 

	typedef void * (*thread_routine) (void *);

	typedef int (*pthread_create_function) (pthread_t *thread, const pthread_attr_t *attr, thread_routine start_routine, void *arg);

	namespace {
		void *routine_start(void *arg) {
			get_main_heap();
			auto pp = reinterpret_cast<std::pair<thread_routine, void *> *>(arg);
			thread_routine real_routine = pp->first;
			void *a = pp->second;
			delete pp; 
    
			void *r = nullptr;
			r = real_routine(a);
			return r;
		} 	 	
 	}

	int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void * (*start_routine)(void *), void *arg) throw() {
		char routine_name[] = "pthread_create";
		static pthread_create_function real_pthread_create = reinterpret_cast<pthread_create_function>(dlsym(RTLD_NEXT, routine_name));

		auto args = new std::pair<thread_routine, void *>(start_routine, arg); 
		int result = real_pthread_create(thread, attr, routine_start, args);
		return result;
	}
}
