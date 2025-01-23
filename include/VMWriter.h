#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <cctype>
#include <map>
#include <unordered_map>
#include <cstdlib>
#include <unordered_set>
#include <algorithm>
#include <set>
#include <map>
#include <unordered_map>
#include <regex>
using namespace std;



class VMWriter{

    private:
    ofstream outputfile;
    public:
    VMWriter(string filename);
    enum class Segment {
        CONST,
        ARG,
        LOCAL,
        STATIC,
        THIS,
        THAT,
        POINTER,
        TEMP
    };
    void writePush(string segment, int index);
    void writePop(string segment, int index);
    void writeArithmetic(string operation);
    void writeLabel(string label);
    void writeGoto(string label);
    void writeIf(string label);
    void writeCall(string name, int nArgs);
    void writeFunction(string name, int nLocals);
    void writeReturn();
    

};