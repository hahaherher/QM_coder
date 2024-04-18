#include <iostream>
//#include <fstream>
//#include <vector>
//#include <string>
//#include <unordered_map>
//#include <bitset>

//#include <map>
//#include <queue>
//#include <algorithm>
#include "qm_coder.h"

using namespace std;

QMCoder::QMCoder() {
    Qc = 0x59EB;//0.49582;
    state = 0;
    A = 0x10000; //0.75?
    C = 0x0000; //1.5
    LPS = true;
    MPS = false;
}

string QMCoder::encode(vector<unsigned char> original_img) {
    int i = 0;
    for (auto& pixel : original_img) {
        //cout << "pixel" << int(pixel) << endl;
        //break;
        if (int(pixel) == MPS) encodeMPS();
        else encodeLPS();
        if (i == 3) break;
        //i++;
    }
    return outstring;
}

void QMCoder::encodeMPS() {
    A -= Qc;
    if (A < 0x8000) {
        if (A < Qc) {
            C += A;
            A = Qc;
        }
        A <<= 1;
        changeState(3);
        outstring.push_back(((C & 0x8000) >> 15) + 48); //push '0' = 48 or '1' = 49
        //cout << ((C & 0x8000) >> 15);// << " MPS " << state << endl; // Output MSB of C
        C <<= 1;
    }
}

void QMCoder::encodeLPS() {
    A -= Qc;
    if (A >= Qc) {
        C += A;
        A = Qc;
    }
    A <<= 1;
    changeState(4); //decreasing
    outstring.push_back(((C & 0x8000) >> 15) + 48);
    //cout << ((C & 0x8000) >> 15);// << " LPS " << state << endl; // Output MSB of C
    C <<= 1;
}

void QMCoder::changeState(int pseudo_column) {
    
    //cout << int(transitions[0].increase_state);
    char column = ' ';
    //cout << pseudo_column;
    if (pseudo_column == 3) {
        column = int(transitions[state].increase_state);

    }
    else if (pseudo_column == 4) {
        column = int(transitions[state].decrease_state);
    }

    if (column >= 48 && column <= 57) {
        // Update Qc according to state transition
        // For example:
        // Qc = 01FB(state29) and changes to 01A4(state30)
        // Qc = 32B4(state08) and changes to 3C3D(state06)
        if (pseudo_column == 3) state += (column - 48);
        else state -= (column - 48);
        Qc = transitions[state].qc_hex;
    }
    else {
        //cout << int(column);
        //S means that MPS and LPS must exchange because we made a wrong guess
        //state = 'S';
        MPS = !MPS;
        LPS = !LPS;
        //cout << "S ";
    }
}

//coding a symbol changes the interval and code stream
void QMCoder::afterMPS() {
    // the MPS probability estimate should ideally be 1-Qc
    // the respective subintervals are then A*Qc and A*(1-Qc)
    // the minimal value of 0.75 is for replacing the multiplacation 
    // for 0.75 <= A < 1.5, A*Qc=Qc
    // renormalization allow qm-coder to use fixed-precision integer arithmetic in the coding operations
    // when Qc is of order 0.5(0x5555), the size of 
    
    // C is unchanged
    A = A - Qc;
    if (A < 0x8000) { // If renormalization is needed
        renornalize(A);
        renornalize(C);
    }
}


void QMCoder::afterLPS() {
    C += A - Qc;    // point c at base of LPS subinterval
    A = Qc;         // set interval to LPS subinterval
    renornalize(A); // Renormalization always needed
    renornalize(C);
}

void QMCoder::renornalize() {
    //if we were to code an MPS, we wound not add 0.5 to C and would therefore get a zero bit after renormalization

}

void QMCoder::flush() {
    //empty the encoder register at the end of the entropy-coded segment

}

string QMCoder::decode(string compress_bitstring)
{
    string binary_code;
    long output_file_bits;
    int output_bit = MPS;
    int state = 0;
    int current_mps = 0;
    //C <<= 16;
    A = 0;
    //CT = 0;

    for (char digit : compress_bitstring) {
        bool digit_bool = (digit == '0') ? false : true;
        A -= Qc;

        if (C < A)
        {
            if (A < 0x8000)
            {
                if (A < (Qc))
                {
                    binary_code.push_back(1 - MPS + 48);
                    changeState(4);
                        //if (state == 0) MPS = 1 - MPS;
                        //state -= transitions[state];
                }
                else {
                    binary_code.push_back(MPS + 48);
                    changeState(3);
                }
                renorm_d(digit_bool);
            }
            else {
                binary_code.push_back(MPS + 48);
            }
        }
        else
        {
            if (A >= (Qc))
            {
                C -= A;
                A = Qc;
                binary_code.push_back(1 - MPS + 48);

                //if (state == 0) current_mps = 1 - current_mps;
                //state -= next_state_lps[state];
                    changeState(4);
            }
            else
            {
                C -= A;
                A = Qc;
                binary_code.push_back(MPS + 48);
                //if (state < 45) state++;
                changeState(3);
            }
            renorm_d(digit_bool);
        }
        
    }
    return binary_code;
}

void QMCoder::renorm_d(bool d) {
    //cout << "d: " << d << endl;
}

//void huffman(vector<unsigned char> original_img, map<unsigned char, double> probability_map, string img_name, string process_type, bool is_dpcm) {
//
//
//    string bitstring = huff_encode(original_img, huffmanTable);
//
//    size_t uncompressed_file_size = (original_img).size(); // 65536
//    size_t compressed_file_size = bitstring.length() / 8; // 499680 / 8 = 62460
//
//    int uncompressed_file_kb = roundf(float(uncompressed_file_size) / 1024); //64KB
//    int compressed_file_kb = roundf(float(compressed_file_size) / 1024); //60KB
//
//    string img_type = (is_dpcm) ? "dpcm" : "original";
//    cout << "=========== " << img_type << " " << img_name + process_type << " =============\n";
//    printf("Your original file size was %zd Bytes (= %d KB). \n", uncompressed_file_size, uncompressed_file_kb);
//    printf("The compressed size is: %zd Bytes (= %d KB).\n", compressed_file_size, compressed_file_kb);
//    printf("Space saving: %0.2f%% \n", float(uncompressed_file_kb - compressed_file_kb) / float(uncompressed_file_kb) * 100);
//
//    print_entropy(probability_map);
//    printf("Huffman encode: %0.2f bits/symbol\n", round(bitstring.length() / uncompressed_file_size));
//
//    string huff_file_name;
//    if (!is_dpcm) {
//        huff_file_name = img_name + process_type + ".huff";
//    }
//    else {
//        huff_file_name = img_name + process_type + "_dpcm.huff";
//    }
//
//    // combine huffman table and bitstring
//
//    //write_char_huff(huff_file_name, bitstring);
//    write_binary_huff(huff_file_name, huffmanTable, bitstring);
//
//
//    //Decoder
//    printf("Decoding.......\n");
//
//    //access huffman table from bitstream
//    Bitstream compress_bitstream = read_binary_huff(huff_file_name);
//
//    string compress_bitstring = compress_bitstream.bitstring;
//    vector<unsigned char> uncompressed_img;
//    vector<bool> binary_code;
//
//    // build a new table equals to map<code, color value>
//    map<vector<bool>, unsigned char> invertedHuffMap;
//    for (const auto& pair : compress_bitstream.huffmanTable) {
//        invertedHuffMap[pair.second] = pair.first;
//    }
//
//    for (char digit : compress_bitstring) {
//        bool digit_bool = (digit == '0') ? false : true;
//        binary_code.push_back(digit_bool);
//        size_t pos = 0;
//        //code += digit;
//        auto letter = invertedHuffMap.find(binary_code);
//        if (letter != invertedHuffMap.end()) {
//            uncompressed_img.push_back(letter->second);
//            binary_code.clear();
//        }
//    }
//
//    // 输出解压后的字符串
//    if (uncompressed_img == original_img) {
//        cout << "Uncompressed img equals to the original img\n" << endl;
//    }
//    else {
//        cout << "Uncompressed img doesn't equals to the original img\n" << endl;
//    }
//}

//void print_huffmanTable(map<unsigned char, vector<bool>> huffmanTable) {
//    //print huffman table
//    cout << "huffmantable.size: " << huffmanTable.size() << endl;
//    for (const auto& pair : huffmanTable) {
//        //pair.first: color value
//        //pair.second: binary code
//        cout << "color: " << static_cast<int>(pair.first) << ", huffman code: ";
//        for (bool bit : pair.second) {
//            cout << bit;
//        }
//        cout << endl;
//    }
//}


string huff_encode(vector<unsigned char> original_img, map<unsigned char, vector<bool>> huffmanTable) {
    // 重新编码图像
    string bitstream = "";
    for (unsigned char pixel : original_img) {
        for (bool bit : huffmanTable[pixel]) {
            //cout << int(bit);
            bitstream += to_string(bit);
        }
        //cout << " ";
    }
    //cout << "new bitstream：" << endl;
    //cout << bitstream <<endl<<endl;
    return bitstream;
}

void write_binary_huff(string filename, map<unsigned char, vector<bool>> huffmanTable, string bitstream) {
    // write bitstream as bitset type
    size_t strlen = bitstream.length();
    //cout << strlen << endl << endl;
    size_t mapSize = huffmanTable.size();

    ofstream outputFile(filename, ios::binary);
    if (outputFile.is_open()) {
        // Write the size of the map
        outputFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

        // Write each pair of the map
        for (const auto& pair : huffmanTable) {
            // Write the unsigned char key
            outputFile.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));

            // Write the size of the vector<bool>
            size_t vecSize = pair.second.size();
            outputFile.write(reinterpret_cast<const char*>(&vecSize), sizeof(vecSize));

            // Write the elements of the vector<bool>
            for (bool b : pair.second) {
                outputFile.write(reinterpret_cast<const char*>(&b), sizeof(b));
            }
        }

        //write bitstream length
        outputFile.write(reinterpret_cast<const char*>(&strlen), sizeof(size_t));

        // create a buffer to save bit
        bitset<32> buffer;
        size_t bufferIndex = 0; //size_t = unsigned int32 = 32 bits

        // convert to binary
        for (char c : bitstream) {
            if (c == '1') {
                buffer.set(bufferIndex);
            }
            ++bufferIndex;
            if (bufferIndex == 32) { // write to file when buffer is full
                outputFile.write(reinterpret_cast<const char*>(&buffer), sizeof(buffer));
                buffer.reset();
                bufferIndex = 0;
            }
        }
        // write to file if there's still data in buffer
        if (bufferIndex > 0) {
            outputFile.write(reinterpret_cast<const char*>(&buffer), sizeof(buffer));
        }
        outputFile.close();
        cout << "Binary data has been written to file.\n" << endl;
    }
    else {
        cerr << "Unable to open file." << endl;
    }
}

void write_char_huff(string filename, string bitstream) {
    ofstream outputFile(filename, ios::binary);
    // write bitstream as char type
    if (outputFile.is_open()) {
        outputFile << bitstream;
        outputFile.close();
    }
    else {
        cerr << "Unable to open file." << endl;
    }
}


Bitstream read_binary_huff(string filename) {
    //read binary file
    ifstream huffFile(filename, ios::in | ios::binary);

    string compressed_img_str = "";
    map<unsigned char, vector<bool>> huffmanTable;
    size_t mapSize;


    // 逐字节读取文件内容
    if (huffFile.is_open()) {
        huffFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

        for (size_t i = 0; i < mapSize; ++i) {
            unsigned char key;
            huffFile.read(reinterpret_cast<char*>(&key), sizeof(key));

            size_t vecSize;
            huffFile.read(reinterpret_cast<char*>(&vecSize), sizeof(vecSize));

            vector<bool> vec;
            for (size_t j = 0; j < vecSize; ++j) {
                bool b;
                huffFile.read(reinterpret_cast<char*>(&b), sizeof(b));
                vec.push_back(b);
            }

            huffmanTable[key] = vec;
        }

        bitset<32> bits;  //62KB
        size_t lastNonZero;
        huffFile.read(reinterpret_cast<char*>(&lastNonZero), sizeof(size_t));
        while (huffFile.read(reinterpret_cast<char*>(&bits), sizeof(bits))) {
            // convert to char
            for (size_t i = 0; i < 32; ++i) {
                if (bits.test(i)) {
                    compressed_img_str += '1';
                }
                else {
                    compressed_img_str += '0';
                }
            }
        }
        huffFile.close();

        // 去除末尾的多余零
        if (lastNonZero != string::npos) {
            compressed_img_str = compressed_img_str.substr(0, lastNonZero);
        }
        else {
            compressed_img_str.clear(); // 如果全是0，则清空字符串
        }
        //cout << "Decoded string: " << compressed_img_str << endl;
        cout << "Read " << compressed_img_str.length() << " bits." << endl;
        Bitstream bitstream = Bitstream(huffmanTable, compressed_img_str);

        huffFile.close();
        return bitstream;
    }
    else {
        cerr << "Unable to open file." << endl;
    }
}

vector<unsigned char> read_raw_img(string file_name) {
    ifstream rawFile(file_name, ios::in | ios::binary);

    // 獲取檔案大小
    rawFile.seekg(0, ios::end);
    streampos fileSize = rawFile.tellg();
    rawFile.seekg(0, ios::beg);

    // 讀取所有數據到一個緩衝區
    vector<unsigned char> original_img(fileSize);
    rawFile.read(reinterpret_cast<char*>(original_img.data()), fileSize);

    cout << "已成功讀取 " << fileSize << " 個字節的數據" << endl << endl;

    // 關閉檔案
    rawFile.close();

    return original_img;
}


map<unsigned char, double> get_probability_map(vector<unsigned char> buffer) {

    // 計算每個數值的出現次數
    unordered_map<unsigned char, int> frequency_map;
    for (unsigned char value : buffer) {
        frequency_map[value]++;
    }
    // 計算機率分佈
    //cout << "灰階圖中每個數值的出現機率分佈：" << endl;

    map<unsigned char, double> probability_map;
    for (const auto& pair : frequency_map) {
        //pair.first: color value
        //pair.second: appear frequency
        double probability = static_cast<double>(pair.second) / buffer.size();
        probability_map[pair.first] = probability;
        cout << "數值 " << static_cast<int>(pair.first) << ": " << probability << endl;
    }

    return probability_map;
}


void print_entropy(map<unsigned char, double> probability_map) {
    // 計算一階熵
    double entropy = 0.0;
    for (const auto& pair : probability_map) {
        //pair.second: appear probability
        entropy -= pair.second * log2(pair.second);
    }

    // 輸出結果
    printf("First-order entropy: %0.2f bits/symbol\n", entropy);
}