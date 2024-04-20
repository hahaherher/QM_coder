//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <string>
//#include <unordered_map>
//#include <bitset>
//
//#include <map>
//#include <queue>
//#include <algorithm>
#include "qm_coder.h"
#include "gray_process.h"

using namespace std;

vector<unsigned char> read_raw_img(string file_name) {
    ifstream rawFile(file_name, ios::in | ios::binary);

    // 獲取檔案大小
    rawFile.seekg(0, ios::end);
    streampos fileSize = rawFile.tellg();
    rawFile.seekg(0, ios::beg);

    // 讀取所有數據到一個緩衝區
    vector<unsigned char> original_img(fileSize);
    rawFile.read(reinterpret_cast<char*>(original_img.data()), fileSize);

    cout << "已成功讀取 " << int(fileSize) << " 個字節的數據" << endl << endl;

    // 關閉檔案
    rawFile.close();

    return original_img;
}

void check_bit_plane(vector<unsigned char> gray_image, vector<unsigned char> original_img) {
    // check bit plane
    if (gray_image == original_img) {
        cout << "bit plane is doing well." << endl;
    }
    else {
        cout << "bit plane is not doing well." << endl;
    }
    auto mismatch_pair = std::mismatch(gray_image.begin(), gray_image.end(), original_img.begin());
    if (mismatch_pair.first != gray_image.end() && mismatch_pair.second != original_img.end()) {
        std::cout << "First mismatch at index: " << std::distance(gray_image.begin(), mismatch_pair.first) << std::endl;
        std::cout << "Value in gray_image: " << static_cast<int>(*mismatch_pair.first) << std::endl;
        std::cout << "Value in original_img: " << static_cast<int>(*mismatch_pair.second) << std::endl;
    }
    else {
        std::cout << "Vectors are equal" << std::endl;
    }

}


int main() {
    int choice;
    string img_name;
    string process_type;
    string data_path = "./Data/RAW/";

    cout << "lena: 1" << endl;
    cout << "baboon: 2" << endl;
    cout << "Please choose the image: ";
    //cin >> choice;
    //cout << endl;
    choice = 1;

    if (choice == 1) {
        img_name = "lena";
    }
    else if (choice == 2) {
        img_name = "baboon";
    }
    else {
        cerr << "Image doesn't exist!" << endl;
        return 1;
    }

    cout << "gray: 1" << endl;
    cout << "halftone: 2" << endl;
    cout << "binary: 3" << endl;
    cout << "Please choose the color type: ";
    //cin >> choice;
    //cout << endl;
    choice = 1;

    if (choice == 1) {
        process_type = "";
    }
    else if (choice == 2) {
        process_type = "_halftone";
    }
    else if (choice == 3) {
        process_type = "_b";
    }
    else {
        cerr << "Image doesn't exist!" << endl;
        return 1;
    }

    //根據使用者選擇的檔案名稱和處理方式執行相應的處理
    string file_name = data_path + img_name + process_type + ".raw";
    cout << file_name << " Loading..." << endl;

    vector<unsigned char> original_img = read_raw_img(file_name);
    
    
    if (choice == 1) {
        //// check gray code works well
        //original_img = {127, 128, 126};
        //for (unsigned char pixel : original_img) {
        //    bitset<8> bs4(grayEncode(pixel));
        //    cout << int(pixel) << "&" << bs4 << " ";
        //}
        
        // without gray code
        // encode 
        vector<vector<unsigned char>> bit_planes = convertToBitPlanes(original_img, false);
        for (const auto& bit_plane : bit_planes) {
            QMCoder encoder;
            string outstring = encoder.encode(original_img, img_name + process_type + ".qm");
            cout << outstring.length() << endl;
        }
        
        //// decode 
        //vector<string> compressed_outstrings = read_qm();
        //vector<vector<unsigned char>> uncompressed_bit_planes;
        //for (const auto& compressed_outstring : compressed_outstrings) {
        //    QMCoder decoder;
        //    vector<unsigned char> uncompressed_bit_plane = decoder.decode(compressed_outstring);
        //    uncompressed_bit_planes.push_back(uncompressed_bit_plane);
        //}
        //vector<unsigned char> gray_image = combineBitPlanes(uncompressed_bit_planes);
        //cout << endl;
        //check_bit_plane(gray_image, original_img);

        // with gray code
        bit_planes = convertToBitPlanes(original_img, true);
        for (const auto& bit_plane : bit_planes) {
            QMCoder encoder;
            string outstring = encoder.encode(original_img, img_name + process_type + "_graycode.qm");
            cout << outstring.length() << endl;
        }

        // decoding
        

    }
    else {
        // encoding
        QMCoder encoder;
        string outstring = encoder.encode(original_img, img_name + process_type + ".qm");
        
        // decoding
        //QMCoder decoder;
        //string uncompress_string = decoder.decode(outstring);
        //cout << uncompress_string;
        //unsigned long Qc = 0x0001;
        //cout << "Qc" << Qc<<endl;
        //int Qe = 0x0001;
        //cout << "Qe" << Qe << endl;
    }
    
    return 0;
}


