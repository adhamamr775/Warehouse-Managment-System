#include "warehouse.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <iomanip> 
#include <limits> 
#include <sstream> 
#include <map> 
#include <vector>

using namespace std;

// ==========================================
// CONSTRUCTOR
// ==========================================
Warehouse::Warehouse() {
    loadWorkers();   
    loadInventory(); 
    
    // Seed defaults only if inventory is empty
    if (inventory.empty()) {
        addProduct(101, "Gaming Laptop", 10, 1200.00, 950.00, "Computers", "TechCorp", true);
        addProduct(102, "Wireless Mouse", 50, 45.00, 25.00, "Accessories", "TechCorp", true);
        saveInventory();
    }
}

// ==========================================
// WORKER MANAGEMENT (FIXED)
// ==========================================
void Warehouse::loadWorkers() {
    ifstream file("workers.txt");
    staffList.clear();

    if (file) {
        string line;
        // Skip Header
        getline(file, line); 

        while (getline(file, line)) {
            if (line.empty()) continue;
            // Skip lines that are just headers repeated
            if (!isdigit(line[0])) continue;

            stringstream ss(line);
            string segment;
            vector<string> tokens;
            while(getline(ss, segment, '|')) tokens.push_back(segment);
            // ID | Name | Role | Salary | TotalOrders | VIPOrders | Wallet
            if (tokens.size() >= 7) { 
                try {
                    WorkerRecord w;
                    w.id = stoi(tokens[0]);
                    w.name = tokens[1];
                    w.role = tokens[2];
                    
                    // --- NEW DATA MAPPING ---
                    w.salary = stod(tokens[3]);        // Column 4 is Salary
                    w.totalOrders = stoi(tokens[4]);   // Column 5 is Total Orders
                    w.vipOrders = stoi(tokens[5]);     // Column 6 is VIP Count
                    w.totalEarnings = stod(tokens[6]); // Column 7 is Wallet
                    
                    // Reset internal counters (not saved in file)
                    w.normalCount = w.totalOrders % 5; 
                    w.batchProfit = 0.0;

                    staffList.push_back(w);
                } catch (...) { continue; }
            }
        }
        file.close();
    }

    if (staffList.empty()) {
        cout << "[SYSTEM] No workers found. Creating Default Admin.\n";
        // ID, Name, Role, Salary, Wallet, TotalOrders, VIP, NormalCount, BatchProf
        WorkerRecord admin = {999, "Admin", "Admin", 10000.0, 0.0, 0, 0, 0, 0.0};
        staffList.push_back(admin);
        saveWorkers();
    }
}
void Warehouse::saveWorkers() {
    ofstream file("workers.txt");
    //Header
    file << "ID|Name|Role|Salary|TotalOrders|VIPOrders|Wallet" << endl;
    for (const auto& w : staffList) {
        file << w.id << "|" 
             << w.name << "|" 
             << w.role << "|" 
             << w.salary << "|"        // Save Salary
             << w.totalOrders << "|"   // Save Orders
             << w.vipOrders << "|"     // Save VIP Count
             << w.totalEarnings << endl; // Save Wallet
    }
    file.close();
}

bool Warehouse::validateLogin(int id, string &retName, string &retRole) {
    for (const auto& w : staffList) {
        if (w.id == id) { 
            retName = w.name; 
            retRole = w.role; 
            return true; 
        }
    }
    return false;
}


void Warehouse::payWorkerBonus(int id) {
    bool found = false;
    for (auto& w : staffList) {
        if (w.id == id) {
            found = true;
            if (w.totalEarnings <= 0) {
                cout << "   [INFO] " << w.name << " has no pending bonus to pay.\n";
                return;
            }
            cout << "\n   $$$ PAYROLL PROCESSING $$$\n";
            cout << "   --------------------------\n";
            cout << "   Worker: " << w.name << endl;
            cout << "   Amount: $" << fixed << setprecision(2) << w.totalEarnings << endl;
            string msg = "[PAYOUT] Paid $" + to_string(w.totalEarnings) + " to " + w.name + " (ID:" + to_string(id) + ")";
            logEvent(msg);
            w.totalEarnings = 0.0; 
            saveWorkers();        
            cout << "   [SUCCESS] Funds Transferred. Balance reset to $0.00.\n";
            break;
        }
    }
    if (!found) cout << "   [ERROR] Worker ID not found.\n";
}

void Warehouse::promoteWorker(int id) {
    if (id == operatorID) { cout << "   [SECURITY] You cannot promote yourself!\n"; return; }
    bool found = false;
    for (auto& w : staffList) {
        if (w.id == id) {
            if (w.role == "Admin") { cout << "   [ERROR] Cannot change Admin status.\n"; return; }
            if (w.role == "Manager") { cout << "   [INFO] User is already a Manager.\n"; return; }
            w.role = "Manager";
            found = true;
            cout << "   [SUCCESS] Promoted " << w.name << " to Manager.\n";
            logEvent("[ADMIN] Promoted " + w.name + " (ID:" + to_string(id) + ") to Manager");
            break;
        }
    }
    if (found) saveWorkers(); 
    else cout << "   [ERROR] Worker ID not found.\n";
}

void Warehouse::addNewWorker(int id, string name, string role, double salary) {
    // Check vector directly for duplicates
    for(const auto& w : staffList) { 
        if(w.id == id) { cout << "[ERROR] ID Exists!\n"; return; }
    }

    // ID, Name, Role, Salary, Wallet, TotalOrders, VIP, NormalCount, BatchProfit
    WorkerRecord w = {id, name, role, salary, 0.0, 0, 0, 0, 0.0};
    
    staffList.push_back(w);
    saveWorkers(); // This saves it to workers.txt with the new format
    
    cout << "[SUCCESS] Added " << role << ": " << name << " (Salary: $" << salary << ")\n";
}

void Warehouse::removeWorker(int id) {
    bool found = false;
    for (size_t i = 0; i < staffList.size(); i++) {
        if (staffList[i].id == id) {
            if (staffList[i].id == operatorID) { cout << "[ERROR] Cannot remove yourself during shift!\n"; return; }
            staffList.erase(staffList.begin() + i);
            found = true;
            break;
        }
    }
    if (found) {
        saveWorkers();
        cout << "[SUCCESS] Worker removed.\n";
    } else cout << "[ERROR] Not found.\n";
}

void Warehouse::listWorkers() {
    cout << "\n=================================== STAFF STATS ===================================\n";
    cout << left << setw(8) << "ID" << setw(15) << "Name" << setw(10) << "Role" 
         << setw(15) << "LIFETIME ORDS" << setw(15) << "BONUS PROG" << setw(15) << "UNPAID CASH" << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    for (const auto& w : staffList) {
        string prog = to_string(w.normalCount % 5) + "/5 Orders"; 
        cout << left << setw(8) << w.id 
             << setw(15) << w.name 
             << setw(10) << w.role 
             << setw(15) << w.totalOrders 
             << setw(15) << prog
             << "$" << fixed << setprecision(2) << w.totalEarnings << endl;
    }
    cout << "===================================================================================\n";
}

bool Warehouse::workerExists(int id) { 
    for(const auto& w : staffList) if(w.id == id) return true;
    return false;
}

string Warehouse::getWorkerRole(int id) {
    for(const auto& w : staffList) if(w.id == id) return w.role;
    return "Unknown";
}

// ==========================================
// SHIFT & REPORTS
// ==========================================
void Warehouse::startShift(string shiftName, string opName, int opID, string role) {
    currentShiftName = shiftName;
    operatorName = opName;
    operatorID = opID; 
    operatorRole = role;
    sessionRevenue = 0.0; sessionProfit = 0.0; sessionItemsSold = 0;
    sessionSalesLog.clear(); 
    cout << "\n[SYSTEM] Shift Started: " << shiftName << " | Operator: " << opName << endl;
}

void Warehouse::endShift() {
    printShiftReport(); 
    archiveShift();     
    saveInventory();    
}

void Warehouse::clearShiftHistory() {
    ofstream file("warehouse_report.txt", ios::trunc); 
    if (file) {
        file << "=== REPORT HISTORY CLEARED ===\n"; 
        file.close();
        cout << "   [SUCCESS] Reports wiped.\n";
    } else {
        cout << "   [ERROR] Access denied.\n";
    }
}

void Warehouse::archiveShift() {
    ofstream file("warehouse_report.txt", ios::app); 
    if (file) {
        file << "\n======================================================\n";
        file << "                  SHIFT REPORT                        \n";
        file << "======================================================\n";
        file << " Date/Time: (System Timestamp)\n";
        file << " Shift:    " << currentShiftName << "\n";
        file << " Operator: " << operatorName << " (ID: " << operatorID << ")\n";
        file << " ----------------------------------------------------\n";
        
        // DYNAMIC SALES SUMMARY 
        file << " ITEMS SOLD THIS SHIFT:\n";
        if (sessionSalesLog.empty()) {
            file << "  (No activity)\n";
        } else {
            map<string, int> counts;
            for (const string& line : sessionSalesLog) {
                if (line.find("[Normal]") != string::npos || line.find("[VIP]") != string::npos) {
                    size_t typeEnd = line.find("] ");
                    size_t qtyStart = line.rfind(" x"); 
                    if (typeEnd != string::npos && qtyStart != string::npos) {
                        string pName = line.substr(typeEnd + 2, qtyStart - (typeEnd + 2));
                        string sQty = line.substr(qtyStart + 2, line.find(" |") - (qtyStart + 2));
                        counts[pName] += stoi(sQty);
                    }
                }
            }
            for (auto const& [name, count] : counts) {
                file << "  - " << left << setw(20) << name << ": " << count << " units\n";
            }
        }

        file << " ----------------------------------------------------\n";
        file << " FINANCIALS:\n";
        file << "  - Session Revenue: $" << fixed << setprecision(2) << sessionRevenue << "\n";
        file << "  - Session Profit:  $" << sessionProfit << "\n";
        file << "======================================================\n";
        file.close();
    }
}

void Warehouse::printShiftReport() {
    cout << "\n--- LIVE SHIFT DASHBOARD ---\n";
    cout << "Operator: " << operatorName << "\n";
    cout << "Revenue:  $" << sessionRevenue << "\n";
    
    for(const auto& w : staffList) {
        if(w.id == operatorID) {
            cout << " > My Orders:     " << w.totalOrders << endl; 
            cout << " > Bonus Prog:    " << (w.normalCount % 5) << "/5\n";
            cout << " > Payable Cash:  $" << w.totalEarnings << endl;
            break;
        }
    }
    cout << "----------------------------\n";
    cout << "[Recent Logs]:\n";
    int start = (sessionSalesLog.size() > 5) ? sessionSalesLog.size() - 5 : 0;
    for(size_t i = start; i < sessionSalesLog.size(); ++i) {
        cout << " " << sessionSalesLog[i] << endl;
    }
}

void Warehouse::logEvent(string msg) { sessionSalesLog.push_back(msg); }

// ==========================================
// INVENTORY
// ==========================================
void Warehouse::addProduct(int id, string name, int quantity, double price, double cost, string category, string supplier, bool silent) {
    if (idIndex.searchNode(idIndex.root, id)) { if (!silent) cout << "[ERROR] ID Taken!\n"; return; }
    
    Product p(id, name, quantity, price, cost, category, supplier);
    inventory.push_back(p);
    
    idIndex.root = idIndex.insertNode(idIndex.root, id);
    layout.append(id);
    bubbleSort(inventory); 

    
    Action act = {ADD_PRODUCT, id, quantity, p}; 
    historyStack.push(act);

    
    saveInventory();
    if (!silent) cout << "[OK] Added: " << name << "\n";
}
int Warehouse::findProductIndex(int id) { return binarySearchRec(inventory, 0, inventory.size() - 1, id); }
bool Warehouse::searchUsingTree(int id) { return idIndex.searchNode(idIndex.root, id); }

void Warehouse::listInventory() {
    cout << "\n=== INVENTORY ===\n";
    cout << left << setw(6) << "ID" << setw(20) << "Name" << setw(6) << "Qty" << setw(10) << "Price" << endl;
    for (const auto &p : inventory) {
        cout << left << setw(6) << p.getId() << setw(20) << p.getName() 
             << setw(6) << p.getQuantity() << setw(10) << p.getPrice() << endl;
    }
}

void Warehouse::removeProduct(int id) {
    int idx = findProductIndex(id);
    if(idx == -1) { cout << "Not Found.\n"; return; }
    
    Product p = inventory[idx]; // Keep a copy for undoing
    
    inventory.erase(inventory.begin() + idx);
    idIndex.root = idIndex.deleteNode(idIndex.root, id);
    layout.removeNode(id);
    
    // This pushes the action correctly
    Action act = {DELETE_PRODUCT, id, 0, p}; 
    historyStack.push(act);
    
    saveInventory();
    cout << "[OK] Deleted.\n";
}
void Warehouse::manualRestock(int id, int qty) {
    int idx = findProductIndex(id);
    if(idx == -1) { cout << "Not Found.\n"; return; }
    
    inventory[idx].restock(qty);
    Product d; 
    Action act = {RESTOCK_PRODUCT, id, qty, d}; 
    historyStack.push(act);
    
    saveInventory();
    cout << "[OK] Restocked.\n";
}

void Warehouse::peekProduct(int id) {
    int idx = findProductIndex(id);
    if(idx!=-1) cout << "Found: " << inventory[idx].getName() << " | $" << inventory[idx].getPrice() << endl;
}

string Warehouse::getProductName(int id) { int i=findProductIndex(id); return (i!=-1)?inventory[i].getName():"Unknown"; }
double Warehouse::getProductPrice(int id) { int i=findProductIndex(id); return (i!=-1)?inventory[i].getPrice():0.0; }
int Warehouse::getProductQuantity(int id) { int i=findProductIndex(id); return (i!=-1)?inventory[i].getQuantity():0; }

// ==========================================
// ORDERS & BONUS LOGIC
// ==========================================
void Warehouse::addToOrderQueue(int id, int qty, string pay, string n, string p) {
    Order o = {id, qty, pay, n, p, nullptr}; orderQueue.enqueue(o);
}
void Warehouse::addVIPOrder(int id, int qty, string pay, string n, string p) {
    Order o = {id, qty, pay, n, p, nullptr}; vipQueue.enqueue(o);
}

void Warehouse::processOrders() {
    cout << "\n[1] VIP  [2] Normal  [3] All  [0] Back: ";
    int choice; cin >> choice; if(choice==0) return;

    // Identify current worker
    WorkerRecord* currentWorker = nullptr;
    for(auto &w : staffList) {
        if(w.id == operatorID) { currentWorker = &w; break; }
    }

    auto proc = [&](Queue& q, string type) {
        if(q.isEmpty()) { cout << type << " queue empty.\n"; return; }
        
        cout << "--- Processing " << type << " ---\n";
        while(!q.isEmpty()) {
            Order o = q.dequeue();
            int idx = findProductIndex(o.id);
            
            if(idx != -1 && inventory[idx].getQuantity() >= o.qty) {
                // Sell Logic
                inventory[idx].sell(o.qty);
                double pr = inventory[idx].getPrice();
                double cost = inventory[idx].getCost();
                double sub = pr * o.qty;
                
                if(type=="VIP") sub *= 1.10; // VIP Markup
                
                double itemProfit = (sub - (cost * o.qty)); 
                if(currentWorker != nullptr) {
                    currentWorker->totalOrders++; 

                    if (type == "Normal") {
                        currentWorker->batchProfit += itemProfit;
                        currentWorker->normalCount++;
                        
                        // Trigger Bonus every 5 orders
                        if (currentWorker->normalCount % 5 == 0) {
                            double rate = (currentWorker->role == "Manager") ? 0.03 : 0.02; 
                            double bonus = currentWorker->batchProfit * rate;
                            
                            currentWorker->totalEarnings += bonus;
                            
                            // Deduct bonus from company net profit
                            netProfit -= bonus;
                            sessionProfit -= bonus;

                            cout << "   >>> BATCH BONUS! Added $" << bonus << " to wallet <<<\n";
                            logEvent("[BONUS] Batch Reward: $" + to_string(bonus) + " to " + currentWorker->name);
                            
                            currentWorker->batchProfit = 0; // Reset batch pool
                        }
                    } 
                    else if (type == "VIP") {
    currentWorker->vipOrders++; // Increment VIP count

    // --- SALARY PERCENTAGE BONUS LOGIC ---
    // Example: Manager gets 5% of Salary, Worker gets 2% of Salary per VIP order
    double percentage = (currentWorker->role == "Manager") ? 0.05 : 0.02;
    double vBonus = currentWorker->salary * percentage; 

    // Add to wallet
    currentWorker->totalEarnings += vBonus;
    
    // Deduct from company profit (Expense)
    netProfit -= vBonus;
    sessionProfit -= vBonus;

    cout << "   >>> VIP BONUS! Added $" << vBonus << " (Based on Salary) <<<\n";
    logEvent("[BONUS] VIP Reward: $" + to_string(vBonus) + " to " + currentWorker->name);
}
                }
                // ---------------------------------------------

                revenue += sub; 
                netProfit += itemProfit;
                sessionRevenue += sub; 
                sessionProfit += itemProfit;
                sessionItemsSold += o.qty;

                // Log format used for archiving
                stringstream ss; ss << fixed << setprecision(2) << sub;
                string l = "[" + type + "] " + inventory[idx].getName() + " x" + to_string(o.qty) + " | $" + ss.str();
                sessionSalesLog.push_back(l);
                
                Product d; Action act = {SELL_PRODUCT, o.id, o.qty, d}; historyStack.push(act);
                cout << "Processed: " << inventory[idx].getName() << " | Profit: $" << itemProfit << endl;
            } else cout << "[ERROR] Stock issue for ID " << o.id << endl;
        }
    };

    if(choice==1||choice==3) proc(vipQueue, "VIP");
    if(choice==2||choice==3) proc(orderQueue, "Normal");
    
    saveInventory(); 
    saveWorkers(); 
}

void Warehouse::viewPendingOrders() {
    cout << "Normal: "; orderQueue.display(); cout << endl;
    cout << "VIP:    "; vipQueue.display(); cout << endl;
}
void Warehouse::smartReorder() {}

// ==========================================
// RETURNS
// ==========================================
void Warehouse::returnProduct(int id, int qty) {
    int idx = findProductIndex(id);
    if(idx == -1) { cout << "Not Found.\n"; return; }
    
    inventory[idx].restock(qty);
    double ref = (inventory[idx].getPrice() * qty);
    
    revenue -= ref; sessionRevenue -= ref;
    
    string l = "[RETURN] ID:" + to_string(id) + " | Refund: $" + to_string(ref);
    sessionSalesLog.push_back(l);
    
    saveInventory();
    cout << "[SUCCESS] Refunded $" << fixed << setprecision(2) << ref << endl;
}

// ==========================================
// FILES & UTILS
// ==========================================
void Warehouse::saveInventory() {
    ofstream f("inventory.txt");
    f << "Count: " << inventory.size() << endl;
    f << "ID|Name|Qty|Price|Cost|Category|Supplier" << endl; 
    for(const auto& p : inventory) 
        f << p.getId() << "|" << p.getName() << "|" << p.getQuantity() << "|" 
          << p.getPrice() << "|" << p.getCost() << "|" 
          << p.getCategory() << "|" << p.getSupplier() << endl;
    f.close();
    
    ofstream ff("financials.txt");
    ff << "Rev: " << revenue << endl << "Tax: " << taxCollected << endl << "Prof: " << netProfit << endl;
    ff.close();
}

void Warehouse::loadInventory() {
    ifstream f("inventory.txt"); 
    if(!f) return;
    
    inventory.clear(); 
    string l;
    getline(f, l); // Skip Count
    getline(f, l); // Skip Header

    while(getline(f, l)) {
        if(l.empty()) continue;
        if(isdigit(l[0]) && l.find('|') != string::npos) {
            stringstream ss(l); string s; vector<string> t;
            while(getline(ss,s,'|')) t.push_back(s);
            if(t.size() >= 5) {
                addProduct(stoi(t[0]), t[1], stoi(t[2]), stod(t[3]), stod(t[4]), 
                           t.size()>5?t[5]:"Gen", t.size()>6?t[6]:"Gen", true);
            }
        }
    }
    f.close();
    
    ifstream ff("financials.txt");
    if(ff) { string k; double v; while(ff >> k >> v) { if(k=="Rev:") revenue=v; else if(k=="Prof:") netProfit=v; } }
}

void Warehouse::exportToFile() { archiveShift(); } 

void Warehouse::showRevenue() {
    cout << "\n=== FINANCIAL DASHBOARD ===\n";
    cout << left << setw(20) << "METRIC" << "VALUE" << endl;
    cout << "------------------------------\n";
    cout << left << setw(20) << "Lifetime Revenue:" << "$" << revenue << endl;
    cout << left << setw(20) << "Lifetime Profit:"  << "$" << netProfit << endl;
    cout << left << setw(20) << "Current Shift:"    << "$" << sessionRevenue << endl;
}

void Warehouse::showStorageLayout() { layout.printList(); cout << endl; }
void Warehouse::sortByID() { 
    // 1. Sort the vector (Inventory)
    bubbleSort(inventory); 

    // 2. Wipe the visual layout (Linked List)
    layout.clear(); 

    // 3. Re-append IDs from the NOW SORTED vector into the layout
    for(const auto& p : inventory) {
        layout.append(p.getId());
    }

    cout << "[SUCCESS] Inventory sorted. Storage layout updated.\n";
}
void Warehouse::sortByPrice() { 
    for(size_t i=0; i<inventory.size()-1; i++) 
        for(size_t j=0; j<inventory.size()-i-1; j++) 
            if(inventory[j].getPrice() > inventory[j+1].getPrice()) swap(inventory[j], inventory[j+1]);
    cout << "Sorted Price.\n";
}

void Warehouse::undoLastAction() {
    if(historyStack.isEmpty()) { 
        cout << "   [INFO] Nothing to undo.\n"; 
        return; 
    }
    
    Action a = historyStack.pop();
    
    // CASE A: Undoing a Delete -> Put it back
    if(a.type == DELETE_PRODUCT) {
        inventory.push_back(a.p); 
        idIndex.root = idIndex.insertNode(idIndex.root, a.productID);
        layout.append(a.productID);
        bubbleSort(inventory); 
        cout << "   [UNDO] Restored: " << a.p.getName() << endl;
    }
    // CASE B: Undoing an Add -> Remove it
    else if(a.type == ADD_PRODUCT) {
        // We use our existing remove function, but we wrap it to be silent
        int idToRemove = a.productID;
        int idx = findProductIndex(idToRemove);
        if(idx != -1) {
            inventory.erase(inventory.begin() + idx);
            idIndex.root = idIndex.deleteNode(idIndex.root, idToRemove);
            layout.removeNode(idToRemove);
            cout << "   [UNDO] Removed accidentally added product ID: " << idToRemove << endl;
        }
    }
    // CASE C: Undoing Restock or Sell -> Reverse the quantity
    else if(a.type == RESTOCK_PRODUCT || a.type == SELL_PRODUCT) {
        int idx = findProductIndex(a.productID);
        if(idx != -1) {
            // If we restocked, we now sell (subtract)
            if (a.type == RESTOCK_PRODUCT) inventory[idx].sell(a.quantity);
            // If we sold, we now restock (add)
            else inventory[idx].restock(a.quantity);
            cout << "   [UNDO] Quantity change reversed for ID: " << a.productID << endl;
        }
    }
    
    saveInventory();
}
void Warehouse::debugHistory() { historyStack.printStack(); }
