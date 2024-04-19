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

    // ����ɮפj�p
    rawFile.seekg(0, ios::end);
    streampos fileSize = rawFile.tellg();
    rawFile.seekg(0, ios::beg);

    // Ū���Ҧ��ƾڨ�@�ӽw�İ�
    vector<unsigned char> original_img(fileSize);
    rawFile.read(reinterpret_cast<char*>(original_img.data()), fileSize);

    cout << "�w���\Ū�� " << fileSize << " �Ӧr�`���ƾ�" << endl << endl;

    // �����ɮ�
    rawFile.close();

    return original_img;
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

    //�ھڨϥΪ̿�ܪ��ɮצW�٩M�B�z�覡����������B�z
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
        vector<vector<unsigned char>> bit_planes = convertToBitPlanes(original_img, false);
        for (const auto& bit_plane : bit_planes) {
            QMCoder encoder;
            string outstring = encoder.encode(original_img, img_name + process_type + ".qm");
            cout << outstring.length() << endl;
        }

        cout << endl;
        // with gray code
        bit_planes = convertToBitPlanes(original_img, true);
        for (const auto& bit_plane : bit_planes) {
            QMCoder encoder;
            string outstring = encoder.encode(original_img, img_name + process_type + "_graycode.qm");
            cout << outstring.length() << endl;
        }
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


