#include "../../header/DataStructures/BST.h"

// Insert Node (Standard BST Logic)
TreeNode* BST::insertNode(TreeNode* root, int key) {
    if (root == nullptr) {
        return new TreeNode(key);
    }
    
    if (key < root->key) {
        root->left = insertNode(root->left, key);
    } else if (key > root->key) {
        root->right = insertNode(root->right, key);
    }
    
    return root;
}

// Search Node (Recursive)
bool BST::searchNode(TreeNode* root, int key) {
    if (root == nullptr) return false;
    
    if (root->key == key) return true;
    
    if (key < root->key) {
        return searchNode(root->left, key);
    } else {
        return searchNode(root->right, key);
    }
}

// Helper: Find Minimum Value Node
TreeNode* BST::findMin(TreeNode* root) {
    if (root == nullptr) return nullptr;  // FIX: Handle empty tree
    while (root->left != nullptr) {
        root = root->left;
    }
    return root;
}

// Delete Node (Standard BST Deletion)
TreeNode* BST::deleteNode(TreeNode* root, int key) {
    if (root == nullptr) return root;

    if (key < root->key) {
        root->left = deleteNode(root->left, key);
    } else if (key > root->key) {
        root->right = deleteNode(root->right, key);
    } else {
        // Node found
        
        // Case 1: No child (Leaf)
        if (root->left == nullptr && root->right == nullptr) {
            delete root;
            return nullptr;
        }
        // Case 2: One child
        else if (root->left == nullptr) {
            TreeNode* temp = root->right;
            delete root;
            return temp;
        } else if (root->right == nullptr) {
            TreeNode* temp = root->left;
            delete root;
            return temp;
        }
        // Case 3: Two children (Find In-Order Successor)
        else {
            TreeNode* temp = findMin(root->right);
            root->key = temp->key;
            root->right = deleteNode(root->right, temp->key);
        }
    }
    return root;
}