//#include <vector>
//#include <array>
//#include <bitset>
#include "gray_process.h"

using namespace std;

// Function to apply Gray coding to a byte
unsigned char grayEncode(unsigned char byte) {
    return byte ^ (byte >> 1);
}

// Function to convert original image vector to bit-planes with Gray coding
array<vector<unsigned char>, 8> convertToBitPlanes(const vector<unsigned char>& original_img) {
    // Initialize bit-planes array
    array<vector<unsigned char>, 8> bit_planes;

    // Temporary buffer to store bits
    bitset<8> buffer;

    // Index to track bits in buffer
    int buffer_index = 0;

    // Iterate over each byte in original image vector
    for (const auto& byte : original_img) {
        // Apply Gray coding to the byte
        unsigned char gray_byte = grayEncode(byte);

        // Iterate over each bit in the Gray-coded byte
        for (int i = 0; i < 8; ++i) {
            // Extract i-th bit and store it in buffer
//            buffer[buffer_index] = (gray_byte >> i) & 0x01;
            buffer[buffer_index] = (gray_byte & (0x80 >> i)) >> (7 - i);


            // Increment buffer index
            ++buffer_index;

            // If buffer is full, store it in bit-plane and reset buffer
            if (buffer_index == 8) {
                // Store buffer in corresponding bit-plane
                for (int j = 0; j < 8; ++j) {
                    bit_planes[j].push_back(buffer[j]);
                }

                // Reset buffer and buffer index
                buffer.reset();
                buffer_index = 0;
            }
        }
    }

    return bit_planes;
}
