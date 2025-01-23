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

class JackTokenizer{
    private:
    ifstream infile;
    ofstream outfile;

    
    string trim(string& s);
    
    int callCount;
    vector<string> split(string& s, char delim);
    vector<string>splitQuote(string &str);

    public:
    unordered_set<string> keywords = {"class", "constructor", "function", "method", "field", "static", 
                                            "var", "int", "char", "boolean", "void", "true", "false", 
                                            "null", "this", "let", "do", "if", "else", "while", "return"};

    unordered_set<char> symbols = {'{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', 
                                    '|', '<', '>', '=', '~'};
    bool isStringConst(string s);
    bool isIntegerConst(string s);
    bool isIdentifier(string s);

    //Constructor
    JackTokenizer(string filename);
    bool hasMoreTokens();
    void advance(); //finds the next token
    string lookAhead(int n);//Finds the next n tokens
    vector<string> tokensList(string line);

    string tokenType(string s); //the token-type of the current token
    vector<string> tokens;

    string currentToken;

    string fileContent;
    string keyWord();
    string symbol();
    string identifier();
    string intVal();

};
