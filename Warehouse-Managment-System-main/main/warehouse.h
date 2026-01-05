#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

// Includes for your custom data structures
#include "product.h" 
#include "../header/DataStructures/BST.h"
#include "../header/DataStructures/queue.h" 
#include "../header/DataStructures/stack.h"
#include "../header/DataStructures/Linkedlist.h"
#include "../header/algos/Searching.h"
#include "../header/algos/sorting.h"

using namespace std;

// Struct to hold worker details from workers.txt
struct WorkerRecord {
    int id;
    string name;
    string role; // "Admin", "Manager", "Worker"
};

class Warehouse {
private:
    // Core Data
    vector<Product> inventory;
    BST idIndex;
    LinkedList layout;
    
    // Orders
    Queue orderQueue;
    Queue vipQueue;
    Stack historyStack;
    
    // Worker Management
    BST workerDB; 
    vector<WorkerRecord> staffList; 
    
    // Financials (Lifetime)
    double revenue = 0.0;     
    double netProfit = 0.0;   
    double taxCollected = 0.0;
    vector<string> salesLog; // Lifetime log

    // Session / Shift Data
    string currentShiftName;
    string operatorName;
    int operatorID = 0;       
    string operatorRole; 
    
    double sessionRevenue = 0.0;
    double sessionProfit = 0.0;
    int sessionItemsSold = 0;
    vector<string> sessionSalesLog; // Current shift log

public:
    Warehouse();
    
    // --- WORKER MANAGEMENT ---
    void loadWorkers();             
    void saveWorkers();             
    bool validateLogin(int id, string &retName, string &retRole); 
    void addNewWorker(int id, string name, string role);
    void removeWorker(int id);
    string getCurrentRole() { return operatorRole; } 
    string getWorkerRole(int id); // Helper for Manager Override

    // --- SHIFT CONTROL ---
    void startShift(string shiftName, string opName, int opID, string role); 
    void endShift();

    // --- INVENTORY ---
    void addProduct(int id, string name, int quantity, double price, double cost, string category, string supplier);
    void listInventory();
    int findProductIndex(int id);
    bool searchUsingTree(int id);
    void removeProduct(int id);
    void manualRestock(int id, int qty);
    void peekProduct(int id);
    
    // --- ORDERS & QUEUES ---
    void addToOrderQueue(int id, int qty, string payment, string name, string phone);
    void addVIPOrder(int id, int qty, string payment, string name, string phone);
    void processOrders();     // UPDATED: Selective Processing + VIP Pricing
    void viewPendingOrders(); 
    void smartReorder();
    
    // --- RETURNS ---
    void returnProduct(int id, int qty); // UPDATED: Handles refunds

    // --- UNDO / DEBUG ---
    void undoLastAction();
    void debugHistory(); 
    
    // --- REPORTS & FILES ---
    void showStorageLayout();
    void exportToFile();
    void saveInventory();
    void loadInventory();
    void showRevenue();
    
    // --- SORTING ---
    void sortByID();
    void sortByPrice();
    string getProductName(int id); 
    double getProductPrice(int id);
    int getProductQuantity(int id);
};

#endif
