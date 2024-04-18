#pragma once
#ifndef QM_CODEC_H
#define QM_CODEC_H

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

class QMCoder {
private:
    double Qc; // Prob(LPS)
    int state;
    int A;
    int C;
    bool LPS, MPS;
    string outstring;
    void renorm_d(bool d);
    void encodeMPS();
    void encodeLPS();

public:
    QMCoder();

    string encode(vector<unsigned char> original_img);

    string decode(string compress_bitstring);


private:


    void changeState(int pseudo_column);
};

vector<unsigned char> read_raw_img(string file_name);

void print_entropy(map<unsigned char, double> probability_map);

map<unsigned char, double> get_probability_map(vector<unsigned char> buffer);

void write_binary_huff(string filename, map<unsigned char, vector<bool>> huffmanTable, string bitstream);

Bitstream read_binary_huff(string huff_file_name);

//string huff_encode(vector<unsigned char> original_img, map<unsigned char, vector<bool>> huffmanTable);

//void print_huffmanTable(map<unsigned char, vector<bool>> huffmanTable);

//void huffman(vector<unsigned char> original_img, map<unsigned char, double> probability_map, string img_name, string process_type, bool is_dpcm);

#endif // QM_CODEC_H
