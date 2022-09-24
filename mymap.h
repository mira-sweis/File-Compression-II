// 
// mymap.h
//
// CS251 Project #5
// By: Mira Sweis
// UIC Spring 2022
//
// This file is respopnsible for building a class named mymap
// using a threaded BST
// Added helper functions: putNode / findCurrentKey / inOrder / empty / copyNode / copy
// Descriptions for each is above said functions
//
#pragma once

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

template<typename keyType, typename valueType>
class mymap {
private:
    struct NODE {
        keyType key;  // used to build BST
        valueType value;  // stored data for the map
        NODE* left;  // links to left child
        NODE* right;  // links to right child
        int nL;  // number of nodes in left subtree
        int nR;  // number of nodes in right subtree
        bool isThreaded;
    };
    NODE* root;  // pointer to root node of the BST
    int size;  // # of key/value pairs in the mymap

    //
    // iterator:
    // This iterator is used so that mymap will work with a foreach loop.
    //
    struct iterator {
    private:
        NODE* curr;  // points to current in-order node for begin/end

    public:
        iterator(NODE* node) {
            curr = node;
        }

        keyType operator *() {
            return curr->key;
        }

        bool operator ==(const iterator& rhs) {
            return curr == rhs.curr;
        }

        bool operator !=(const iterator& rhs) {
            return curr != rhs.curr;
        }

        bool isDefault() {
            return !curr;
        }

        //
        // operator++:
        //
        // This function should advance curr to the next in-order node.
        // O(logN)
        //
        // this function goes right when threaded, or goes to the leftmost
        // node in the right subtree if not threaded.
        iterator operator++() {
            // checks if nullptr first
            if (curr == nullptr) {
                return iterator(nullptr);
            }
            if (curr->isThreaded) {
                curr = curr->right;
            }
            else {
                curr = curr->right;
                // copied from begin();
                while (curr->left != nullptr) {
                    curr = curr->left;
                }
            }
            return iterator(curr);
        }
    };
    //
    // helper functions:
    //
    // helper function for put
    // responsible for finding the correct spot for the new NODE
    // given in the put function
    //
    void putNode(NODE* newNode, keyType key) {
        NODE* cur = this->root;
        // traverses the BST
        while (cur != nullptr) {
            if (cur->key > key) {
                // checks if the left node is null
                if (cur->left == nullptr) {
                    // sets left node to new node if it is null
                    cur->left = newNode;
                    newNode->isThreaded = true;
                    cur->nL++;
                    break;
                }
                else {
                    cur = cur->left;
                }
            }
            else {
                // now for the right side
                if (cur->right == nullptr) {
                    cur->right = newNode;
                    // the newnode is now threaded
                    newNode->isThreaded = true;
                    cur->isThreaded = false;
                    // increment number on right
                    cur->nR++;
                    break;
                }
                else {
                    cur = (cur->isThreaded) ? nullptr : cur->right;
                }
            }
        }
    }
    //
    // helper function for put and contains
    // mainly to keep from writing the same code twice
    // this function iterates through a BST to find a key while keeping track of
    // current node
    //
    bool findCurrentKey(keyType key, NODE*& cur) {
        while (cur != nullptr) {
            if (cur->key == key) {
                return true;
            }
            else if (cur->key < key) {
                cur = (cur->isThreaded) ? nullptr : cur->right;
            }
            else {
                cur = cur->left;
            }
        }
        return false;
    }
    //
    // helper function for toString
    // since I only know how to traverse a tree in order recursivley;
    // a helper function is needed to recursivley print the BST
    //
    void inOrder(NODE* node, ostream& output, int n, vector<pair<keyType, valueType>>& vec) {
        if (node == nullptr) {
            return;
        }
        // added threading
        NODE* Right = (node->isThreaded) ? nullptr : node->right;
        inOrder(node->left, output, n, vec);
        // if n is zero, operate function for toString()
        // if n is one, operate function for toVector()
        // if n is two, operate function for checkBalance()
        if (n == 0) {
            output << "key: " << node->key << " value: " << node->value << endl;
        } else if (n == 1) {
            vec.push_back(make_pair(node->key, node->value));
        } else {
            output << "key: " << node->key << ", nL: " << node->nL << ", nR: " << node->nR << endl;
        }
        inOrder(Right, output, n, vec);
    }
    //
    // yet another helper function for clear
    // same idea as inOrder(), the deleting proccess must be done recursivley
    //
    void empty(NODE* cur) {
        if (cur == nullptr) {
            return;
        }
        // checks if threaded first
        NODE* Right = (cur->isThreaded) ? nullptr : cur->right;
        empty(cur->left);
        empty(Right);
        delete cur;
    }
    //
    // helper function to create a new node
    // main purpose to help with copy functions
    // creates a copy node
    //
    void copyNode(NODE*& copy, NODE* node) {
        copy = new NODE();
        copy->key = node->key;
        copy->value = node->value;
        copy->nL = node->nL;
        copy->nR = node->nR;
        copy->isThreaded = node->isThreaded;
        copy->left = nullptr;
        copy->right = nullptr;
    }
    //
    // last helper function for copy functions
    // recursivley copys one BST into another BST
    //
    void copy(NODE*& cur, NODE* otherCur) {
        // begin copying node
        this->put(otherCur->key, otherCur->value);
        // recursive code
        if (otherCur->right != nullptr) {
            // if its threaded nevermind
            if (otherCur->isThreaded) {
                return;
            }
            copy(cur->right, otherCur->right);
        }
        // goes left if not null
        if (otherCur->left != nullptr) {
            copy(cur->left, otherCur->left);
        }
    }

public:
    //
    // default constructor:
    //
    // Creates an empty mymap.
    // Time complexity: O(1)
    //
    // defines a default empty tree
    mymap() {
        this->root = nullptr;
        this->size = 0;
    }

    //
    // copy constructor:
    //
    // Constructs a new mymap which is a copy of the "other" mymap.
    // Sets all member variables appropriately.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    mymap(const mymap& other) {
        //check if nullptr first
        if (other.root == nullptr) {
            this->root = nullptr;
        }
        copyNode(this->root, other.root);
        size++;
        copy(this->root, other.root);
        this->size = other.size;
    }

    //
    // operator=:
    //
    // Clears "this" mymap and then makes a copy of the "other" mymap.
    // Sets all member variables appropriately.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    mymap& operator=(const mymap& other) {
        if (this == &other) {
            return *this;
        }
        clear();
        //check if nullptr first
        if (other.root == nullptr) {
            this->root = nullptr;
            return *this;
        }
        // helper functions defined above
        copyNode(this->root, other.root);
        size++;
        copy(this->root, other.root);
        this->size = other.size;
        return *this;
    }

    // clear:
    //
    // Frees the memory associated with the mymap; can be used for testing.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    void clear() {
        empty(this->root);
        this->root = nullptr;
        this->size = 0;
    }

    //
    // destructor:
    //
    // Frees the memory associated with the mymap.
    // Time complexity: O(n), where n is total number of nodes in threaded,
    // self-balancing BST.
    //
    ~mymap() {
        // clear deletes the tree
        clear();
    }

    //
    // put:
    //
    // Inserts the key/value into the threaded, self-balancing BST based on
    // the key.
    // Time complexity: O(logn + mlogm), where n is total number of nodes in the
    // threaded, self-balancing BST and m is the number of nodes in the
    // sub-tree that needs to be re-balanced.
    // Space complexity: O(1)
    //
    void put(keyType key, valueType value) {
        NODE* cur = this->root;
        bool keyExists = findCurrentKey(key, cur);

        // if the key already exists, just update the value
        if (keyExists == true) {
            cur->value = value;
            // then exit the function
            return;
        }
        // if the key doesnt exist
        // create a new node with input value and key
        NODE* newNode = new NODE;
        newNode->key = key;
        newNode->value = value;
        newNode->left = nullptr;
        newNode->right = nullptr;
        newNode->nL = 0;
        newNode->nR = 0;
        // a new node is not threaded yet
        newNode->isThreaded = false;

        // if it is an empty tree make new node the root
        if (this->root == nullptr) {
            this->root = newNode;
            // increment size
            this->size++;
            return;
        }
        // helper function defined above
        putNode(newNode, key);
        // incrememnt size
        this->size++;
    }

    //
    // contains:
    // Returns true if the key is in mymap, return false if not.
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    bool contains(keyType key) {
        NODE* cur = this->root;
        // helper function defined above
        // checks if key exists
        bool keyExists = findCurrentKey(key, cur);
        return keyExists;
    }

    //
    // get:
    //
    // Returns the value for the given key; if the key is not found, the
    // default value, valueType(), is returned (but not added to mymap).
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    valueType get(keyType key) {
        NODE* cur = this->root;
        // helper function defined above
        // responsible for finding key and current value at key
        bool keyExists = findCurrentKey(key, cur);

        if (keyExists == false) {
            return valueType();
        }
        else {
            return cur->value;
        }
    }

    //
    // operator[]:
    //
    // Returns the value for the given key; if the key is not found,
    // the default value, valueType(), is returned (and the resulting new
    // key/value pair is inserted into the map).
    // Time complexity: O(logn + mlogm), where n is total number of nodes in the
    // threaded, self-balancing BST and m is the number of nodes in the
    // sub-trees that need to be re-balanced.
    // Space complexity: O(1)
    //
    valueType operator[](keyType key) {
        NODE* cur = this->root;
        bool keyExists = findCurrentKey(key, cur);

        // checks if key exists, if not insert it
        // uses public functions defined above
        if (keyExists == true) {
            return get(key);
        }
        else {
            put(key, valueType());
            return valueType();
        }
    }

    //
    // Size:
    //
    // Returns the # of key/value pairs in the mymap, 0 if empty.
    // O(1)
    //
    int Size() {
        return this->size;
    }

    //
    // begin:
    //
    // returns an iterator to the first in order NODE.
    // Time complexity: O(logn), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    iterator begin() {
        NODE* cur = this->root;
        // keep iterating to the left-most node
        while (cur->left != nullptr) {
            cur = cur->left;
        }
        return iterator(cur);
    }

    //
    // end:
    //
    // returns an iterator to the last in order NODE.
    // this function is given to you.
    // 
    // Time Complexity: O(1)
    //
    iterator end() {
        return iterator(nullptr);
    }

    //
    // toString:
    //
    // Returns a string of the entire mymap, in order.
    // Format for 8/80, 15/150, 20/200:
    // "key: 8 value: 80\nkey: 15 value: 150\nkey: 20 value: 200\n
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    string toString() {
        vector<pair<keyType, valueType>> vec;
        // creates a string stream similar to animate function in proj #3
        stringstream ss("");
        // helper function defined above
        inOrder(this->root, ss, 0, vec);
        return (ss.str());
    }

    //
    // toVector:
    //
    // Returns a vector of the entire map, in order.  For 8/80, 15/150, 20/200:
    // {{8, 80}, {15, 150}, {20, 200}}
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    vector<pair<keyType, valueType> > toVector() {
        vector<pair<keyType, valueType>> mapToVector;
        stringstream ss("");
        // helper functon defined above
        inOrder(this->root, ss, 1, mapToVector);
        return mapToVector;
    }

    //
    // checkBalance:
    //
    // Returns a string of mymap that verifies that the tree is properly
    // balanced.  For example, if keys: 1, 2, 3 are inserted in that order,
    // function should return a string in this format (in pre-order):
    // "key: 2, nL: 1, nR: 1\nkey: 1, nL: 0, nR: 0\nkey: 3, nL: 0, nR: 0\n";
    // Time complexity: O(n), where n is total number of nodes in the
    // threaded, self-balancing BST
    //
    string checkBalance() {
        vector<pair<keyType, valueType>> vec;
        stringstream ss("");
        // helper function defined above
        inOrder(this->root, ss, 2, vec);
        return (ss.str());
    }
};
