# Smart Warehouse System 

A high-performance, secure **Kiosk-style** warehouse management system built in C++. It features advanced data structures, role-based security, and professional financial tracking.

## 🚀 Key Features

### 🔐 Security & Kiosk Mode
* **Secure Login:** Access is restricted via Employee ID verification using a dedicated **Binary Search Tree (BST)**.
* **Kiosk Loop:** The application runs continuously; logging out returns to the login screen for the next operator rather than closing the program.
* **Smart Input:** Automatically corrects capitalization for names and shift types (e.g., "ahmed" → "Ahmed").

### 🛒 Inventory Management
* **BST Optimized Search:** Instant product lookup by ID (`O(log n)` complexity).
* **Dynamic Inventory:** Live stock tracking with automatic reordering warnings.
* **Admin Details:** View itemized costs, suppliers, and categories.

### 💰 Professional Financials
* **Profit Tracking:** Real-time calculation of Net Profit vs. Gross Revenue.
* **Tax Liability:** Automatic **14% tax** collection calculation for every sale.
* **Inventory Valuation:** Tracks the total dollar value of stock at cost.
* **Margins:** Automatic profit margin percentage calculation.

### 🕒 Shift & Audit Management
* **Session Tracking:** Logs Operator Name, **Employee ID**, and Shift Name.
* **Detailed History:** Every shift is permanently logged to `sales_history.txt`.
* **Management Reports:** Export professional `.txt` reports with full financial dashboards and shift sales logs.

### 🛠️ Robust Core
* **Undo System:** Integrated **Stack-based** undo for sales, deletions, and restocks.
* **Visual Debugging:** Includes a "Stack Dump" feature to view the entire history of actions before undoing.
* **Priority Orders:** Dedicated **VIP Queue** for expedited processing.
* **Data Persistence:** Automatic saving/loading for Inventory and Financials.

---

## 📂 File Structure

| File Name | Description |
| :--- | :--- |
| **`warehouse.exe`** | The primary management application (Kiosk Terminal). |
| **`inventory.txt`** | Database of products (ID, Name, Qty, Price, Cost, Category, Supplier). |
| **`financials.txt`** | Professional financial statement (Revenue, Tax, COGS, Profit). |
| **`sales_history.txt`** | Chronological log of every shift ever completed. |
| **`warehouse_report.txt`** | Exported management report (generated via Option 10). |

---

## 🛠️ Data Structures Used

* **Binary Search Tree (BST):** Used twice—once for fast product ID lookups and once for the **Secure Worker Database**.
* **Linked List:** Simulates the physical warehouse storage layout.
* **Queue / Priority Queue:** Manages order processing (VIP orders get priority over Normal orders).
* **Stack:** Powers the "Undo" feature and History Log.
* **Vectors:** Used for dynamic inventory storage and sorting algorithms.

---

## 💻 How to Run
1. Compile the source code in the terminal powershell:
   ```bash
  g++ (Get-ChildItem -Recurse -Filter *.cpp).FullName -o warehouse_app.exe
  wait untill the path is shown then write this .\warehouse_app.exe
  and also try the exe file
