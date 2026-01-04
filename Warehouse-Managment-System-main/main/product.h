#ifndef PRODUCT_H
#define PRODUCT_H

#include <iostream>
#include <string>
using namespace std;

class Product {
private:
    int id;
    string name;
    int quantity;
    double price;
    double costPrice; // [NEW] Cost for profit calc
    string category;
    string supplier;

public:
    // --- Constructors ---
    Product();
    Product(int id, string name, int quantity, double price, double cost, string category, string supplier);

    // --- Getters ---
    int getId() const;
    string getName() const;
    int getQuantity() const;
    double getPrice() const;
    double getCost() const; // [NEW]
    string getCategory() const;
    string getSupplier() const;

    // --- Setters & Logic ---
    void setQuantity(int newQuantity);
    void restock(int amount);
    bool sell(int amount);
    void printProduct() const;

    // ==========================================
    // OPERATOR OVERLOADING (Crucial for Algorithms)
    // ==========================================
    
    // 1. For SORTING (Comparing Product vs Product)
    // Allows: if (productA > productB) ...
    bool operator<(const Product& other) const {
        return this->id < other.id;
    }

    bool operator>(const Product& other) const {
        return this->id > other.id;
    }

    bool operator==(const Product& other) const {
        return this->id == other.id;
    }

    // 2. For SEARCHING (Comparing Product vs Int ID)
    // Allows: if (productA == 101) ...
    bool operator==(int targetID) const {
        return this->id == targetID;
    }

    bool operator>(int targetID) const {
        return this->id > targetID;
    }

    bool operator<(int targetID) const {
        return this->id < targetID;
    }
};

#endif