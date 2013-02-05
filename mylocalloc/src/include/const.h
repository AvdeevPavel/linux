#ifndef CONST_H_ 
#define CONST_H_

#include <cstdio>

#define PAGE_SIZE 4096 //in Linux x86_64
#define ALIGNMENT 4096 //in Linux x86_64

#define SUPERBLOCK_SIZE 0x10000UL //Size in super block with header

#define EMPTY_FRACTATION 8 // Is f - empty fractation in Hoard
#define COUNT_SUPERBLOCK 2 //Is K - in Hoard

#endif
