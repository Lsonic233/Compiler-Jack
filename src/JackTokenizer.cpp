#include "JackTokenizer.h"

string trimmedFileContent(string infilename){
    string ans="";
    ifstream infile(infilename);
    string line;
    bool inMultiComment=false;
    while(getline(infile, line)){
        if(inMultiComment){
            size_t end = line.find("*/");
            if(end!=string::npos){
                line = line.substr(end+2);
                inMultiComment = false;
            }
            else continue;
        }
        size_t singleComment = line.find("//");
        size_t multiComment = line.find("/**");
        size_t multiCommentEnd = line.find("*/");

        if(multiComment!=string::npos){
            if(multiCommentEnd!=string::npos && multiCommentEnd > multiComment){
                line.erase(multiComment, multiCommentEnd-multiComment+2);
            }
            else{
                inMultiComment=true;
                line = line.substr(0, multiComment);
            }
            
        }
        else if(singleComment != string::npos){
            line = line.substr(0, singleComment);
        }
        line = regex_replace(line, regex("\\s+$"), "");
        line = regex_replace(line, regex("^\\s+"), "");
        if(!line.empty()) ans+=line;
    }
    return ans;
}


vector<string> JackTokenizer::split(string &str, char delim){
    vector<string> tokens;
    string token;
    istringstream s(str);
    while(getline(s, token, delim))
    {
        tokens.push_back(token);
    }
    return tokens;
}

vector<string>JackTokenizer::splitQuote(string &input) {
    vector<string> tokens;
    string token;
    bool insideQuotes = false;
    int count=0;
    for (int i=0; i < input.size(); i++) {
        if (input[i] == '"') {
            count++;
            if(count%2==1){ //opening
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                token += input[i];
            }
            else{ //closing
                token+=input[i];
                tokens.push_back(token);
                token.clear();
            }            
            insideQuotes = !insideQuotes;
        } 
        else if (insideQuotes) {
            token += input[i];
        } 
        else if (isspace(input[i])) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } 
        else {
            token += input[i];
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

bool JackTokenizer::isStringConst(string s){
    regex stringConstantReg("\"[^\n\"]*\"");
    if(regex_match(s, stringConstantReg)){
        return true;
    }
    return false;

}
bool JackTokenizer::isIntegerConst(string s){
    if(s.empty())return false;
    for(char c : s){
        if(!isdigit(c))return false;
    }
    int num = stoi(s);
    if(num>=0 && num<=32767) return true;
    else return false;
}

bool JackTokenizer::isIdentifier(string s){
    if(isdigit(s[0])) return false;
    regex pattern("^[a-zA-Z_][a-z0-9A-Z_]*");
    if(regex_match(s, pattern)) return true;
    else return false;
}

JackTokenizer::JackTokenizer(string filename){
    ifstream infile(filename);
    string outfilename = split(filename, '.')[0] + ".xml";  
    ofstream outfile(outfilename);
    string fileContent = trimmedFileContent(filename);
    callCount=0;
    tokens = tokensList(fileContent);
}

string JackTokenizer::tokenType(string s){
        if(keywords.find(s)!=keywords.end()){
        //s is a keyword
            return "keyword";
        }
        else if(s.length()==1 && symbols.find(s[0])!=symbols.end()){
            //s is a symbol
            return "symbol";
        }
        else if(isStringConst(s)){
            return "stringConstant";
        }
        else if(isIntegerConst(s)){
            return "integerConstant";
        }
        else if(isIdentifier(s)){
            return "identifier";
        }
        return "NONE";
}

vector<string>JackTokenizer::tokensList(string input){
    vector<string> tokens;
    vector<string> initial = splitQuote(input);
    int index=0;
    for(string s : initial){
        if(tokenType(s)!="NONE"){
            tokens.push_back(s);
        }
        else{
            string t;
            t+=s[0];
            for(int i=0; i < s.length()-1; i++){
                if(tokenType(t+s[i+1])!="NONE"){
                    t+=s[i+1];
                }
                else{
                    tokens.push_back(t);
                    t=s[i+1];
                }
            }
            tokens.push_back(t);
        }
    }
    
    return tokens;
}

bool JackTokenizer::hasMoreTokens(){
    if(tokens.size()>0 && callCount<tokens.size()){
        return true;
    }
    return false;
}

void JackTokenizer::advance(){
    currentToken = tokens[callCount++];
    //return currentToken;
}

string JackTokenizer::lookAhead(int n){ 
    //gets the token n tokens away from the current token
    return tokens[callCount+n];
}