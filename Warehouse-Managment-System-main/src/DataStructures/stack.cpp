#include "../../header/DataStructures/stack.h"

// Constructor
StackNode::StackNode(Action val) : data(val), next(nullptr) {}

Stack::Stack() {
    top = nullptr;
}

Stack::~Stack() {
    while (!isEmpty()) {
        pop();
    }
}

void Stack::push(Action val) {
    StackNode* newNode = new StackNode(val);
    newNode->next = top;
    top = newNode;
}

Action Stack::pop() {
    if (isEmpty()) {
        // Return a dummy empty action or error state
        // In a real app we might throw, but here we return a harmless action
        Product dummyP;
        return {ADD_PRODUCT, -1, 0, dummyP}; 
    }
    
    StackNode* temp = top;
    Action val = temp->data;
    top = top->next;
    
    delete temp;
    return val;
}

Action Stack::peek() {
    if (isEmpty()) {
        Product dummyP;
        return {ADD_PRODUCT, -1, 0, dummyP};
    }
    return top->data;
}

bool Stack::isEmpty() {
    return top == nullptr;
}
void Stack::printStack() {
    StackNode* current = top;
    cout << "Stack contents (top to bottom):" << endl;
    while (current != nullptr) {
        cout << "Action Type: " << current->data.type 
             << ", Product ID: " << current->data.productID 
             << ", Quantity: " << current->data.quantity << endl;
        current = current->next;
    }
}