//
// util.h
//
// CS251 Project #6
// By: Mira Sweis
// UIC Spring 2022
//
// This file is responsible for implementing the huffman code
//

#include <iostream>
#include <fstream>
#include <map>
#include <queue>          // std::priority_queue
#include <vector>         // std::vector
#include <functional>     // std::greater
#include <string>
#include "bitstream.h"
#include "hashmap.h"
#include "mymap.h"
#pragma once

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};

//
// added by student from project 6 jumpstart - slide 31
//
class prioritize {
public:
    // changed from pair<int, int>& to HuffmanNode*
    // then points to count instead of second
    bool operator()(const HuffmanNode* p1, const HuffmanNode* p2) const {
        return p1->count > p2->count;
    }
};

//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode*& node) {
    // will use pre-oder traveral, same as project 5
    if (node == nullptr) {
        return;
    }
    // traverse the "tree" using then delete current node
    freeTree(node->zero);
    freeTree(node->one);
    delete node;
}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmap &map) {
    if (isFile) {
        ifstream file(filename);
        char c;
        // while the file has chars
        // check if the chars are repeating
        while (file.get(c)) {
            // if the same char is found again
            // increment it's value by 1
            // if not add one to the lone char
            if (map.containsKey(c)) {
                int cValue = map.get(c);
                map.put(c, cValue + 1);
            } else {
                map.put(c, 1);
            }
        }
    } else {
        // if isFile is false do the same as above but on a string
        for (char c : filename) {
            if (map.containsKey(c)) {
                int cValue = map.get(c);
                map.put(c, cValue + 1);
            } else {
                map.put(c, 1);
            }
        }
    }
    // increment end of file charachter once
    map.put(PSEUDO_EOF, 1);
}

//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmap &map) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, prioritize> pq;
    // put map into a vector
    vector<int> mapToVec = map.keys();

    // the following loop extracts all info from the map into a Huffman node
    // then pushed into a queue
    for (int key : mapToVec) {
        HuffmanNode* newNode = new HuffmanNode();
        newNode->character = key;
        newNode->count = map.get(key);
        newNode->one = nullptr;
        newNode->zero = nullptr;
        pq.push(newNode);
    }
    // now to create a Huffman tree from the queue
    HuffmanNode* newNode = nullptr;
    while (pq.size() > 1) {
        HuffmanNode* right;
        HuffmanNode* left;
        // take the top two nodes in the queue to combine into a tree
        left = pq.top();
        // remove from queue after accessing it
        pq.pop();
        right = pq.top();
        pq.pop();
        newNode = new HuffmanNode();
        // set up new node to be the combination of left and right node
        newNode->count = (left->count) + (right->count);
        newNode->character = NOT_A_CHAR;
        newNode->zero = left;
        newNode->one = right;
        pq.push(newNode);
    }
    return newNode;
}

//
// helper function for buildEncodingMap(node) to help with recursion
// 
void BEMHelper(mymap<int, string>& encodingMap, HuffmanNode* node, string str) {
    if (node == nullptr) {
        return;
    }
    // if the character is valid
    // put into map with binary code string
    if (node->character != NOT_A_CHAR) {
        encodingMap.put((int)node->character, str);
    }
    // whenever the recursive code goes to one or zero it adds to the string
    BEMHelper(encodingMap, node->zero, str + "0");
    BEMHelper(encodingMap, node->one, str + "1");
}

//
// *This function builds the encoding map from an encoding tree.
//
mymap <int, string> buildEncodingMap(HuffmanNode* tree) {
    mymap <int, string> encodingMap;
    // if its empty don't bother
    if (tree == nullptr) {
        return encodingMap;
    }
    string str = "";
    BEMHelper(encodingMap, tree, str);
    return encodingMap;
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, mymap <int, string> &encodingMap,
              ofbitstream& output, int &size, bool makeFile) {
    string binary = "";
    char c;
    // add the encoded string to binary
    while (input.get(c)) {
        binary += encodingMap[c];
    }
    // don't foget the eof
    binary += encodingMap[256];
    size = binary.size();
    // the following creates a stringstream of the binary code
    // then finds if it's a one or zero, and outputs the binary string
    if (makeFile == true) {
        stringstream ss(binary);
        char ch;
        while (ss.get(ch)) {
            if (ch == '0') {
                output.writeBit(0);
            } else {
                output.writeBit(1);
            }
        }
    }
    return binary;
}

//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    HuffmanNode* node = encodingTree;
    string result = "";
    // the loop goes through the input till it reaches the end of file
    while (!input.eof()) {
        // if the bit is 1 node goes to the right
        // if the bit is 0 node goes to the left
        int bit = input.readBit();
        if (bit == 1) {
            node = node->one;
        }
        if (bit == 0) {
            node = node->zero;
        }
        // if the character is valid, put it into the result string
        // and output it
        if (node->character != NOT_A_CHAR) {
            if (node->character == PSEUDO_EOF) {
                break;
            }
            // add character to result
            result += node->character;
            output.put(node->character);
            node = encodingTree;
        }
    }
    return result;
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) { 
    string compStr = "";
    // build frequency map
    hashmap map;
    buildFrequencyMap(filename, true, map);
    // build encoding tree
    HuffmanNode* encodingTree = buildEncodingTree(map);
    // build encoding map
    mymap<int, string> encodingMap = buildEncodingMap(encodingTree);
    // creates input and output streams
    ofbitstream output(filename + ".huf");
    ifstream input(filename);
    output << map;
    int size = 0;
    // encode string
    compStr = encode(input, encodingMap, output, size, true);
    // must delete tree
    freeTree(encodingTree);
    return compStr;
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {
    string decoStr = "";
    size_t pos = filename.find(".txt.huf");
    // if the position is found
    // create a substring from 0 to pos
    if ((int)pos > 0) {
        filename = filename.substr(0, pos);
    }
    ifbitstream input(filename + ".txt.huf");
    ofstream output(filename + "_unc.txt");
    hashmap map;
    input >> map;
    // build encoding tree
    HuffmanNode* encodingTree = buildEncodingTree(map);
    // decode tree
    decoStr = decode(input, encodingTree, output);
    // must delete tree
    freeTree(encodingTree);
    return decoStr;
}
