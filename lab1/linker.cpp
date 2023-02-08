#include<stdio.h>
#include <vector>
#include<iostream>
#include<fstream>
// #include<string.h>  
#include "error.cpp"

using namespace std;

bool syntaxVerify(string line, int index);
string convertToString(char *a, int size);
void tokenize(string line, int type);

int main(int argc, char **argv)
{
    string fileName = convertToString(argv[1], sizeof(argv) / sizeof(char)+1);
    cout << "openning: " << fileName << endl;
    string line;
    ifstream inFile;
    inFile.open(fileName);
    vector<string> stringVector;

    while (getline(inFile, line))
    {
        if (line.empty()) {
            break;
        }
        // syntaxVerify(line, 0);
        stringVector.push_back(line);
    }
 
    for (string &s: stringVector){
        cout << s << endl;
        tokenize(s, 0);
    }
    inFile.close();
}

/*
 *
 *
 */
void tokenize(string line, int type) {
    // Issues: str tok will get segmentation fault
    // char *lineArr = new char[line.length() + 1];
    char lineArr[line.length() + 1];
    strcpy(lineArr, line.c_str());
    cout << sizeof(lineArr) / sizeof(char) << endl;
    char *token = strtok(lineArr, " ");
    cout << "Token: " << token << endl;
    while (token != NULL)
    {
        token = strtok(NULL, " ");
        cout << "token: " << token << endl;
    }
    cout << "lineArr[]" << lineArr << "token:" << token << endl;
}

bool syntaxVerify(string line, int index) {
    
    int length = line.length();
    char *lineArr = new char[length + 1];
    strcpy(lineArr, line.c_str());

    vector<char *> tokenVector;
    char *token = strtok(lineArr, " \0");
    int num = stoi(token);
    cout << "token: " << token << endl;
    while (token != NULL)
    {
        token = strtok(NULL, " ");
        cout << "token: " << token << endl;
    }
    cout << "lineArr[]" << lineArr << "token:" << token << endl;
    return true;
}

string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

class Module {
    private:
        vector<string> defList;
        vector<string> useList;
        vector<string> progText;
    public:
        Module(vector<string> defLst, vector<string> useLst, vector<string> progTxt) {
            defList = defLst;
            useList = useLst;
            progText = progTxt;
        }
};
