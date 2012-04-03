#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 10

void printString(char *str, int size_) {
	int n_written = 0; 

	#ifdef NDEBUG
		printf("Print: %s\n", str);
	#endif
			
	while(size_ > 0) {
		n_written = write(1, str, size_);

		if (n_written < 0) { 
			exit(1); 
		} 	 	

		str = str + n_written; 
		size_ = size_ - n_written; 		
	}
} 

void reverseString(char *str, int size_) { 
	int i = 0;

	for (i = 0; i < size_ / 2; ++i) {
		str[i] = str[i] ^ str[size_ - i - 1];
		str[size_ - i - 1] = str[i] ^ str[size_ - i - 1]; 	 
		str[i] = str[i] ^ str[size_ - i - 1]; 
	} 	
} 

int findEndString(char *str, int len) {
	int i = 0; 

	while((len != i) && *str != '\n') { 
		++str; 
		++i; 
	} 

	if (len == i) { 
		return -1; 
	} else {  
		return i; 	 
	} 
} 

int getStringInConsole(char *buffer, int *indexRead, int sub) { 
	int n_read = read(0, buffer + sub, BUFSIZE - sub);
	if (n_read == 0) { 	
		return 0; 
	} else if (n_read < 0) { 
		exit(1); 
	} else {
		*indexRead += n_read; 				
		return *indexRead; 
	} 	
}  

char *getStringInBuf(char *buffer, int *len, int *indexRead, int *index_) { 
	int indexEnd = findEndString(buffer + *index_, *indexRead);
	char *out = NULL; 
	
	#ifdef NDEBUG
		printf("Go to getStrinBuffer %d\n", indexEnd);
	#endif

	if (indexEnd < 0) { 
		return NULL; 
	} else { 
		out = buffer + *index_; 
		*len = (indexEnd + 1) - *index_;
    		*index_ = indexEnd + 1;
		return out;		
	}
} 

int getline_(char **str) {
	static int indexRead = 0; 
	static int index_ = 0; 	
	static char buffer[BUFSIZE]; 

	int n_read = 0;
	int length_ = 0; 
	char *tempStr = *str; 

	for(;;) {
		#ifdef NDEBUG
			printf("start: %s\n", buffer);
		#endif
		
		if (index_ != 0) {
			indexRead -= index_;
			memmove(buffer, buffer + index_, indexRead);
			index_ = 0;
		} 

		tempStr = getStringInBuf(buffer, &length_, &indexRead, &index_); 
		
		if (tempStr != NULL) { 
			*str = tempStr; 
			return length_;
		} else {
			#ifdef NDEBUG
				printf("Go to no str: %d\n", indexRead);
			#endif
			if (BUFSIZE == indexRead) { 
				while(tempStr == NULL) { 
					index_ = 0; 
					indexRead = 0;
					n_read = getStringInConsole(buffer, &indexRead, 0);
					if (n_read == 0) { 
						*str = NULL;
						return 0; 
					} 
					tempStr = getStringInBuf(buffer, &length_, &indexRead, &index_);										
				} 
			} else {
				#ifdef NDEBUG
					printf("read string\n");
				#endif
				n_read = getStringInConsole(buffer, &indexRead, indexRead);
				if (n_read == 0) { 
					*str = NULL; 
					return 0; 
				} 
			} 
		} 
	} 
} 

void run() { 
	int readBytes = 0; 
	char *str = NULL; 

	for(;;) { 
		readBytes = getline_(&str);
		
		if (str == NULL) { 
			exit(1);
		} else if (readBytes == 0) { 
			return;
		} else { 
			#ifdef NDEBUG
				printf("Go to reverse and print\n");
			#endif
			reverseString(str, readBytes - 1); 
			printString(str, readBytes);	
		} 
	}
} 

int main() { 
	run();
	return 0;
} 
