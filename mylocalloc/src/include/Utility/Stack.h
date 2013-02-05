#ifndef STACK_H_
#define STACK_H_

#include <cassert>

struct Stack {
	struct Node;

	Stack()
		: size(0)
		, head()
	{	
	}

	void push(void *p) { 
		Node* h = reinterpret_cast<Node*>(p); 
		h->next = head.next;  
		head.next = h; 
		++size;
	} 

	void* pop() { 
		Node *h = head.next; 

		if (!h) {
			assert(size == 0); 
			return nullptr;
		} 	
		head.next = h->next;
		--size; 
		return h;
	} 
	
	Node* peek() { 
		return head.next;
	}  

	void clear() { 
		head.next = nullptr;
		size = 0; 
	} 

	size_t get_size() const { 
		return size;
	} 

	struct Node { 
		Node() 
			:next(nullptr)
		{
		}
	
		Node* next;
	private: 
		Node(const Node&); 
		Node &operator=(const Node&); 
	}; 

private: 
	size_t size; 
	Node head; 
}; 
#endif
