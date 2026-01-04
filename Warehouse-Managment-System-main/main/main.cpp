#include <iostream>
#include <limits>
#include <string>
#include <vector> 
#include <iomanip> // [ADDED] Needed for neat tables
#include "warehouse.h" 

using namespace std;

// ------------------------------------------------------------------
// [SYSTEM CREDENTIALS]
// 999 | Adham Amr      | Admin   
// 205 | Mohamed Talaat | Manager 
// 101 | Mohanad Fattah | Worker  
// ------------------------------------------------------------------

struct CartItem {
    int id;
    int qty;
};

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string cleanInput(string input) {
    if (input.empty()) return "Cash";
    for (char &c : input) c = tolower(c);
    if (isalpha(input[0])) input[0] = toupper(input[0]);
    return input;
}

string formatName(string name) {
    if (name.empty()) return "Unknown";
    for (char &c : name) c = tolower(c);
    if (isalpha(name[0])) name[0] = toupper(name[0]);
    return name;
}

void displayMenu(string role) {
    cout << "\n==========================================" << endl;
    cout << "    SMART WAREHOUSE SYSTEM    " << endl;
    cout << "    Current Role: " << role << endl; 
    cout << "==========================================" << endl;
    cout << "1.  Add New Product" << endl;
    cout << "2.  View Inventory" << endl;
    cout << "3.  Search Product" << endl; 
    cout << "4.  Place Normal Order (Cart & Confirm)" << endl; 
    cout << "5.  Place VIP Order (Cart & Confirm)" << endl;    
    cout << "6.  Process All Orders" << endl;  
    cout << "7.  Show Storage Layout" << endl; 
    cout << "8.  Sort Inventory by ID" << endl;
    cout << "9.  Sort Inventory by Price" << endl;
    cout << "10. Export Report to File" << endl;
    cout << "11. View Total Revenue" << endl;
    cout << "------------------------------------------" << endl;
    cout << "12. DELETE Product" << endl;
    cout << "13. Manual Restock" << endl;
    cout << "14. UNDO Last Action" << endl;
    cout << "17. PROCESS RETURN " << endl; 
    
    if (role == "Admin" || role == "Manager") {
        cout << "------------------------------------------" << endl;
        cout << "15. [ADMIN] Add New Worker" << endl;
        cout << "16. [ADMIN] Remove Worker" << endl;
    }
    
    cout << "0.  End Shift (Logout)" << endl; 
    cout << "==========================================" << endl;
    cout << "Enter your choice: ";
}

int main() {
    Warehouse mySystem;
    
    while (true) {
        string shift;
        int opID = 0;
        bool isAuthenticated = false; 

        cout << "\n\n";
        cout << "##########################################" << endl;
        cout << "#       WAREHOUSE LOGIN            #" << endl;
        cout << "##########################################" << endl;
        cout << "   (Type '0' to Shutdown)                 " << endl;
        cout << "------------------------------------------" << endl;

        cout << "Enter Shift Name: ";
        cin.clear();
        getline(cin, shift);
        
        if (shift == "0") return 0;
        shift = formatName(shift); 

        string dbName, dbRole;
        
        while (!isAuthenticated) {
            cout << "Enter Employee ID: ";
            string idInput;
            getline(cin, idInput); 

            if (idInput == "0") return 0;

            try {
                if (!idInput.empty()) opID = stoi(idInput);
            } catch (...) {
                opID = -1;
            }

            if (mySystem.validateLogin(opID, dbName, dbRole)) {
                cout << "\n[SUCCESS] ID Verified." << endl;
                cout << "Welcome, " << dbName << " (" << dbRole << ")!" << endl;
                isAuthenticated = true; 
            } else {
                cout << "[ACCESS DENIED] ID " << opID << " not found." << endl;
            }
        }

        mySystem.startShift(shift, dbName, opID, dbRole);
        bool loggedIn = true;
        
        while (loggedIn) {
            displayMenu(mySystem.getCurrentRole()); 
            
            int choice;
            if (!(cin >> choice)) {
                cout << "[ERROR] Invalid input." << endl;
                clearInput();
                continue;
            }

            switch (choice) {
                case 1: {
                    if (mySystem.getCurrentRole() == "Worker") {
                        cout << "\n[ACCESS DENIED] Only Admins or Managers can add new products." << endl;
                        break; 
                    }
                    int id; 
                    cout << "\n--- Add New Product ---" << endl;
                    cout << "Enter Product ID: ";
                    while (!(cin >> id)) { cout << "Invalid input. ID: "; clearInput(); }
                    
                    if (mySystem.searchUsingTree(id)) { 
                        cout << "[ERROR] Product ID already exists!\n"; 
                        break; 
                    }
                    int qty; double price, cost; string name, cat, supp;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                    cout << "Enter Name: "; getline(cin, name);
                    cout << "Enter Quantity: "; cin >> qty;
                    cout << "Enter Selling Price: "; cin >> price;
                    cout << "Enter Cost Price: "; cin >> cost;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Enter Category: "; getline(cin, cat);
                    cout << "Enter Supplier: "; getline(cin, supp);
                    mySystem.addProduct(id, name, qty, price, cost, cat, supp);
                    break;
                }

                case 2: mySystem.listInventory(); break;
                
                case 3: { 
                    int id; cout << "Enter ID: "; cin >> id; 
                    if(mySystem.searchUsingTree(id)) mySystem.peekProduct(id); 
                    else cout << "[ERROR] Not found.\n"; 
                    break; 
                }

                // --- CASE 4 & 5: CART WITH NAMES, PRICES & TOTALS ---
                case 4: 
                case 5: { 
                    string type = (choice == 4) ? "NORMAL" : "VIP";
                    cout << "\n--- " << type << " Order (Cart Mode) ---" << endl;
                    
                    vector<CartItem> tempCart; 
                    string cName, cPhone, pMethod;
                    cout << "Customer Name: "; cin.ignore(); getline(cin, cName);
                    cout << "Phone: "; cin >> cPhone;
                    cout << "Payment: "; cin >> pMethod; pMethod = cleanInput(pMethod);

                    // 1. Fill Cart
                    while(true) {
                        int id, qty; 
                        cout << "Product ID (0 to finish): "; cin >> id;
                        if (id == 0) break;

                        if(mySystem.searchUsingTree(id)) { 
                            string pName = mySystem.getProductName(id); // [NEW] Show name immediately
                            cout << "  -> Selected: " << pName << "\n";
                            cout << "  Quantity: "; cin >> qty; 
                            tempCart.push_back({id, qty});
                            cout << "  [Cart] Item Added.\n"; 
                        } else {
                            cout << "  [ERROR] Not found." << endl;
                        }
                    }

                    // 2. Show Receipt & Confirm
                    if (tempCart.empty()) {
                        cout << "[CANCELLED] Cart empty." << endl;
                        break;
                    }

                    cout << "\n================= CONFIRM CART (" << type << ") =================" << endl;
                    cout << left << setw(5) << "ID" 
                         << left << setw(20) << "Name" 
                         << left << setw(8) << "Qty" 
                         << left << setw(10) << "Price" 
                         << left << setw(10) << "Subtotal" << endl;
                    cout << "---------------------------------------------------------" << endl;

                    double cartTotal = 0;
                    
                    for(auto &item : tempCart) {
                        string name = mySystem.getProductName(item.id);
                        double price = mySystem.getProductPrice(item.id);
                        double lineTotal = price * item.qty;
                        cartTotal += lineTotal;

                        cout << left << setw(5) << item.id 
                             << left << setw(20) << name 
                             << left << setw(8) << item.qty 
                             << "$" << left << setw(9) << price 
                             << "$" << lineTotal << endl;
                    }
                    cout << "---------------------------------------------------------" << endl;
                    
                    double finalTotal = cartTotal;
                    if (type == "VIP") {
                         cout << "Subtotal:       $" << cartTotal << endl;
                         cout << "VIP Fee (10%): +$" << (cartTotal * 0.10) << endl;
                         finalTotal = cartTotal * 1.10;
                    }
                    
                    cout << "ESTIMATED TOTAL: $" << fixed << setprecision(2) << finalTotal << endl;
                    cout << "=========================================================" << endl;
                    
                    char confirm;
                    cout << "Confirm Order? (y/n): "; cin >> confirm;

                    if (confirm == 'y' || confirm == 'Y') {
                        for (auto &item : tempCart) {
                            if (choice == 4) mySystem.addToOrderQueue(item.id, item.qty, pMethod, cName, cPhone);
                            else mySystem.addVIPOrder(item.id, item.qty, pMethod, cName, cPhone);
                        }
                        cout << "[SUCCESS] Order sent to " << type << " Queue." << endl;
                    } else {
                        cout << "[CANCELLED] Order discarded." << endl;
                    }
                    break;
                }

                case 6: mySystem.processOrders(); break;
                
                case 7: mySystem.showStorageLayout(); break;
                case 8: mySystem.sortByID(); break;
                case 9: mySystem.sortByPrice(); break;
                case 10: mySystem.exportToFile(); break;
                case 11: mySystem.showRevenue(); break;

                case 12: { 
                    if (mySystem.getCurrentRole() == "Worker") {
                        cout << "\n[ACCESS DENIED] Only Admins or Managers can delete products." << endl;
                        break;
                    }
                    int id; cout << "Delete ID: "; cin >> id; 
                    mySystem.removeProduct(id); 
                    break; 
                }

                case 13: { 
                    if (mySystem.getCurrentRole() == "Worker") {
                        cout << "\n[ACCESS DENIED] Only Admins or Managers can restock inventory." << endl;
                        break;
                    }
                    int id, q; 
                    cout << "\n--- Manual Restock ---" << endl;
                    cout << "Restock ID: "; cin >> id; 
                    cout << "Qty to add: "; cin >> q; 
                    mySystem.manualRestock(id, q); 
                    break; 
                }

                case 14: 
                    mySystem.debugHistory(); 
                    cout << "Undo? (y/n): "; char c; cin >> c; 
                    if(c=='y'||c=='Y') mySystem.undoLastAction(); 
                    break;
                
                case 17: {
                    cout << "\n--- Process Return / Refund ---" << endl;
                    char warranty;
                    cout << "Check Warranty (y/n): "; cin >> warranty;
                    if (warranty == 'n' || warranty == 'N') {
                        cout << "[REJECTED] Warranty Invalid." << endl;
                        break;
                    }

                    if (mySystem.getCurrentRole() == "Worker") {
                        int authID;
                        cout << "Manager ID Required: "; cin >> authID;
                        string authRole = mySystem.getWorkerRole(authID);
                        if (authRole != "Admin" && authRole != "Manager") {
                            cout << "[ACCESS DENIED] Invalid Manager ID." << endl;
                            break; 
                        }
                        cout << "[AUTHORIZED] Override accepted." << endl;
                    }

                    int id, qty;
                    cout << "Product ID: "; cin >> id;
                    if (!mySystem.searchUsingTree(id)) { cout << "[ERROR] ID not found." << endl; break; }
                    cout << "Return Qty: "; cin >> qty;
                    mySystem.returnProduct(id, qty);
                    break;
                }

                case 15: {
                    if (mySystem.getCurrentRole() != "Admin" && mySystem.getCurrentRole() != "Manager") {
                        cout << "[ACCESS DENIED]" << endl; break;
                    }
                    int nid; string nname, nrole;
                    cout << "New ID: "; cin >> nid; cin.ignore();
                    cout << "Name: "; getline(cin, nname); nname = formatName(nname); 
                    cout << "Role: "; cin >> nrole; nrole = formatName(nrole); 
                    mySystem.addNewWorker(nid, nname, nrole);
                    break;
                }
                
                case 16: {
                    if (mySystem.getCurrentRole() != "Admin" && mySystem.getCurrentRole() != "Manager") {
                        cout << "[ACCESS DENIED]" << endl; break;
                    }
                    int rid; cout << "Worker ID to remove: "; cin >> rid;
                    mySystem.removeWorker(rid);
                    break;
                }

                case 0: 
                    mySystem.saveInventory();
                    mySystem.endShift(); 
                    cout << "\n[INFO] Logging out..." << endl;
                    cin.ignore(); cin.get();
                    loggedIn = false; 
                    break;

                default: cout << "[ERROR] Invalid choice." << endl;
            }
        }
    }
}
