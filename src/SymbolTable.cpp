#include "SymbolTable.h"
using namespace std;

SymbolTable::SymbolTable(){

}

void SymbolTable::defineClass(string name, string type, string kind, int index){
    symbolTable[name] = {type, kind, index};
}

void SymbolTable::defineSub(string name, string type, string kind, int index){
    symbolTable[name] = {type, kind, index};
}


int SymbolTable::varCount(string kind){
    int varc=0;
    for(auto it=symbolTable.begin(); it!=symbolTable.end(); ++it){
        if(it->second.kind == kind){
            varc++;
        }
    }
    return varc;
}

string SymbolTable::kindOf(string name) {
    auto it = symbolTable.find(name);
    if (it != symbolTable.end()) {
        return it->second.kind;
    }
    return ""; 
}

string SymbolTable::typeOf(string name) {
    auto it = symbolTable.find(name);
    if (it != symbolTable.end()) {
        return it->second.type;
    }
    return ""; 
}

int SymbolTable::indexOf(string name) {
    auto it = symbolTable.find(name);
    if (it != symbolTable.end()) {
        return it->second.index;
    }
    return -1; 
}

void SymbolTable::restart(){
    symbolTable.clear();
}

Symbol SymbolTable::findVar(string name, SymbolTable classTable, SymbolTable subTable){
    if(classTable.symbolTable.find(name)!=classTable.symbolTable.end()){
        return classTable.symbolTable.at(name);
    }
    else if(subTable.symbolTable.find(name)!=subTable.symbolTable.end()){
        return subTable.symbolTable.at(name);
    }
    throw runtime_error("This variable is not present in current subroutine or is not a class level variable\n");
}

void SymbolTable::print(){
    for(auto it=symbolTable.begin(); it!=symbolTable.end(); ++it){
        cout << it->first << " " << it->second.type << " " << it->second.kind << " " << it->second.index << endl;
    }
}
