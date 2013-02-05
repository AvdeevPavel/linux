#ifndef INIT_H_ 
#define INIT_H_

#ifdef _WIN32
	#include <windows.h>
#elif MACOS
	#include <sys/param.h>
	#include <sys/sysctl.h>
#else
	#include <unistd.h>
#endif

#include <iostream>
#include <cstdio>

int get_count_processors(); 

#endif

