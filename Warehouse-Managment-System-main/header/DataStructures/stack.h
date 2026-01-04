#ifndef STACK_H
#define STACK_H

#include <iostream>
#include <string>
#include "../../main/product.h" // Needed for saving product state
using namespace std;

// Action Type for Undo
enum ActionType { ADD_PRODUCT, SELL_PRODUCT, RESTOCK_PRODUCT, DELETE_PRODUCT };

struct Action {
    ActionType type;
    int productID;
    int quantity;
    Product backupData; // Used when restoring a deleted product
};

// Node Structure
struct StackNode {
    Action data;
    StackNode* next;
    
    // Constructor defined in .cpp
    StackNode(Action val);
};

// Stack Class
class Stack {
private:
    StackNode* top;

public:
    Stack();
    ~Stack(); // Destructor

    void push(Action val);
    Action pop();
    Action peek();
    bool isEmpty();
    void printStack();
};

#endif