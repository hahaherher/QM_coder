#include <iostream>
#include <fstream>
#include <vector>

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

int main() {
    vector<unsigned char> original_img = read_raw_img("lena.raw");
    vector<unsigned char> decode_img = read_raw_img("lena_decode.raw");
    if (original_img == decode_img) {
        cout << "the same!" << endl;
    }
    else {
        cout << "not the same!" << endl;
    }

    //// 打开输出文件
    //std::ofstream outfile("output.raw", std::ios::binary);

    //// 检查文件是否成功打开
    //if (!outfile.is_open()) {
    //    std::cerr << "Error opening output file!" << std::endl;
    //    return 1;
    //}

    //// 写入字节值为1
    //char byte = 0b00000001;
    //outfile.write(&byte, sizeof(byte));

    //// 关闭文件
    //outfile.close();

    //std::cout << "Binary data written to output.raw successfully!" << std::endl;

    //open
    // 打开输入文件
    std::ifstream infile("lena_b_hong.qm", std::ios::binary);

    // 检查文件是否成功打开
    if (!infile.is_open()) {
        std::cerr << "Error opening input file!" << std::endl;
        return 1;
    }

    // 读取文件内容并打印每个字节的整数值
    //char byte2;
    //int i = 0;
    //while (infile.read(&byte2, sizeof(byte2)) && i <100) {
    //    int value = static_cast<unsigned char>(byte2); // 将无符号字符转换为整数值
    //    std::cout << value-48;
    //    i++;
    //}
    //cout << endl;

    vector<unsigned char> original_img3 = read_raw_img("lena_b_hong.qm");
    for (size_t i = 0; i < 100; ++i) {
        std::cout << static_cast<int>(original_img3[i])-48 << " ";
    }
    cout << endl<<endl;

    vector<unsigned char> original_img2 = read_raw_img("lena_b.qm");
    for (size_t i = 0; i < 100; ++i) {
        std::cout << static_cast<int>(original_img2[i])<<" ";
    }


    //// 关闭文件
    //infile.close();


    return 0;
}