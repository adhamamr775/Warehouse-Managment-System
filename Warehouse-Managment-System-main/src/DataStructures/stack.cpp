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
    if (isEmpty()) {
        cout << "   [Stack is Empty]\n";
        return;
    }
    StackNode* temp = top;
    cout << "\n--- History Stack (Top to Bottom) ---\n";
    while (temp != nullptr) {
        string typeStr;
        
        // Use the actual enum names here instead of 0, 1, 2...
        switch (temp->data.type) {
            case ADD_PRODUCT:     typeStr = "ADD PRODUCT   "; break;
            case DELETE_PRODUCT:  typeStr = "DELETE PRODUCT"; break;
            case RESTOCK_PRODUCT: typeStr = "RESTOCK        "; break;
            case SELL_PRODUCT:    typeStr = "SELL           "; break;
            default:              typeStr = "UNKNOWN        ";
        }
        
        cout << " >> " << typeStr << " | ID: " << temp->data.productID 
             << " | Item: " << temp->data.p.getName() << endl;
        
        temp = temp->next;
    }
    cout << "-------------------------------------\n";
}
