# Smart Warehouse System 

A high-performance warehouse management system built in C++ featuring advanced data structures and professional financial tracking.

## 🚀 Key Features

### 🛒 Inventory Management
- **BST Optimized Search**: Instant product lookup by ID.
- **Dynamic Inventory**: Live stock tracking with automatic reordering warnings.
- **Admin Details**: View itemized costs, suppliers, and categories.

### 💰 Professional Financials
- **Profit Tracking**: Real-time calculation of Net Profit vs Gross Revenue.
- **Tax Liability**: Automatic 14% tax collection calculation for every sale.
- **Inventory Valuation**: Track the total dollar value of your stock at cost.
- **Margins**: Automatic profit margin percentage calculation.

### 🕒 Shift & Audit Management
- **Shift Login**: Personalized operator sessions.
- **Detailed History**: Every shift is permanently logged to `sales_history.txt`.
- **Management Reports**: Export professional `.txt` reports with full financial dashboards and shift sales logs.

### 🛠️ Robust Core
- **Undo System**: Integrated Stack based undo for sales, deletions, and restocks.
- **Priority Orders**: Dedicated VIP Queue for expedited processing.
- **Data Persistence**: Separate clean files for Inventory, Financials, and Audit History.

## 📂 File Structure
- `inventory.txt`: Pure product database (ID|Name|Qty|Price|Cost...).
- `financials.txt`: Professional financial statement (Revenue, Tax, COGS, Profit).
- `sales_history.txt`: Chronological log of every shift ever completed.
- `warehouse_report.txt`: Exported professional management report (Option 10).
- `warehouse.exe`: The primary management application.

## 🛠️ Data Structures Used
- **Binary Search Tree (BST)**: Primary index for fast searching.
- **Linked List**: Warehouse storage layout simulation.
- **Queue / Priority Queue**: Order processing (VIP first).
- **Stack**: Multi-level undo history.
- **Vectors**: Dynamic inventory storage and sorting.


