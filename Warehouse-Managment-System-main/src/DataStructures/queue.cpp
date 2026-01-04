#include "../../header/DataStructures/queue.h"

// --- Constructor ---
Queue::Queue() {
    front = nullptr;
    rear = nullptr;
}

// --- Destructor: Free all remaining orders ---
Queue::~Queue() {
    while (!isEmpty()) {
        dequeue();
    }
}

// --- Check if Empty ---
bool Queue::isEmpty() {
    return front == nullptr;
}

// --- Enqueue (Add to Rear) ---
void Queue::enqueue(Order inputData) {
    // 1. Create a new node in memory
    Order* temp = new Order();
    
    // 2. Fill it with data
    temp->id = inputData.id;
    temp->qty = inputData.qty;
    temp->paymentMethod = inputData.paymentMethod;
    temp->customerName = inputData.customerName;
    temp->customerPhone = inputData.customerPhone;
    
    // 3. Set pointer
    temp->next = nullptr;

    // 4. Link it into the queue
    if (isEmpty()) {
        front = rear = temp;
    } else {
        rear->next = temp; // Link old rear to new node
        rear = temp;       // Update rear pointer
    }
}

// --- Dequeue (Remove from Front) ---
Order Queue::dequeue() {
    // Handle empty case
    if (isEmpty()) {
        // Return a "dummy" empty order
        return {0, 0, "", "", "", nullptr};
    }

    // 1. Save the node to delete
    Order* temp = front;
    
    // 2. Save the data to return
    Order item = *front; 

    // 3. Move front pointer
    front = front->next;

    // 4. If queue is now empty, reset rear
    if (front == nullptr) {
        rear = nullptr;
    }

    // 5. Free memory and return data
    delete temp;
    return item;
}

// --- Display (Print List) ---
void Queue::display() {
    if (isEmpty()) {
        cout << "(Empty)";
        return;
    }

    Order* temp = front;
    while (temp != nullptr) {
        cout << "[ID:" << temp->id 
             << " x" << temp->qty 
             << " (" << temp->customerName << ")] -> ";
        temp = temp->next;
    }
    cout << "END";
}