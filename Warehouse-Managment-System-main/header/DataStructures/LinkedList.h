#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// Used to represent warehouse shelf/storage layout (physical order)
struct Node {
    int data;
    Node* next;
};

class LinkedList {
private:
    Node* head;
    Node* tail;

public:
    LinkedList();
    ~LinkedList();  // Destructor to free memory
    void append(int value);      // Add new product ID to storage layout
    void removeNode(int value);  // Remove from layout if removed from system
    void printList();            // Show layout order
};

#endif
