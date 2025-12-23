#include <bits/stdc++.h>
#include <fstream>
using namespace std;

void removespacesandcomments(string& s){
    string f="";
    for(int i=0;i<s.size();i++){
        if(i+1<s.size() && s[i]=='/' && s[i+1]=='/'){
            s=f;
            return;
        }
        else if(s[i]!=' '){
            f.push_back(s[i]);
        }
    }
    s=f;
}

string getjmp(string s){
    if(s=="JLT") return "100";
    else if(s=="JEQ") return "010";
    else if(s=="JGT") return "001";
    else if(s=="JLE") return "110";
    else if(s=="JGE") return "011";
    else if(s=="JNE") return "101";
    else if(s=="JMP") return "111";
    else return "000";
}

string getdest(string s){
    string bits = "000";
    if (s == "M")       bits = "001";
    else if (s == "D")  bits = "010";
    else if (s == "MD" || s == "DM") bits = "011";
    else if (s == "A")  bits = "100";
    else if (s == "AM" || s == "MA") bits = "101";
    else if (s == "AD" || s == "DA") bits = "110";
    else if (s == "AMD" || s == "ADM" || s == "MAD" || s == "MDA" || s == "DAM" || s == "DMA") 
        bits = "111";
    return bits;
}

string trim(const string& s) {
    int start = -1, end = -1;
    for(int i=0;i<(int)s.size();++i){
        if(s[i]!=' ' && s[i]!='\t' && s[i]!='\r' && s[i]!='\n'){ start = i; break; }
    }
    if(start==-1) return "";
    for(int i=(int)s.size()-1;i>=0;--i){
        if(s[i]!=' ' && s[i]!='\t' && s[i]!='\r' && s[i]!='\n'){ end = i; break; }
    }
    return s.substr(start, end-start+1);
}

string getcompute(string s){
    if (s == "0") return "0101010";
    else if (s == "1") return "0111111";
    else if (s == "-1") return "0111010";
    else if (s == "D") return "0001100";
    else if (s == "A") return "0110000";
    else if (s == "M") return "1110000";
    else if (s == "!D") return "0001101";
    else if (s == "!A") return "0110001";
    else if (s == "!M") return "1110001";
    else if (s == "-D") return "0001111";
    else if (s == "-A") return "0110011";
    else if (s == "-M") return "1110011";
    else if (s == "D+1") return "0011111";
    else if (s == "A+1") return "0110111";
    else if (s == "M+1") return "1110111";
    else if (s == "D-1") return "0001110";
    else if (s == "A-1") return "0110010";
    else if (s == "M-1") return "1110010";
    else if (s == "D+A"|| s== "A+D") return "0000010";
    else if (s == "D+M"|| s=="M+D") return "1000010";
    else if (s == "D-A") return "0010011";
    else if (s == "D-M") return "1010011";
    else if (s == "A-D") return "0000111";
    else if (s == "M-D") return "1000111";
    else if (s == "D&A") return "0000000";
    else if (s == "D&M") return "1000000";
    else if (s == "D|A") return "0010101";
    else if (s == "D|M") return "1010101";
    else return "ERROR";
}

bool isNumberString(const string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isdigit((unsigned char)c)) return false;
    }
    return true;
}

string convert_binary(int a){
    if(a==0) return "0";
    string ans="";
    while(a!=0){
        int rem=a%2;
        ans=to_string(rem)+ans;
        a/=2;
    }
    return ans;
}

string toUpper(string s){
    for(char &c : s) c = toupper((unsigned char)c);
    return s;
}

int main(int argc,char * argv[]){

    unordered_map<string,int> mp;
    mp["SP"]=0; mp["LCL"]=1; mp["ARG"]=2; mp["THIS"]=3; mp["THAT"]=4;
    for(int i=0;i<16;i++) mp["R"+to_string(i)] = i;
    mp["SCREEN"]=16384; mp["KBD"]=24576;

    int counter=0;
    int var=16;
    ifstream inputfile(argv[1],ios::in);
    if(!inputfile){ cerr<<"Cannot open input file\n"; return 1; }
    ofstream interfile("inter.txt", ios::out);
    string line;

    string filename=argv[1];
    int dot = -1;
    for(int i=0;i<(int)filename.size();++i) if(filename[i]=='.') dot=i;
    if(dot!=-1) filename = filename.substr(0, dot);
    filename += ".hack";

    vector<string> var_order;

    // PASS 1
    while(getline(inputfile, line)){
        removespacesandcomments(line);
        line = trim(line);
        if(line.empty()) continue;

        if(line[0] == '@') {
            string to_add = line.substr(1);
            if(!isNumberString(to_add) && mp.find(to_add) == mp.end()){
                mp[to_add] = -1;
                var_order.push_back(to_add);
            }
            interfile << '@' << to_add << '\n';
            counter++;
        }
        else if(line[0] == '(') {
            string to_add = line.substr(1, line.size()-2);
            mp[to_add] = counter; 
            continue; // no counter increment
        }
        else {
            interfile << line << '\n';
            counter++;
        }
    }

    // Assign variables in order
    for(const string &name : var_order){
        if(mp[name] == -1){
            mp[name] = var++;
        }
    }

    // PASS 2
    inputfile.close();
    interfile.close();
    ifstream infile("inter.txt",ios::in);
    ofstream outfile(filename);
    if(!infile){ cerr<<"Cannot open inter.txt\n"; return 1; }

    while(getline(infile,line)){
        if(line.empty()) continue;

        if(line[0]=='@'){
            string to_address=line.substr(1);
            if(isNumberString(to_address)){
                to_address=convert_binary(stoi(to_address));
            }
            else {
                to_address = convert_binary(mp[to_address]);
            }
            while((int)to_address.size()<16) to_address='0'+to_address;
            outfile << to_address << '\n';
        }
        else {
            string dest="000", comp="", jump="000";

            int eq=-1, sc=-1;
            for(int i=0;i<(int)line.size();++i){
                if(line[i]=='=' && eq==-1) eq=i;
                if(line[i]==';' && sc==-1) sc=i;
            }

            if(eq!=-1) {
                dest = getdest(line.substr(0,eq));
                if(sc==-1) comp = line.substr(eq+1);
                else comp = line.substr(eq+1, sc-eq-1);
            }
            else {
                if(sc==-1) comp=line;
                else comp=line.substr(0, sc);
            }

            if(sc!=-1) {
                jump = getjmp(line.substr(sc+1));
            }

            comp = getcompute(comp);
            string to_write = "111" + comp + dest + jump;
            outfile << to_write << '\n';
        }
    }

    infile.close();
    outfile.close();
    return 0;
}
