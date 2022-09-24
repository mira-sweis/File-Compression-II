//
// hashmap.cpp
//
// CS251 Project #6
// By: Mira Sweis
// UIC Spring 2022
//
// This file is responsible for implementing the hashmap functionality
//

#include "hashmap.h"
#include <string>   // VS made me add this lib for "stoi" in operator>>
#include <vector>
using namespace std;

//
// This constructor chooses number of buckets, initializes size of map to 0, and
// creates a bucket array with nBuckets number of pointers to linked lists.
//
hashmap::hashmap() {
    this->nBuckets=10;
    this->nElems=0;
    this->buckets=createBucketArray(nBuckets);
}

//
// Given a number of buckets, creates a hashtable (array of linked list heads).
// @param nBuckets the number of buckets you want in the hashtable.
// return an array of heads of linked lists of key_val_pairs
//
hashmap::bucketArray hashmap::createBucketArray(int nBuckets) {
    bucketArray newBuckets = new key_val_pair*[nBuckets];
    for (int i = 0; i < nBuckets; i++) {
        newBuckets[i] = nullptr;
    }
    return newBuckets;
}

//
// This destructor frees memory for all elements of buckets & also all elements
// of linked lists that those bucket pointers may point to.
//
hashmap::~hashmap() {
    for (int i=0; i < nBuckets; i++){
        key_val_pair *front =  buckets[i];
        while (front != nullptr) {
            key_val_pair *temp = front->next;
            delete front;
            front = temp;
        }
    }
    delete[] buckets;
}

//
// This method puts key/value pair in the map.  It checks to see if key is
// already in map while traversing the list to find the end of it.
//
void hashmap::put(int key, int value) {
    int index = hashFunction(key) % nBuckets;
    // create new node with key and value;
    key_val_pair* cur = buckets[index];
    key_val_pair* prev = nullptr;
    key_val_pair* newNode = new key_val_pair;
    newNode->key = key;
    newNode->value = value;
    newNode->next = nullptr;

    // if the list is empty set newnode to root
    if (buckets[index] == nullptr) {
        buckets[index] = newNode;
        nElems++;
        // if the list is not empty find the end then insert the new node
        // then increment size of elements
    } else {
        while (cur != nullptr) {
            prev = cur;
            if (cur->key == key) {
                // if the key exists already
                // update then delete the new node
                cur->value = value;
                delete newNode;
                return;
            }
            cur = cur->next;
        }
        prev->next = newNode;
        nElems++;
    }
}

//
// This method returns the value associated with key.
//
int hashmap::get(int key) const {
    int index = hashFunction(key) % nBuckets;
    // throws an error if the list is empty
    if (buckets[index] == nullptr) {
        throw runtime_error("hashmap is empty");
    }
    else {
        key_val_pair* cur = buckets[index];
        // traverse the list to find the key
        // then return the value if found
        while (cur != nullptr) {
            if (cur->key == key) {
                return cur->value;
            }
            cur = cur->next;
        }
        throw runtime_error("key does not exist in hashmap");
    }
}

//
// This function checks if the key is already in the map.
//
bool hashmap::containsKey(int key) {
    int index = hashFunction(key) % nBuckets;
    key_val_pair* cur = buckets[index];
    // traverse the list to find the key
    while (cur != nullptr) {
        if (cur->key == key) {
            return true;
        }
        cur = cur->next;
    }
    return false;
}

//
// This method goes through all buckets and adds all keys to a vector.
//
vector<int> hashmap::keys() const {
    vector<int> hashToVec;
    // this for loop is to traverse each linked list in the array
    for (int i = 0; i < nBuckets; ++i) {
        key_val_pair* cur = buckets[i];
        // adds bucket contents to vector
        while (cur != nullptr) {
            hashToVec.push_back(cur->key);
            cur = cur->next;
        }
    }
    return hashToVec;
}

//
// The hash function for hashmap implementation.
// For an extension, you might want to improve this function.
//
// @param input - an integer to be hashed
// return the hashed integer
//
int hashmap::hashFunction(int input) const {
    // use unsigned integers for calculation
    // we are also using so-called "magic numbers"
    // see https://stackoverflow.com/a/12996028/561677 for details
    unsigned int temp = ((input >> 16) ^ input) * 0x45d9f3b;
    temp = (temp >> 16) ^ temp;

    // convert back to positive signed int
    // (note: this ignores half the possible hashes!)
    int hash = (int) temp;
    if (hash < 0) {
        hash *= -1;
    }

    return hash;
}

//
// This function returns the number of elements in the hashmap.
//
int hashmap::size() {
    return nElems;
}

//
// Copy constructor
//
hashmap::hashmap(const hashmap &myMap) {
    // make a deep copy of the map
    nBuckets = myMap.nBuckets;

    buckets = createBucketArray(nBuckets);

    // walk through the old array and add all elements to this one
    vector<int> keys = myMap.keys();
    for (size_t i=0; i < keys.size(); i++) {
        int key = keys[i];
        int value = myMap.get(key);
        put(key,value);
    }

}

//
// Equals operator.
//
hashmap& hashmap::operator= (const hashmap &myMap) {
    // make a deep copy of the map

    // watch for self-assignment
    if (this == &myMap) {
        return *this;
    }

    // if data exists in the map, delete it
    for (int i=0; i < nBuckets; i++) {
        hashmap::key_val_pair* bucket = buckets[i];
        while (bucket != nullptr) {
            // walk the linked list and delete each node
            hashmap::key_val_pair* temp = bucket;
            bucket = bucket->next;
            delete temp;
        }
        buckets[i] = nullptr;
    }
    nElems = 0;
    // walk through the old array and add all elements to this one
    vector<int> keys = myMap.keys();
    for (size_t i=0; i < keys.size(); i++) {
        int key = keys[i];
        int value = myMap.get(key);
        put(key,value);
    }

    // return the existing object so we can chain this operator
    return *this;
}

//
// This function overloads the << operator, which allows for ease in printing
// to screen or inserting into a stream, in general.
//
ostream &operator<<(ostream &out, hashmap &myMap) {
    out << "{";
    vector<int> keys = myMap.keys();
    for (size_t i=0; i < keys.size(); i++) {
        int key = keys[i];
        int value = myMap.get(key);
        out << key << ":" << value;
        if (i < keys.size() - 1) { // no commas after the last one
            out << ", ";
        }
    }
    out << "}";
    return out;
}

//
// This function overloads the >> operator, which allows for ease at extraction
// from streams/files.
//
istream &operator>>(istream &in, hashmap &myMap) {
    // assume the format {1:2, 3:4}
    bool done = false;
    in.get(); // get the first char, {
    int nextChar = in.get(); // get the first real character
    while (!done) {
        string nextInput;
        while ((nextChar != ',') && (nextChar != '}')) {
                nextInput += nextChar;
                nextChar = in.get();
        }
        if (nextChar == ',') {
            // read the space as well
            in.get(); // should be a space
            nextChar = in.get(); // get the next character
        } else {
            done = true; // we have reached }
        }
        // at this point, nextInput should be in the form 1:2
        // (we should have two integers separated by a colon)
        // BUT, we might have an empty map (special case)
        if (nextInput != "") {
            //vector<string> kvp;
            size_t pos = nextInput.find(":");
            myMap.put(stoi(nextInput.substr(0, pos)),
                      stoi(nextInput.substr(pos+1, nextInput.length() - 1)));
        }
    }
    return in;
}



