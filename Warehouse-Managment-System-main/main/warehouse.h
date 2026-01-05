#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "product.h" 
#include "../header/DataStructures/BST.h"
#include "../header/DataStructures/queue.h" 
#include "../header/DataStructures/stack.h"
#include "../header/DataStructures/Linkedlist.h"
#include "../header/algos/Searching.h"
#include "../header/algos/sorting.h"

using namespace std;
struct WorkerRecord {
    // --- IDENTITY ---
    int id;
    string name;
    string role; 

    // --- FINANCIALS (New!) ---
    double salary;          // Base Salary (Needed for % calculation)
    double totalEarnings;   // This acts as the "Wallet" for bonuses

    // --- METRICS ---
    int totalOrders;        // Total lifetime orders
    int vipOrders;          // Count of VIP orders (New!)

    // --- INTERNAL TRACKERS (For Batch Logic) ---
    int normalCount;        // Tracks progress (e.g., 3/5 orders)
    double batchProfit;     // Temporary profit pool for the current batch
};

class Warehouse {
private:
    vector<Product> inventory;
    BST idIndex;
    LinkedList layout;
    Queue orderQueue;
    Queue vipQueue;
    Stack historyStack;
    BST workerDB; 
    vector<WorkerRecord> staffList; 
    
    double revenue = 0.0;     
    double netProfit = 0.0;   
    double taxCollected = 0.0;
    vector<string> salesLog; 

    string currentShiftName;
    string operatorName;
    int operatorID = 0;       
    string operatorRole; 
    
    double sessionRevenue = 0.0;
    double sessionProfit = 0.0;
    int sessionItemsSold = 0;
    vector<string> sessionSalesLog; 

public:
    Warehouse();
    
    void loadWorkers();             
    void saveWorkers();             
    bool validateLogin(int id, string &retName, string &retRole); 
   void addNewWorker(int id, string name, string role, double salary);
    bool workerExists(int id);
    void removeWorker(int id);
    void promoteWorker(int id); 
    void payWorkerBonus(int id); // [NEW]
    void listWorkers(); 
    
    string getCurrentRole() { return operatorRole; } 
    string getWorkerRole(int id); 

    void startShift(string shiftName, string opName, int opID, string role); 
    void endShift();

    void addProduct(int id, string name, int quantity, double price, double cost, string category, string supplier, bool silent = false);
    void listInventory();
    int findProductIndex(int id);
    bool searchUsingTree(int id);
    void removeProduct(int id);
    void manualRestock(int id, int qty);
    void peekProduct(int id);
    
    void addToOrderQueue(int id, int qty, string payment, string name, string phone);
    void addVIPOrder(int id, int qty, string payment, string name, string phone);
    void processOrders();     
    void viewPendingOrders(); 
    void smartReorder();
    
    void returnProduct(int id, int qty); 

    void undoLastAction();
    void debugHistory(); 
    
    void showStorageLayout();
    void exportToFile();
    void saveInventory();
    void loadInventory();
    void showRevenue();
    void logEvent(string message); 
    void printShiftReport();       
    void archiveShift();
    void clearShiftHistory();           
    
    void sortByID();
    void sortByPrice();
    string getProductName(int id); 
    double getProductPrice(int id);
    int getProductQuantity(int id);
};

#endif
