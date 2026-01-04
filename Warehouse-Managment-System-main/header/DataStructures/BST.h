#ifndef BST_H
#define BST_H

#include <iostream>
using namespace std;

// Node Structure
struct TreeNode {
    int key;       // Using 'key' to match your code
    TreeNode* left;
    TreeNode* right;
    
    // Constructor
    TreeNode(int val) : key(val), left(nullptr), right(nullptr) {}
};

class BST {
public:
    TreeNode* root;

    BST() { root = nullptr; }

    // Destructor to clean up memory
    ~BST() { destroy(root); }
    void destroy(TreeNode* node) {
        if (node) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

    // --- Core Functions Only ---
    TreeNode* insertNode(TreeNode* root, int key);
    bool searchNode(TreeNode* root, int key);
    TreeNode* deleteNode(TreeNode* root, int key);
    TreeNode* findMin(TreeNode* root); // Helper for deletion
};

#endif