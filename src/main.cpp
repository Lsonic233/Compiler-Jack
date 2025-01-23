#include <iostream>
#include <filesystem>
#include <sys/types.h>  // For types like DIR*
#include <sys/stat.h>  // For directory traversal
#include <dirent.h>  // For directory traversal
#include "JackTokenizer.h"
#include "CompilationEngine.h"

using namespace std;    

vector<string>split(string &str, char delim){
    vector<string> tokens;
    string token;
    istringstream s(str);
    while(getline(s, token, delim))
    {
        tokens.push_back(token);
    }
    return tokens;
}

bool isDirectory(const string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        return false;  // Error handling, could not get file status
    }
    return S_ISDIR(statbuf.st_mode);  // Returns true if it's a directory
}

bool isFile(const string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
        return false;  // Error handling, could not get file status
    }
    return S_ISREG(statbuf.st_mode);  // Returns true if it's a regular file
}

vector<string> getFilesWithExtension(const string& directoryPath, const string& extension) {
    vector<string> files;
    DIR* dir = opendir(directoryPath.c_str());
    
    if (dir == nullptr) {
        cerr << "The provided path is not a valid directory." << endl;
        return files;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;
        
        // Skip "." and ".." entries
        if (filename == "." || filename == "..") {
            continue;
        }

        // Check if the file has the desired extension
        if (filename.length() >= extension.length() &&
            filename.substr(filename.length() - extension.length()) == extension) {
            string filePath = directoryPath + "/" + filename;
            struct stat statbuf;
            if (stat(filePath.c_str(), &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
                files.push_back(filePath);
            }
        }
    }
    closedir(dir);

    return files;
}

int main(int argc, char* argv[]){
    string directoryPath = argv[1];
    if(isFile(argv[1])){
        string fullPath = argv[1];
        size_t pos = fullPath.find_last_of("/\\");
        string infileName = fullPath.substr(pos+1);
        string outFileNameXML = split(infileName, '.')[0] + ".xml";
        string outFileNameVM = split(infileName, '.')[0] + ".vm";
        CompilationEngine compile(fullPath, directoryPath + "/" + outFileNameXML, directoryPath + "/" + outFileNameVM);
        compile.compileClass();
    }
    else{
        string fileExtension = ".jack"; // Change to your desired file extension
        vector<string> files = getFilesWithExtension(directoryPath, fileExtension);
        for(const auto& filePath : files){
            string infileName = filePath.substr(filePath.find_last_of("/\\") + 1);
            string outFileNameXML = directoryPath + "/" + split(infileName, '.')[0] + ".xml";
            string outFileNameVM = directoryPath + "/" + split(infileName, '.')[0] + ".vm";
            CompilationEngine compile(filePath, outFileNameXML, outFileNameVM);
            compile.compileClass();
        }
    }

}