#ifndef PERPROCESSORHEAP_H_
#define PERPROCESSORHEAP_H_

#include "Heaps/GlobalHeap.h"
#include "Utility/Statistics.h"
#include "Utility/MmapWrapper.h"

struct PerProcessorHeap: public BaseHeap {
	typedef	BaseHeap::lock_type lock_type;
	typedef Superblock<SUPERBLOCK_SIZE> superblock_type; 
	typedef SuperblockHeader<SUPERBLOCK_SIZE> superblockheader_type;
	typedef Bin_traits<superblockheader_type, SUPERBLOCK_SIZE> bin_type;
	typedef BinManager<superblock_type, EMPTY_FRACTATION> manager_type;

	PerProcessorHeap()
		: global_heap()
		, statistics() 
		, bins()
		, mutual_exclusion()
		, magic(MAGIC_NUMBER)
	{ 
	} 

	void *malloc(size_t sz); 
	virtual void free(void* ptr, superblock_type *sb);
	
	bool isValid() { 
		return MAGIC_NUMBER == magic;
	} 
	
	virtual lock_type& locker() { 
		assert(isValid());
		return mutual_exclusion;
	}  	
private: 
	bool invariant (size_t u, size_t a, size_t class_size) {
		return ((u * EMPTY_FRACTATION) < (EMPTY_FRACTATION - 1) * a) 
		&& (u < a - COUNT_SUPERBLOCK * (superblock_type::get_buffer_size() / class_size)); 
	}

	void* get_object(size_t size_index);

	void put_superblock(superblock_type *sb, size_t size_index); 
	bool request_superblock(size_t size_index);
	void free_superblock(size_t size_index);	

	void adder_stats(superblock_type* sb, size_t size_index); 		
	void suber_stats(superblock_type* sb, size_t size_index);
private: 
	GlobalHeap global_heap;

	std::array<Statistic, bin_type::bins_count> statistics; 
	std::array<manager_type, bin_type::bins_count> bins;
	lock_type mutual_exclusion;

	static const size_t MAGIC_NUMBER = 0xdeadbeaf; 
	const size_t magic; 
};   

void* PerProcessorHeap::malloc(size_t sz) {
	assert(isValid());
	
	std::lock_guard<lock_type> heap_lock(locker());

#ifdef M_DEBUG
	std::cerr << "PerProcessorHeap::malloc( " << sz << " )" << std::endl;
#endif	
	while(true) { 
		size_t size_index = bin_type::get_bin_index(sz);
		assert(bin_type::get_bin_size(size_index) >= sz); 
			
		void* ptr = get_object(size_index); 
		if (ptr) { 
#ifdef M_DEBUG
			std::cerr << "PerProcessorHeap::malloc(): " << ptr << std::endl;
#endif	
			return ptr;
		} 

		if (!request_superblock(size_index)) { 
			char message[] = "PerProcessorHeap::malloc() error: no memory\n";
			ssize_t sz = write(STDERR_FILENO, message, sizeof(message) - 1);
			sz = sz;
			return nullptr;
		} 
	} 
}

void PerProcessorHeap::free(void* ptr, superblock_type *sb) { 
	assert(isValid()); 
#ifdef M_DEBUG
	std::cerr << "PerProcessorHeap::free( " << ptr << " " << static_cast<void*>(sb) << " )" << std::endl;
#endif	
	assert(sb);
	assert(sb->isValid());
	assert(sb->get_owner() == dynamic_cast<BaseHeap*>(this)); 

	std::lock_guard<lock_type> heap_guard(locker());

	const size_t size = sb->get_size();
	const size_t size_index = bin_type::get_bin_index(size);
	
	bins[size_index].free(ptr, sb);
	Statistic& stats = statistics[size_index];
	stats.decrementUsed();

	if (invariant(stats.getUsed(), stats.getAllocated(), size)) {
		free_superblock(size_index);
	}
} 
	
void* PerProcessorHeap::get_object(size_t size_index) { 
	assert(isValid());
	
	void* ptr = bins[size_index].malloc(bin_type::get_bin_size(size_index));
	if (ptr) { 
#ifdef M_DEBUG
		std::cerr << "PerProcessorHeap::get_object( " << size_index << " ): " << ptr << " bin_size = " << bin_type::get_bin_size(size_index) << std::endl;
#endif	
		statistics[size_index].incrementUsed();
	} 
	return ptr;
} 

void PerProcessorHeap::put_superblock(superblock_type *sb, size_t size_index) { 
	assert(isValid());
	assert(sb);
	assert(sb->isValid());
	
	sb->set_owner(this);
	adder_stats(sb, size_index); 	
	bins[size_index].put_superblock(sb);
	
	assert(bins[size_index].owned_superblock(sb));
}  

bool PerProcessorHeap::request_superblock(size_t size_index) { 
	assert(isValid());
	superblock_type *sb = global_heap.get_superblock(size_index, this); 

	if (sb) { 		
		if (!sb->isValid()) {
			return false;
		} 
	} else { 
		void *ptr = MmapWrapper::wrap_map(sizeof(superblock_type));
		if (ptr) { 
			new (ptr) superblock_type(bin_type::get_bin_size(size_index));
			sb = static_cast<superblock_type*>(ptr);
		}  
	}

	if (sb)  { 
		put_superblock(sb, size_index); 
		assert(sb->get_owner() == this);
		return true;
	} else { 
		return false; 
	}  
} 

void PerProcessorHeap::free_superblock(size_t size_index) { 
	assert(isValid());
	superblock_type* sb = bins[size_index].get_superblock();
	
	assert(sb);
	assert(sb->isValid());

	suber_stats(sb, size_index);
	global_heap.put_superblock(sb, size_index);
}

void PerProcessorHeap::adder_stats(superblock_type* sb, size_t size_index) { 
	assert(isValid()); 
	assert(sb);
	assert(sb->isValid());

	Statistic &stats = statistics[size_index];
	auto allocated = sb->get_total_object();
	stats.incrementUsed(allocated - sb->get_count_free_obj()); 
	stats.incrementAllocated(allocated);
} 

void PerProcessorHeap::suber_stats(superblock_type* sb, size_t size_index) { 
	assert(isValid());
	assert(sb);
	assert(sb->isValid()); 

	Statistic &stats = statistics[size_index];
	auto allocated = sb->get_total_object();
	stats.decrementUsed(allocated - sb->get_count_free_obj()); 
	stats.decrementAllocated(allocated);	
} 
#endif
