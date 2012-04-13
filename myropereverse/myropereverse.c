#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ROPE_SIZE 10
#define BUFSIZE 3

typedef struct Node Node; 

struct Node {
	int size; 
	int sizeStr;
	char *str; 
	Node *prev;
};

void printString(char *str, int size_) {
	int n_written = 0; 
		
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

int findEndString(char *str, int start, int len) {
	int i = start; 	
	str += start;

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

/*rope*/
Node* addNode(int size_, int sizeStr_, char *str_, Node *prev_) { 
	Node *result = (Node*) malloc(sizeof(Node));
	result->size = size_;
	result->sizeStr = sizeStr_;
	result->str = str_; 
	result->prev = prev_; 
	return result;
} 

void delete(Node *current) { 
	if (current->prev != NULL) { 
		delete(current->prev);
	} 

	if (current->str != NULL) { 
		free(current->str); 
	}

	free(current);
} 

void reverseToPrint(Node* current) { 
	if (current->sizeStr > 0) { 
		reverseString(current->str, current->sizeStr);
		printString(current->str, current->sizeStr);
	}  

	if (current->prev != NULL) 
		reverseToPrint(current->prev);
} 

Node* addNewString(Node *parent, char *str, int start, int len) { 
	char *buf = (char *) malloc((len - start) * sizeof(char));
	memcpy(buf, str + (start) * sizeof(char), (len - start) * sizeof(char));
	return addNode((len - start) + parent->size, (len - start), buf, parent);
}


void run() {
	Node *rope = addNode(0, 0, NULL, NULL);
	char *buffer = (char*) malloc(BUFSIZE * sizeof(char)); 
	int devnul = 0;
	int n_read = 0;
	int index_ = -1; 
	int oldIndex = 0; 

	for(;;) {
		n_read = read(0, buffer, BUFSIZE);
		if (n_read == 0) { 
			delete(rope); 
			free(buffer);
			buffer = NULL;
			return;
		} else if (n_read < 0) { 
			delete(rope); 
			free(buffer);
			buffer = NULL;
			rope = NULL;
			exit(1);
		} else { 
			do { 
				oldIndex = index_ + 1;
				index_ = findEndString(buffer, oldIndex, n_read);
				if (index_ == -1) { 
					if (devnul != 1) {
						if ((rope->size + n_read) > ROPE_SIZE) 
							devnul = 1; 
						else  
							rope = addNewString(rope, buffer, oldIndex, n_read);	
					} 
				} else { 
					if (((rope->size + index_) <= ROPE_SIZE) && (devnul != 1)) { 
						rope = addNewString(rope, buffer, oldIndex, index_);
						reverseToPrint(rope);
						printString("\n", 1); 
					}
					delete(rope);
					devnul = 0; 
					rope = addNode(0, 0, NULL, NULL); 
				} 						
			} while(index_ != -1); 
		} 	
	} 
	
	free(buffer);
} 

int main() { 
	run();
	return 0;
} 
