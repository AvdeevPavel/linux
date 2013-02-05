#ifndef BIN64_H_
#define BIN64_H_

template<class superblock_header>
struct Bin_traits<superblock_header, 0x10000> {
	static const size_t superblock_size = 0x10000;
	static const size_t superblockheader_size = sizeof(superblock_header);
	static const size_t biggest_object_size = (superblock_size - superblockheader_size) / 2;
	static const size_t minimal_object_size = sizeof(void *);
	static const size_t bins_count = 14;

	static size_t get_bin_index(size_t size);
	static size_t get_bin_size(size_t index);  
	//static size_t getRealSize(size_t size);
	
private:
	static const size_t bins_sizes[bins_count];
};

template <class Header>
const size_t Bin_traits<Header, 0x10000>::bins_sizes[bins_count] = { 
	minimal_object_size, 2 * minimal_object_size, 3 * minimal_object_size, 4 * minimal_object_size, 5 * minimal_object_size,
	1 << 7, 1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, biggest_object_size
};

template<class superblock_header>
size_t Bin_traits<superblock_header, 0x10000>::get_bin_index(size_t size) {
	assert(size <= biggest_object_size);

	size = size ? size : minimal_object_size;

	auto it = std::lower_bound(bins_sizes, bins_sizes + bins_count, size);
	assert(it != bins_sizes + bins_count);
	return std::distance(bins_sizes, it);
}

template<class superblock_header>
inline size_t Bin_traits<superblock_header, 0x10000>::get_bin_size(size_t index) {
	assert(index < bins_count);
	assert(get_bin_index(bins_sizes[index]) == index);
	return bins_sizes[index];
}

/*
template<class superblock_header>
inline size_t bins_traits<superblock_header, 0x10000>::get_real_size(size_t size)
{
  size_t newsize = getBinsSize(getBinsIndex(size));
  assert(newsize >= size);
  return newsize;
}
*/

#endif

