#ifndef PROJECT_SEARCHING_H
#define PROJECT_SEARCHING_H

#include <vector>
using namespace std;

// 1. Primitive Array (int[])
int binarySearchRec(int arr[], int left, int right, int key);

// 2. Specific Vector (vector<int>)
// We define this specifically so we can put the code in the .cpp file
int binarySearchRec(const vector<int>& vec, int left, int right, int key);

// 3. Generic Template (vector<T>)
// This handles Product, string, double, etc.
template <typename T>
int binarySearchRec(const vector<T>& vec, int left, int right, int key);

#include "Searching.tpp" // Template logic included at the end

#endif //PROJECT_SEARCHING_H