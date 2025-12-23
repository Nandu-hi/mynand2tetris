#include <bits/stdc++.h>
using namespace std;

string trim(const string& s) {
    int start = -1, end = -1;
    for(int i=0;i<(int)s.size();++i){
        if(!isspace(s[i])) { start=i; break; }
    }
    if(start==-1) return "";
    for(int i=(int)s.size()-1;i>=0;--i){
        if(!isspace(s[i])) { end=i; break; }
    }
    return s.substr(start, end-start+1);
}

unordered_map<string,string> mp = {
    {"argument","ARG"},
    {"local","LCL"},
    {"this","THIS"},
    {"that","THAT"}
};

unordered_map<string,string> sym = {
    {"add","+"},
    {"sub","-"},
    {"and","&"},
    {"or","|"},
    {"eq","JEQ"},
    {"lt","JLT"},
    {"gt","JGT"}
};


void writePush(ofstream &ofile, string segment, int index, string name) {
    if(segment=="constant"){
        ofile << "@" << index << "\n";
        ofile << "D=A\n";
    } else if(segment=="temp") {
        ofile << "@5\n";
        ofile << "D=A\n";
        ofile << "@" << index << "\n";
        ofile << "A=D+A\n";
        ofile << "D=M\n";
    } else if(segment=="pointer") {
        if(index==0) ofile << "@THIS\nD=M\n";
        else ofile << "@THAT\nD=M\n";
    } else if(segment=="static") {
        ofile << "@" << name << "." << index << "\n";
        ofile << "D=M\n";
    } else if(mp.count(segment)) {
        ofile << "@" << mp[segment] << "\n";
        ofile << "D=M\n";
        ofile << "@" << index << "\n";
        ofile << "A=D+A\n";
        ofile << "D=M\n";
    }
    ofile << "@SP\n";
    ofile << "A=M\n";
    ofile << "M=D\n";
    ofile << "@SP\n";
    ofile << "M=M+1\n";
}

void writePop(ofstream &ofile, string segment, int index, string name) {
    if(segment=="temp") {
        ofile << "@5\n";
        ofile << "D=A\n";
        ofile << "@" << index << "\n";
        ofile << "D=D+A\n";
        ofile << "@R13\nM=D\n";
    } else if(segment=="pointer") {
        if(index==0) ofile << "@SP\nAM=M-1\nD=M\n@THIS\nM=D\n";
        else ofile << "@SP\nAM=M-1\nD=M\n@THAT\nM=D\n";
        return;
    } else if(segment=="static") {
        ofile << "@SP\nAM=M-1\nD=M\n@" << name << "." << index << "\nM=D\n";
        return;
    } else if(mp.count(segment)) {
        ofile << "@" << mp[segment] << "\n";
        ofile << "D=M\n";
        ofile << "@" << index << "\n";
        ofile << "D=D+A\n";
        ofile << "@R13\nM=D\n";
    }
    ofile << "@SP\n";
    ofile << "AM=M-1\n";
    ofile << "D=M\n";
    ofile << "@R13\n";
    ofile << "A=M\n";
    ofile << "M=D\n";
}

void writeBinary(ofstream &ofile, string op) {
    ofile << "@SP\n";
    ofile << "AM=M-1\n";
    ofile << "D=M\n";
    ofile << "A=A-1\n";
    ofile << "M=M" << sym[op] << "D\n";
}

void writeUnary(ofstream &ofile, string op) {
    ofile << "@SP\n";
    ofile << "A=M-1\n";
    if(op=="neg") ofile << "M=-M\n";
    else ofile << "M=!M\n";
}

void writeCompare(ofstream &ofile, string op, int &counter) {
    counter++;
    int trueLabel = counter;
    counter++;
    int endLabel = counter;

    ofile << "@SP\n";
    ofile << "AM=M-1\n";
    ofile << "D=M\n";
    ofile << "A=A-1\n";
    ofile << "D=M-D\n";
    ofile << "@ALabel_" << trueLabel << "\n";
    ofile << "D;" << sym[op] << "\n";
    ofile << "D=0\n";
    ofile << "@ALabel_" << endLabel << "\n";
    ofile << "0;JMP\n";
    ofile << "(ALabel_" << trueLabel << ")\n";
    ofile << "D=-1\n";
    ofile << "(ALabel_" << endLabel << ")\n";
    ofile << "@SP\n";
    ofile << "A=M-1\n";
    ofile << "M=D\n";
}
void writeLabel(ofstream &ofile, string label,string name){
    ofile << "(" << name << "." << label << ")" << endl;
}

void Writegoto(ofstream & ofile,string label,string name){
    ofile << "@" << name << "." << label << endl << "0;JMP" << endl;
}

void writeifgoto(ofstream & ofile,string label,string name){
    ofile << "@SP\nAM=M-1\nD=M\n";
    ofile << "@" << name << "." << label << endl;
    ofile << "D;JNE\n";
}

int main(int argc, char* argv[]) {
    ifstream infile(argv[1]);

    string filename=argv[1];
    int dot = filename.find('.');
    if(dot!=-1) filename = filename.substr(0, dot);
    
    string name = filename.substr(filename.find_last_of("/\\")+1);
    ofstream ofile(filename+".asm");

    int counter=0;
    string s;
    while(getline(infile,s)) {
        s = trim(s);
        if(s.empty()) continue;
        if(s.rfind("//",0)==0) continue;

        stringstream ss(s);
        string ins, segment; int index=0;
        ss >> ins >> segment >> index;

        if(ins=="push") writePush(ofile, segment, index, name);
        else if(ins=="pop") writePop(ofile, segment, index, name);

        else if(ins=="add"||ins=="sub"||ins=="and"||ins=="or") writeBinary(ofile, ins);

        else if(ins=="neg"||ins=="not") writeUnary(ofile, ins);

        else if(ins=="eq"||ins=="lt"||ins=="gt") writeCompare(ofile, ins, counter);

        else if(ins=="label") writeLabel(ofile,segment,name);

        else if(ins=="goto") Writegoto(ofile,segment,name);

        else if(ins=="if-goto") writeifgoto(ofile,segment,name);
    }
}
