﻿#include <iostream>
//#include <fstream>
//#include <vector>
//#include <string>
//#include <unordered_map>
//#include <bitset>

//#include <map>
//#include <queue>
//#include <algorithm>
#include "qm_coder.h"
#include "gray_process.h"

using namespace std;

QMCoder::QMCoder() {
    Qc = 0x59EB;//23019=2^12*5+2^8*9+2^4*14*2^0*11
    state = 0;
    A = 0x10000; //2^16=65536
    C = 0x0000; //0
    LPS = true;
    MPS = false;
    //outstream = 0x0000;
    //outstring = "";

    // for decoder
    //Cx = 0;     // the portion of the code register containing the offset or pointer to the subinterval
    //Clow = 0;   //contains up to eight bits of new data
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

char* str2chararr(string str_name) {
    char* char_name = new char[str_name.length() + 1];
    errno_t errcode2 = strcpy_s(char_name, str_name.length() + 1, str_name.c_str());
    return char_name;
}

// supochi & my bit plane
void QMCoder::encode(vector<vector<bool>> bitplanes, string qm_name, long img_size) {
    
    BIT_FILE* output = OpenOutputBitFile(str2chararr(qm_name));
    OutputBits(output, img_size*8, 32);

    for (int i = 0; i < bitplanes.size(); i++) {
        vector<bool> bitplane = bitplanes[i];
        int new_bit;
        int stuffing = 0;
        CT = 11;
        is_data_in_code_buffer = 0;
        SC = 0;
        
        Qc = 0x59EB;//23019=2^12*5+2^8*9+2^4*14*2^0*11
        state = 0;
        A = 0x10000; //2^16=65536
        C = 0x0000; //0
        LPS = true;
        MPS = false;

        int j = 0;
        do{
            if (j < bitplane.size()) { 
                new_bit = bitplane[j];
            }
            else { 
                new_bit = 0; 
                stuffing++;  //output 0s at last
            }
            j++;

            if (new_bit == MPS) encodeMPS(output);
            else encodeLPS(output);

        } while (stuffing < 20);
        flush(output); //output remain bits
        printf("%d-bit plane length: %d Bytes\n", i, output->byte_count);
        output->byte_count = 0;
    }

    CloseOutputBitFile(output);
    cout << "done!" << endl;
}

// supochi
void QMCoder::encode(string raw_name, string qm_name, long img_size) {
    BIT_FILE* input = OpenInputBitFile(str2chararr(raw_name));
    BIT_FILE* output = OpenOutputBitFile(str2chararr(qm_name));
    OutputBits(output, img_size, 32);

    int new_bit; 
    int stuffing = 0;
    CT = 11;
    is_data_in_code_buffer = 0;
    SC = 0;
    do {
        new_bit = InputBit(input);

        if (new_bit == EOF) { new_bit = 0; stuffing++; }

        //cout << "pixel" << int(pixel) << endl;
        //break;
        if (new_bit == MPS) encodeMPS(output);
        else encodeLPS(output);
        //if (i == 3) break;
        //i++;
    } while (stuffing < 20);
    flush(output);
    
    CloseInputBitFile(input);
    CloseOutputBitFile(output);
    cout << "done!" << endl;
}

// supochi
void QMCoder::encodeMPS(BIT_FILE* output) {
    // the MPS probability estimate should ideally be 1-Qc
    // the respective subintervals are then A*Qc and A*(1-Qc)
    // the minimal value of 0.75 is for replacing the multiplacation 
    // for 0.75 <= A < 1.5, A*Qc=Qc
    // renormalization allow qm-coder to use fixed-precision integer arithmetic in the coding operations
    // when Qc is of order 0.5(0x5555), the size of 
    // C is unchanged
    A -= Qc;
    if (A < 0x8000) {   // If renormalization is needed
        // conditional exchange
        if (A < Qc) {   // Prob(MPS) < Prob(LPS)
            C += A;     // point to LPS subinterval base
            A = Qc;     // set interval to LPS subinterval
        }
        changeState(3);     // estimate_ & Qe(S)
        renorm_e(output);
    }
}

// supochi
void QMCoder::encodeLPS(BIT_FILE* output) {

    A -= Qc;            // calculate MPS subinterval
    if (A >= Qc) {      // Prob(MPS) > Prob(LPS)
        C += A;         // point c at base of LPS subinterval
        A = Qc;         // set interval to LPS subinterval
    }
    changeState(4);     //decreasing
    renorm_e(output);
}

//mine
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
//mine
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
//mine
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

//supochi
void QMCoder::renorm_e(BIT_FILE* output)
{
    do
    {
        A <<= 1;
        C <<= 1;
        CT--;
        if (CT == 0) { byte_out(output); CT = 8; } //byte_out??
    } while (A < 0x8000);//?
}

//supochi
void QMCoder::flush(BIT_FILE* output)
{
    unsigned long T;

    T = (C + A - 1) & 0xffff0000;
    if (T < C) T += 0x8000;
    C = (T << CT);
    byte_out(output);
    C <<= 8;
    byte_out(output);
}
//supochi
void QMCoder::byte_out(BIT_FILE* output)
{                            /* Never put 0xff in code_buffer */
    unsigned long T;

    T = (C >> 19);

    if (T > 0xff)
    {
        code_buffer += 1;
        if (is_data_in_code_buffer) OutputBits(output, code_buffer, 8);
        /*if(code_buffer == 0xff) fputc(0,output);*/
        for (; SC > 0; SC--) OutputBits(output, 0, 8);
        if (SC != 0) printf("Error: SC != 0\n");
        code_buffer = (unsigned char)(T & 0x000000ff);
        /*printf("Carry produced! \n");*/
    }
    if (T < 0xff)
    {
        if (is_data_in_code_buffer) OutputBits(output, code_buffer, 8);
        for (; SC > 0; SC--) { OutputBits(output, 0xff, 8); /*fputc(0,output);*/ }
        if (SC != 0) printf("Error: SC != 0\n");
        code_buffer = (unsigned char)(T & 0x000000ff);
    }
    if (T == 0xff) SC += 1;
    C &= 0x0007ffff;
    is_data_in_code_buffer = 1;
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
    //cout << output_file_bits;
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

// supochi & my bit plane
vector<vector<bool>>  QMCoder::decode_bitplanes(string qm_name, string decode_raw_name)
{
    // decodes a binary decision by determining which subinterval is pointed to by the code stream
    // the A regeister is not large enough to contain more than the current subinterval available for decoding new symbols
    // the code stream becomes a pointer into the current interval relative to the base of the current intreval 
    // and is guaranteed to have a value within the current interval

    vector<vector<bool>> bit_planes;
    BIT_FILE* input = OpenInputBitFile(str2chararr(qm_name));
    BIT_FILE* output = OpenOutputBitFile(str2chararr(decode_raw_name));
    int output_bit;
    long output_file_bits = 8 * ((long)InputBits(input, 32));
    //cout << output_file_bits;

    for (int i = 0; i < 8; i++) {
        Qc = 0x59EB;//23019=2^12*5+2^8*9+2^4*14*2^0*11
        state = 0;
        LPS = true;
        MPS = false;

        C = InputBits(input, 16);
        C <<= 16;
        A = 0;
        CT = 0;
        vector<bool> bit_plane;
        vector<bool> *output_plane = &bit_plane;

        for (long temp_output_file_bits = output_file_bits/8; temp_output_file_bits > 0; temp_output_file_bits--) {

            A -= (Qc << 16);
            output_bit = MPS;

            if (C < A)  //0 < MPS
            {
                if (A < 0x80000000)
                {
                    if (A < (Qc << 16))
                    {
                        //LPS
                        output_bit = (1 - MPS);
                        changeState(4);
                    }
                    else {
                        changeState(3);
                    }
                    renorm_d(input);
                }
            }
            else
            {
                if (A >= (Qc << 16))
                {
                    //LPS
                    C -= A;
                    A = (Qc << 16);
                    output_bit = (1 - MPS);

                    changeState(4);
                }
                else
                {
                    C -= A;
                    A = (Qc << 16);
                    changeState(3);
                }
                renorm_d(input);
            }
            OutputBit(output_plane, output, output_bit);
        }
        bit_planes.push_back(bit_plane);
    }
    CloseInputBitFile(input);
    CloseOutputBitFile(output);
    cout << "done!" << endl;
    write_gray_img(bit_planes, decode_raw_name);
    return bit_planes;    
}

void QMCoder::write_gray_img(vector<vector<bool>> decoded_bit_planes, string decode_raw_name) {
    vector<unsigned char> gray_img = combineBitPlanes(decoded_bit_planes);
    BIT_FILE* output = OpenOutputBitFile(str2chararr(decode_raw_name));
    for (unsigned char& pixel: gray_img) {
        putc(pixel, output->file);
    }
    CloseOutputBitFile(output);
}

//supochi
void QMCoder::decode(string qm_name, string decode_raw_name)
{
    // decodes a binary decision by determining which subinterval is pointed to by the code stream
    // the A regeister is not large enough to contain more than the current subinterval available for decoding new symbols
    // the code stream becomes a pointer into the current interval relative to the base of the current intreval 
    // and is guaranteed to have a value within the current interval
    
    BIT_FILE* input = OpenInputBitFile(str2chararr(qm_name));
    BIT_FILE* output = OpenOutputBitFile(str2chararr(decode_raw_name));
    int output_bit;
    long output_file_bits = 8 * ((long)InputBits(input, 32));
    //cout << output_file_bits;

    C = InputBits(input, 16);
    C <<= 16;
    A = 0;
    CT = 0;

    for (; output_file_bits > 0; output_file_bits--) {

        A -= (Qc << 16);
        output_bit = MPS;

        if (C < A)  //0 < MPS
        {
            if (A < 0x80000000)
            {
                if (A < (Qc << 16))
                {
                    //LPS
                    output_bit = (1 - MPS);
                    changeState(4);
                }
                else {
                    changeState(3);
                }
                renorm_d(input);
            }
        }
        else
        {
            if (A >= (Qc << 16))
            {
                //LPS
                C -= A;
                A = (Qc << 16);
                output_bit = (1 - MPS);

                changeState(4);
            }
            else
            {
                C -= A;
                A = (Qc << 16);
                changeState(3);
            }
            renorm_d(input);
        }
        OutputBit(output, output_bit);
    }
    CloseInputBitFile(input);
    CloseOutputBitFile(output);
    cout << "done!" << endl;
}

//supochi
void QMCoder::renorm_d(BIT_FILE* input)
{
    do
    {
        if (CT == 0) { byte_in(input); CT = 8; }
        A <<= 1;
        C <<= 1;
        CT--;
    } while (A < 0x80000000);
}

//supochi
void QMCoder::byte_in(BIT_FILE* input)
{
    int input_buffer;

    /*  if(is_terminate) return;

      if((input_buffer = InputBits(input,8))==0xff)
      {
          if((input_buffer = InputBits(input,8)) == 0)
             C |= 0x0000ff00;
          else
          {
              C &= 0xffff0000;
              is_terminate = 1;
          }
      }
      else C += (input_buffer << 8);
    */
    input_buffer = InputBits(input, 8);

    if (input_buffer == EOF) input_buffer = 0;
    C += (input_buffer << 8);
    CT = 8;
}