#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <string.h>
#include <iomanip>

using namespace std;

#define DEFINITION_LIST 0;
#define USE_LIST 1;
#define PROGRAM_TEXT 2;

class Token
{
    int lineNum;
    int lineOffset;
    string token;

public:
    Token(int ln, int lo, string tok)
    {
        lineNum = ln;
        lineOffset = lo;
        token = tok;
    }

    int getLineNum()
    {
        return lineNum;
    }

    int getLineOffset()
    {
        return lineOffset;
    }

    string getToken()
    {
        return token;
    }
};

class OpPair
{
public:
    string op;
    int addr;
    OpPair(string o, int ad)
    {
        op = o;
        addr = ad;
    }
};

class Module
{
private:
    int number;
    int offset;
    vector<string> useList;
    vector<OpPair> progText;

public:
    Module()
    {
    }

    void setNumber(int n) {
        number = n;
    }

    int getNumber() {
        return number;
    }

    int getOffset()
    {
        return offset;
    }

    void setOffset(int os)
    {
        offset = os;
    }

    vector<string> getUseList()
    {
        return useList;
    }

    void setUseList(string symbol)
    {
        useList.push_back(symbol);
    }

    vector<OpPair> getProgText()
    {
        return progText;
    }

    void setProgText(string op, int addr)
    {
        OpPair toAdd = OpPair(op, addr);
        progText.push_back(toAdd);
    }

    void toString()
    {
        cout << "--- Module info: ---" << endl;
        cout << "Module number: " << number << endl;
        cout << "Offset: " << offset << endl;
        cout << "Use List: ";
        for (string &s : useList)
        {
            cout << s << " ";
        }
        cout << endl;
        cout << "Prog Text: " << endl;
        for (OpPair &opp : progText)
        {
            cout << opp.op << " " << opp.addr << endl;
        }
    }
};

static vector<Token> tokenList;
static std::map<string, int> symbolTable;
static vector<Module> moduleList;
static int offset = 0;
const char delim[5] = "\t 	 ";

string convertToString(char *a, int size);
void tokenizer(string line, int lineNum);
void passOne();
void passTwo();
bool isNumber(const string &s);

int main(int argc, char **argv)
{
    string fileName = convertToString(argv[1], strlen(argv[1]));
    // Issues: imput "sometext.txt" and the length is only 8, which is "sometext".
    //  cout << sizeof(argv[1]) << " " << sizeof(char) << endl;
    //  cout << "Size of argv[1]" << sizeof(argv) / sizeof(char)+1 << endl;
    // cout << "openning: " << fileName << endl;
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

    inFile.close();

    passOne();
    // cout << endl;
    // cout << "====== after pass one =======" << endl;
    // for (Module &m : moduleList)
    // {
    //     m.toString();
    // }
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
        // Issue: strange string
        if (strcmp(token, "") != 0)
        {
            Token toAdd = Token(lineNum, lineOffset, token);
            tokenList.push_back(toAdd);
            // cout << "Token: " << toAdd.getLineNum() << ":" << toAdd.getLineOffset() << " " << toAdd.getToken() << endl;
            lineOffset += strlen(token) + 1;
        }
        else
        {
            lineOffset++;
        }
    }
}

void passOne()
{
    int tokenPairNum = 0;
    int index = 0;
    int type = DEFINITION_LIST;
    int moduleNum = 1;

    Module newModule;
    // cout << "====== Pass one ======" << endl;
    while (tokenPairNum == 0 && index < tokenList.size())
    {
        string tokenStr = tokenList[index].getToken();
        if (isNumber(tokenStr))
        {
            tokenPairNum = stoi(tokenStr);
        }
        else
        {
            cout << "ERROR: NUMBER EXPECTED" << endl;
        }
        index++;

        switch (type)
        {
        case 0:
            newModule = Module();
            newModule.setNumber(moduleNum);
            newModule.setOffset(offset);
            // cout << "Parsing def list" << endl;
            while (tokenPairNum > 0)
            {
                // cout << "pair num: " << tokenPairNum << endl;
                string symbol = tokenList[index].getToken();
                index++;
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
                // cout << "Use: " << usedSymbol << endl;
                index++;
                tokenPairNum--;

                newModule.setUseList(usedSymbol);
            }

            type++;
            // newModule.toString();
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
                // cout << "Operator: " << op << " Addr: " << addr << endl;
                index++;
                newModule.setProgText(op, addr);
                tokenPairNum--;
            }
            type = 0;
            // newModule.toString();
        default:

            moduleList.push_back(newModule);
            moduleNum++;
            break;
        }
    }
}

void passTwo()
{   
    // cout << endl;
    // cout << "====== Pass two ======" << endl;
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
        // m.toString();
        // cout << "+" << m.getOffset() << endl;
        for (OpPair &opp : m.getProgText())
        {
            // cout << "OP Pair: " << opp.op << opp.addr << endl;
            // printf("%3d: %4d\n", count, opp.addr);
            if (opp.op.compare("R") == 0)
            {
                // cout << "R!" << endl;
                currOperationAddr = m.getOffset() + opp.addr;
            }
            else if (opp.op.compare("A") == 0)
            {
                currOperationAddr = opp.addr;
                // cout << "A!" << endl;
            }
            else if (opp.op.compare("E") == 0)
            {
                int opcode = opp.addr / 1000;
                int operand = opp.addr % 1000;
                // cout << "operand: " << operand << endl;
                vector<string> useList = m.getUseList();
                // cout << "Use list size: " << useList.size() << endl;
                string tokenUsed = m.getUseList()[operand];
                // cout << "Token used here: " << tokenUsed << endl;
                int var = symbolTable[tokenUsed];
                // int var = 0;
                currOperationAddr = opcode * 1000 + var;
                // cout << "E!" << endl;
            }
            else if (opp.op.compare("I") == 0)
            {
                currOperationAddr = opp.addr;
                // cout << "I!" << endl;
            }
            else
            {
                cout << "ERROR: WRONG OPERATOR" << endl;
            }
            cout << setw(3) << setfill('0') << count << ": ";
            cout << setw(4) << setfill('0') << currOperationAddr << endl;
            count++;
        }
    }
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
