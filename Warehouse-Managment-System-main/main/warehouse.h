#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <vector>
#include <string>
#include <fstream> // Required for file handling
#include "product.h" 

// Data Structures & Algorithms
#include "../header/DataStructures/BST.h"
#include "../header/DataStructures/queue.h" 
#include "../header/DataStructures/stack.h"
#include "../header/DataStructures/Linkedlist.h"
#include "../header/algos/Searching.h"
#include "../header/algos/sorting.h"

using namespace std;

// [NEW] Struct to hold worker details
struct WorkerRecord {
    int id;
    string name;
    string role; // "Admin", "Manager", "Worker"
};

class Warehouse {
private:
    vector<Product> inventory;
    BST idIndex;
    LinkedList layout;
    Queue orderQueue;
    Queue vipQueue;
    Stack historyStack;
    
    // [NEW] Worker Management
    BST workerDB; // Still used for fast ID validation
    vector<WorkerRecord> staffList; // Stores full details (Role/Name)
    
    double revenue = 0.0;     
    double netProfit = 0.0;   
    double taxCollected = 0.0;
    vector<string> salesLog;

    // Shift / Session Data
    string currentShiftName;
    string operatorName;
    int operatorID = 0;       
    string operatorRole; // [NEW] Track current user's role
    
    double sessionRevenue = 0.0;
    double sessionProfit = 0.0;
    int sessionItemsSold = 0;
    vector<string> sessionSalesLog; 

public:
    Warehouse();
    
    // [NEW] Worker Management
    void loadWorkers();             // Load from workers.txt
    void saveWorkers();             // Save to workers.txt
    bool validateLogin(int id, string &retName, string &retRole); // Modified to return info
    void addNewWorker(int id, string name, string role);
    void removeWorker(int id);
    string getCurrentRole() { return operatorRole; } // Getter for Main.cpp

    void startShift(string shiftName, string opName, int opID, string role); // Updated
    void endShift();

    // Inventory
    void addProduct(int id, string name, int quantity, double price, double cost, string category, string supplier);
    void listInventory();
    
    int findProductIndex(int id);
    bool searchUsingTree(int id);
    
    // Orders
    void addToOrderQueue(int id, int qty, string payment, string name, string phone);
    void addVIPOrder(int id, int qty, string payment, string name, string phone);
    void processOrders();
    void viewPendingOrders(); 
    void smartReorder();
    
    // Management
    void manualRestock(int id, int qty);
    void removeProduct(int id);
    
    void undoLastAction();
    void debugHistory(); 
    
    // Reports & Sorting
    void showStorageLayout();
    void exportToFile();
    void saveInventory();
    void loadInventory();
    void showRevenue();
    void sortByID();
    void sortByPrice();
    void peekProduct(int id);
    // [NEW] Returns & Manager Override
    string getWorkerRole(int id);        // Helper to check ID role without full login
    void returnProduct(int id, int qty); // Handles the refund math
};

#endif
