#include <iostream>
#include <limits>
#include <string>
#include "warehouse.h" 

using namespace std;

// ------------------------------------------------------------------
// [SYSTEM CREDENTIALS - Loaded from workers.txt]
// 
// 999 | Adham Amr      | Admin   (Full Access)
// 205 | Mohamed Talaat | Manager (Full Access)
// 101 | Mohanad Fattah | Worker  (Sales Only)
// ------------------------------------------------------------------

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
    cout << "4.  Place Normal Order" << endl;
    cout << "5.  Place VIP Order (Priority)" << endl; 
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
    cout << "17. PROCESS RETURN " << endl; // warranty check + manager override
    
    // [ADMIN ONLY]
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
    
    // ====================================================
    // OUTER LOOP: LOGIN SCREEN
    // ====================================================
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

        // --- 1. Get Shift Name ---
        cout << "Enter Shift Name: ";
        cin.clear();
        getline(cin, shift);
        
        if (shift == "0") return 0;
        shift = formatName(shift); 

        // --- 2. SECURE ID CHECK ---
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

        // ====================================================
        // INNER LOOP: MAIN MENU
        // ====================================================
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
                // --- CASE 1: ADD PRODUCT (SECURE) ---
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

                case 4: { 
                    cout << "\n--- Normal Order ---" << endl;
                    string cName, cPhone, pMethod;
                    cout << "Customer Name: "; cin.ignore(); getline(cin, cName);
                    cout << "Phone: "; cin >> cPhone;
                    cout << "Payment: "; cin >> pMethod; pMethod = cleanInput(pMethod);
                    while(true) {
                        int id, qty; cout << "Product ID: "; cin >> id;
                        if(mySystem.searchUsingTree(id)) { 
                            cout << "Qty: "; cin >> qty; 
                            mySystem.addToOrderQueue(id, qty, pMethod, cName, cPhone); 
                            cout << "[OK] Added.\n"; 
                        } else cout << "[ERROR] Not found.\n";
                        cout << "More? (y/n): "; char m; cin >> m; if(m=='n'||m=='N') break;
                    }
                    break;
                }

                case 5: { 
                    cout << "\n--- VIP Order ---" << endl;
                    string cName, cPhone, pMethod;
                    cout << "VIP Name: "; cin.ignore(); getline(cin, cName);
                    cout << "Phone: "; cin >> cPhone;
                    cout << "Payment: "; cin >> pMethod; pMethod = cleanInput(pMethod);
                    while(true) {
                        int id, qty; cout << "Product ID: "; cin >> id;
                        if(mySystem.searchUsingTree(id)) { 
                            cout << "Qty: "; cin >> qty; 
                            mySystem.addVIPOrder(id, qty, pMethod, cName, cPhone); 
                            cout << "[VIP] Added.\n"; 
                        } else cout << "[ERROR] Not found.\n";
                        cout << "More? (y/n): "; char m; cin >> m; if(m=='n'||m=='N') break;
                    }
                    break;
                }

                case 6: mySystem.processOrders(); break;
                case 7: mySystem.showStorageLayout(); break;
                case 8: mySystem.sortByID(); break;
                case 9: mySystem.sortByPrice(); break;
                case 10: mySystem.exportToFile(); break;
                case 11: mySystem.showRevenue(); break;

                // --- CASE 12: DELETE PRODUCT (SECURE) ---
                case 12: { 
                    if (mySystem.getCurrentRole() == "Worker") {
                        cout << "\n[ACCESS DENIED] Only Admins or Managers can delete products." << endl;
                        break;
                    }
                    int id; 
                    cout << "Delete ID: "; cin >> id; 
                    mySystem.removeProduct(id); 
                    break; 
                }

                // --- CASE 13: RESTOCK (SECURE) ---
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
                
                // --- CASE 17: RETURNS (Warranty + Manager ID Check) ---
                case 17: {
                    cout << "\n--- Process Return / Refund ---" << endl;
                    
                    // 1. Warranty Check
                    char warranty;
                    cout << "Check Warranty: Is product within valid warranty period? (y/n): ";
                    cin >> warranty;
                    if (warranty == 'n' || warranty == 'N') {
                        cout << "[REJECTED] Cannot return item without valid warranty." << endl;
                        break;
                    }

                    // 2. Manager Authorization (If user is a Worker)
                    if (mySystem.getCurrentRole() == "Worker") {
                        int authID;
                        cout << "\n[SECURITY] Manager Authorization Required." << endl;
                        cout << "Enter Admin/Manager ID to approve: ";
                        cin >> authID;

                        // Check if the typed ID is an Admin or Manager
                        string authRole = mySystem.getWorkerRole(authID);
                        
                        if (authRole == "Admin" || authRole == "Manager") {
                             cout << "[AUTHORIZED] Override accepted. Verified by: " << authRole << endl;
                        } else {
                            cout << "[ACCESS DENIED] That ID does not have Manager privileges." << endl;
                            break; // Stop here, return failed
                        }
                    }

                    // 3. Process the Return
                    int id, qty;
                    cout << "\nEnter Product ID to return: ";
                    cin >> id;
                    
                    // Check if product exists before asking for quantity
                    if (!mySystem.searchUsingTree(id)) {
                        cout << "[ERROR] Product ID not found in system." << endl;
                        break;
                    }

                    cout << "Enter Quantity to return: ";
                    cin >> qty;
                    
                    // Call the logic to fix stock and refund money
                    mySystem.returnProduct(id, qty);
                    break;
                }

                // --- ADMIN OPTIONS ---
                case 15: {
                    if (mySystem.getCurrentRole() != "Admin" && mySystem.getCurrentRole() != "Manager") {
                        cout << "[ACCESS DENIED] Permission needed." << endl; break;
                    }
                    int nid; string nname, nrole;
                    cout << "\n--- Add New Worker ---" << endl;
                    cout << "New ID: "; cin >> nid;
                    cin.ignore();
                    cout << "Name: "; getline(cin, nname); nname = formatName(nname); 
                    cout << "Role (Admin/Manager/Worker): "; cin >> nrole; nrole = formatName(nrole); 
                    mySystem.addNewWorker(nid, nname, nrole);
                    break;
                }
                
                case 16: {
                    if (mySystem.getCurrentRole() != "Admin" && mySystem.getCurrentRole() != "Manager") {
                        cout << "[ACCESS DENIED] Permission needed." << endl; break;
                    }
                    int rid;
                    cout << "\n--- Remove Worker ---" << endl;
                    cout << "Worker ID to remove: "; cin >> rid;
                    mySystem.removeWorker(rid);
                    break;
                }

                case 0: 
                    mySystem.saveInventory();
                    mySystem.endShift(); 
                    cout << "\n[INFO] Logging out..." << endl;
                    cout << "Press Enter to return to Login Screen...";
                    cin.ignore(); cin.get();
                    loggedIn = false; 
                    break;

                default: 
                    cout << "[ERROR] Invalid choice." << endl;
            }
        }
    }
}
