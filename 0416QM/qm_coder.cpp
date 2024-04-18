#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <bitset>

#include <map>
#include <queue>
#include <algorithm>

using namespace std;


struct Bitstream {
    map<unsigned char, vector<bool>> huffmanTable;
    string bitstring;

    Bitstream(map<unsigned char, vector<bool>> h, string b) : huffmanTable(h), bitstring(b) {}
};

struct TransitionTable {
    int qc_hex;
    float qc_dec;
    char increase_state;
    char decrease_state;
    TransitionTable(int h, float d, char increase, char decrease) : qc_hex(h), qc_dec(d), increase_state(increase), decrease_state(decrease) {}
};

class QMCoder {
private:
    double Qc; //Prob(LPS)
    int state;
    int A;
    int C;
    bool LPS, MPS;
    string outstring;

public:
    QMCoder() {
        Qc = 0.49582;
        state = 0;
        A = 0x10000; //0.75?
        C = 0x0000; //1.5
        LPS = true;
        MPS = false;
        
    }
    string encode(vector<unsigned char> original_img) {
        int i = 0;
        for (auto& pixel : original_img) {
            //cout << "pixel" << int(pixel) << endl;
            //break;
            if (int(pixel) == MPS) encodeMPS();
            else encodeLPS();
            if (i==3) break;
            //i++;
        }
        return outstring;
    }
    void encodeMPS() {
        A -= Qc;
        if (A < 0x8000) {
            if (A < Qc) {
                C += A;
                A = Qc;
            }
            A <<= 1;
            changeState(3);
            outstring.push_back(((C & 0x8000) >> 15) + 48);
            //cout << ((C & 0x8000) >> 15);// << " MPS " << state << endl; // Output MSB of C //?
            C <<= 1;
        }
    }

    void encodeLPS() {
        A -= Qc;
        if (A >= Qc) {
            C += A;
            A = Qc;
        }
        A <<= 1;
        changeState(4); //decreasing
        outstring.push_back(((C & 0x8000) >> 15) + 48);
        //cout << ((C & 0x8000) >> 15);// << " LPS " << state << endl; // Output MSB of C?
        C <<= 1;
    }

private:
    void changeState(int pseudo_column) {
        static const TransitionTable transitions[46] = {
            {0x59EB, 0.49582, '1', 'S'},
            {0x5522, 0.46944, '1', '1'},
            {0x504F, 0.44283, '1', '1'},
            {0x4B85, 0.41643, '1', '1'},
            {0x4639, 0.38722, '1', '1'},
            {0x415E, 0.36044, '1', '1'},
            {0x3C3D, 0.33216, '1', '1'},
            {0x375E, 0.30530, '1', '1'},
            {0x32B4, 0.27958, '1', '2'},
            {0x2E17, 0.25415, '1', '1'},
            {0x299A, 0.22940, '1', '2'},
            {0x2516, 0.20450, '1', '1'},
            {0x1EDF, 0.17023, '1', '1'},
            {0x1AA9, 0.14701, '1', '2'},
            {0x174E, 0.12581, '1', '1'},
            {0x1424, 0.11106, '1', '2'},
            {0x119C, 0.09710, '1', '1'},
            {0x0F6B, 0.08502, '1', '2'},
            {0x0D51, 0.07343, '1', '2'},
            {0x0BB6, 0.06458, '1', '1'},
            {0x0A40, 0.05652, '1', '2'},
            {0x0861, 0.04620, '1', '2'},
            {0x0706, 0.03873, '1', '2'},
            {0x05CD, 0.03199, '1', '2'},
            {0x04DE, 0.02684, '1', '1'},
            {0x040F, 0.02238, '1', '2'},
            {0x0363, 0.01867, '1', '2'},
            {0x02D4, 0.01559, '1', '2'},
            {0x025C, 0.01301, '1', '2'},
            {0x01F8, 0.01086, '1', '2'},
            {0x01A4, 0.00905, '1', '2'},
            {0x0160, 0.00758, '1', '2'},
            {0x0125, 0.00631, '1', '2'},
            {0x00F6, 0.00530, '1', '2'},
            {0x00CB, 0.00437, '1', '2'},
            {0x00AB, 0.00368, '1', '1'},
            {0x008F, 0.00308, '1', '2'},
            {0x0068, 0.00224, '1', '2'},
            {0x004E, 0.00168, '1', '2'},
            {0x003B, 0.00127, '1', '2'},
            {0x002C, 0.00095, '1', '2'},
            {0x001A, 0.00056, '1', '3'},
            {0x000D, 0.00028, '1', '2'},
            {0x0006, 0.00013, '1', '2'},
            {0x0003, 0.00006, '1', '2'},
            {0x0001, 0.00002, '0', '1'}
        };
        //cout << int(transitions[0].increase_state);
        char column;
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
            if (pseudo_column==3) state += (column - 48);
            else state -= (column - 48);
            Qc = transitions[state].qc_dec;
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
};

vector<unsigned char> read_raw_img(string file_name);
void print_entropy(map<unsigned char, double> probability_map);
map<unsigned char, double> get_probability_map(vector<unsigned char> buffer);
vector<unsigned char> get_dpcm_img(vector<unsigned char> original_img);

//huffman
//void write_binary_huff(string filename, string bitstream);


void write_binary_huff(string filename, map<unsigned char, vector<bool>> huffmanTable, string bitstream);
Bitstream read_binary_huff(string huff_file_name);
//void write_char_huff(string filename, string bitstream);

string huff_encode(vector<unsigned char> original_img, map<unsigned char, vector<bool>> huffmanTable);
void print_huffmanTable(map<unsigned char, vector<bool>> huffmanTable);
void huffman(vector<unsigned char> original_img, map<unsigned char, double> probability_map, string img_name, string process_type, bool is_dpcm);


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

    // 根據使用者選擇的檔案名稱和處理方式執行相應的處理
    string file_name = data_path + img_name + process_type + ".raw";
    cout << file_name << " Loading..." << endl;

    vector<unsigned char> original_img = read_raw_img(file_name);

    //map<unsigned char, double> probability_map = get_probability_map(original_img);
    //cout << endl;

    QMCoder encoder;

    // Example usage
    string outstring = encoder.encode(original_img);
    //cout << outstring;
    //huffman(original_img, probability_map, img_name, process_type, false);
    //huffman(dpcm_img, dpcm_probability_map, img_name, process_type, true);
    
    //decoding

    return 0;
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

void print_huffmanTable(map<unsigned char, vector<bool>> huffmanTable) {
    //print huffman table
    cout << "huffmantable.size: " << huffmanTable.size() << endl;
    for (const auto& pair : huffmanTable) {
        //pair.first: color value
        //pair.second: binary code
        cout << "color: " << static_cast<int>(pair.first) << ", huffman code: ";
        for (bool bit : pair.second) {
            cout << bit;
        }
        cout << endl;
    }
}


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


vector<unsigned char> get_dpcm_img(vector<unsigned char> original_img) {
    // 預處理圖像
    vector<unsigned char> dpcm_img(original_img); // 預處理圖像，先複製原本的圖

    for (size_t i = 257; i < original_img.size(); ++i) { //i=0
        // 最上排和最左排像素不用修改
        if (i % 256 != 0) {//&& i > 255) {
            // DPCM 處理
            int prediction = (original_img[i - 257] + original_img[i - 256] + original_img[i - 1]) / 3; // 取左邊、左上、正上方 3 個像素的平均值作為預測值
            int d = original_img[i] - prediction;
            dpcm_img[i] = d + 128; // 將差值加上 128，以使其在 0 到 255 的範圍內
            //cout << d+128 << " ";
        }
        /*else {
            dpcm_img[i] = original_img[i];
        }*/
    }

    //for (int value : dpcm_img) {
        //cout << value << " ";
    //}

    return dpcm_img;

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