#include <vector>
using namespace std;

// --- BUBBLE SORT (VECTOR) ---
template <typename T>
void bubbleSort(vector<T>& vec) {
    int n = vec.size();
    bool swapped;
    
    for (int i = 0; i < n - 1; i++) {
        swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            // "Blind" comparison using overloaded operators
            if (vec[j] > vec[j + 1]) {
                // Swap
                T temp = vec[j];
                vec[j] = vec[j + 1];
                vec[j + 1] = temp;
                swapped = true;
            }
        }
        // Optimization: Stop if already sorted
        if (!swapped) break;
    }
}

// --- INSERTION SORT (VECTOR) ---
template <typename T>
void insertionSort(vector<T>& vec) {
    int n = vec.size();
    for (int i = 1; i < n; i++) {
        T key = vec[i];
        int j = i - 1;

        // Move elements of vec[0..i-1] that are greater than key
        // to one position ahead of their current position
        while (j >= 0 && vec[j] > key) {
            vec[j + 1] = vec[j];
            j = j - 1;
        }
        vec[j + 1] = key;
    }
}