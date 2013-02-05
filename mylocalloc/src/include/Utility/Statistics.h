#ifndef STATISTICS_H_
#define STATISTICS_H_

#include <cassert>

struct Statistic { 
	Statistic() 
		: used(0)
		, allocated(0)
	{
	}


	Statistic(size_t u, size_t a) 
		: used(u)
		, allocated(a)
	{
	}

	void setUsed(size_t u) { 
		used = u; 
	} 

	void incrementUsed() { 
		++used;
	} 

	void incrementUsed(size_t v) { 	
		used += v; 
	}  

	void decrementUsed() { 
		assert(used > 0UL);
		--used;
	} 

	void decrementUsed(size_t v) { 	
		assert(used >= v);
		used -= v;
	}  

	size_t getUsed() { 
		return used; 
	} 

	void setAllocated(size_t a) { 
		allocated = a; 
	} 

	void incrementAllocated() { 
		++allocated;
	} 

	void incrementAllocated(size_t v) { 	
		allocated += v; 
	}  

	void decrementAllocated() { 
		assert(allocated > 0UL);
		--allocated;
	} 

	void decrementAllocated(size_t v) { 	
		assert(allocated >= v);
		allocated -= v;
	}  
	
	size_t getAllocated() { 
		return allocated;
	} 
private: 
	size_t used; 
	size_t allocated;
}; 

#endif
