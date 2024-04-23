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
    choice = 3;

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
    string raw_name = data_path + img_name + process_type + ".raw"; //"./Data/RAW/lena_decode.raw";
    cout << raw_name << " Loading..." << endl;
    string qm_name;
    string decode_raw_name;

    // use bitfile
    //char* raw_name = new char[file_name.length() + 1]; 
    //errno_t errcode = strcpy_s(raw_name, file_name.length() + 1, file_name.c_str());
    ////strcpy(raw_name, file_name.c_str());
    //
    
    vector<unsigned char> original_img = read_raw_img(raw_name);


    

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
            //string outstring = encoder.encode(original_img, img_name + process_type + "_graycode.qm");
            //cout << outstring.length() << endl;
            
        }

        // decoding
        

    }
    else {        
        // encoding
        qm_name = img_name + process_type + ".qm";
        QMCoder encoder;
        //string outstring = encoder.encode(original_img, img_name + process_type + ".qm");
        encoder.encode(raw_name, qm_name, original_img.size());
        
        // decoding
        decode_raw_name =  img_name + process_type + "_decode.raw";
        QMCoder decoder;
        decoder.decode(qm_name, decode_raw_name);

        //string uncompress_string = decoder.decode(outstring);
        //cout << uncompress_string;

    }

    vector<unsigned char> original_raw = read_raw_img(raw_name);
    cout << decode_raw_name;
    vector<unsigned char> decode_raw = read_raw_img(decode_raw_name);
    //vector<unsigned char> decode_raw = read_raw_img("lena_b_hong.raw");
    if (original_raw == decode_raw) {
        cout << "the same!" << endl;
    }
    else {
        cout << "not the same!" << endl;
    }
    
    return 0;
}


