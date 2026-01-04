#include "../../header/algos/sorting.h"
#include <iostream>

using namespace std;

// ==========================================
// 1. BUBBLE SORT IMPLEMENTATION (int[])
// ==========================================
void bubbleSort(int arr[], int n) {
    bool swapped;
    for (int i = 0; i < n - 1; i++) {
        swapped = false;
        // Last i elements are already in place
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Manual swap for integers
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = true;
            }
        }
        // Optimization: If no two elements were swapped by inner loop, then break
        if (!swapped) break;
    }
}

// ==========================================
// 2. INSERTION SORT IMPLEMENTATION (int[])
// ==========================================
void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        // Move elements of arr[0..i-1], that are greater than key,
        // to one position ahead of their current position
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}