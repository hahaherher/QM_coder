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
    Qc = 0x59EB;//23019=2^12*5+2^8*9+2^4*14*2^0*11
    state = 0;
    A = 0x10000; //2^16=65536
    C = 0x0000; //0
    LPS = true;
    MPS = false;
    //outstream = 0x0000;
    outstring = "";

    // for decoder
    Cx = 0;     // the portion of the code register containing the offset or pointer to the subinterval
    Clow = 0;   //contains up to eight bits of new data
}

string QMCoder::encode(vector<unsigned char> original_img, string filename) {
    ofstream outputFile(filename, ios::binary);
    //int i = 0;
    for (auto& pixel : original_img) {
        //cout << "pixel" << int(pixel) << endl;
        //break;
        if (int(pixel) == MPS) encodeMPS(filename);
        else encodeLPS(filename);
        //if (i == 3) break;
        //i++;
    }
    cout << "done!" << endl;
    return outstring;
}


void QMCoder::encodeMPS(string filename) {
    // the MPS probability estimate should ideally be 1-Qc
    // the respective subintervals are then A*Qc and A*(1-Qc)
    // the minimal value of 0.75 is for replacing the multiplacation 
    // for 0.75 <= A < 1.5, A*Qc=Qc
    // renormalization allow qm-coder to use fixed-precision integer arithmetic in the coding operations
    // when Qc is of order 0.5(0x5555), the size of 
    ofstream outputFile(filename, ios::app | ios::binary);
    // C is unchanged
    A -= Qc;
    if (A < 0x8000) {   // If renormalization is needed
        // conditional exchange
        if (A < Qc) {   // Prob(MPS) < Prob(LPS)
            C += A;     // point to LPS subinterval base
            A = Qc;     // set interval to LPS subinterval
        }
        A <<= 1;            // renorm_e()
        changeState(3);     // estimate_ & Qe(S)
        if (outputFile.is_open()) {
            // Write the size of the map
            char temp_c = MSB(C) + 48;
            outputFile.write(reinterpret_cast<const char*>(&temp_c), sizeof(temp_c));
            outputFile.close();
        }
        outstring.push_back(MSB(C) + 48); // byte_out()   //push '0' = 48 or '1' = 49
        //cout << ((C & 0x8000) >> 15);// << " MPS " << state << endl; // Output MSB of C
        C <<= 1;            // renorm_e()
    }
}


void QMCoder::encodeLPS(string filename) {
    ofstream outputFile(filename, ios::app | ios::binary);

    A -= Qc;            // calculate MPS subinterval
    if (A >= Qc) {      // Prob(MPS) > Prob(LPS)
        C += A;         // point c at base of LPS subinterval
        A = Qc;         // set interval to LPS subinterval
    }
    A <<= 1;            // Renormalization always needed
    changeState(4);     //decreasing
    if (outputFile.is_open()) {
        // Write the size of the map
        char temp_c = MSB(C) + 48;
        outputFile.write(reinterpret_cast<const char*>(&temp_c), sizeof(temp_c));
        outputFile.close();
    }
    //outstream <<= 1;
    //outstream += MSB(C);
    outstring.push_back(MSB(C) + 48);
    //cout << ((C & 0x8000) >> 15);// << " LPS " << state << endl; // Output MSB of C
    C <<= 1;
}

void QMCoder::changeState(int pseudo_column) {
    // estimate_MPS
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
        if (pseudo_column == 3) state += (column - 48);
        else state -= (column - 48);
        Qc = transitions[state].qc_hex;
    }
    else {
        // conditional exchange
        // to avoid interval size inversion, the assignment of LPS and MPS to the two intervals is interchanged
        // only occurs when 0.5 >= Qc > A-Qc
        // both subintervals are clearly less than 0.75(0x8000), and renormalization must occur
        // 
        //S means that MPS and LPS must exchange because we made a wrong guess
        //state = 'S';
        MPS = !MPS;
        LPS = !LPS;
        //cout << "S ";
    }
}

unsigned int QMCoder::MSB(unsigned int interval) {
    return (interval & 0x8000) >> 15;
}


//void QMCoder::flush() {
//    //empty the encoder register at the end of the entropy-coded segment
//
//}

string QMCoder::decode(string compress_bitstring)
{
    // decodes a binary decision by determining which subinterval is pointed to by the code stream
    // the A regeister is not large enough to contain more than the current subinterval available for decoding new symbols
    // the code stream becomes a pointer into the current interval relative to the base of the current intreval 
    // and is guaranteed to have a value within the current interval

    
    string binary_code;
    
    // 将二进制字符串转换为对应的无符号整数
    C = bitset<8>(compress_bitstring).to_ulong();
    unsigned int output_file_bits = 8 * compress_bitstring.length();
    cout << output_file_bits;
    C <<= 16;
    A = 0;
    //CT = 0;

    for (; output_file_bits > 0; output_file_bits--){
        //bool digit_bool = (digit == '0') ? false : true;
        
        A -= (Qc << 16);

        if (C < A)  //0 < MPS
        {
            if (A < 0x80000000)
            {
                if (A < (Qc << 16))
                {
                    //LPS
                    binary_code.push_back(1 - MPS + 48);
                    changeState(4);
                    //if (state == 0) MPS = 1 - MPS;
                }
                else {
                    binary_code.push_back(MPS + 48);
                    changeState(3);
                }
                renorm_d();
            }
            else {
                binary_code.push_back(MPS + 48);
            }
        }
        else
        {
            if (A >= (Qc << 16))
            {
                //LPS
                C -= A;
                A = (Qc << 16);
                binary_code.push_back(1 - MPS + 48);

                //if (state == 0) current_mps = 1 - current_mps;
                //state -= next_state_lps[state];
                changeState(4);
            }
            else
            {
                C -= A;
                A = (Qc << 16);
                binary_code.push_back(MPS + 48);
                //if (state < 45) state++;
                changeState(3);
            }
            renorm_d();
        }    
    }
    return binary_code;
}


void QMCoder::renorm_d() {
    A <<= 1;
    C <<= 1;
}