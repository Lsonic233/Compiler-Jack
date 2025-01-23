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
#include <set>
#include <map>
#include <unordered_map>
#include <regex>
using namespace std;

struct Symbol{
    string type;
    string kind;
    int index;
};

class SymbolTable{

    public:
    SymbolTable();
    map<string, Symbol> symbolTable;
    void defineClass(string name, string type, string kind, int index);
    void defineSub(string name, string type, string kind, int index);
    int varCount(string kind);
    string kindOf(string name);
    string typeOf(string name);
    int indexOf(string name);
    static Symbol findVar(string name, SymbolTable classTable, SymbolTable subTable);
    void restart();
    void print();

    private:


};
