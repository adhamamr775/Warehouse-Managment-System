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
// CONSTRUCTOR
// ==========================================
Warehouse::Warehouse() {
    loadWorkers();   
    loadInventory(); 
    
    if (inventory.empty()) {
        addProduct(101, "Gaming Laptop", 10, 1200.00, 950.00, "Computers", "TechCorp", true);
        addProduct(102, "Wireless Mouse", 50, 45.00, 25.00, "Accessories", "TechCorp", true);
        saveInventory();
    }
}

// ==========================================
// WORKER MANAGEMENT
// ==========================================
void Warehouse::loadWorkers() {
    ifstream file("workers.txt");
    if (!file) {
        WorkerRecord w = {999, "Super Admin", "Admin", 0, 0.0, 0.0, 0};
        staffList.push_back(w);
        workerDB.root = workerDB.insertNode(workerDB.root, 999);
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
            
            int n_cnt = (tokens.size() > 3) ? stoi(tokens[3]) : 0;
            double b_prof = (tokens.size() > 4) ? stod(tokens[4]) : 0.0;
            double tot_earn = (tokens.size() > 5) ? stod(tokens[5]) : 0.0;
            int tot_ords = (tokens.size() > 6) ? stoi(tokens[6]) : 0; 

            WorkerRecord w = {id, name, role, n_cnt, b_prof, tot_earn, tot_ords};
            staffList.push_back(w);
            workerDB.root = workerDB.insertNode(workerDB.root, id);
        }
    }
    file.close();
}

void Warehouse::saveWorkers() {
    ofstream file("workers.txt");
    for (const auto& w : staffList) {
        file << w.id << "|" << w.name << "|" << w.role << "|" 
             << w.normalCount << "|" << w.batchProfit << "|" << w.totalEarnings << "|" << w.totalOrders << endl;
    }
    file.close();
}

void Warehouse::payWorkerBonus(int id) {
    bool found = false;
    for (auto& w : staffList) {
        if (w.id == id) {
            found = true;
            
            if (w.totalEarnings <= 0) {
                cout << "   [INFO] " << w.name << " has no pending bonus ($0.00).\n";
                return;
            }

            cout << "   [PAYROLL] Processing payout of $" << fixed << setprecision(2) << w.totalEarnings 
                 << " to " << w.name << "...\n";
            
            string msg = "[PAYOUT] Paid $" + to_string(w.totalEarnings) + " to " + w.name + " (ID:" + to_string(id) + ")";
            logEvent(msg);

            w.totalEarnings = 0.0;
            saveWorkers();
            
            cout << "   [SUCCESS] Payout recorded. Balance reset to $0.00.\n";
            break;
        }
    }
    if (!found) cout << "   [ERROR] Worker ID not found.\n";
}

// [UPDATED] Promote Worker with Security Checks
void Warehouse::promoteWorker(int id) {
    // 1. Prevent Self-Promotion
    if (id == operatorID) {
        cout << "   [SECURITY] You cannot promote yourself!\n";
        return;
    }

    bool found = false;
    for (auto& w : staffList) {
        if (w.id == id) {
            // 2. Prevent tampering with Admins
            if (w.role == "Admin") { 
                cout << "   [ERROR] Cannot change Admin status.\n"; 
                return; 
            }
            // 3. Prevent redundant promotions
            if (w.role == "Manager") { 
                cout << "   [INFO] User is already a Manager.\n"; 
                return; 
            }
            
            // 4. Logic: Managers can only promote Workers
            // (If we reached here, w.role is likely "Worker")
            
            w.role = "Manager";
            found = true;
            cout << "   [SUCCESS] Promoted " << w.name << " to Manager.\n";
            break;
        }
    }
    
    if (found) {
        saveWorkers(); 
    } else {
        cout << "   [ERROR] Worker ID not found.\n";
    }
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
        cout << "[ERROR] Worker ID " << id << " already exists!\n"; return; 
    }
    WorkerRecord w = {id, name, role, 0, 0.0, 0.0, 0};
    staffList.push_back(w);
    workerDB.root = workerDB.insertNode(workerDB.root, id);
    saveWorkers();
    cout << "[SUCCESS] Added " << role << ": " << name << endl;
}

void Warehouse::removeWorker(int id) {
    bool found = false;
    for (size_t i = 0; i < staffList.size(); i++) {
        if (staffList[i].id == id) {
            if (staffList[i].id == operatorID) { cout << "[ERROR] Cannot remove self!\n"; return; }
            staffList.erase(staffList.begin() + i);
            found = true;
            break;
        }
    }
    if (found) {
        saveWorkers();
        workerDB.root = workerDB.deleteNode(workerDB.root, id);
        cout << "[SUCCESS] Removed.\n";
    } else cout << "[ERROR] Not found.\n";
}

void Warehouse::listWorkers() {
    cout << "\n=================================== STAFF STATS ===================================\n";
    cout << left << setw(8) << "ID" << setw(20) << "Name" << setw(10) << "Role" 
         << setw(15) << "TOTAL ORDERS" << setw(15) << "Next Bonus" << setw(15) << "UNPAID BONUS" << endl;
    cout << "-----------------------------------------------------------------------------------" << endl;
    for (const auto& w : staffList) {
        string prog = to_string(w.normalCount % 5) + "/5"; 
        cout << left << setw(8) << w.id 
             << setw(20) << w.name 
             << setw(10) << w.role 
             << setw(15) << w.totalOrders 
             << setw(15) << prog
             << "$" << w.totalEarnings << endl;
    }
    cout << "===================================================================================\n";
}

bool Warehouse::workerExists(int id) { return workerDB.searchNode(workerDB.root, id); }
string Warehouse::getWorkerRole(int id) {
    for(const auto& w : staffList) if(w.id == id) return w.role;
    return "Unknown";
}

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

void Warehouse::archiveShift() {
    ofstream file("warehouse_report.txt", ios::app); 
    if (file) {
        file << "\n======================================================\n";
        file << "                   SHIFT REPORT                       \n";
        file << "======================================================\n";
        file << " Shift:    " << currentShiftName << "\n";
        file << " Operator: " << operatorName << " (ID: " << operatorID << ")\n";
        file << " ----------------------------------------------------\n";
        file << " Financials:\n";
        file << "  - Revenue: $" << fixed << setprecision(2) << sessionRevenue << "\n";
        file << "  - Items:   " << sessionItemsSold << "\n";
        
        for(const auto& w : staffList) {
            if(w.id == operatorID) {
                file << " ----------------------------------------------------\n";
                file << " OPERATOR STATS:\n";
                file << "  - Lifetime Orders: " << w.totalOrders << "\n"; 
                file << "  - Bonus Pending:   $" << w.totalEarnings << "\n";
            }
        }

        file << " ----------------------------------------------------\n";
        file << " ACTIVITY LOG:\n";
        if (sessionSalesLog.empty()) file << "  (No activity)\n";
        for (const auto& log : sessionSalesLog) file << "  " << log << "\n";
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
            cout << "[My Stats]:\n";
            cout << " Total Orders:   " << w.totalOrders << endl; 
            cout << " Pending Bonus:  " << (w.normalCount % 5) << "/5 orders\n";
            cout << " Unpaid Cash:    $" << w.totalEarnings << endl;
        }
    }
    cout << "----------------------------\n";
    cout << "[Log]:\n";
    for(const auto& l : sessionSalesLog) cout << " " << l << endl;
}

void Warehouse::logEvent(string msg) { sessionSalesLog.push_back(msg); }

void Warehouse::addProduct(int id, string name, int quantity, double price, double cost, string category, string supplier, bool silent) {
    if (idIndex.searchNode(idIndex.root, id)) { if (!silent) cout << "[ERROR] ID Taken!\n"; return; }
    Product p(id, name, quantity, price, cost, category, supplier);
    inventory.push_back(p);
    idIndex.root = idIndex.insertNode(idIndex.root, id);
    layout.append(id);
    bubbleSort(inventory); 
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
    Product p = inventory[idx];
    inventory.erase(inventory.begin() + idx);
    idIndex.root = idIndex.deleteNode(idIndex.root, id);
    layout.removeNode(id);
    Action act = {DELETE_PRODUCT, id, 0, p}; historyStack.push(act);
    saveInventory();
    cout << "[OK] Deleted.\n";
}

void Warehouse::manualRestock(int id, int qty) {
    int idx = findProductIndex(id);
    if(idx == -1) { cout << "Not Found.\n"; return; }
    inventory[idx].restock(qty);
    Product d; Action act = {RESTOCK_PRODUCT, id, qty, d}; historyStack.push(act);
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

void Warehouse::addToOrderQueue(int id, int qty, string pay, string n, string p) {
    Order o = {id, qty, pay, n, p, nullptr}; orderQueue.enqueue(o);
}
void Warehouse::addVIPOrder(int id, int qty, string pay, string n, string p) {
    Order o = {id, qty, pay, n, p, nullptr}; vipQueue.enqueue(o);
}

void Warehouse::processOrders() {
    cout << "\n[1] VIP  [2] Normal  [3] All  [0] Back: ";
    int choice; cin >> choice; if(choice==0) return;

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
                inventory[idx].sell(o.qty);
                
                double pr = inventory[idx].getPrice();
                double cost = inventory[idx].getCost();
                double sub = pr * o.qty;
                
                if(type=="VIP") sub *= 1.10; 
                double profit = (pr - cost) * o.qty;
                if(type=="VIP") profit += (sub - (pr*o.qty));

                if(currentWorker != nullptr) {
                    currentWorker->totalOrders++; 

                    if (type == "Normal") {
                        currentWorker->batchProfit += profit;
                        currentWorker->normalCount++;
                        if (currentWorker->normalCount % 5 == 0) {
                            double rate = (currentWorker->role == "Manager") ? 0.03 : 0.02; 
                            double bonus = currentWorker->batchProfit * rate;
                            currentWorker->totalEarnings += bonus;
                            cout << "   >>> BATCH BONUS! $" << bonus << " <<<\n";
                            currentWorker->batchProfit = 0; 
                        }
                    } 
                    else if (type == "VIP") {
                        double vRate = (currentWorker->role == "Manager") ? 0.07 : 0.05;
                        double vBonus = profit * vRate;
                        currentWorker->totalEarnings += vBonus;
                        cout << "   >>> VIP COMMISSION! $" << vBonus << " <<<\n";
                    }
                }

                revenue += (sub * 1.14); 
                netProfit += profit;
                sessionRevenue += (sub * 1.14); 
                sessionItemsSold += o.qty;

                stringstream ss; ss << fixed << setprecision(2) << (sub * 1.14);
                string l = "[" + type + "] " + inventory[idx].getName() + " x" + to_string(o.qty) + " | $" + ss.str();
                sessionSalesLog.push_back(l);
                
                Product d; Action act = {SELL_PRODUCT, o.id, o.qty, d}; historyStack.push(act);
                cout << "Processed: " << inventory[idx].getName() << " | Profit: $" << profit << endl;
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

void Warehouse::returnProduct(int id, int qty) {
    int idx = findProductIndex(id);
    if(idx == -1) { cout << "Not Found.\n"; return; }
    inventory[idx].restock(qty);
    double ref = (inventory[idx].getPrice() * qty) * 1.14;
    revenue -= ref; sessionRevenue -= ref;
    string l = "[RETURN] ID:" + to_string(id) + " | Refund: $" + to_string(ref);
    sessionSalesLog.push_back(l);
    saveInventory();
    cout << "[SUCCESS] Refunded $" << fixed << setprecision(2) << ref << endl;
}

void Warehouse::saveInventory() {
    ofstream f("inventory.txt");
    f << "Count: " << inventory.size() << endl << "Header" << endl;
    for(const auto& p : inventory) 
        f << p.getId() << "|" << p.getName() << "|" << p.getQuantity() << "|" << p.getPrice() << "|" << p.getCost() << "|" << p.getCategory() << "|" << p.getSupplier() << endl;
    f.close();
    
    ofstream ff("financials.txt");
    ff << "Rev: " << revenue << endl << "Tax: " << taxCollected << endl << "Prof: " << netProfit << endl;
    ff.close();
}

void Warehouse::loadInventory() {
    ifstream f("inventory.txt"); if(!f) return;
    inventory.clear(); string l;
    while(getline(f,l)) {
        if(isdigit(l[0]) && l.find('|')!=string::npos) {
            stringstream ss(l); string s; vector<string> t;
            while(getline(ss,s,'|')) t.push_back(s);
            if(t.size() >= 5) {
                addProduct(stoi(t[0]), t[1], stoi(t[2]), stod(t[3]), stod(t[4]), t.size()>5?t[5]:"Gen", t.size()>6?t[6]:"Gen", true);
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
    cout << "Lifetime Revenue: $" << revenue << endl;
    cout << "Lifetime Profit:  $" << netProfit << endl;
    cout << "Current Shift:    $" << sessionRevenue << endl;
}

void Warehouse::showStorageLayout() { layout.printList(); cout << endl; }
void Warehouse::sortByID() { bubbleSort(inventory); cout << "Sorted ID.\n"; }
void Warehouse::sortByPrice() { 
    for(size_t i=0; i<inventory.size()-1; i++) 
        for(size_t j=0; j<inventory.size()-i-1; j++) 
            if(inventory[j].getPrice() > inventory[j+1].getPrice()) swap(inventory[j], inventory[j+1]);
    cout << "Sorted Price.\n";
}

void Warehouse::undoLastAction() {
    if(historyStack.isEmpty()) { cout << "Nothing to undo.\n"; return; }
    Action a = historyStack.pop();
    if(a.type == ADD_PRODUCT) removeProduct(a.productID);
    else if(a.type == RESTOCK_PRODUCT || a.type == SELL_PRODUCT) {
        int idx = findProductIndex(a.productID);
        if(idx != -1) {
            if (a.type == RESTOCK_PRODUCT) inventory[idx].sell(a.quantity);
            else inventory[idx].restock(a.quantity);
        }
    }
    saveInventory();
    cout << "Undone.\n";
}

void Warehouse::debugHistory() { historyStack.printStack(); }
