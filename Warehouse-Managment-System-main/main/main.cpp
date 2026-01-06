#include <iostream>
#include <limits>
#include <string>
#include <vector> 
#include <iomanip> 
#include <cctype>   // Needed for isdigit, isalpha
#include "warehouse.h" 

using namespace std;

struct CartItem { int id; int qty; };

// ==========================================
// INPUT HELPERS (VALIDATION)
// ==========================================
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int getValidInt(string prompt) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x) {
            // Check if the next char is a newline (prevents inputs like 10abc)
            if (cin.peek() == '\n') return x;
        }
        cout << "   [ERROR] Invalid input. Please enter a number.\n";
        clearInput();
    }
}

// 1. STRICT NAME (No Digits allowed)
string getValidName(string prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (cin.peek() == '\n') cin.ignore(); 
        getline(cin, s);
        
        bool valid = true;
        if (s.empty()) valid = false;
        
        // Scan for digits
        for (char c : s) {
            if (isdigit(c)) {
                valid = false;
                break;
            }
        }

        if (valid) return s;
        cout << "   [ERROR] Names cannot contain numbers. Try again.\n";
    }
}

string getValidPhone(string prompt) {
    string s;
    while (true) {
        cout << prompt;
        cin >> s;
        
        bool valid = true;
        // Check length (optional, e.g., must be > 3 digits)
        if (s.length() < 3) valid = false;

        // Scan for non-digits
        for (char c : s) {
            if (!isdigit(c)) {
                valid = false;
                break;
            }
        }

        if (valid) return s;
        cout << "   [ERROR] Invalid Phone. Must be digits only (0-9).\n";
        clearInput(); // Clear buffer in case of messy input
    }
}

string getPaymentMethod() {
    int choice;
    while (true) {
        cout << "\n   --- Select Payment Method ---\n";
        cout << "   1. Cash\n";
        cout << "   2. Visa / Mastercard\n";
        cout << "   3. Instapay / Wallet\n";
        cout << "   Select: ";
        if (cin >> choice) {
            if (choice == 1) return "Cash";
            if (choice == 2) return "Visa/Mastercard";
            if (choice == 3) return "Instapay";
        }
        cout << "   [ERROR] Invalid selection. Try 1, 2, or 3.\n";
        clearInput();
    }
}

// Flexible String (Allows numbers and text)
// Used for: Product Name, Supplier
string getLineString(string prompt) {
    string s;
    while (true) {
        cout << prompt;
        if (cin.peek() == '\n') cin.ignore(); 
        getline(cin, s);
        if (!s.empty()) return s;
        cout << "   [ERROR] Input cannot be empty.\n";
    }
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
    cout << "    Role: " << role << endl; 
    cout << "==========================================" << endl;
    cout << "1.  Add Product" << endl;
    cout << "2.  View Inventory" << endl;
    cout << "3.  Search Product" << endl; 
    cout << "4.  Normal Order (Cart)" << endl; 
    cout << "5.  VIP Order (Cart)" << endl;    
    cout << "6.  Process Orders" << endl;  
    cout << "7.  Show Storage Layout" << endl; 
    cout << "8.  Sort by ID" << endl;
    cout << "9.  Sort by Price" << endl;
    cout << "10. Shift Report (Manager)" << endl; 
    cout << "11. Total Revenue (Manager)" << endl;
    cout << "------------------------------------------" << endl;
    cout << "12. DELETE Product" << endl;
    cout << "13. Manual Restock" << endl;
    cout << "14. UNDO Last Action" << endl;
    cout << "17. RETURN Item" << endl; 
    
    if (role == "Admin" || role == "Manager") {
        cout << "------------------------------------------" << endl;
        cout << "15. [ADMIN] Add Worker" << endl;
        cout << "16. [ADMIN] Remove Worker" << endl;
        cout << "18. [ADMIN] View Staff Stats" << endl;
        cout << "19. [ADMIN] Promote Worker" << endl;
        cout << "20. [ADMIN] Pay Worker Bonus" << endl;
        cout << "21. [ADMIN] Clear Shift History" << endl; 
    }
    cout << "0.  End Shift" << endl; 
}


int main() {
    Warehouse mySystem;
    
    while (true) {
        string shift;
        int opID = 0;
        bool isAuthenticated = false; 

        cout << "\n\n### WAREHOUSE LOGIN ###\n";
        cout << "(Type '0' to Shutdown)\n";

        // --- 1. SHIFT VALIDATION ---
        while (true) {
            cout << "Enter Shift (Morning/Night): ";
            cin.clear();
            getline(cin, shift);
            if (shift == "0") {
                cout << "[SYSTEM] Shutting Down...\n";
                return 0;
            }
            shift = formatName(shift);
            if (shift == "Morning" || shift == "Night") break;
            cout << "[ERROR] Invalid Shift. Try 'Morning' or 'Night'.\n";
        }

        // --- 2. ID VALIDATION ---
        string dbName, dbRole;
        while (!isAuthenticated) {
            opID = getValidInt("Enter Employee ID: ");
            
            if (opID == 0) {
                cout << "[SYSTEM] Shutting Down...\n";
                return 0;
            }

            if (mySystem.validateLogin(opID, dbName, dbRole)) {
                cout << "\n[SUCCESS] Welcome " << dbRole << ": " << dbName << endl;
                isAuthenticated = true; 
            } else {
                cout << "[ACCESS DENIED] ID not found.\n";
            }
        }

        mySystem.startShift(shift, dbName, opID, dbRole);
        bool loggedIn = true;
        
        while (loggedIn) {
            displayMenu(mySystem.getCurrentRole()); 
            int choice = getValidInt("Enter Choice: ");

            switch (choice) {
                case 1: {
                    if (mySystem.getCurrentRole() == "Worker") { cout << "\n[DENIED] Admin/Manager Only.\n"; break; }
                    cout << "\n--- Add Product ---\n";
                    int id = getValidInt("Product ID: ");
                    if (mySystem.searchUsingTree(id)) { cout << "[ERROR] ID Exists!\n"; break; }
                    
                    string name = getLineString("Name: ");
                    int qty = getValidInt("Quantity: ");
                    double price, cost;
                    cout << "Selling Price: "; cin >> price;
                    cout << "Cost Price: "; cin >> cost;
                    string cat = getLineString("Category: ");
                    string supp = getLineString("Supplier: ");
                    
                    mySystem.addProduct(id, name, qty, price, cost, cat, supp);
                    break;
                }

                case 2: mySystem.listInventory(); break;
                
                case 3: { 
                    int id = getValidInt("Enter ID to Search: ");
                    if(mySystem.searchUsingTree(id)) mySystem.peekProduct(id); 
                    else cout << "[ERROR] Not found.\n"; 
                    break; 
                }

                case 4: 
                case 5: { 
                    string type = (choice == 4) ? "NORMAL" : "VIP";
                    vector<CartItem> tempCart; 
                    
                    // [REQUESTED FEATURE] Show Inventory First
                    mySystem.listInventory(); 

                    cout << "\n--- " << type << " Cart Entry ---\n";
                    
                    // [REQUESTED FEATURE] Strict Inputs
                    string cName = getValidName("Customer Name: ");     // No Numbers
                    string cPhone = getValidPhone("Customer Phone: ");  // No Letters
                    string pMethod = getPaymentMethod();                // Menu Selection

                    while(true) {
                        cout << "\n[Add to Cart] ";
                        int id = getValidInt("Product ID (0 to finish): ");
                        if (id == 0) break;

                        if(mySystem.searchUsingTree(id)) { 
                            string pName = mySystem.getProductName(id); 
                            int stock = mySystem.getProductQuantity(id);
                            cout << " -> Found: " << pName << " (In Stock: " << stock << ")\n";
                            
                            int qty = getValidInt("   Quantity: ");
                            if (qty > stock) { cout << "   [ERROR] Not enough stock.\n"; continue; }
                            if (qty <= 0) { cout << "   [ERROR] Invalid qty.\n"; continue; }

                            tempCart.push_back({id, qty});
                            cout << "   [OK] Added to cart.\n"; 
                        } else cout << "   [ERROR] Product ID not found.\n";
                    }

                    if (tempCart.empty()) { cout << "[CANCELLED] Cart Empty.\n"; break; }

                    cout << "\n--- Confirm " << type << " Order ---\n";
                    cout << "Customer: " << cName << " | Phone: " << cPhone << " | Pay: " << pMethod << endl;
                    cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(10) << "Qty" << setw(10) << "Price" << endl;
                    double total = 0;
                    for(auto &i : tempCart) {
                        double p = mySystem.getProductPrice(i.id);
                        cout << left << setw(5) << i.id << setw(20) << mySystem.getProductName(i.id) 
                             << setw(10) << i.qty << "$" << p << endl;
                        total += (p * i.qty);
                    }
                    if(type=="VIP") { cout << "VIP Fee (10%): +$" << (total * 0.10) << endl; total *= 1.10; }
                    cout << "TOTAL: $" << fixed << setprecision(2) << total << endl;

                    char confirm; cout << "Confirm (y/n): "; cin >> confirm;
                    if (confirm == 'y' || confirm == 'Y') {
                        mySystem.logEvent(">>> NEW ORDER [" + type + "] " + cName);
                        for (auto &i : tempCart) {
                            if (choice == 4) mySystem.addToOrderQueue(i.id, i.qty, pMethod, cName, cPhone);
                            else mySystem.addVIPOrder(i.id, i.qty, pMethod, cName, cPhone);
                        }
                        cout << "[SUCCESS] Sent to Order Queue.\n";
                    } else cout << "[CANCELLED]\n";
                    break;
                }

                case 6: mySystem.processOrders(); break;
                case 7: mySystem.showStorageLayout(); break;
                case 8: mySystem.sortByID(); break;
                case 9: mySystem.sortByPrice(); break;
                
                case 10: { 
                    if (mySystem.getCurrentRole() == "Worker") { cout << "[DENIED] Manager Only.\n"; break; }
                    mySystem.printShiftReport(); 
                    cout << "Press Enter..."; cin.ignore(); cin.get();
                    break;
                }

                case 11: 
                    if (mySystem.getCurrentRole() == "Worker") { cout << "[DENIED] Manager Only.\n"; break; }
                    mySystem.showRevenue(); 
                    break;

                case 12: { 
                    if (mySystem.getCurrentRole() == "Worker") { cout << "[DENIED] Admin/Manager Only.\n"; break; }
                    int id = getValidInt("Delete ID: "); mySystem.removeProduct(id); break; 
                }

                case 13: { 
                    if (mySystem.getCurrentRole() == "Worker") { cout << "[DENIED] Admin/Manager Only.\n"; break; }
                    int id = getValidInt("ID: "); int q = getValidInt("Add Qty: ");
                    mySystem.manualRestock(id, q); break; 
                }

                case 14: {
    mySystem.debugHistory(); 
    cout << "Undo last action? (y/n): "; 
    char confirm; 
    cin >> confirm; 
    
    if(confirm == 'y' || confirm == 'Y') {
        mySystem.undoLastAction(); 
    } else {
        cout << "[INFO] Undo cancelled.\n";
    }
    break;
}
                case 17: {
                    cout << "\n--- Process Return ---\n";
                    char w; cout << "Warranty Valid? (y/n): "; cin >> w;
                    if (w == 'n' || w == 'N') { cout << "Return Rejected.\n"; break; }

                    if (mySystem.getCurrentRole() == "Worker") {
                        int mid = getValidInt("Manager ID Override: ");
                        string r = mySystem.getWorkerRole(mid);
                        if (r != "Admin" && r != "Manager") { 
                            cout << "[DENIED] Invalid Manager ID.\n"; 
                            break; 
                        }
                        cout << "[AUTHORIZED] Override Accepted.\n";
                    }

                    int id = getValidInt("Product ID: ");
                    if (!mySystem.searchUsingTree(id)) { cout << "ID Not Found.\n"; break; }
                    
                    int qty = getValidInt("Return Qty: ");
                    mySystem.returnProduct(id, qty);
                    break;
                }

                case 15: {
    if (mySystem.getCurrentRole() == "Worker") { cout << "[DENIED] Admin Only.\n"; break; }
    cout << "\n--- Add New Worker ---\n";
    int nid = getValidInt("New ID: "); 
    
    if (mySystem.workerExists(nid)) {
        cout << "   [ERROR] ID Taken! Try another.\n";
        break; 
    }
    
    string nname = getValidName("Name: "); 
    string nrole = getValidName("Role: ");
    
    // --- NEW: ASK FOR SALARY ---
    double nsalary;
    cout << "Salary: "; 
    while(!(cin >> nsalary)) { // Basic validation for double
        cout << "Invalid number. Salary: ";
        cin.clear(); cin.ignore(1000, '\n');
    }
    
    // Pass salary to the function
    mySystem.addNewWorker(nid, nname, nrole, nsalary);
    break;
}
                
                case 16: {
                    if (mySystem.getCurrentRole() == "Worker") { cout << "[DENIED] Admin Only.\n"; break; }
                    int rid = getValidInt("Remove ID: "); mySystem.removeWorker(rid); break;
                }

                case 18: {
                    if (mySystem.getCurrentRole() == "Worker") { cout << "[DENIED] Admin/Manager Only.\n"; break; }
                    mySystem.listWorkers(); 
                    break;
                }

                case 19: {
                    if (mySystem.getCurrentRole() == "Worker") { cout << "[DENIED] Admin/Manager Only.\n"; break; }
                    cout << "\n--- Promote Staff ---\n";
                    mySystem.listWorkers(); 
                    int pid = getValidInt("Enter Worker ID to Promote: ");
                    mySystem.promoteWorker(pid);
                    break;
                }

                case 20: {
                    if (mySystem.getCurrentRole() == "Worker") { cout << "[DENIED] Admin/Manager Only.\n"; break; }
                    cout << "\n--- Pay Bonus ---\n";
                    mySystem.listWorkers(); 
                    int pid = getValidInt("Enter Worker ID to Pay: ");
                    mySystem.payWorkerBonus(pid);
                    break;
                }

                case 21: {
                    string r = mySystem.getCurrentRole();
                    if (r != "Admin" && r != "Manager") { cout << "[DENIED] Admin or Manager Only.\n"; break; }
                    cout << "\n[WARNING] This will delete ALL past shift reports.\n";
                    cout << "Are you sure? (y/n): ";
                    char confirm; cin >> confirm;
                    if (confirm == 'y' || confirm == 'Y') {
                        mySystem.clearShiftHistory();
                    } else {
                        cout << "[CANCELLED] History safe.\n";
                    }
                    break;
                }

                case 0: 
                    mySystem.endShift(); 
                    cout << "\n[INFO] Logged out.\n";
                    cin.ignore(); cin.get();
                    loggedIn = false; 
                    break;

                default: cout << "[ERROR] Invalid choice.\n";
            }
        }
    }
}
