#ifndef GLOBALHEAP_H_ 
#define GLOBALHEAP_H_

#include "const.h"
#include "Heaps/BaseHeap.h" 
#include "BinManager.h"
#include "BinTraits/bin_traits.h"

struct GlobalHeapInstance: public BaseHeap { 
	typedef std::recursive_mutex lock_type;
	typedef Superblock<SUPERBLOCK_SIZE> superblock_type; 
	typedef SuperblockHeader<SUPERBLOCK_SIZE> superblockheader_type;
	typedef Bin_traits<superblockheader_type, SUPERBLOCK_SIZE> bin_type;
	typedef BinManager<superblock_type, EMPTY_FRACTATION> manager_type;
	
	superblock_type* get_superblock(size_t size_index, BaseHeap* dest_heap) { 
#ifdef M_DEBUG
		std::cerr << "GlobalHeap::get_superblock( " << size_index << " " << static_cast<void*>(dest_heap) << " )" << std::endl;
#endif
		assert(isValid()); 		

		std::lock_guard<lock_type> heap_lock(locker()); 

		superblock_type *sb = bins[size_index].get_superblock();
		if (sb) { 
			assert(sb->isValid()); 
			assert(sb->get_owner() == this); 
			sb->set_owner(dest_heap);
		} 

#ifdef M_DEBUG
		std::cerr << "GlobalHeap::get_superblock(): " << static_cast<void*>(sb) << " )" << std::endl;
#endif
		return sb; 
	}

	void put_superblock(superblock_type *sb, size_t size_index) { 
		assert(isValid());
#ifdef M_DEBUG
		std::cerr << "GlobalHeap::put_superblock( " << static_cast<void*>(sb) << " " << size_index << " )" << std::endl;
#endif
		std::lock_guard<lock_type> heap_lock(locker());
		
		assert(sb);
		assert(sb->isValid());	
		assert(sb->get_owner() != dynamic_cast<BaseHeap*>(this));

		sb->set_owner(this);
		bins[size_index].put_superblock(sb);
	} 

	virtual void free(void* ptr, superblock_type* sb) { 
		assert(isValid());

#ifdef M_DEBUG
		std::cerr << "GlobalHeap::free( " << ptr << " " << static_cast<void*>(sb) << " )" << std::endl;
#endif

		assert(sb);
		assert(sb->isValid());
		assert(sb->get_owner() == dynamic_cast<BaseHeap*>(this));
		
		std::lock_guard<lock_type> heap_lock(this->locker());
		
		const size_t size = sb->get_size();
		const size_t size_index = bin_type::get_bin_index(size);
		bins[size_index].free(ptr, sb);			
	} 

	virtual lock_type& locker() { 
		return mutual_exclusion;
	} 
private: 
	std::array<manager_type, bin_type::bins_count> bins;
	lock_type mutual_exclusion; 
};

struct GlobalHeap { 
	typedef Superblock<SUPERBLOCK_SIZE> superblock_type; 

	GlobalHeapInstance& operator()() { 
		static double buf[sizeof(GlobalHeapInstance) / sizeof(double) + 1];
		static GlobalHeapInstance *instance_ptr = new (buf) GlobalHeapInstance;
		return *instance_ptr;
	} 

	superblock_type* get_superblock(size_t size_index, BaseHeap* dest_heap) { 
		return (*this)().get_superblock(size_index, dest_heap);
	} 

	void put_superblock(superblock_type *sb, size_t size_index) { 
		(*this)().put_superblock(sb, size_index);
	} 

}; 

#endif
