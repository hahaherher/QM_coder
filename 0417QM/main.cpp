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

using namespace std;

//int main() {
//
//    CompressFile_qm(BIT_FILE * input, BIT_FILE * output, long input_file_size);
//    ExpandFile_qm(BIT_FILE * input, BIT_FILE * output);
//    return 0;
//}
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
    string file_name = data_path + img_name + process_type + ".raw";
    cout << file_name << " Loading..." << endl;

    vector<unsigned char> original_img = read_raw_img(file_name);
    

    /*for (unsigned char pixel : original_img) {
        cout << int(pixel) << " ";

    }
    map<unsigned char, double> probability_map = get_probability_map(original_img);
    for (const auto& pair : probability_map) {
        cout << int(pair.first) << " " << pair.second << endl;
    }*/
    //cout << endl;
    QMCoder encoder;

    // Example usage
    string outstring = encoder.encode(original_img);
    cout << outstring;
    //huffman(original_img, probability_map, img_name, process_type, false);
    //huffman(dpcm_img, dpcm_probability_map, img_name, process_type, true);

    //decoding
    string uncompress_string = encoder.decode(outstring);
    cout << uncompress_string;
    //unsigned long Qc = 0x0001;
    //cout << "Qc" << Qc<<endl;
    //int Qe = 0x0001;
    //cout << "Qe" << Qe << endl;

    return 0;
}
