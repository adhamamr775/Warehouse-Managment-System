#include "../../header/algos/Searching.h"

// --- 1. PRIMITIVE ARRAY IMPLEMENTATION ---
int binarySearchRec(int arr[], int left, int right, int key) {
    if (left > right) return -1;

    int mid = left + (right - left) / 2;

    if (arr[mid] == key) return mid;
    if (arr[mid] > key) return binarySearchRec(arr, left, mid - 1, key);
    
    return binarySearchRec(arr, mid + 1, right, key);
}

// --- 2. VECTOR<INT> IMPLEMENTATION ---
// This lives here because it is NOT a template. It is specific to ints.
int binarySearchRec(const vector<int>& vec, int left, int right, int key) {
    if (left > right) {
        return -1;
    }

    int mid = left + (right - left) / 2;

    if (vec[mid] == key) {
        return mid;
    }

    if (vec[mid] > key) {
        return binarySearchRec(vec, left, mid - 1, key);
    }

    return binarySearchRec(vec, mid + 1, right, key);
}