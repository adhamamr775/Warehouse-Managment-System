#ifndef QUEUE_H
#define QUEUE_H

#include <string>
#include <iostream>
using namespace std;

// --- THE CONTAINER ---
// This groups all customer info into one "packet"
struct Order {
    int id;
    int qty;
    string paymentMethod;
    string customerName;  // New
    string customerPhone; // New
    
    // Pointer to the next order in line
    Order* next;
};

// --- THE MANAGER ---
class Queue {
private:
    Order* front;
    Order* rear;

public:
    Queue(); // Constructor
    ~Queue(); // Destructor to free remaining orders

    bool isEmpty();
    
    // Enqueue now takes the full Struct object for cleaner code
    void enqueue(Order newOrder);
    
    // Returns the full Struct so you get all the data back
    Order dequeue();
    
    void display();
};

#endif