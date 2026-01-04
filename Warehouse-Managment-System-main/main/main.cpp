#include <iostream>
#include <limits>
#include <string>
#include "warehouse.h" 

using namespace std;

// --- Helper to clear buffer ---
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void displayMenu() {
    cout << "\n==========================================" << endl;
    cout << "    SMART WAREHOUSE SYSTEM     " << endl;
    cout << "==========================================" << endl;
    cout << "1.  Add New Product" << endl;
    cout << "2.  View Inventory" << endl;
    // [CLEANED] Removed "(Tree Search)" text
    cout << "3.  Search Product" << endl; 
    cout << "4.  Place Normal Order" << endl;
    cout << "5.  Place VIP Order (Priority)" << endl; 
    cout << "6.  Process All Orders" << endl;
    // [CLEANED] Removed "(Linked List)" text
    cout << "7.  Show Storage Layout" << endl; 
    cout << "8.  Sort Inventory by ID" << endl;
    cout << "9.  Sort Inventory by Price" << endl;
    cout << "10. Export Report to File" << endl;
    cout << "11. View Total Revenue" << endl;
    cout << "------------------------------------------" << endl;
    cout << "12. DELETE Product" << endl;
    cout << "13. Manual Restock" << endl;
    cout << "14. UNDO Last Action" << endl;
    cout << "0.  End Shift (Logout)" << endl; 
    cout << "==========================================" << endl;
    cout << "Enter your choice: ";
}

string cleanInput(string input) {
    if (input.empty()) return "Cash";
    for (char &c : input) c = tolower(c);
    if (isalpha(input[0])) input[0] = toupper(input[0]);
    return input;
}

int main() {
    Warehouse mySystem;
    
    // ====================================================
    // OUTER LOOP: SECURE LOGIN SCREEN
    // ====================================================
    while (true) {
        string shift, op;
        int opID = 0;
        bool isAuthenticated = false; 

        cout << "\n\n";
        cout << "##########################################" << endl;
        cout << "#      WAREHOUSE LOGIN            #" << endl;
        cout << "##########################################" << endl;
        cout << "   (Type '0' to Shutdown)                 " << endl;
        cout << "------------------------------------------" << endl;

        // --- 1. Get Shift Name ---
        cout << "Enter Shift Name: ";
        cin.clear();
        getline(cin, shift);
        if (shift == "0") return 0;

        if (!shift.empty()) {
            for (char &c : shift) c = tolower(c);
            if (isalpha(shift[0])) shift[0] = toupper(shift[0]);
        } else shift = "General"; 

        // --- 2. Get Operator Name ---
        cout << "Enter Operator Name: ";
        getline(cin, op);
        if (op == "0") return 0;
        
        if (!op.empty()) {
            for (char &c : op) c = tolower(c);
            if (isalpha(op[0])) op[0] = toupper(op[0]);
        } else op = "Unknown"; 

        // --- 3. SECURE ID CHECK ---
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

            // // Uses BST to validate ID (Implementation detail hidden)
            if (mySystem.validateLogin(opID)) {
                cout << "\n[SUCCESS] Welcome, " << op << "!" << endl;
                cout << "Loading System..." << endl;
                isAuthenticated = true; 
            } else {
                cout << "[ACCESS DENIED] Invalid Credentials." << endl;
                cout << "Please try again.\n" << endl;
            }
        }

        mySystem.startShift(shift, op, opID);

        // ====================================================
        // INNER LOOP: MAIN MENU
        // ====================================================
        bool loggedIn = true;
        while (loggedIn) {
            displayMenu(); 
            
            int choice;
            if (!(cin >> choice)) {
                cout << "[ERROR] Invalid input. Please enter a number." << endl;
                clearInput();
                continue;
            }

            switch (choice) {
                case 1: {
                    int id;
                    cout << "\n--- Add New Product ---" << endl;
                    cout << "Enter Product ID: ";
                    while (!(cin >> id)) { cout << "Invalid input. ID: "; clearInput(); }
                    // // Uses BST Search
                    if (mySystem.searchUsingTree(id)) { cout << "[ERROR] Taken!\n"; break; }
                    
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
                    // [CLEANED] User sees "Search", Code uses BST
                    int id; 
                    cout << "\n--- Search Product ---" << endl; 
                    cout << "Enter ID to Search: "; cin >> id;
                    
                    if(mySystem.searchUsingTree(id)) { 
                        cout << "[OK] Product Found.\n"; 
                        mySystem.peekProduct(id);
                    } else cout << "[ERROR] Not found.\n";
                    break;
                }
                
                case 4: {
                    // // Uses Queue
                    cout << "\n--- Normal Order ---" << endl;
                    string cName, cPhone;
                    cout << "Customer Name: "; cin.ignore(); getline(cin, cName);
                    cout << "Phone: "; cin >> cPhone;
                    string pMethod; cout << "Payment (Cash/Visa): "; cin >> pMethod;
                    pMethod = cleanInput(pMethod);
                    
                    while(true) {
                        int id, qty;
                        cout << "Product ID: "; cin >> id;
                        if(mySystem.searchUsingTree(id)) {
                             cout << "Qty: "; cin >> qty;
                             mySystem.addToOrderQueue(id, qty, pMethod, cName, cPhone);
                             cout << "[OK] Added to cart.\n";
                        } else cout << "[ERROR] Not found.\n";
                        
                        cout << "Add more? (y/n): "; char m; cin >> m;
                        if(m=='n'||m=='N') break;
                    }
                    break;
                }
                
                case 5: {
                    // // Uses VIP Queue
                     cout << "\n--- VIP Order ---" << endl;
                    string cName, cPhone;
                    cout << "VIP Name: "; cin.ignore(); getline(cin, cName);
                    cout << "Phone: "; cin >> cPhone;
                    string pMethod; cout << "Payment (Cash/Visa): "; cin >> pMethod;
                    pMethod = cleanInput(pMethod);
                    
                    while(true) {
                        int id, qty;
                        cout << "Product ID: "; cin >> id;
                        if(mySystem.searchUsingTree(id)) {
                             cout << "Qty: "; cin >> qty;
                             mySystem.addVIPOrder(id, qty, pMethod, cName, cPhone);
                             cout << "[VIP] Added to priority list.\n";
                        } else cout << "[ERROR] Not found.\n";
                        
                        cout << "Add more? (y/n): "; char m; cin >> m;
                        if(m=='n'||m=='N') break;
                    }
                    break;
                }
                case 6: mySystem.processOrders(); break;
                
                case 7: 
                    // // Uses Linked List Visualization
                    cout << "\n--- Storage Layout View ---" << endl;
                    mySystem.showStorageLayout(); 
                    break;
                    
                case 8: mySystem.sortByID(); break;
                case 9: mySystem.sortByPrice(); break;
                case 10: mySystem.exportToFile(); break;
                case 11: mySystem.showRevenue(); break;
                case 12: { int id; cout << "Delete ID: "; cin >> id; mySystem.removeProduct(id); break; }
                case 13: { int id, q; cout << "Restock ID: "; cin >> id; cout<<"Qty: "; cin>>q; mySystem.manualRestock(id, q); break; }
                
                case 14:
                    // // Uses Stack
                    mySystem.debugHistory(); 
                    cout << "\nConfirm Undo? (y/n): "; char c; cin >> c;
                    if(c=='y'||c=='Y') mySystem.undoLastAction();
                    break;

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