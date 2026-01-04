#include "warehouse.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <iomanip> // For formatting money
#include <limits> 
#include <sstream> // For log formatting 

using namespace std;

// ==========================================
// CONSTRUCTOR & STARTUP
// ==========================================
Warehouse::Warehouse() {
    loadWorkers();   // [NEW] Load Security Database first
    loadInventory(); // Load Product Data
    
    // Seed sample products if empty
    if (inventory.empty()) {
        cout << "[SYSTEM] Seeding initial inventory..." << endl;
        addProduct(101, "Gaming Laptop", 10, 1200.00, 950.00, "Computers", "TechCorp");
        addProduct(102, "Wireless Mouse", 50, 45.00, 25.00, "Accessories", "TechCorp");
        addProduct(103, "Mech Keyboard", 30, 85.00, 50.00, "Accessories", "TechCorp");
        addProduct(201, "PS5 Console", 15, 499.00, 450.00, "Gaming", "SonyHub");
        addProduct(202, "Elden Ring", 40, 59.99, 35.00, "Games", "FromSoft");
        
        while(!historyStack.isEmpty()) historyStack.pop();
        saveInventory(); 
        cout << "[SYSTEM] Initial inventory created." << endl;
    }
}

// ==========================================
// SECURITY & SHIFT MANAGEMENT
// ==========================================

void Warehouse::loadWorkers() {
    // [NEW] Valid Worker IDs
    // In a real app, you would read this from a secure file
    workerDB.root = workerDB.insertNode(workerDB.root, 101); // Worker A
    workerDB.root = workerDB.insertNode(workerDB.root, 102); // Worker B
    workerDB.root = workerDB.insertNode(workerDB.root, 205); // Manager
    workerDB.root = workerDB.insertNode(workerDB.root, 999); // Admin
    
    cout << "[SYSTEM] Worker Security Database loaded." << endl;
}

bool Warehouse::validateLogin(int id) {
    // Check if ID exists in the workerDB tree
    return workerDB.searchNode(workerDB.root, id);
}

void Warehouse::startShift(string shiftName, string opName, int opID) {
    currentShiftName = shiftName;
    operatorName = opName;
    operatorID = opID; // Store ID
    
    sessionRevenue = 0.0;
    sessionProfit = 0.0;
    sessionItemsSold = 0;
    sessionSalesLog.clear(); 
    
    cout << "\n[SYSTEM] Shift Started: " << shiftName 
         << " (Operator: " << opName << " | ID: " << opID << ")" << endl;
    cout << "[SYSTEM] Session trackers reset to 0." << endl;
}

void Warehouse::endShift() {
    cout << "\n==================================================" << endl;
    cout << "             END OF SHIFT REPORT                  " << endl;
    cout << "==================================================" << endl;
    cout << " Shift:    " << currentShiftName << endl;
    cout << " Operator: " << operatorName << " (ID: " << operatorID << ")" << endl; 
    cout << " --------------------------------------------------" << endl;
    cout << " Items Sold:      " << sessionItemsSold << endl;
    cout << " Session Revenue: $" << fixed << setprecision(2) << sessionRevenue << endl;
    cout << " Session Profit:  $" << fixed << setprecision(2) << sessionProfit << endl;
    cout << " --------------------------------------------------" << endl;
    cout << " DETAILED LOG:" << endl;
    if (sessionSalesLog.empty()) {
        cout << " (No sales this session)" << endl;
    } else {
        for (const auto& log : sessionSalesLog) {
            cout << " - " << log << endl;
        }
    }
    cout << " --------------------------------------------------" << endl;
    cout << " [SYSTEM] Logging out..." << endl;
    cout << "==================================================\n" << endl;

    // --- Persistent History Save ---
    ofstream historyFile("sales_history.txt", ios::app);
    if (historyFile) {
        historyFile << "=== SHIFT: " << currentShiftName 
                    << " | OP: " << operatorName << " (ID:" << operatorID << ") ===\n";
        historyFile << "Revenue: $" << fixed << setprecision(2) << sessionRevenue 
                    << " | Profit: $" << sessionProfit << "\n";
        for (const auto& log : sessionSalesLog) historyFile << " - " << log << "\n";
        historyFile << "--------------------------------------------------\n";
        historyFile.close();
    }
}

// ==========================================
// INVENTORY MANAGEMENT
// ==========================================
void Warehouse::addProduct(int id, string name, int quantity, double price, double cost, string category, string supplier) {
    if (idIndex.searchNode(idIndex.root, id)) {
        cout << "[ERROR] Product ID " << id << " already exists!" << endl;
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
    cout << "[OK] Product Added: " << name << endl;
}

int Warehouse::findProductIndex(int id) {
    return binarySearchRec(inventory, 0, inventory.size() - 1, id);
}

bool Warehouse::searchUsingTree(int id) {
    return idIndex.searchNode(idIndex.root, id);
}

void Warehouse::listInventory() {
    cout << "\n================================ WAREHOUSE INVENTORY ================================" << endl;
    if (inventory.empty()) {
        cout << "                                  (Warehouse is empty)                                  " << endl;
        cout << "=====================================================================================" << endl;
        return;
    }
    
    cout << left << setw(6)  << "ID" 
         << left << setw(20) << "Name" 
         << left << setw(10) << "Qty" 
         << left << setw(12) << "Price($)" 
         << left << setw(12) << "Cost($)" 
         << left << setw(15) << "Category" 
         << left << setw(15) << "Supplier" << endl;
         
    cout << string(90, '-') << endl;

    for (const Product &p : inventory) {
        cout << left << setw(6)  << p.getId() 
             << left << setw(20) << p.getName() 
             << left << setw(10) << p.getQuantity() 
             << left << setw(12) << fixed << setprecision(2) << p.getPrice()
             << left << setw(12) << fixed << setprecision(2) << p.getCost()
             << left << setw(15) << p.getCategory() 
             << left << setw(15) << p.getSupplier() << endl;
    }
    cout << "=====================================================================================\n" << endl;
}

void Warehouse::removeProduct(int id) {
    int index = findProductIndex(id);
    if (index == -1) {
        cout << "[ERROR] Product not found!" << endl;
        return;
    }

    Product p = inventory[index];
    inventory.erase(inventory.begin() + index);
    idIndex.root = idIndex.deleteNode(idIndex.root, id);
    layout.removeNode(id);
    
    Action act = {DELETE_PRODUCT, id, 0, p}; 
    historyStack.push(act);
    cout << "[OK] Product " << id << " deleted successfully." << endl;
}

void Warehouse::manualRestock(int id, int qty) {
    int index = findProductIndex(id);
    if (index == -1) {
        cout << "[ERROR] Product not found!" << endl;
        return;
    }
    inventory[index].restock(qty);
    cout << "[OK] Restocked " << qty << " units." << endl;
    
    Product dummy;
    Action act = {RESTOCK_PRODUCT, id, qty, dummy};
    historyStack.push(act);
}

// ==========================================
// UNDO & HISTORY
// ==========================================

void Warehouse::undoLastAction() {
    if (historyStack.isEmpty()) {
        cout << "[INFO] Nothing to undo." << endl;
        return;
    }

    Action last = historyStack.pop();
    int idx = findProductIndex(last.productID);

    switch (last.type) {
        case ADD_PRODUCT:
            cout << "Undoing ADD of ID " << last.productID << "..." << endl;
            {
                int rIdx = findProductIndex(last.productID);
                if(rIdx != -1) inventory.erase(inventory.begin() + rIdx);
                idIndex.root = idIndex.deleteNode(idIndex.root, last.productID);
                layout.removeNode(last.productID);
            }
            break;

        case SELL_PRODUCT:
            if (idx != -1) {
                inventory[idx].restock(last.quantity);
                double price = inventory[idx].getPrice();
                double cost = inventory[idx].getCost();
                double subtotal = price * last.quantity;
                double tax = subtotal * 0.14;
                double profit = (price - cost) * last.quantity;
                
                revenue -= (subtotal + tax);
                taxCollected -= tax;
                netProfit -= profit;
                cout << "Undoing SALE. Stock restored & Finances adjusted." << endl;
            }
            break;
            
        case RESTOCK_PRODUCT:
             if (idx != -1) {
                inventory[idx].sell(last.quantity); 
                cout << "Undoing RESTOCK. Stock reduced." << endl;
            }
            break;

        case DELETE_PRODUCT:
            cout << "Undoing DELETE. Restoring " << last.backupData.getName() << "..." << endl;
            inventory.push_back(last.backupData);
            bubbleSort(inventory);
            idIndex.root = idIndex.insertNode(idIndex.root, last.productID);
            layout.append(last.productID);
            break;
    }
}

void Warehouse::debugHistory() {
    cout << "\n[DEBUG] Requesting Stack Dump..." << endl;
    historyStack.printStack(); 
}

// ==========================================
// ORDER QUEUES
// ==========================================

void Warehouse::addToOrderQueue(int id, int qty, string payment, string name, string phone) {
    if (!searchUsingTree(id)) { 
        cout << "[ERROR] Product not found." << endl;
        return;
    }
    Order newOrder = {id, qty, payment, name, phone, nullptr};
    orderQueue.enqueue(newOrder);
}

void Warehouse::addVIPOrder(int id, int qty, string payment, string name, string phone) {
    if (!searchUsingTree(id)) { 
        cout << "[ERROR] Product not found." << endl;
        return;
    }
    Order newOrder = {id, qty, payment, name, phone, nullptr};
    vipQueue.enqueue(newOrder);
}

void Warehouse::viewPendingOrders() {
    cout << "\n   [Current Cart Status]" << endl;
    cout << "   Normal: ";
    orderQueue.display(); 
    cout << endl;
    cout << "   VIP:    ";
    vipQueue.display();   
    cout << "\n   ---------------------" << endl;
}

void Warehouse::processOrders() {
    cout << "\n--- Processing Orders ---" << endl;
    if (vipQueue.isEmpty() && orderQueue.isEmpty()) {
        cout << "No orders to process." << endl;
        return;
    }

    auto handleOrder = [&](Queue& q, string type) {
        while (!q.isEmpty()) {
            Order ord = q.dequeue();
            int index = findProductIndex(ord.id);
            
            if (index != -1) {
                if (inventory[index].getQuantity() >= ord.qty) {
                    inventory[index].sell(ord.qty); 
                    
                    double price = inventory[index].getPrice();
                    double cost = inventory[index].getCost();
                    double subtotal = price * ord.qty;      
                    double tax = subtotal * 0.14;           
                    double total = subtotal + tax;          
                    double profit = (price - cost) * ord.qty; 
                    
                    revenue += total;
                    taxCollected += tax;
                    netProfit += profit;
                    
                    sessionRevenue += total;
                    sessionProfit += profit;
                    sessionItemsSold += ord.qty;

                    cout << "[" << type << "] Sold: " << inventory[index].getName() 
                         << " x" << ord.qty 
                         << " to " << ord.customerName
                         << " | Total: $" << total << endl;

                    Product dummy;
                    Action act = {SELL_PRODUCT, ord.id, ord.qty, dummy};
                    historyStack.push(act);

                    stringstream ss;
                    ss << fixed << setprecision(2) << total;
                    string formattedTotal = ss.str();

                    string logEntry = "[" + type + "] " + inventory[index].getName() + 
                                      " (ID:" + to_string(ord.id) + ")" +
                                      " | Customer: " + ord.customerName + " (" + ord.customerPhone + ")" +
                                      " | Pay: " + ord.paymentMethod +
                                      " | Total: $" + formattedTotal;
                    salesLog.push_back(logEntry);
                    sessionSalesLog.push_back(logEntry);

                } else {
                    cout << "[ERROR] Not enough stock for " << inventory[index].getName() << endl;
                }
            }
        }
    };

    handleOrder(vipQueue, "VIP");
    handleOrder(orderQueue, "Normal");

    smartReorder();
    saveInventory(); 
    cout << "[OK] Orders processed.\n" << endl;
}

void Warehouse::smartReorder() {
    for (auto &p : inventory) {
        if (p.getQuantity() < 3) {
            cout << "[!] Auto Restock -> " << p.getName() << " +5 units\n";
            p.restock(5);
        }
    }
}

void Warehouse::showStorageLayout() {
    cout << "\n[STORAGE] Layout: ";
    layout.printList();
    cout << endl;
}

// ==========================================
// REPORTS & FILES
// ==========================================
void Warehouse::saveInventory() {
    ofstream file("inventory.txt");
    if (!file) {
        cout << "[ERROR] Could not save inventory!" << endl;
        return;
    }
    file << "Total Products: " << inventory.size() << endl;
    file << "ID|Name|Qty|Price|Cost|Category|Supplier" << endl; 
    for (const auto& p : inventory) {
        file << p.getId() << "|" 
             << p.getName() << "|" 
             << p.getQuantity() << "|" 
             << p.getPrice() << "|" 
             << p.getCost() << "|" 
             << p.getCategory() << "|" 
             << p.getSupplier() << endl;
    }
    
    ofstream finFile("financials.txt");
    if (finFile) {
        double costOfGoods = revenue - taxCollected - netProfit;
        finFile << "================ FINANCIAL STATEMENT ================" << endl;
        finFile << "Total Revenue (Gross): $" << fixed << setprecision(2) << revenue << endl;
        finFile << "(-) Tax Collected:     $" << fixed << setprecision(2) << taxCollected << endl;
        finFile << "(-) Cost of Goods:     $" << fixed << setprecision(2) << costOfGoods << endl;
        finFile << "-----------------------------------------------------" << endl;
        finFile << "(=) Net Profit:        $" << fixed << setprecision(2) << netProfit << endl;
        finFile << "=====================================================" << endl;
        finFile.close();
    }
    file.close();
    cout << "[SYSTEM] data saved." << endl;
}

void Warehouse::loadInventory() {
    ifstream file("inventory.txt");
    if (!file) {
        cout << "[SYSTEM] No save file found. Starting fresh." << endl;
        return;
    }
    
    inventory.clear();
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line.find("Total Products:") != string::npos) continue;
        if (line.find("ID|") != string::npos) continue;
        if (isdigit(line[0]) && line.find('|') == string::npos) continue;

        size_t pos = 0;
        vector<string> tokens;
        while ((pos = line.find('|')) != string::npos) {
            tokens.push_back(line.substr(0, pos));
            line.erase(0, pos + 1);
        }
        tokens.push_back(line); 

        if (tokens.size() >= 7) {
            int id = stoi(tokens[0]);
            string name = tokens[1];
            int qty = stoi(tokens[2]);
            double price = stod(tokens[3]);
            double cost = stod(tokens[4]); 
            string cat = tokens[5];
            string supp = tokens[6];
            addProduct(id, name, qty, price, cost, cat, supp);
        } else if (tokens.size() == 6) {
            int id = stoi(tokens[0]);
            string name = tokens[1];
            int qty = stoi(tokens[2]);
            double price = stod(tokens[3]);
            double cost = price * 0.7; 
            string cat = tokens[4];
            string supp = tokens[5];
            addProduct(id, name, qty, price, cost, cat, supp);
        }
    }
    
    while(!historyStack.isEmpty()) historyStack.pop();

    ifstream finFile("financials.txt");
    if (finFile) {
        string finLine;
        while (getline(finFile, finLine)) {
            size_t colonPos = finLine.find(':');
            if (colonPos != string::npos) {
                string valStr = finLine.substr(colonPos + 1);
                size_t numStart = valStr.find_first_of("0123456789.-");
                if (numStart != string::npos) {
                    double val = stod(valStr.substr(numStart));
                    if (finLine.find("Total Revenue") != string::npos) revenue = val;
                    else if (finLine.find("Tax Collected") != string::npos) taxCollected = val;
                    else if (finLine.find("Net Profit") != string::npos) netProfit = val;
                }
            }
        }
        finFile.close();
    }
    file.close();
    cout << "[SYSTEM] Data loaded successfully (" << inventory.size() << " items)." << endl;
}

void Warehouse::exportToFile() {
    ofstream file("warehouse_report.txt");
    if (!file) {
        cout << "[ERROR] Could not create report file.\n";
        return;
    }

    file << "====================================================================================================\n";
    file << "                                      ELECTRONICS STORE - MANAGEMENT REPORT                        \n";
    file << "====================================================================================================\n";
    file << " Shift: " << left << setw(20) << currentShiftName 
         << " Operator: " << operatorName << " (ID: " << operatorID << ")\n";
    file << " Generated: 2026-01-03\n";
    file << "====================================================================================================\n\n";
    
    file << "[CURRENT INVENTORY SNAPSHOT]\n";
    file << left << setw(6)  << "ID" 
         << left << setw(20) << "Name" 
         << left << setw(10) << "Qty" 
         << left << setw(12) << "Price($)" 
         << left << setw(12) << "Cost($)" 
         << left << setw(15) << "Category" 
         << left << setw(15) << "Supplier" << endl;
    file << string(95, '-') << endl;

    double totalStockValue = 0.0;
    for(const auto &p : inventory) {
        file << left << setw(6)  << p.getId() 
             << left << setw(20) << p.getName() 
             << left << setw(10) << p.getQuantity() 
             << left << setw(12) << fixed << setprecision(2) << p.getPrice()
             << left << setw(12) << fixed << setprecision(2) << p.getCost()
             << left << setw(15) << p.getCategory() 
             << left << setw(15) << p.getSupplier() << endl;
        totalStockValue += (p.getQuantity() * p.getCost());
    }

    file << "\n[FINANCIAL PERFORMANCE - LIFETIME]\n";
    double costOfGoods = revenue - taxCollected - netProfit;
    double margin = (revenue > 0) ? (netProfit / revenue) * 100.0 : 0.0;
    file << "--------------------------------------------------\n";
    file << " Total Cash In (Revenue): $" << fixed << setprecision(2) << revenue << "\n";
    file << " (-) Total Tax Collected: $" << taxCollected << "\n";
    file << " (-) Cost of Goods Sold:  $" << costOfGoods << "\n";
    file << " --------------------------------------------------\n";
    file << " NET PROFIT:              $" << netProfit << " (" << margin << "% Margin)\n";
    file << " Inventory Valuation:     $" << totalStockValue << " (at Cost)\n";
    file << "--------------------------------------------------\n\n";

    file << "[SHIFT SALES LOG - " << currentShiftName << "]\n";
    file << "--------------------------------------------------\n";
    if (sessionSalesLog.empty()) {
        file << " (No sales recorded this session)\n";
    } else {
        for (const auto& log : sessionSalesLog) {
            file << " " << log << "\n";
        }
    }
    file << "--------------------------------------------------\n\n";
    file << "=============================== [ END OF REPORT ] ===============================\n";
    file.close();
    cout << "[FILE] Report exported to 'warehouse_report.txt'.\n";
}

void Warehouse::showRevenue() {
    double costOfGoods = revenue - taxCollected - netProfit;
    double margin = (revenue > 0) ? (netProfit / revenue) * 100.0 : 0.0;
    cout << "\n================ FINANCIAL DASHBOARD ================" << endl;
    cout << " [LIFETIME TOTALS]" << endl;
    cout << "  Gross Revenue:    $" << fixed << setprecision(2) << revenue << endl;
    cout << "  (-) Tax (14%):    $" << taxCollected << endl;
    cout << "  (-) Cost of Sales: $" << costOfGoods << endl;
    cout << "  ------------------------------------" << endl;
    cout << "  NET PROFIT:       $" << netProfit << endl;
    cout << "  Profit Margin:    " << margin << "%" << endl;
    cout << "\n [CURRENT SHIFT: " << currentShiftName << "]" << endl;
    cout << "  Items Sold:       " << sessionItemsSold << endl;
    cout << "  Session Revenue:  $" << sessionRevenue << endl;
    cout << "  Session Profit:   $" << sessionProfit << endl;
    cout << "====================================================" << endl;
}

void Warehouse::sortByID() {
    bubbleSort(inventory); 
    cout << "[OK] Inventory sorted by ID.\n";
}

void Warehouse::sortByPrice() {
    for (size_t i = 0; i < inventory.size() - 1; i++)
        for (size_t j = 0; j < inventory.size() - i - 1; j++)
            if (inventory[j].getPrice() > inventory[j+1].getPrice())
                swap(inventory[j], inventory[j+1]);
    cout << "[OK] Inventory sorted by Price.\n";
}

void Warehouse::peekProduct(int id) {
    int index = findProductIndex(id);
    if (index != -1) {
        cout << "   [SCANNER] Found: " << inventory[index].getName() 
             << " | Unit Price: $" << inventory[index].getPrice() << endl;
    }
}