#include "VMWriter.h"

VMWriter::VMWriter(string filename){
    outputfile.open(filename);
}

void VMWriter::writePush(string segment, int index){
    outputfile << "push " << segment << " " << to_string(index) << "\n";
}
void VMWriter::writePop(string segment, int index){
    outputfile << "pop " << segment << " " << to_string(index) << "\n";
}
void VMWriter::writeArithmetic(string operation){
    outputfile << operation <<  "\n";
}
void VMWriter::writeLabel(string label){
    outputfile << "label " << label << "\n";
}
void VMWriter::writeGoto(string label){
    outputfile << "goto " << label << "\n";
}
void VMWriter::writeIf(string label){
    outputfile << "if-goto " << label << "\n";
}
void VMWriter::writeCall(string name, int nArgs){
    outputfile << "call " << name << " " << nArgs << "\n";
}
void VMWriter::writeFunction(string name, int nLocals){
    outputfile << "function " << name << " " << nLocals << "\n";
}
void VMWriter::writeReturn(){
    outputfile << "return\n";
}
