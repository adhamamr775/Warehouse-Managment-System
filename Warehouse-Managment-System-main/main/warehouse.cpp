#include "warehouse.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <iomanip> 
#include <limits> 
#include <sstream> 

using namespace std;

// ==========================================
// CONSTRUCTOR & DATA LOADING
// ==========================================
Warehouse::Warehouse() {
    loadWorkers();   
    loadInventory(); 
    
    // Safety check: Seed items if file was totally empty/missing
    if (inventory.empty()) {
        cout << "[SYSTEM] Inventory empty. creating defaults...\n";
        addProduct(101, "Gaming Laptop", 10, 1200.00, 950.00, "Computers", "TechCorp");
        addProduct(102, "Wireless Mouse", 50, 45.00, 25.00, "Accessories", "TechCorp");
        saveInventory();
    }
}

// ==========================================
// WORKER MANAGEMENT
// ==========================================
void Warehouse::loadWorkers() {
    ifstream file("workers.txt");
    if (!file) {
        // Create default admin if file missing
        addNewWorker(999, "Super Admin", "Admin"); 
        return;
    }

    staffList.clear();
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string segment;
        vector<string> tokens;
        while(getline(ss, segment, '|')) tokens.push_back(segment);
        
        if (tokens.size() >= 3) {
            int id = stoi(tokens[0]);
            string name = tokens[1];
            string role = tokens[2];
            WorkerRecord w = {id, name, role};
            staffList.push_back(w);
            workerDB.root = workerDB.insertNode(workerDB.root, id);
        }
    }
    file.close();
}

void Warehouse::saveWorkers() {
    ofstream file("workers.txt");
    for (const auto& w : staffList) {
        file << w.id << "|" << w.name << "|" << w.role << endl;
    }
    file.close();
}

bool Warehouse::validateLogin(int id, string &retName, string &retRole) {
    if (!workerDB.searchNode(workerDB.root, id)) return false;
    for (const auto& w : staffList) {
        if (w.id == id) { 
            retName = w.name; 
            retRole = w.role; 
            return true; 
        }
    }
    return false;
}

void Warehouse::addNewWorker(int id, string name, string role) {
    if (workerDB.searchNode(workerDB.root, id)) { 
        cout << "[ERROR] Worker ID " << id << " already exists!\n"; 
        return; 
    }
    WorkerRecord w = {id, name, role};
    staffList.push_back(w);
    workerDB.root = workerDB.insertNode(workerDB.root, id);
    saveWorkers();
    cout << "[SUCCESS] Added " << role << ": " << name << endl;
}

void Warehouse::removeWorker(int id) {
    bool found = false;
    for (size_t i = 0; i < staffList.size(); i++) {
        if (staffList[i].id == id) {
            if (staffList[i].id == operatorID) { 
                cout << "[ERROR] You cannot delete yourself!\n"; 
                return; 
            }
            staffList.erase(staffList.begin() + i);
            found = true;
            break;
        }
    }
    if (found) {
        saveWorkers();
        workerDB.root = workerDB.deleteNode(workerDB.root, id);
        cout << "[SUCCESS] Worker removed.\n";
    } else {
        cout << "[ERROR] ID not found.\n";
    }
}

string Warehouse::getWorkerRole(int id) {
    for(const auto& w : staffList) {
        if(w.id == id) return w.role;
    }
    return "Unknown";
}

// ==========================================
// SHIFT MANAGEMENT
// ==========================================
void Warehouse::startShift(string shiftName, string opName, int opID, string role) {
    currentShiftName = shiftName;
    operatorName = opName;
    operatorID = opID; 
    operatorRole = role;
    
    sessionRevenue = 0.0; 
    sessionProfit = 0.0; 
    sessionItemsSold = 0;
    sessionSalesLog.clear(); 
    
    cout << "\n[SYSTEM] Shift Started: " << shiftName << " | Operator: " << opName << endl;
}

// In warehouse.cpp

void Warehouse::endShift() {
    cout << "\n=== END OF SHIFT REPORT ===\n";
    cout << "Items Sold: " << sessionItemsSold << endl;
    cout << "Session Revenue: $" << fixed << setprecision(2) << sessionRevenue << endl;
    cout << "Session Profit:  $" << fixed << setprecision(2) << sessionProfit << endl;
    
    // [NEW] Automatically generate the text file report
    exportToFile(); 
    
    // Append to History File (Log book)
    ofstream historyFile("sales_history.txt", ios::app);
    if (historyFile) {
        historyFile << "=== SHIFT: " << currentShiftName << " | OP: " << operatorName << " ===\n";
        historyFile << "Revenue: $" << fixed << setprecision(2) << sessionRevenue 
                    << " | Profit: $" << sessionProfit << "\n";
        for (const auto& log : sessionSalesLog) historyFile << " - " << log << "\n";
        historyFile << "--------------------------------------------------\n";
        historyFile.close();
    }
}
// ==========================================
// INVENTORY ACTIONS
// ==========================================
void Warehouse::addProduct(int id, string name, int quantity, double price, double cost, string category, string supplier) {
    if (idIndex.searchNode(idIndex.root, id)) { 
        cout << "[ERROR] Product ID Taken!\n"; 
        return; 
    }
    Product newProd(id, name, quantity, price, cost, category, supplier);
    inventory.push_back(newProd);
    
    idIndex.root = idIndex.insertNode(idIndex.root, id);
    layout.append(id);
    
    Product dummy; 
    Action act = {ADD_PRODUCT, id, quantity, dummy}; 
    historyStack.push(act);

    bubbleSort(inventory); 
    saveInventory();
    cout << "[OK] Product Added: " << name << "\n";
}

int Warehouse::findProductIndex(int id) {
    return binarySearchRec(inventory, 0, inventory.size() - 1, id);
}

bool Warehouse::searchUsingTree(int id) { 
    return idIndex.searchNode(idIndex.root, id); 
}

void Warehouse::listInventory() {
    cout << "\n=== INVENTORY ===\n";
    cout << left << setw(6) << "ID" << setw(20) << "Name" << setw(6) << "Qty" << setw(10) << "Price" << endl;
    cout << "------------------------------------------" << endl;
    for (const Product &p : inventory) {
        cout << left << setw(6) << p.getId() << setw(20) << p.getName() 
             << setw(6) << p.getQuantity() << setw(10) << p.getPrice() << endl;
    }
    cout << endl;
}

void Warehouse::removeProduct(int id) {
    int index = findProductIndex(id);
    if (index == -1) { cout << "Not found.\n"; return; }
    
    Product p = inventory[index];
    inventory.erase(inventory.begin() + index);
    idIndex.root = idIndex.deleteNode(idIndex.root, id);
    layout.removeNode(id);
    
    Action act = {DELETE_PRODUCT, id, 0, p}; 
    historyStack.push(act);
    
    saveInventory();
    cout << "[OK] Deleted.\n";
}

void Warehouse::manualRestock(int id, int qty) {
    int index = findProductIndex(id);
    if (index != -1) {
        inventory[index].restock(qty);
        
        Product dummy;
        Action act = {RESTOCK_PRODUCT, id, qty, dummy};
        historyStack.push(act);
        
        saveInventory();
        cout << "[OK] Restocked.\n";
    } else {
        cout << "[ERROR] Product not found.\n";
    }
}

void Warehouse::peekProduct(int id) {
    int idx = findProductIndex(id);
    if(idx != -1) {
        cout << "Found: " << inventory[idx].getName() 
             << " | Price: $" << inventory[idx].getPrice() << endl;
    }
}string Warehouse::getProductName(int id) {
    int idx = findProductIndex(id);
    if (idx != -1) return inventory[idx].getName();
    return "Unknown";
}

double Warehouse::getProductPrice(int id) {
    int idx = findProductIndex(id);
    if (idx != -1) return inventory[idx].getPrice();
    return 0.0;
}

// ==========================================
// ORDER PROCESSING (VIP & SELECTIVE)
// ==========================================

void Warehouse::addToOrderQueue(int id, int qty, string payment, string name, string phone) {
    Order newOrder = {id, qty, payment, name, phone, nullptr};
    orderQueue.enqueue(newOrder);
}

void Warehouse::addVIPOrder(int id, int qty, string payment, string name, string phone) {
    Order newOrder = {id, qty, payment, name, phone, nullptr};
    vipQueue.enqueue(newOrder);
}

void Warehouse::processOrders() {
    cout << "\n==========================================" << endl;
    cout << "         PENDING ORDER QUEUES            " << endl;
    cout << "==========================================" << endl;
    cout << " [1] VIP Queue    (Pending: "; vipQueue.display(); cout << ")" << endl;
    cout << " [2] Normal Queue (Pending: "; orderQueue.display(); cout << ")" << endl;
    cout << " [3] Process EVERYTHING (Auto)" << endl;
    cout << " [0] Cancel / Go Back" << endl;
    cout << "------------------------------------------" << endl;
    cout << "Decision: ";
    
    int choice;
    cin >> choice;

    if (choice == 0) return;

    // Helper Lambda to process a specific queue
    auto processQueue = [&](Queue& q, string type) {
        if (q.isEmpty()) {
            cout << "[INFO] " << type << " queue is empty.\n";
            return;
        }
        
        cout << "\n--- Processing " << type << " Orders ---" << endl;
        while (!q.isEmpty()) {
            Order ord = q.dequeue();
            int index = findProductIndex(ord.id);
            
            if (index != -1) {
                if (inventory[index].getQuantity() >= ord.qty) {
                    
                    // 1. Update Stock
                    inventory[index].sell(ord.qty); 
                    
                    // 2. Financials Math
                    double price = inventory[index].getPrice();
                    double cost = inventory[index].getCost();
                    double subtotal = price * ord.qty; 
                    
                    // [VIP LOGIC] +10% Surcharge
                    if (type == "VIP") {
                        subtotal = subtotal * 1.10; 
                    }

                    double tax = subtotal * 0.14;           
                    double total = subtotal + tax;          
                    double profit = (price - cost) * ord.qty; 
                    
                    // Add VIP surcharge purely to profit
                    if (type == "VIP") profit += (subtotal - (price * ord.qty)); 
                    
                    // 3. Update Global Totals
                    revenue += total;
                    taxCollected += tax;
                    netProfit += profit;
                    
                    sessionRevenue += total;
                    sessionProfit += profit;
                    sessionItemsSold += ord.qty;

                    // Display Receipt Info
                    cout << "Processing: " << inventory[index].getName() << " x" << ord.qty << endl;
                    if (type == "VIP") cout << " -> VIP Surcharge Applied (+10%)" << endl;
                    cout << " -> Total Receipt: $" << fixed << setprecision(2) << total << endl;

                    // Log Logic
                    stringstream ss; ss << fixed << setprecision(2) << total;
                    string logEntry = "[" + type + "] " + inventory[index].getName() + 
                                      " (ID:" + to_string(ord.id) + ")" +
                                      " | Total: $" + ss.str();
                    salesLog.push_back(logEntry); // Lifetime log
                    sessionSalesLog.push_back(logEntry); // Session log
                    
                    // Add to Undo Stack
                    Product dummy;
                    Action act = {SELL_PRODUCT, ord.id, ord.qty, dummy};
                    historyStack.push(act);

                } else {
                    cout << "[ERROR] Out of stock: " << inventory[index].getName() << endl;
                }
            } else {
                cout << "[ERROR] Product ID " << ord.id << " no longer exists.\n";
            }
        }
    };

    // Execute based on selection
    if (choice == 1 || choice == 3) processQueue(vipQueue, "VIP");
    if (choice == 2 || choice == 3) processQueue(orderQueue, "Normal");

    saveInventory(); // Immediate Save ensures files are never empty
    cout << "\n[SYSTEM] Processing Complete. Files Updated.\n" << endl;
}

void Warehouse::viewPendingOrders() {
    cout << "Normal: "; orderQueue.display(); cout << endl;
    cout << "VIP:    "; vipQueue.display(); cout << endl;
}

void Warehouse::smartReorder() {
    // Optional auto-reorder logic stub
}

// ==========================================
// RETURNS
// ==========================================
void Warehouse::returnProduct(int id, int qty) {
    int index = findProductIndex(id);
    if (index == -1) {
        cout << "[ERROR] Product not found.\n";
        return;
    }
    
    // 1. Restore Stock
    inventory[index].restock(qty);
    
    // 2. Reverse Money
    double price = inventory[index].getPrice();
    double cost = inventory[index].getCost();
    
    double subtotal = price * qty;
    double tax = subtotal * 0.14;
    double totalRefund = subtotal + tax; 
    double profitReversal = (price - cost) * qty;

    revenue -= totalRefund;
    taxCollected -= tax;
    netProfit -= profitReversal;

    sessionRevenue -= totalRefund;
    sessionProfit -= profitReversal;
    sessionItemsSold -= qty;

    string logEntry = "[RETURN] ID:" + to_string(id) + " | Qty:" + to_string(qty) + " | Refund: $" + to_string(totalRefund);
    sessionSalesLog.push_back(logEntry);

    saveInventory();
    cout << "[SUCCESS] Refund Processed & Stock Restored.\n";
}

// ==========================================
// FILES & REPORTS
// ==========================================
void Warehouse::saveInventory() {
    // 1. Inventory File
    ofstream file("inventory.txt");
    file << "Total Products: " << inventory.size() << endl;
    file << "ID|Name|Qty|Price|Cost|Category|Supplier" << endl; 
    for (const auto& p : inventory) {
        file << p.getId() << "|" << p.getName() << "|" << p.getQuantity() << "|" 
             << p.getPrice() << "|" << p.getCost() << "|" << p.getCategory() << "|" << p.getSupplier() << endl;
    }
    file.close();
    
    // 2. Financials File
    ofstream finFile("financials.txt"); 
    finFile << "Total Revenue: " << revenue << endl;
    finFile << "Tax Collected: " << taxCollected << endl;
    finFile << "Net Profit: " << netProfit << endl;
    finFile.close();
}

void Warehouse::loadInventory() {
    ifstream file("inventory.txt");
    if (!file) return; 
    
    inventory.clear();
    string line;
    while (getline(file, line)) {
        if (isdigit(line[0]) && line.find('|') != string::npos) {
            stringstream ss(line);
            string seg; vector<string> t;
            while(getline(ss, seg, '|')) t.push_back(seg);
            
            // Handle optional fields logic
            if (t.size() >= 7) {
                 addProduct(stoi(t[0]), t[1], stoi(t[2]), stod(t[3]), stod(t[4]), t[5], t[6]);
            } else if (t.size() >= 5) {
                 // Fallback for older file formats
                 addProduct(stoi(t[0]), t[1], stoi(t[2]), stod(t[3]), stod(t[4]), "General", "Generic");
            }
        }
    }
    file.close();

    // Load Financials
    ifstream finFile("financials.txt");
    if (finFile) {
        string tag; double val;
        // Simple parsing: looks for numbers after labels
        while (finFile >> tag >> tag >> val) { 
            if (tag == "Revenue:") revenue = val;
            if (tag == "Collected:") taxCollected = val;
            if (tag == "Profit:") netProfit = val;
        }
        finFile.close();
    }
}

void Warehouse::exportToFile() {
    ofstream file("warehouse_report.txt");
    file << "=======================================\n";
    file << "       SHIFT REPORT: " << currentShiftName << "\n";
    file << "=======================================\n";
    file << "Operator: " << operatorName << " (ID: " << operatorID << ")\n";
    file << "Revenue:  $" << revenue << "\nProfit:   $" << netProfit << "\n";
    file << "\n--- Inventory Snapshot ---\n";
    for(const auto &p : inventory) {
        file << p.getName() << " (ID:" << p.getId() << ") Qty: " << p.getQuantity() << "\n";
    }
    file.close();
    cout << "[FILE] Report Exported to warehouse_report.txt\n";
}

void Warehouse::showRevenue() {
    cout << "\n=== FINANCIAL DASHBOARD ===\n";
    cout << "Total Revenue: $" << revenue << endl;
    cout << "Total Profit:  $" << netProfit << endl;
    cout << "Shift Revenue: $" << sessionRevenue << endl;
}

// ==========================================
// HELPERS & UNDO
// ==========================================
void Warehouse::showStorageLayout() { layout.printList(); cout << endl; }
void Warehouse::sortByID() { bubbleSort(inventory); cout << "Sorted by ID.\n"; }
void Warehouse::sortByPrice() { 
    for (size_t i = 0; i < inventory.size() - 1; i++)
        for (size_t j = 0; j < inventory.size() - i - 1; j++)
            if (inventory[j].getPrice() > inventory[j+1].getPrice()) swap(inventory[j], inventory[j+1]);
    cout << "Sorted by Price.\n";
}

void Warehouse::undoLastAction() {
    if (historyStack.isEmpty()) { cout << "Nothing to undo.\n"; return; }
    Action last = historyStack.pop();
    
    // Simple undo logic
    if (last.type == ADD_PRODUCT) {
        removeProduct(last.productID);
        cout << "Undid Add Product.\n";
    } else if (last.type == RESTOCK_PRODUCT) {
         int idx = findProductIndex(last.productID);
         if(idx!=-1) inventory[idx].sell(last.quantity);
         cout << "Undid Restock.\n";
    } else if (last.type == SELL_PRODUCT) {
         int idx = findProductIndex(last.productID);
         if(idx!=-1) inventory[idx].restock(last.quantity);
         cout << "Undid Sale (Stock restored).\n";
    }
    saveInventory();
}

void Warehouse::debugHistory() {
    historyStack.printStack();
}
int Warehouse::getProductQuantity(int id) {
    int idx = findProductIndex(id);
    if (idx != -1) return inventory[idx].getQuantity();
    return 0;
}
