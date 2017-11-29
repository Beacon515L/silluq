#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <regex>
using namespace std;

/*
SILLUQ - by Liam Greenough
---------------------------
Tool for simply versifying markdown urtext and checking citation completion

Silluq expects input text in stdin and directs output to stdout and messages to stderr. It can operate in realtime. It will terminate when stdin ends.

# Silluqification of regular text file
$ cat input.txt | silluq > bar.silluq

Here, input.txt is any regular text file and bar.silluq is the output urtext file. Silluqification refers to the process of rearranging a text file to make it easily citable. Physically, silluq adds line breaks after all sentence-final punctuation marks (by default !?.,).  The set of punctuation marks can be overridden:

$ cat input.txt | silluq -c "." > bar.silluq
will only linebreak at full stops.

Text is then cited by line number in the output file, e.g. (foo:420). The citation format is (text_designator:line_number), inclusive of the parentheses. The designator is completely arbitrary and not part of silluqification: it is purely used when citing the text to refer to different urtext files. The only constraint is that it not contain the colon or equals sign.

Citations can be ranges, e.g. (foo:1-2,6), (foo:3,5,6,1,5).

Silluqification is not mandatory, but helpful if texts are not already broken into many lines with relatively high granularity (e.g. the Bible). There are no constraints on line length in urtext files except that they cannot contain newlines (this being their delimiter).

# Verification of urtext citations

Given the following input.txt:
>According to some file, the sky is blue. (foo:1).

and the following bar.silluq:
>The sky is blue.
>The sea is green.

$ cat input.txt | silluq foo=bar.silluq

will produce:

    foo: 1/2 cited from 1 (50%)
    ---
    total: 1/2 cited from 1 (50%)

Silluq can operate on multiple urtext files:
$ cat input.txt | silluq foo=bar.silluq bar=foo.silluq

    foo: 1/2 cited from 1 (50%)
    bar: 0/8 cited from 1  (0%)
    ---
    total: 1/10 cited from 1 (10%)

*/

class Line{
public:
    string line = "";
    int uses = 0;
};

int main(int argc, char **argv) {
    //cerr << "silluq" << endl;
    map<string,vector<Line>*> urtexts; stringstream s; string input, chars = "!?.,";
    
    for (int i = 1; i < argc; i++){
        //Parse punctuation marks to override
        if(string(argv[i]).compare("-c")==0&&i<argc-1) {chars = string(argv[++i]).substr(1,string::npos); 
            chars = chars.substr(0,chars.length()-1);}
    else {//Parse the urtext argument
        //Check the argument is valid
       
        string arg = string(argv[i]); size_t delimPos = arg.find("=");
       if(delimPos!=string::npos&&arg.length()>delimPos+1){ 
           string name = arg.substr(0,delimPos), path = arg.substr(delimPos+1,string::npos);
           if(urtexts.count(name)!=0) cerr << name << "=" << path << ": designator collision, discarding" << endl;
           else {
            ifstream urtext (path);
            if(urtext.good()) {
               
               string line; vector<Line>* lines = new vector<Line>();
               
                while ( getline (urtext,line) ) {Line l; l.line = line; lines->push_back(l);}
                urtext.close();
                            
               urtexts[name] = lines;
            }
        else cerr << name << ": couldn't find " << path << endl;
     
       }
           
        }
        }
    }
    
    while(cin.good()) {string ln; getline(cin,ln); s << ln << endl;}
    input = s.str();
    
   // cerr << "Input done." << endl;
   // cerr << urtextCount << endl;
    
    //Process input
   // urtexts.size();
    //cerr << "LOL" << endl;
  if(!urtexts.empty()) {
    //Seek citations in input string
  
   regex re("\\(([^=:]*):([0-9\\-,]*)\\)");
sregex_iterator next(input.begin(), input.end(), re), end;
while (next != end) {
    smatch match = *next; string name = match[1].str(); 
    if(urtexts.count(name)!=0){
        string ranges = match[2].str()+",";
        int a = 0, b = 0; bool range = false;
        for (int i = 0; i < ranges.length(); i++){
            switch (ranges.at(i)){
                case '0': if(!range) a=a*10+0; else b=b*10+0; break;
                case '1': if(!range) a=a*10+1; else b=b*10+1; break;
                case '2': if(!range) a=a*10+2; else b=b*10+2; break;
                case '3': if(!range) a=a*10+3; else b=b*10+3; break;
                case '4': if(!range) a=a*10+4; else b=b*10+4; break;
                case '5': if(!range) a=a*10+5; else b=b*10+5; break;
                case '6': if(!range) a=a*10+6; else b=b*10+6; break;
                case '7': if(!range) a=a*10+7; else b=b*10+7; break;
                case '8': if(!range) a=a*10+8; else b=b*10+8; break;
                case '9': if(!range) a=a*10+9; else b=b*10+9; break;
                case '-': range = true;    break;
                default: if(a!=0)
                  try{  if(a>b) b=a;
                      for(int j = a-1; j<b; j++) urtexts.at(name)->at(j).uses++;
                           }
                            catch (const out_of_range& oor) {
                            if(range) cerr << match[0].str() << ": line range " << a << " to " << b << " out of urtext range" << endl;
                                else cerr << match[0].str() << ": line " << a << " out of urtext range" << endl;
  }                        // cerr << a << " " << b << name;
                            a = 0; b = 0; range = false; break;
                    
            }
        }
        
        
    }
    else cerr << match[0].str() << ": no urtext by this designator" << endl; 
    next++;
  } 
  
  //Totalize line counts
  int total_cited = 0, total_lines = 0, total_uses = 0;
  //cerr << "Totalizing." << endl;
  for(map<string,vector<Line>*>::iterator iter = urtexts.begin(); iter != urtexts.end(); ++iter)
{
string k = iter->first; vector<Line>* v = iter->second; int lines_cited = 0, uses = 0;
for(int i = 0; i < v->size(); i++){if(v->at(i).uses>0) lines_cited++; uses += v->at(i).uses;}
total_cited += lines_cited; total_lines += v->size(); total_uses += uses;
cout << k << ": " << lines_cited << "/" << v->size() << " cited from " << uses << " (" << (100*lines_cited/v->size()) << "%)" << endl;
}
cout << "---" << endl
<< "total: " << total_cited << "/" << total_lines << " cited from " << total_uses << " (" << (100*total_cited/total_lines) << "%)" << endl;
    
    }
    //Silluqification
    else {//cerr << "Silluqification in progress." << endl;
        size_t pos = -1;
        do {
            pos = input.find_first_of(chars,pos+1);
            input = input.substr(0,pos+1) + '\n' + input.substr(pos+1,string::npos);
            pos+=2;
        } while(pos!=string::npos&&pos+2<input.size());
        cout << input;
    }

    return 0;
}
