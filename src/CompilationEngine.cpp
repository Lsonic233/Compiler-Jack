#include "CompilationEngine.h"

CompilationEngine::CompilationEngine(string infile, string xmlout, string vmout) : tokenizer(infile), output(xmlout), writer(vmout){
    tokenizer.advance();
}

bool CompilationEngine::isClassVarDec(){
    if(tokenizer.currentToken=="static" || tokenizer.currentToken=="field"){
        return true;
    }
    return false;
}

bool CompilationEngine::isType(){
    regex reg(typeReg);
    if(regex_match(tokenizer.currentToken, reg))return true;
    return false;
}

bool CompilationEngine::isSubroutineDec(){
    regex reg("(constructor|method|function)");
    if(regex_match(tokenizer.currentToken, reg))return true;
    return false;
}


void CompilationEngine::compileClass(){
    output << "<class>\n";
    eat("class");
    className=tokenizer.currentToken;
    eat(tokenizer.currentToken); //className
    eat("{");
    while(isClassVarDec()){
        compileClassVarDec();
    }

    while(isSubroutineDec()){
        compileSubroutineDec();
    }
    eat("}");
    classTable.restart();
    output << "</class>\n";
}

void CompilationEngine::compileClassVarDec(){
    output << "<classVarDec>\n";
    string kind = tokenizer.currentToken;
    string varName = tokenizer.lookAhead(1);
    string type = tokenizer.lookAhead(0);
    string kindadd="static";
    if(kind=="field") kindadd="this";
    classTable.defineClass(varName, type, kindadd, classCount[kindadd]++);
    eat(tokenizer.currentToken); //static/field
    if(isType()) eat(tokenizer.currentToken); //type
    eat(tokenizer.currentToken); //varName
    while(tokenizer.currentToken == ","){
        eat(",");
        classTable.defineClass(tokenizer.currentToken, type, kindadd, classCount[kindadd]++);
        eat(tokenizer.currentToken); //varName
    }
    eat(";");
    output << "</classVarDec>\n";
}

void CompilationEngine::compileSubroutineDec(){
    output << "<subroutineDec>\n";

    string subroutineType = tokenizer.currentToken;
    eat(subroutineType); //'constructor' | 'function' | 'method'
    if(subroutineType == "method")subTable.defineSub("this", className, "argument", subCount["argument"]++);
    eat(tokenizer.currentToken); //'void' | type)
    string subroutineName = tokenizer.currentToken;
    eat(subroutineName ); //subroutineName
    eat("("); 
    compileParameterList();
    eat(")");

    eat("{");
    while(tokenizer.currentToken == "var"){
        compileVarDec();
    }

    int nLocals = subCount["local"];
    writer.writeFunction(className + "." + subroutineName, nLocals); 
    if(subroutineType == "constructor"){
        writer.writePush("constant", classCount["this"]);
        writer.writeCall("Memory.alloc", 1);
        writer.writePop("pointer", 0);
    }
    else if(subroutineType == "method"){
        writer.writePush("argument", 0);
        writer.writePop("pointer", 0);
    }
    compileStatements();
    eat("}");
    subTable.restart();
    subCount.clear();
    output << "</subroutineDec>\n";
}



void CompilationEngine::compileParameterList(){
    output << "<parameterList>\n";

    if(tokenizer.currentToken!=")"){

        string type = tokenizer.currentToken;
        eat(type);
        string varName = tokenizer.currentToken;
        eat(varName);
        subTable.defineSub(varName, type, "argument", subCount["argument"]++);
        while(tokenizer.currentToken==",") 
        {
            eat(",");
            string type = tokenizer.currentToken;
            eat(type);
            string varName = tokenizer.currentToken;
            eat(varName);
            subTable.defineSub(varName, type, "argument", subCount["argument"]++);
        }
    }
    output << "</parameterList>\n";
}

void CompilationEngine::compileSubroutineBody(){ //returns the number of local variables
    output << "<subroutineBody>\n";
    eat("{");
    while(tokenizer.currentToken == "var"){
        compileVarDec();
    }
    compileStatements();
    eat("}");
    output << "</subroutineBody>\n";
}

void CompilationEngine::compileVarDec(){
    output << "<varDec>\n";
    eat("var");
    string type=tokenizer.currentToken;
    eat(type);
    string varName=tokenizer.currentToken;
    eat(varName);
    
    subTable.defineSub(varName, type, "local", subCount["local"]++);
    while(tokenizer.currentToken==","){
        eat(",");
        
        subTable.defineSub(tokenizer.currentToken, type, "local", subCount["local"]++);
        
        eat(tokenizer.currentToken);//varName
    }
    eat(";");
    output << "</varDec>\n";
}

void CompilationEngine::compileStatements(){
    output << "<statements>\n";
    regex reg("let|if|while|do|return");
    while(regex_match(tokenizer.currentToken, reg)){
        compileStatement();
    }
    output<< "</statements>\n";
}

void CompilationEngine::compileStatement(){
    
    if(tokenizer.currentToken == "let"){
        compileLet();
    }
    else if(tokenizer.currentToken == "if"){
        compileIf();
    }
    else if(tokenizer.currentToken == "while"){
        compileWhile();
    }
    else if(tokenizer.currentToken == "do"){
        compileDo();
    }
    else if(tokenizer.currentToken == "return"){
        compileReturn();
    }
}

void CompilationEngine::compileLet(){
    
    output << "<letStatement>\n";
    eat("let");
    string varName = tokenizer.currentToken;
    eat(varName);//varName
    Symbol row = SymbolTable::findVar(varName, classTable, subTable);
    //compiling varName[expression1] = expression2
    if(tokenizer.currentToken=="["){
        eat("[");
        writer.writePush(row.kind, row.index);
        compileExpression(); //expression1
        writer.writeArithmetic("add");
        eat("]");
        eat("=");
        compileExpression(); //expression2
        writer.writePop("temp", 0);
        writer.writePop("pointer", 1);
        writer.writePush("temp", 0);
        writer.writePop("that", 0);
        eat(";");
    }
    else{
        eat("=");
        compileExpression();
        writer.writePop(row.kind, row.index);
        eat(";");
    }

    output << "</letStatement>\n";
}

void CompilationEngine::compileIf(){
    output << "<ifStatement>\n";
    eat("if");
    
    eat("(");
    compileExpression();
    string trueLabel = "IF_TRUE" + to_string(ifCount);
    string falseLabel = "IF_FALSE" + to_string(ifCount);
    string endLabel = "IF_END" + to_string(ifCount);
    ifCount++;
    eat(")");
    eat("{");

    writer.writeIf(trueLabel); //if-goto trueLabel
    writer.writeGoto(falseLabel); //goto falseLabel
    writer.writeLabel(trueLabel); //(trueLabel)
    
    compileStatements();
    writer.writeGoto(endLabel);
    eat("}");
    if(tokenizer.currentToken=="else"){
        eat("else");
        eat("{");
        writer.writeLabel(falseLabel);
        compileStatements();
        eat("}");
    }
    else{
        writer.writeLabel(falseLabel);
        writer.writeGoto(endLabel);
    }
    writer.writeLabel(endLabel);
    
    output << "</ifStatement>\n";
}

void CompilationEngine::compileWhile(){
    output << "<whileStatement>\n";
    eat("while");
    eat("(");
    string whileTrue = "WHILE_EXP" + to_string(whileCount);
    string whileEnd = "WHILE_END" + to_string(whileCount);
    writer.writeLabel(whileTrue);
    compileExpression();
    writer.writeArithmetic("not");
    eat(")");
    eat("{");
    writer.writeIf(whileEnd);
    compileStatements();
    writer.writeGoto(whileTrue);
    writer.writeLabel(whileEnd);
    eat("}");
    whileCount++;
    output << "</whileStatement>\n";
}

void CompilationEngine::compileDo(){
    output << "<doStatement>\n";
    eat("do");
    compileSubroutineCall();
    writer.writePop("temp", 0);
    eat(";");
    output << "</doStatement>\n";
}

void CompilationEngine::compileReturn(){
    output << "<returnStatement>\n";
    eat("return");
    if(tokenizer.currentToken!=";"){
        compileExpression();
        writer.writeReturn();
    }
    else{
        writer.writePush("constant", 0);
        writer.writeReturn();
    }
    eat(";");
    output << "</returnStatement>\n";
}

void CompilationEngine::compileExpression(){
    output << "<expression>\n";
    compileTerm();
    while(operatorSet.find(tokenizer.currentToken) != operatorSet.end()){
        string token=tokenizer.currentToken; //operator  
        if(token=="<")eat("&lt;", "symbol");
        else if(token==">")eat("&gt;", "symbol");
        else if(token=="&")eat("&amp;", "symbol");
        else eat(tokenizer.currentToken); //op
        compileTerm();
        compileBinaryOp(token);
    }
    output << "</expression>\n";
}

void CompilationEngine::compileBinaryOp(string op){
    if(op == "*") writer.writeCall("Math.multiply", 2);
    else if(op=="/") writer.writeCall("Math.divide", 2);
    else {
        writer.writeArithmetic(OperatorMap[op]);
    }
}

void CompilationEngine::compileTerm(){
    output << "<term>\n";
    string token = tokenizer.currentToken;
    string nextToken = tokenizer.lookAhead(0);
    if(token == "true"){
        eat(token);
        writer.writePush("constant", 0);
        writer.writeArithmetic("not");
    }
    else if(token=="this"){
        eat(token);
        writer.writePush("pointer", 0);
    }
    else if(token=="false"||token=="null"){
        eat(tokenizer.currentToken);//currentToken is a keyword/integerConst
        writer.writePush("constant", 0);
    }
    else if(tokenizer.isIntegerConst(token)){
        eat(tokenizer.currentToken);
        writer.writePush("constant", stoi(token));
    }
    else if(tokenizer.isStringConst(token)){
        string result = token.substr(1,token.size()-2); 
        handleString();
        eat(result, "stringConstant");
    }
    else if(tokenizer.isIdentifier(token)){
        if(nextToken=="["){ //array
            string varName = token;
            eat(token);
            eat("[");
            Symbol row = SymbolTable::findVar(varName, subTable, classTable);
            writer.writePush(row.kind, row.index);
            compileExpression();
            writer.writeArithmetic("add");
            writer.writePop("pointer", 1);
            writer.writePush("that", 0);
            eat("]");
        }
        else if(nextToken=="(" || nextToken==".")//subroutineCall
        {
            compileSubroutineCall();
        }
        else{ //just an identifier
            Symbol row = SymbolTable::findVar(token, classTable, subTable);
            writer.writePush(row.kind, row.index);
            eat(tokenizer.currentToken);
        }
    }
    else if(token == "("){
        eat("(");
        compileExpression();
        eat(")");
    }
    else if(token=="-"){
        eat(token);
        compileTerm(); 
        writer.writeArithmetic("neg"); 
    }
    else if(token == "~"){
        eat(token);
        compileTerm();
        writer.writeArithmetic("not");

    }
    output << "</term>\n";
}

int CompilationEngine::compileExpressionList(){ //returns the number of expressions in the expressions list
    int expCount=0;    
    output << "<expressionList>\n";
    if(tokenizer.currentToken!=")")
    {
        expCount++;
        compileExpression();
        while(tokenizer.currentToken==","){
            expCount++;
            eat(",");
            compileExpression();
        }
    }
    return expCount;
    output << "</expressionList>\n";
}

void CompilationEngine::compileSubroutineCall(){
    string token = tokenizer.currentToken;
    string nextToken = tokenizer.lookAhead(0);

    if(nextToken=="(")
    {
        string subroutineName = tokenizer.currentToken;
        eat(subroutineName);//subroutineName

        eat("(");
        int expCount = compileExpressionList();
        if(expCount==0){
            writer.writePush("pointer", 0);
            writer.writeCall(className + "." + subroutineName, 1);
        }
        else writer.writeCall(token, expCount);
        eat(")");
    }
    else if(nextToken=="."){
        //Could be calling a method on an object, just a function or an OS method
        string objectName = tokenizer.currentToken;
        string methodName = tokenizer.lookAhead(1);

        eat(tokenizer.currentToken);
        eat(".");
        eat(tokenizer.currentToken);
        eat("(");

        try{ //if method is called on an object
            Symbol row = SymbolTable::findVar(objectName, classTable, subTable);
            writer.writePush(row.kind, row.index);
            int expCount = compileExpressionList();
            writer.writeCall(row.type + nextToken + methodName, expCount+1);
        }
        catch(runtime_error e){
            int expCount = compileExpressionList();
            writer.writeCall(token + nextToken + methodName, expCount);
        }
        eat(")");
    }
}

void CompilationEngine::handleString(){
    string token = tokenizer.currentToken;
    int size = token.size()-2;
    writer.writePush("constant", size);
    writer.writeCall("String.new", 1);
    for(char c : token){
        if(c=='"')continue;
        int pushValue = static_cast<int>(c);
        writer.writePush("constant", pushValue);
        writer.writeCall("String.appendChar", 2);
    }
}

//if no tag is mentioned then keyword by default
void CompilationEngine::eat(string s)
{
    if(s!=tokenizer.currentToken){
        cout << "Wrong syntax, current token is : " <<tokenizer.currentToken << " and string is " << s << endl;
        return;
    }
    string type = tokenizer.tokenType(s);
    output << "<" + type+ ">" + s + "</" + type + ">\n";
    tokenizer.advance();
}

void CompilationEngine::eat(string s, string t){
    output << "<" + t+ ">" + s + "</" + t + ">\n";
    tokenizer.advance();
}