#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <vector>
#include <string>
#include "product.h" 

// Data Structures & Algorithms
#include "../header/DataStructures/BST.h"
#include "../header/DataStructures/queue.h" 
#include "../header/DataStructures/stack.h"
#include "../header/DataStructures/Linkedlist.h"
#include "../header/algos/Searching.h"
#include "../header/algos/sorting.h"

using namespace std;

class Warehouse {
private:
    vector<Product> inventory;
    BST idIndex;
    LinkedList layout;
    Queue orderQueue;
    Queue vipQueue;
    Stack historyStack;
    
    // [NEW] Security Database
    BST workerDB; 
    
    double revenue = 0.0;     
    double netProfit = 0.0;   
    double taxCollected = 0.0;
    vector<string> salesLog;

    // Shift / Session Data
    string currentShiftName;
    string operatorName;
    int operatorID = 0;       // [NEW] Store Operator ID
    
    double sessionRevenue = 0.0;
    double sessionProfit = 0.0;
    int sessionItemsSold = 0;
    vector<string> sessionSalesLog; 

public:
    Warehouse();
    
    // Security & Shift Management
    void loadWorkers();             // [NEW] Load valid IDs
    bool validateLogin(int id);     // [NEW] Check ID
    void startShift(string shiftName, string opName, int opID); 
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
};

#endif