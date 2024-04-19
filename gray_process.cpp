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
vector<vector<unsigned char>> convertToBitPlanes(const vector<unsigned char>& original_img, bool with_graycode) {
    // Initialize bit-planes array
    vector<vector<unsigned char>> bit_planes(8);

    // Iterate over each byte in original image vector
    for (const auto& pixel : original_img) {
        // Apply Gray coding to the byte
        unsigned char new_pixel = pixel;
        if (with_graycode) {
            new_pixel = grayEncode(pixel);
        }
        // Iterate over each bit in the Gray-coded byte
        for (int i = 0; i < 8; ++i) {
            // Extract i-th bit and store it in corresponding bit-plane
            bit_planes[i].push_back(new_pixel & (0x80 >> i));
        }
    }

    //// check size of each bit plane
    //for (auto& bit_plane : bit_planes) {
    //    cout << bit_plane.size() << endl;
    //}

    return bit_planes;
}
