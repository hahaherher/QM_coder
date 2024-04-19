#pragma once
#ifndef GRAY_PROCESS_H
#define GRAY_PROCESS_H

#include <iostream>
#include <vector>
#include <array>
#include <bitset>

using namespace std;

// Function to apply Gray coding to a byte
unsigned char grayEncode(unsigned char byte);

// Function to convert original image vector to bit-planes with Gray coding
array<vector<unsigned char>, 8> convertToBitPlanes(const vector<unsigned char>& original_img);

#endif // GRAY_PROCESS_H