#ifndef STACK_H    
#define STACK_H    

#include <iostream>
#include <string>
#include "../../main/product.h" 

using namespace std;

// 1. Action Type for Undo
enum ActionType { ADD_PRODUCT, SELL_PRODUCT, RESTOCK_PRODUCT, DELETE_PRODUCT };

// 2. Action Structure (Renamed to 'p' to match warehouse.cpp)
struct Action {
    ActionType type;
    int productID;
    int quantity;
    Product p; 
};

// Node Structure
struct StackNode {
    Action data;
    StackNode* next;
    
    // Constructor
    StackNode(Action val);
};

// Stack Class
class Stack {
private:
    StackNode* top;

public:
    Stack();
    ~Stack(); 

    void push(Action val);
    Action pop();
    Action peek();
    bool isEmpty();
    void printStack();
};

#endif 
