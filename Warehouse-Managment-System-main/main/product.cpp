#include "product.h"
#include <iostream>

using namespace std;

// Default Constructor
Product::Product() {
    id = 0;
    name = "Unknown";
    quantity = 0;
    price = 0.0;
    costPrice = 0.0; // [NEW]
    category = "General";
    supplier = "None";
}

// Parameterized Constructor
Product::Product(int id, string name, int quantity, double price, double cost, string category, string supplier) {
    this->id = id;
    this->name = name;
    this->quantity = quantity;
    this->price = price;
    this->costPrice = cost; // [NEW]
    this->category = category;
    this->supplier = supplier;
}

// Getters
int Product::getId() const { return id; }
string Product::getName() const { return name; }
int Product::getQuantity() const { return quantity; }
double Product::getPrice() const { return price; }
double Product::getCost() const { return costPrice; } // [NEW]
string Product::getCategory() const { return category; }
string Product::getSupplier() const { return supplier; }

// --- Setters / Logic ---
void Product::setQuantity(int newQuantity) {
    quantity = newQuantity;
}

void Product::restock(int amount) {
    quantity += amount;
}

bool Product::sell(int amount) {
    if (quantity >= amount) {
        quantity -= amount;
        return true;
    }
    return false;
}

// --- PRINT (Clean Text Only) ---
void Product::printProduct() const {
    cout << "ID: " << id 
         << " | Name: " << name
         << " | Cat: " << category
         << " | Sup: " << supplier
         << " | Qty: " << quantity
         << " | Cost: $" << price << endl;
}