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
            bit_planes[i].push_back(new_pixel >> (7-i) & 0x0001);
        }
    }

    //// check size of each bit plane
    //for (auto& bit_plane : bit_planes) {
    //    cout << bit_plane.size() << endl;
    //}

    return bit_planes;
}


vector<unsigned char> combineBitPlanes(vector<vector<unsigned char>> bit_planes) {
    vector<unsigned char> grayscale_image;
    // 合成位平面
    for (int y = 0; y < bit_planes[0].size(); ++y) {
        unsigned int grayscale_pixel = 0;
        // 逐位組合
        for (int i = 0; i < 8; ++i) {
            grayscale_pixel += (bit_planes[i][y] << 7-i);
        }
        grayscale_image.push_back(grayscale_pixel);
    }
    return grayscale_image;
}

//int main() {
//    std::vector<std::vector<unsigned char>> bit_planes; // 位平面數據
//    // 加載位平面數據到 bit_planes
//
//    // 合成灰度圖像
//    std::vector<unsigned char> grayscale_image;
//    combineBitPlanes(bit_planes, grayscale_image);
//
//    // 保存圖像到 test_planes.raw
//    std::ofstream outfile("test_planes.raw", std::ios::out | std::ios::binary);
//    if (outfile.is_open()) {
//        outfile.write(reinterpret_cast<char*>(grayscale_image.data()), grayscale_image.size());
//        outfile.close();
//        std::cout << "Grayscale image saved as test_planes.raw" << std::endl;
//    }
//    else {
//        std::cerr << "Unable to open file for writing" << std::endl;
//        return 1;
//    }
//
//    return 0;
//}

