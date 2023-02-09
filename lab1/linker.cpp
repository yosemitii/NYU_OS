#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <string.h>
#include <iomanip>
#include "Module.cpp"
#include "Token.cpp"

using namespace std;
#define DEFINITION_LIST 0;
#define USE_LIST 1;
#define PROGRAM_TEXT 2;


static vector<Token> tokenList;
static std::map<string, int> symbolTable;
vector<Module> moduleList;
static int offset = 0;

const char delim[5] = "\t 	 ";

bool syntaxVerify(string line, int index);
string convertToString(char *a, int size);
int passOne(string line, int type);
void __parseerror(int linenum, int lineoffset, int errcode);
void tokenizer(string line, int lineNum);
void passOneV2();
void passTwo();
bool isNumber(const string &s);

int main(int argc, char **argv)
{
    string fileName = convertToString(argv[1], sizeof(argv) / sizeof(char) + 1);
    // Issues: imput "sometext.txt" and the length is only 8, which is "sometext".
    //  cout << sizeof(argv[1]) << " " << sizeof(char) << endl;
    //  cout << "Size of argv[1]" << sizeof(argv) / sizeof(char)+1 << endl;
    cout << "openning: " << fileName << endl;
    string line;
    ifstream inFile;
    inFile.open(fileName);
    vector<string> stringVector;

    while (!inFile.eof())
    {
        getline(inFile, line);
        stringVector.push_back(line);
    }
    int lineNum = 1;
    int tokenLeft = 0;
    for (string &s : stringVector)
    {
        tokenizer(s, lineNum);
        lineNum++;
    }

    // regex re("[\\|,:\t]");
    // sregex_token_iterator first{line.begin(), line.end(), re, -1}, last;
    // //the '-1' is what makes the regex split (-1 := what was not matched)
    // vector<string> tokens = {first, last};

    // for (const string &s : tokenList) {
    //     cout << "Token: " << s << endl;
    // }
    inFile.close();

    passOneV2();

    for (Module &m : moduleList) {
        m.toString();
    }
    passTwo();

    
}

void tokenizer(string line, int lineNum)
{
    char *lineArr = new char[line.length() + 1];
    strcpy(lineArr, line.c_str());
    char *token;
    int lineOffset = 1;
    while ((token = strsep(&lineArr, delim)) != NULL)
    {
        // cout << "Strcmp: " << strcmp(token, "") << endl;
        // cout << sizeof(token)/sizeof(char *) << endl;
        // if (convertToString(token, sizeof(token)/sizeof(char *)) == " ") {
        //     cout << "white space" << endl;
        // }
        // Issue: strange string
        if (strcmp(token, "") != 0)
        {
            Token toAdd = Token(lineNum, lineOffset, token);
            tokenList.push_back(toAdd);
            cout << "Token: " << toAdd.getLineNum() << ":" << toAdd.getLineOffset() << " " << toAdd.getToken() << endl;
            lineOffset += strlen(token) + 1;
        }
        else
        {
            lineOffset++;
        }
    }
}

void passOneV2()
{
    int tokenPairNum = 0;
    int index = 0;
    int type = DEFINITION_LIST;
    Module newModule;
    while (tokenPairNum == 0 && index < tokenList.size())
    {
        // cout << "index: " << index << ": " << tokenList[index].getToken() << endl;
        // Parse the number of pairs
        string tokenStr = tokenList[index].getToken();
        if (isNumber(tokenStr))
        {
            tokenPairNum = stoi(tokenStr);
            cout << "Pair Number: " << tokenPairNum << endl;
        }
        else
        {
            cout << "ERROR: NUMBER EXPECTED" << endl;
        }
        index++;

        newModule = Module();
        newModule.setOffset(offset);
        switch (type)
        {
        case 0:
            cout << "Parsing def list" << endl;
            while (tokenPairNum > 0)
            {
                cout << "pair num: " << tokenPairNum << endl;
                string symbol = tokenList[index].getToken();
                index++;
                cout << "index: " << index << endl;
                string valStr = tokenList[index].getToken();
                int symbolVal;
                if (isNumber(valStr))
                {
                    symbolVal = stoi(valStr);
                }
                else
                {
                    cout << "ERROR: NUMBER EXPECTED" << endl;
                }

                index++;
                symbolTable[symbol] = symbolVal + offset;
                tokenPairNum--;
            }
            type++;
            break;
        case 1:
            while (tokenPairNum > 0)
            {
                string usedSymbol = tokenList[index].getToken();
                cout << "Use: " << usedSymbol << endl;
                index++;
                tokenPairNum--;
                
                newModule.setUseList(usedSymbol);
            }
            newModule.toString();
            type++;
            break;
        case 2:
            
            offset += tokenPairNum;
            while (tokenPairNum > 0)
            {
                string op = tokenList[index].getToken();
                if (isNumber(op))
                {
                    cout << "ERROR: SYMBOL EXPECTED" << endl;
                }
                index++;
                string addrStr = tokenList[index].getToken();
                if (!isNumber(addrStr))
                {
                    cout << "ERROR: NUMBER EXPECTED" << endl;
                }
                int addr = stoi(addrStr);
                cout << "Operator: " << op << " Addr: " << addr << endl;
                index++;
                newModule.setProgText(op, addr);
                tokenPairNum--;
            }
            type = 0;
        default:
            moduleList.push_back(newModule);
            break;
        }
    }
}

void passTwo()
{
    cout << "====== Pass two ======" << endl;
    cout << "Symbol Table" << endl;

    for (const auto &kv : symbolTable)
    {
        cout << kv.first << "=" << kv.second << endl;
    }
    cout << endl;
    cout << "Memory Map" << endl;

    int count = 0;
    int currOperationAddr;
    for (Module &m : moduleList)
    {
        m.toString();
        cout << "+" << m.getOffset() << endl;
        for (OpPair& opp : m.getProgText()){
            // cout << "OP Pair: " << opp.op << opp.addr << endl;
            // printf("%3d: %4d\n", count, opp.addr);
            if (opp.op.compare("R") == 0) {
                // cout << "R!" << endl;
                currOperationAddr = m.getOffset() + opp.addr;
            }
            else if (opp.op.compare("A") == 0) {
                currOperationAddr = opp.addr;
                // cout << "A!" << endl;
            }
            else if (opp.op.compare("E") == 0) {
                int opcode = opp.addr / 1000;
                int operand = opp.addr % 1000;
                cout << "operand: " << operand << endl;
                vector<string> useList = m.getUseList();
                // cout << "Use list size: " << useList.size() << endl;
                // string tokenUsed = m.getUseList()[operand];
                // cout << "Token used here: " << tokenUsed << endl;
                // int var = symbolTable[tokenUsed];
                int var = 0;
                currOperationAddr = opcode * 1000 + var;
                // cout << "E!" << endl;
            }
            else if (opp.op.compare("I") == 0) {
                currOperationAddr = opp.addr;
                // cout << "I!" << endl;
            }
            else{
                cout << "ERROR: WRONG OPERATOR" << endl;
            }
            cout << setw(3) << setfill('0') << count << ": ";
            cout << setw(4) << setfill('0') << currOperationAddr << endl;
            count++;
        }
    }
}

int passOne(string line, int type, int tokenLeft)
{
    // Issues: str tok will get segmentation fault
    // char *lineArr = new char[line.length() + 1];

    // Issues: use char lineArr[] and the issue stays
    // char lineArr[line.length() + 1];
    char *lineArr = new char[line.length() + 1];
    strcpy(lineArr, line.c_str());
    int lineOffset = 1;
    // cout << sizeof(lineArr) / sizeof(char) << endl;
    // char *token = strtok(lineArr, " ");
    // cout << "Token: " << token << endl;

    char *token;
    // const char delim[5] = "\t 	 ";
    if (type % 3 == 0)
    {
        int count = 0;
        string symbol;
        int symbolVal;
        int totalPairs;
        while ((token = strsep(&lineArr, delim)) != NULL)
        {
            cout << "line: " << type << "Token: " << token << " size: " << sizeof(char) << endl;
            if (count == 0)
            {
                totalPairs = stoi(token);
            }
            else if (count % 2 == 1)
            {
                symbol = token;
                // if (symbolTable[symbol] != NULL) {
                //     return false;
                //     __parseerror(lineNum, lineOffset, 0);
                // }
            }
            else
            {
                // Define a symbol
                // Error 2: symbol multiple definition
                symbolVal = stoi(token) + offset;
                symbolTable[symbol] = symbolVal;
            }
            count++;
            lineOffset += sizeof(token) / sizeof(char *);
        }
        // Error 1: symbol number mismatch
        // Verify: total pair number matches or not
        if (totalPairs != (count - 1) / 2)
        {
            __parseerror(type, lineOffset, 0);
            // return false;
        }
        else
        {
            for (const auto &kv : symbolTable)
            {
                cout << kv.first << "=" << kv.second << endl;
            }
        }
    }
    else if (type % 3 == 1)
    {
        while ((token = strsep(&lineArr, " ")) != NULL)
        {
            cout << "Token: " << token << endl;
        }
    }
    else
    {
        if ((token = strsep(&lineArr, " ")) != NULL)
        {
            offset += stoi(token);
            cout << "new offset: " << offset << endl;
        }
    }
    return true;

    // while (token != NULL)
    // {
    //     token = strtok(NULL, " ");
    //     cout << "token: " << token << endl;
    // }
    // cout << "lineArr[]" << lineArr << "token:" << token << endl;
}

bool syntaxVerify(string line, int index)
{

    int length = line.length();
    char *lineArr = new char[length + 1];
    strcpy(lineArr, line.c_str());

    vector<char *> tokenVector;
    char *token = strtok(lineArr, " \0");
    int num = stoi(token);
    cout << "token: " << token << endl;
    while (token != NULL)
    {
        token = strtok(NULL, " \n");
        cout << "token: " << token << endl;
    }
    cout << "lineArr[]" << lineArr << "token:" << token << endl;
    return true;
}

string convertToString(char *a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++)
    {
        s = s + a[i];
    }
    return s;
}

bool isNumber(const string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}



void __parseerror(int linenum, int lineoffset, int errcode)
{
    static char *errstr[] = {
        "NUM_EXPECTED",           // Number expect, anything >= 2^30 is not a number either
        "SYM_EXPECTED",           // Symbol Expected
        "ADDR_EXPECTED",          // Addressing Expected which is A/E/I/R
        "SYM_TOO_LONG",           // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE", // > 16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "TOO_MANY_INSTR”, // total num_instr exceeds memory size (512)"};
    printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
}
