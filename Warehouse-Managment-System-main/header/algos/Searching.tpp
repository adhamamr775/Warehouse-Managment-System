// ==========================================
// FILE: header/algos/Searching.tpp
// ==========================================

// FIX: Add these lines so the editor knows what a vector is!
#include <vector>
using namespace std;

template <typename T>
int binarySearchRec(const vector<T>& vec, int left, int right, int key) {
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