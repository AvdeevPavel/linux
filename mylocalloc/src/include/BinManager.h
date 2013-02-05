#ifndef BINMANAGER_H_
#define BINMANAGER_H_

#include <cassert>
#include <array> 

#include "const.h"

template<class superblock_type, size_t emptiness_frac>
struct BinManager { 
	void *malloc(size_t size);
	void free(void *ptr, superblock_type *sb); 

	void put_superblock(superblock_type *sb);
	superblock_type* get_superblock();
	bool owned_superblock(const superblock_type* sb) const; 
private:
	void move_superblock(superblock_type *sb, size_t old_degree, size_t new_degree); 
	size_t get_emptiness_degree(superblock_type* sb) const;
private: 
	std::array<superblock_type*, emptiness_frac> blocks;
};

template<class superblock_type, size_t emptiness_frac>
void* BinManager<superblock_type, emptiness_frac>::malloc(size_t size) {

#ifdef M_DEBUG
	std::cerr << "BinManger::malloc( " << size << " )" << std::endl;
#endif	

	for(long i = emptiness_frac - 2L; i >= 0; i--) { 
		superblock_type* sb = blocks[i];
	
		if (sb) { 
			size_t old_degree = get_emptiness_degree(sb);	
			void *ptr = sb->malloc();
			assert(ptr);
			size_t new_degree = get_emptiness_degree(sb);
			if (old_degree != new_degree) { 	
				move_superblock(sb, old_degree, new_degree);
			} 

#ifdef M_DEBUG
			std::cerr << "BinManger::malloc(): " << ptr << std::endl;
#endif	

			return ptr; 
		} 
	} 
	return nullptr;		
} 

template<class superblock_type, size_t emptiness_frac>
void BinManager<superblock_type, emptiness_frac>::free(void *ptr, superblock_type *sb) { 
#ifdef M_DEBUG
	std::cerr << "BinManger::free( " << ptr << " " << static_cast<void*>(sb) << " )" << std::endl;
#endif	

	const size_t old_degree = get_emptiness_degree(sb);
	sb->free(ptr);
	const size_t new_degree = get_emptiness_degree(sb);
	if (new_degree != old_degree) { 
		move_superblock(sb, old_degree, new_degree);
	} 		
} 

template<class superblock_type, size_t emptiness_frac>
void BinManager<superblock_type, emptiness_frac>::move_superblock(superblock_type *sb, size_t old_degree, size_t new_degree) { 
	assert(old_degree != new_degree);
	
	superblock_type *prev = sb->get_prev();
	superblock_type *next = sb->get_next();

	if (prev) { 
		prev->set_next(next);
	} 

	if (next) { 
		next->set_prev(prev);
	} 

		
	if (blocks[old_degree] == sb) { 
		assert(prev == nullptr);
		blocks[old_degree] = next;
	} 

	sb->set_prev(nullptr);
	superblock_type* head = blocks[new_degree];
	sb->set_next(head);
	
	if(head) {
		assert(head->get_prev() == nullptr);
		head->set_prev(sb); 
	} 

	blocks[new_degree] = sb;
} 

template<class superblock_type, size_t emptiness_frac>
size_t BinManager<superblock_type, emptiness_frac>::get_emptiness_degree(superblock_type* sb) const { 
	size_t total = sb->get_total_object();
	size_t free = sb->get_count_free_obj();

	if (total == free) { 
		return 0;
	} else { 
		return (((emptiness_frac - 2) * (total - free)) / total + 1);
	} 
} 

template<class superblock_type, size_t emptiness_frac>
void BinManager<superblock_type, emptiness_frac>::put_superblock(superblock_type *sb) { 
	assert(sb->get_next() == nullptr);
	assert(sb->get_prev() == nullptr);		

	size_t degree = get_emptiness_degree(sb);
	superblock_type* head = blocks[degree];

	sb->set_next(head);

	if (head) { 
		assert(head->get_prev() == nullptr); 
		head->set_prev(sb); 
	} 

	blocks[degree] = sb;
} 

template<class superblock_type, size_t emptiness_frac>
bool BinManager<superblock_type, emptiness_frac>::owned_superblock(const superblock_type* sb) const { 
	for(size_t i = 0; i < emptiness_frac; ++i) { 
		for (superblock_type* t = blocks[i]; t != NULL; t = t->get_next()) { 
			assert(t->isValid());
			if (t == sb) { 
				return true;
			} 
		}
	} 	
	return false; 
}  

template<class superblock_type, size_t emptiness_frac>
superblock_type* BinManager<superblock_type, emptiness_frac>::get_superblock() { 
	for(size_t i = 0; i < emptiness_frac; ++i) { 
		superblock_type* const sb = blocks[i]; 
		if (sb) { 
			assert(sb->isValid());
			assert(sb->get_prev() == nullptr);
			assert(get_emptiness_degree(sb) == i);
		
			superblock_type* const next = sb->get_next();
			blocks[i] = next; 
			if (next) { 
				sb->set_next(nullptr);
				next->set_prev(nullptr);
			} 
			return sb;
		}
	} 
	return nullptr;
} 
#endif
