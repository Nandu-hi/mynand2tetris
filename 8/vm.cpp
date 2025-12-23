#include <bits/stdc++.h>
using namespace std;

void Pushtostack(ofstream &ofile, string val){
    ofile << "@" << val << endl << "D=A" << endl;
    ofile << "@SP\nA=M\nM=D\n@SP\nM=M+1\n";
}

void Pushtostackv(ofstream &ofile, string val){
    ofile << "@" << val << endl << "D=M" << endl;
    ofile << "@SP\nA=M\nM=D\n@SP\nM=M+1\n";
}

void writeArithmetic(ofstream &ofile, string ins){
    if(ins == "add" || ins == "sub" || ins == "and" || ins == "or"){
        ofile << "@SP\nAM=M-1\nD=M\nA=A-1\n";
        if (ins == "add") ofile << "M=M+D\n";
        else if (ins == "sub") ofile << "M=M-D\n";
        else if (ins == "and") ofile << "M=M&D\n";
        else if (ins == "or")  ofile << "M=M|D\n";
    } 
    else if(ins == "neg" || ins == "not"){
        ofile << "@SP\nA=M-1\n";
        if (ins == "neg") ofile << "M=-M\n";
        else ofile << "M=!M\n";
    } 
    else if(ins == "eq" || ins == "gt" || ins == "lt"){
        static int labelCount = 0;

        string lbl = "LABEL" + to_string(labelCount++);

        ofile << "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n";
        ofile << "M=-1\n@" << lbl << "\n";

        if (ins == "eq") ofile << "D;JEQ\n";
        else if (ins == "gt") ofile << "D;JGT\n";
        else ofile << "D;JLT\n";

        ofile << "@SP\nA=M-1\nM=0\n(" << lbl << ")\n";
    }
}

void writePush(ofstream &ofile, string segment, int index, string name){
    if(segment == "constant"){
        Pushtostack(ofile, to_string(index));
    }
    else if(segment == "local" || segment == "argument" || segment == "this" || segment == "that"){
        string base = (segment == "local") ? "LCL" :
                      (segment == "argument") ? "ARG" :
                      (segment == "this") ? "THIS" : "THAT";

        ofile << "@" << index << "\nD=A\n@" << base << "\nA=M+D\nD=M\n";
        ofile << "@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    } 
    else if(segment == "temp"){
        int addr = 5 + index;
        Pushtostackv(ofile, to_string(addr));
    } 
    else if(segment == "pointer"){
        Pushtostackv(ofile, (index == 0) ? "THIS" : "THAT");
    } 
    else if(segment == "static"){
        ofile << "@" << name << "." << index << "\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
        cout << name << endl;
    }
}

void writePop(ofstream &ofile, string segment, int index, string name){
    if(segment == "local" || segment == "argument" || segment == "this" || segment == "that"){
        string base = (segment == "local") ? "LCL" :
                      (segment == "argument") ? "ARG" :
                      (segment == "this") ? "THIS" : "THAT";
        ofile << "@" << index << "\nD=A\n@" << base << "\nD=M+D\n@R15\nM=D\n";
        ofile << "@SP\nAM=M-1\nD=M\n@R15\nA=M\nM=D\n";
    } 
    else if(segment == "temp"){
        int addr = 5 + index;
        ofile << "@SP\nAM=M-1\nD=M\n@" << addr << "\nM=D\n";
    } 
    else if(segment == "pointer"){
        ofile << "@SP\nAM=M-1\nD=M\n@" << ((index == 0) ? "THIS" : "THAT") << "\nM=D\n";
    } 
    else if(segment == "static"){
        ofile << "@SP\nAM=M-1\nD=M\n@" << name << "." << index << "\nM=D\n";
    }
}

void Writegoto(ofstream &ofile, string label, string name){
    string target = name + string("$") + label;
    ofile << "@" << target << endl << "0;JMP" << endl;
}

void writeLabel(ofstream &ofile, string label, string name){
    string target = name + string("$") + label;
    ofile << "(" << target << ")" << endl;
}

void writeifgoto(ofstream &ofile, string label, string name){
    string target = name + string("$") + label;
    ofile << "@SP\nAM=M-1\nD=M\n@" << target << endl << "D;JNE\n";
}

int main(int argc, char *argv[]) {
    string path = argv[1];
    vector<string> vm;
    
    if(filesystem::is_directory(path)){
        for (auto &p : filesystem::directory_iterator(path)){
            if (p.path().extension() == ".vm"){
                vm.push_back(p.path());
            }
        }
    } 
    else{
        vm.push_back(path);
    }

    string ofileName;
    if(filesystem::is_directory(path)){
        ofileName = path + "/" + filesystem::path(path).stem().string() + ".asm";
    }
    else{
        ofileName = filesystem::path(path).replace_extension(".asm").string();
    }

    ofstream ofile(ofileName);
        
    //bootstrap code
    if(filesystem::is_directory(path)){
        ofile << "@256\nD=A\n@SP\nM=D\n";
    
        string retLabel = "RET_0";
        Pushtostack(ofile, retLabel);
        ofile << "@LCL\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
        ofile << "@ARG\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
        ofile << "@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
        ofile << "@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
        ofile << "@SP\nD=M\n@5\nD=D-A\n@ARG\nM=D\n";
        ofile << "@SP\nD=M\n@LCL\nM=D\n";
        ofile << "@Sys.init\n0;JMP\n";
        ofile << "(" << retLabel << ")\n";
    }


    for(auto file : vm){
        ifstream infile(file);
        string filename = filesystem::path(file).filename().string();
        string name = filename.substr(0, filename.find_last_of('.'));
        string s;

        while (getline(infile, s)) {
            stringstream ss(s);
            string ins, segment;
            int index;
            ss >> ins >> segment >> index;

            if(ins == "push") writePush(ofile, segment, index, name);
            else if(ins == "pop") writePop(ofile, segment, index, name);
            else if(ins == "add" || ins == "sub" || ins == "neg" || ins == "eq" ||
                     ins == "gt" || ins == "lt" || ins == "and" || ins == "or" || ins == "not")
                writeArithmetic(ofile, ins);
            else if(ins == "label") writeLabel(ofile, segment, name);
            else if(ins == "goto") Writegoto(ofile, segment, name);
            else if(ins == "if-goto") writeifgoto(ofile, segment, name);
            else if(ins == "function"){
                ofile << "(" << segment << ")" << endl;
                for (int i = 0; i < index; i++) Pushtostack(ofile, "0");
                cout << segment << endl;
            }
            else if(ins == "call"){
                static int callCount = 1;
                string retLabel = "RET_" + to_string(callCount++);
                Pushtostack(ofile, retLabel);
                ofile << "@LCL\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
                ofile << "@ARG\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
                ofile << "@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
                ofile << "@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
                ofile << "@SP\nD=M\n@" << index << "\nD=D-A\n@5\nD=D-A\n@ARG\nM=D\n";
                ofile << "@SP\nD=M\n@LCL\nM=D\n";
                ofile << "@" << segment << endl << "0;JMP" << endl;
                ofile << "(" << retLabel << ")" << endl;
                cout << segment<< endl;
            }
            else if (ins == "return"){
                ofile << "@LCL\nD=M\n@R13\nM=D\n";
                ofile << "@5\nA=D-A\nD=M\n@R14\nM=D\n";
                writePop(ofile, "argument", 0, name);
                ofile << "@ARG\nD=M+1\n@SP\nM=D\n";
                ofile << "@R13\nAM=M-1\nD=M\n@THAT\nM=D\n";
                ofile << "@R13\nAM=M-1\nD=M\n@THIS\nM=D\n";
                ofile << "@R13\nAM=M-1\nD=M\n@ARG\nM=D\n";
                ofile << "@R13\nAM=M-1\nD=M\n@LCL\nM=D\n";
                ofile << "@R14\nA=M\n0;JMP\n";
            }
        }
    }
    return 0;
}
