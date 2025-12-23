#include <bits/stdc++.h>
using namespace std;

// Remove comments from a line
//it gives empty line if the line is a part of comment
string trim(string s,bool &is_comment){
    string ans="";
    for(int i=0;i<s.size();i++){
        if(is_comment){
            if(i+1<s.size() && s[i]=='*' && s[i+1]=='/') {
                is_comment=false;
                i++;
            }
            continue;
        }
        if(i+1 < s.size() && s[i] == '/' && s[i+1] == '/') break;
        else if(i+1<s.size() && s[i] == '/' && s[i+1] == '*'){
            is_comment = true;
            i++;
        }
        else{
            ans += s[i];
        }
    }
    return ans;
}

// to adjust xml < , > & symbol
string escape_xml(char c){
    if (c=='<') return "&lt;";
    if (c=='>') return "&gt;";
    if (c=='&') return "&amp;";
    return string(1, c);
}

int main(int argc, char *argv[]) {
    vector<string> keyword = {
        "class", "constructor", "function", "method", "field", "static",
        "var", "int", "char", "boolean", "void", "true", "false",
        "null", "this", "let", "do", "if", "else", "while", "return"
    };
    vector<char> c = {
        '{', '}', '(', ')', '[', ']', '.', ',', ';',
        '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'
    };

    string infile_name = argv[1];
    ifstream infile(infile_name);

    string outfile_name = infile_name.substr(0, infile_name.find_last_of('.')) + ".xml";
    ofstream outfile(outfile_name);

    outfile << "<tokens>\n";
    string s;
    bool is_comment = false;

    while(getline(infile, s)){
        s = trim(s, is_comment);
        if(s.empty()) continue;

        for(int i=0;i<s.size();i++) {
            if(isspace(s[i])) continue;
            // Integer constant
            if(isdigit(s[i])){
                string integer = "";
                while(i<s.size() && isdigit(s[i])){
                    integer+=s[i];
                    i++;
                }
                i--;
                outfile << "\t<integerConstant> " << integer << " </integerConstant>\n";
            }
            // Symbol
            else if(find(c.begin(), c.end(), s[i]) != c.end()){
                outfile << "\t<symbol> " << escape_xml(s[i]) << " </symbol>\n";
            }
            // String constant
            else if(s[i]=='"' || s[i]=='\''){
                char quote=s[i];
                i++;
                string st="";
                while(i<s.size() && s[i]!=quote){
                    st+=s[i];
                    i++;
                }
                outfile << "\t<stringConstant> " << st << " </stringConstant>\n";
            }
            // Identifier or keyword
            else{
                string st="";
                while(i<s.size() && !isspace(s[i]) && find(c.begin(),c.end(),s[i])==c.end() && s[i]!='"' && s[i]!='\'') {
                    st+=s[i];
                    i++;
                }
                i--;
                if(find(keyword.begin(),keyword.end(),st)!=keyword.end())
                    outfile << "\t<keyword> " << st << " </keyword>\n";
                else  outfile << "\t<identifier> " << st << " </identifier>\n";
            }
        }
    }

    outfile << "</tokens>\n";
    cout << "Tokens written to: " << outfile_name << endl;
    return 0;
}
