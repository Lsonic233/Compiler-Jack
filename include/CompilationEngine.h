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
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
using namespace std;

class CompilationEngine{
    private:
    JackTokenizer tokenizer;
    ofstream output;
    bool isClassVarDec();
    bool isSubroutineDec();
    bool isType();
    bool isExpression();

    SymbolTable classTable;
    SymbolTable subTable;
    map<string, Symbol>classmap = classTable.symbolTable;
    map<string, Symbol>submap = subTable.symbolTable;


    map<string, int> classCount;
    map<string, int> subCount;

    string className;

    VMWriter writer;

    int ifCount=0;
    int whileCount=0;

    unordered_set<std::string> operatorSet = {
        "+", "-", "*", "/", "&", "|", "<", ">", "="
    };

    unordered_set<std::string> keywordConstantSet = {
    "true", "false", "null", "this"
    };

    unordered_map<string, string> OperatorMap = {
    {"+", "add"},
    {"-", "sub"},
    {"&", "and"},
    {"|", "or"},
    {"!", "not"},
    {"-", "neg"},
    {"=", "eq"},
    {">", "gt"},
    {"<", "lt"}
    };


    public:
    CompilationEngine(string infile, string xmlout, string vmout);
    void compileClass();
    void compileClassVarDec();
    void compileSubroutineDec();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileStatement();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    void compileSubroutineCall();
    int compileExpressionList();

    void compileBinaryOp(string op);
    void compileConstructor();
    void handleString();
    void eat(string s);
    void eat(string s, string tag);
    string identifierReg = "^[a-zA-Z_][a-z0-9A-Z_]*";
    string typeReg = "(int|char|boolean|" + identifierReg + ")";


};
