#include "../../header/DataStructures/Linkedlist.h" 
#include <iostream>
using namespace std;

LinkedList::LinkedList(){
    head = tail = nullptr;
}

// FIX: Added destructor to free all nodes and prevent memory leaks
LinkedList::~LinkedList(){
    Node* current = head;
    while(current != nullptr){
        Node* next = current->next;
        delete current;
        current = next;
    }
    head = tail = nullptr;
}

void LinkedList::append(int value){
    Node* newNode = new Node{value, nullptr};
    if(!head){ head = tail = newNode; return; }
    tail->next = newNode;
    tail = newNode;
}

void LinkedList::removeNode(int value){
    if(!head) return;

    // Removing head
    if(head->data == value){
        Node* temp = head;
        head = head->next;
        if(head == nullptr) tail = nullptr;  // FIX: Update tail if list becomes empty
        delete temp;
        return;
    }

    Node* cur = head;
    while(cur->next && cur->next->data != value)
        cur = cur->next;

    if(cur->next){
        Node* temp = cur->next;
        if(temp == tail) tail = cur;  // FIX: Update tail if removing last element
        cur->next = cur->next->next;
        delete temp;
    }
}

// --- PRINT (Clean Text Only) ---
void LinkedList::printList(){
    Node* temp = head;
    cout << "[ ";
    while(temp){
        cout << temp->data << " ";
        temp = temp->next;
    }
    cout << "]"; // Removed newline here to let warehouse.cpp handle formatting
}