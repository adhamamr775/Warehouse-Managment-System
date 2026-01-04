#ifndef SORTING_H
#define SORTING_H

#include <vector>
using namespace std;

// ==========================================
// 1. PRIMITIVE ARRAY OVERLOADS (int[])
// ==========================================
// Implemented in sorting.cpp

void bubbleSort(int arr[], int n);
void insertionSort(int arr[], int n);


// ==========================================
// 2. GENERIC VECTOR TEMPLATES (vector<T>)
// ==========================================
// Implemented in sorting.tpp

template <typename T>
void bubbleSort(vector<T>& vec);

template <typename T>
void insertionSort(vector<T>& vec);


// ==========================================
// 3. INCLUDE TEMPLATE IMPLEMENTATION
// ==========================================
#include "sorting.tpp"

#endif // SORTING_H