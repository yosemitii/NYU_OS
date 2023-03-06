#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <string.h>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

#define DEFINITION_LIST 0;
#define USE_LIST 1;
#define PROGRAM_TEXT 2;

string convertToString(char *a);
const char delim[6] = "\t 	 \n";
void __parseerror(int errcode, int linenum, int lineoffset);

class Token
{
    int lineNum;
    int lineOffset;
    string token;

public:
    Token()
    {
    }
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
    int size;
    vector<string> useList;
    vector<OpPair> progText;

public:
    Module()
    {
    }

    void setNumber(int n)
    {
        number = n;
    }

    int getNumber()
    {
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
        size++;
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

    int getSize()
    {
        return size;
    }
};

class Tokenizor
{
private:
    // ifstream inFile;
    string fileName;
    int lineNum;
    int nextStartIndex;
    int prevOffset;
    char *token;
    string line;
    char *lineArr;
    int startIndex;
    Token currToken;
    bool notEOF;
    int prevEnd;

public:
    Tokenizor(string fname)
    {
        // inFile.open(fileName);
        fileName = fname;
        lineNum = 0;
        nextStartIndex = 0;
        prevOffset = -1;
        token = NULL;
        lineArr = NULL;
        startIndex = 0;
        notEOF = true;
        prevEnd = -1;
    }

    char *getToken()
    {
        // cout << "Getting token after line: " << lineNum << " index: " << startIndex << endl;
        ifstream inFile;

        inFile.open(fileName);
        string line;
        int lineCnt = 0;
        int charCnt = 0;

        while (!inFile.eof() && lineCnt <= lineNum)
        {
            getline(inFile, line);
            lineCnt++;
        }
        // count to the token
        char *lineArr = new char[line.length() + 1];

        // cout <<lineCnt <<  " Line:" << line << endl;
        strcpy(lineArr, line.c_str());

        char *token;
        string tokenStr;
        // token = strtok(lineArr, delim);
        // while (token)
        map<char *, int> startIndex;
        int lastEnd = 0;

        while ((token = strsep(&lineArr, delim)) != NULL)
        {
            // cout << "xx" << endl;
            tokenStr = convertToString(token);

            startIndex[token] = line.find(tokenStr, lastEnd);
            lastEnd = startIndex[token] + strlen(token);
        }
        for (auto &kv : startIndex)
        {
            // cout << kv.first << endl;
            // cout << "xx" << endl;
            if (strcmp(kv.first, "") != 0)
            {
                if (kv.second > prevOffset)
                {
                    // cout <<  kv.second << " " << strlen(token) << endl;
                    // nextStartIndex = currOffset + strlen(token);
                    prevOffset = kv.second;
                    currToken = Token(lineNum, prevOffset, convertToString(kv.first));
                    // cout << "Now nextStartIndex: " << nextStartIndex << endl;
                    // cout << "Token: " << kv.first << endl;
                    // cout << "Returning: " << kv.first << endl;
                    // getTokenInfo();
                    return kv.first;
                }
            }
        }
        prevOffset = -1;
        nextStartIndex = 0;
        lineNum++;
        // cout << "cannot find it " << endl;
        // return NULL;
        if (inFile.eof())
        {
            inFile.close();
            // cout << "file closed" << endl;
            // int lastLineNum = currToken.getLineNum();
            // int lastIndex = currToken.getLineOffset() + currToken.getToken().length();
            int lastIndex = getLastIndex();
            currToken = Token(lineNum - 2, lastIndex, "");
            notEOF = false;
            // cout << "Returning: NULL" << endl;
            return NULL;
        }
        else
        {
            // cout << "change new line" << endl;
            return getToken();
        }

        // while (charCnt <= startIndex)
        // {
        //     if ((token = strsep(&lineArr, delim)) != NULL)
        //     {
        //         if (strcmp(token, "") != 0)
        //         {
        //             startIndex = line.find(convertToString(token));
        //             charCnt = line.find(convertToString(token)) + strlen(token);
        //             cout << "Token: " << token;
        //             // // cout << "lineNum: " << lineNum << " startIndex: " << startIndex << endl;
        //             cout << " Line: " << lineCnt << " Index: " << charCnt << endl;
        //         }
        //         else
        //         {
        //             charCnt++;
        //         }
        //     }
        //     else
        //     {
        //         break;
        //     }
        // }
        // cout << "Get token: " << token << endl;
        // // adjust and return

        // if (inFile.eof() && charCnt >= line.length())
        // {
        //     cout << "End of file" << endl;
        //     return NULL;
        // }
        // else if (charCnt >= line.length())
        // {
        //     // cout << "End of line" << endl;
        //     startIndex = 0;
        //     lineNum++;
        //     token = getToken();
        //     // return token;
        // }
        // else if (token == NULL)
        // {
        //     cout << "null error" << endl;
        // }
        // else
        // {
        //     startIndex = charCnt + strlen(token);
        //     // return token;
        // }
        // // cout << endl;
        // return token;
    }

    void getTokenInfo()
    {
        cout << "Line: " << currToken.getLineNum() << " Index: " << currToken.getLineOffset() << endl;
    }

    string readIAER()
    {
        // cout << "reading IEAR" << endl;
        char *token = getToken();
        // string tokenStr = convertToString(token);
        if (token == NULL)
        {
            __parseerror(2, currToken.getLineNum(), currToken.getLineOffset());
            exit(1);
        }
        if (strlen(token) == 1 && ((strcmp(token, "I") == 0) || (strcmp(token, "A") == 0) ||
                                   (strcmp(token, "E") == 0) || (strcmp(token, "R") == 0)))
        {
            return currToken.getToken();
        }
        else
        {
            __parseerror(6, currToken.getLineNum(), currToken.getLineOffset());
            exit(1);
        }
    }

    string readSymbol()
    {
        // cout << "reading symbol" << endl;
        char *token = getToken();
        if (token == NULL)
        {
            // cout << "reading symbol but null" << endl;
            __parseerror(1, currToken.getLineNum(), currToken.getLineOffset());
            exit(1);
        }
        if (strlen(token) > 256)
        {
            __parseerror(3, currToken.getLineNum(), currToken.getLineOffset());
            exit(1);
        }
        if (!isalpha(token[0]))
        {
            __parseerror(1, currToken.getLineNum(), currToken.getLineOffset());
            exit(1);
        }
        return currToken.getToken();
    }

    int readInt()
    {
        // cout << "reading Int" << endl;
        char *token = getToken();
        if (token == NULL)
        {
            cout << "reading int but null" << endl;
            // cout << "NULL" << endl;
            __parseerror(0, currToken.getLineNum(), currToken.getLineOffset());
            exit(1);
        }
        // cout << "not null" << endl;
        for (int i = 0; i < strlen(token); i++)
        {
            if (!isdigit(token[i]))
            {
                // cout << "not a digit" << i << endl;
                __parseerror(0, currToken.getLineNum(), currToken.getLineOffset());
                exit(1);
            }
        }
        return stoi(currToken.getToken());
    }

    int readAddr()
    {
        // cout << "reading Int" << endl;
        char *token = getToken();
        if (token == NULL)
        {
            // cout << "NULL" << endl;
            __parseerror(3, currToken.getLineNum(), currToken.getLineOffset());
            exit(1);
        }
        // cout << "not null" << endl;
        for (int i = 0; i < strlen(token); i++)
        {
            if (!isdigit(token[i]))
            {
                // cout << "not a digit" << i << endl;
                __parseerror(3, currToken.getLineNum(), currToken.getLineOffset());
                exit(1);
            }
        }
        return stoi(currToken.getToken());
    }

    bool hasNext()
    {
        return notEOF;
    }

    Token getCurrToken()
    {
        return currToken;
    }

    int getLastIndex()
    {
        ifstream inFile;
        inFile.open(fileName);
        int lineCnt = 0;
        string line;
        while (!inFile.eof() && lineCnt < lineNum - 1)
        {
            getline(inFile, line);
            lineCnt++;
            // cout << lineCnt << " line cnt: "  << line << endl;
        }
        // cout << "Last line: " << lineCnt << endl;
        char *lineArr = new char[line.length() + 1];
        strcpy(lineArr, line.c_str());
        // cout << "Line length: " << line.length() << endl;
        char *token;
        int lastEnd = 0;
        string tokenStr;
        if (line.length() == 0)
        {
            // cout << "Empty line: " << 0 << endl;
            return 0;
        }
        while ((token = strsep(&lineArr, delim)) != NULL)
        {
            // cout << "Token: " << token << endl;
            tokenStr = convertToString(token); 

            startIndex[token] = line.find(tokenStr, lastEnd);
            // cout << "startIndex: " << line.find(tokenStr, lastEnd) << endl;
            // Issues: here strlen(token) = 0, tokenStr.length() = 1;
            // lastEnd = startIndex[token] + strlen(token);
            lastEnd = startIndex[token] + tokenStr.length();
            // cout << tokenStr.length() << endl;
            //
        }
        inFile.close();
        // lastEnd += tokenStr.length();
        // cout << "Last index: " << lastEnd << endl;
        return lastEnd;
    }
};
// static vector<Token> tokenList;
static std::map<string, int> symbolTable;
// static vector<Module> moduleList;

void lineToTokens(vector<Token> &tokenList, string line, int lineNum);
void passOneV2(string fileName);
void passTwoV2(string fileName);
bool isNumber(const string &s);
Token *getToken(string fileName, int &lineNum, int &indexNum);
static int offset;

int main(int argc, char **argv)
{
    string fileName = convertToString(argv[1]);
    // Issues: imput "sometext.txt" and the length is only 8, which is "sometext".
    //  cout << sizeof(argv[1]) << " " << sizeof(char) << endl;
    //  cout << "Size of argv[1]" << sizeof(argv) / sizeof(char)+1 << endl;
    // cout << "openning: " << fileName << endl;
    /* --- Tokenizor tester --- */
    Tokenizor tk = Tokenizor(fileName);
    // Token test1 = tk.getToken();
    char *test1;
    int test2;
    // cout << tk.hasNext() << " xxx " << endl;
    // int test2 = tk.readInt();
    // int test3 = tk.readInt();
    // string x = tk.readSymbol();
    // int test4 = tk.readInt();
    // string AIER = tk.readIAER();
    // cout << test2 << test3 << x << test4 << AIER << endl;
    // while ((test1 = tk.getToken()) != NULL)
    // while (tk.hasNext())
    // {
    //     test1 = tk.getToken();
    //     if (test1 != NULL) {
    //         cout << "Token: " << test1 << endl;
    //     }

    // }
    // PassOneV2 test
    passOneV2(fileName);
    passTwoV2(fileName);
    /* ------- --------*/
    // int lineNum = 0;
    // int index = 0;
    // Token *token = getToken(fileName, lineNum, index);
    // cout << (*token).getToken() << endl;

    // while ((token = getToken(fileName, lineNum, index)) != NULL)
    // {
    //     cout << token << endl;
    //     cout << "lineNum: " << lineNum << " index: " << index << endl;
    // };
    // cout << "Token: "<< token<<"lineNum: " << lineNum << " index: " << index << endl;
    // token = getToken(fileName, lineNum, index);
    // // cout << token << endl;
    // // cout << "Token: "<< token<<"lineNum: " << lineNum << " index: " << index << endl;
    // token = getToken(fileName, lineNum, index);
    // while ((token = getToken(fileName, lineNum, index)) != NULL)
    // {
    //     cout << "Token: " << (*token).getToken() << " Line: " << (*token).getLineNum() << " index: " << (*token).getLineOffset() << endl;
    // }
    // cout << token << endl;"
    // passOne(fileName);
    // cout << endl;
    // cout << "====== after pass one =======" << endl;
    // for (Module &m : moduleList)
    // {
    //     m.toString();
    // }
    // passTwo(fileName);
}

void lineToTokens(vector<Token> &tokenList, string line, int lineNum)
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

Token *getToken(string fileName, int &lineNum, int &startIndex)
{
    // cout << "Getting token after line: " << lineNum << " index: " << startIndex << endl;
    ifstream inFile;
    inFile.open(fileName);
    string line;
    int lineCnt = 0;
    int charCnt = 0;
    while (!inFile.eof() && lineCnt <= lineNum)
    {
        getline(inFile, line);
        lineCnt++;
    }
    // count to the token
    char *lineArr = new char[line.length() + 1];
    // cout << "Line:" << line << endl;
    strcpy(lineArr, line.c_str());

    char *token;
    string tokenStr;
    int offset;
    map<char *, int> symbolToOffset;
    while ((token = strsep(&lineArr, delim)) != NULL)
    {
        // Issue: strange string
        if (strcmp(token, "") != 0)
        {
            tokenStr = convertToString(token);
            // Token toAdd = Token(lineNum, lineOffset, token);
            offset = line.find(tokenStr);

            // cout << "Token: " << toAdd.getLineNum() << ":" << toAdd.getLineOffset() << " " << toAdd.getToken() << endl;
            symbolToOffset[token] = offset;
        }
    }
    for (auto kv : symbolToOffset)
    {
        if (kv.second >= startIndex)
        {
            Token *res = new Token(lineNum, startIndex + 1, convertToString(kv.first));
            startIndex = kv.second + strlen(kv.first);
            return res;
        }
        else
        {
            startIndex = 0;
            lineNum++;
            return getToken(fileName, lineNum, startIndex);
        }
    }

    /* ---  reserved for another method ---
    while (charCnt <= startIndex)
    {
        if ((token = strsep(&lineArr, delim)) != NULL)
        {
            if (strcmp(token, "") != 0)
            {
                startIndex = line.find(convertToString(token));
                charCnt = line.find(convertToString(token)) + strlen(token);
                cout << "Token: " << token;
                // // cout << "lineNum: " << lineNum << " startIndex: " << startIndex << endl;
                cout << " Line: " << lineCnt << " Index: " << charCnt << endl;
            }
            else
            {
                charCnt++;
            }
        }
        else
        {
            break;
        }
    }
    cout << "Get token: " << token << endl;
    // adjust and return

    if (inFile.eof() && charCnt >= line.length())
    {
        cout << "End of file" << endl;
        return NULL;
    }
    else if (charCnt >= line.length())
    {
        // cout << "End of line" << endl;
        startIndex = 0;
        lineNum++;
        token = getToken(fileName, lineNum, startIndex);
        // return token;
    }
    else if (token == NULL) {
        cout << "null error" << endl;
    }
    else
    {
        startIndex = charCnt + strlen(token);
        // return token;
    }
    // cout << endl;
    return token;*/
}

vector<Token> tokenizer(string fileName)
{
    string line;
    ifstream inFile;
    inFile.open(fileName);
    vector<string> stringVector;
    vector<Token> tokenList;
    while (!inFile.eof())
    {
        getline(inFile, line);
        stringVector.push_back(line);
    }
    int lineNum = 1;
    int tokenLeft = 0;
    for (string &s : stringVector)
    {
        lineToTokens(tokenList, s, lineNum);
        lineNum++;
    }
    inFile.close();
    return tokenList;
}

void passOneV2(string fileName)
{
    int globalOffset = 0;
    int tokenPairNum = 0;
    int type = 0;
    int moduleNum = 1;
    Tokenizor tk = Tokenizor(fileName);
    vector<Module> moduleList;
    Module newModule;
    map<string, int> defineTimeTable;
    map<string, int> defineModTable;
    while ((tokenPairNum == 0) && (tk.hasNext()))
    {
        int moduleSize = 0;
        char *tokenPairStr = tk.getToken();
        if (tokenPairStr == NULL)
        {
            break;
        }
        for (int i = 0; i < strlen(tokenPairStr); i++)
        {
            if (!isdigit(tokenPairStr[i]))
            {
                // cout << "not a digit" << i << endl;
                __parseerror(0, tk.getCurrToken().getLineNum(), tk.getCurrToken().getLineOffset());
                exit(1);
            }
        }
        tokenPairNum = stoi(tokenPairStr);
        // tokenPairNum = tk.readInt();
        // cout << "pair num: " << tokenPairNum << endl;
        if (tokenPairNum > 16)
        {
            if (type == 0)
            {
                __parseerror(4, tk.getCurrToken().getLineNum(), tk.getCurrToken().getLineOffset());
                exit(1);
            }
            else if (type == 1)
            {
                __parseerror(5, tk.getCurrToken().getLineNum(), tk.getCurrToken().getLineOffset());
                exit(1);
            }
            // cout << tk.getCurrToken().getToken();
            // tk.getTokenInfo();
        }

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
                string symbol = tk.readSymbol();
                // cout << "|" << symbol << "|" << endl;
                int val = tk.readInt();

                if (defineTimeTable[symbol] == 0)
                {
                    symbolTable[symbol] = val + globalOffset;
                    defineModTable[symbol] = moduleNum;
                }
                else
                {
                    cout << "Warning: Module " << moduleNum << ": "
                         << symbol << " redefined and ignored" << endl;
                }
                defineTimeTable[symbol]++;
                tokenPairNum--;
            }
            type++;
            // cout << "done def list" << endl;
            break;
        case 1:
            while (tokenPairNum > 0)
            {
                string usedSymbol = tk.readSymbol();
                // cout << "Use: " << usedSymbol << endl;
                tokenPairNum--;
                newModule.setUseList(usedSymbol);
            }
            type++;
            // newModule.toString();
            // cout << "done use list" << endl;
            break;
        case 2:
            // cout << "line 701 " << endl;
            moduleSize = tokenPairNum;
            if (globalOffset+moduleSize >= 511) {
                __parseerror(6, tk.getCurrToken().getLineNum(), tk.getCurrToken().getLineOffset());
                exit(1);
            }
            while (tokenPairNum > 0)
            {
                // cout << "line 706 " << endl;
                string op = tk.readIAER();
                // cout << "line 708" << endl;
                int addr = tk.readInt();
                // cout << "Operator: " << op << " Addr: " << addr << endl;
                newModule.setProgText(op, addr);
                tokenPairNum--;
            }
            // cout << "line 713 " << endl;
            for (auto kv : symbolTable)
            {
                if (defineModTable[kv.first] == moduleNum && kv.second - globalOffset >= moduleSize)
                {
                    cout << "Warning: Module " << moduleNum << ": " << kv.first << " too big " << kv.second - globalOffset
                         << " (max=" << moduleSize-1 << ") assume zero relative" << endl;
                    symbolTable[kv.first] = globalOffset + 0;
                }
            }
            globalOffset += moduleSize;
            
            type = 0;
            // cout << "done prog text" << endl;
            // newModule.toString();
        default:
            moduleList.push_back(newModule);
            moduleNum++;
            break;
        }
    }
}

void passTwoV2(string fileName)
{
    map<string, int> defList;
    map<string, int> dupDefineList; /*used to record duplicated defined symbols and its module*/
    vector<string> usedSymbolList;
    map<string, int> useTimes;
    int globalOffset = 0;
    int tokenPairNum = 0;
    int type = 0;
    int moduleNum = 1;
    Tokenizor tk = Tokenizor(fileName);
    vector<Module> moduleList;
    Module newModule;
    while ((tokenPairNum == 0) && (tk.hasNext()))
    {

        char *tokenPairStr = tk.getToken();
        if (tokenPairStr == NULL)
        {
            // cout << "tokenPairNum: NULL" << endl;
            break;
        }
        for (int i = 0; i < strlen(tokenPairStr); i++)
        {
            if (!isdigit(tokenPairStr[i]))
            {
                // cout << "not a digit" << i << endl;
                __parseerror(0, tk.getCurrToken().getLineNum(), tk.getCurrToken().getLineOffset());
                exit(1);
            }
        }
        // tokenPairNum = tk.readInt();
        tokenPairNum = stoi(tokenPairStr);
        // cout << "tokenPairNum: " << tokenPairNum << endl;

        switch (type)
        {
        case 0:
            newModule = Module();
            newModule.setNumber(moduleNum);
            newModule.setOffset(globalOffset);
            // cout << "Parsing def list" << endl;
            while (tokenPairNum > 0)
            {
                // cout << "pair num: " << tokenPairNum << endl;
                string symbol = tk.readSymbol();
                int val = tk.readInt();
                // if (symbolTable.count(symbol))
                // {
                if (dupDefineList.count(symbol) == 0)
                {
                    defList[symbol] = moduleNum;
                }
                dupDefineList[symbol]++;
                // printf("Warning: Module %d: %s redefined and ignored", moduleNum, symbol);
                // }
                // symbolTable[symbol] = val + globalOffset;
                tokenPairNum--;
            }
            type++;
            // cout << "done def list 2" << endl;
            break;
        case 1:
            while (tokenPairNum > 0)
            {
                string usedSymbol = tk.readSymbol();
                // cout << "Use: " << usedSymbol << endl;
                tokenPairNum--;
                newModule.setUseList(usedSymbol);
            }
            type++;
            // newModule.toString();
            // cout << "done use list 2" << endl;
            break;
        case 2:

            globalOffset += tokenPairNum;
            while (tokenPairNum > 0)
            {
                string op = tk.readIAER();
                int addr = tk.readAddr();
                // cout << "Operator: " << op << " Addr: " << addr << endl;
                newModule.setProgText(op, addr);
                tokenPairNum--;
            }
            type = 0;
            // newModule.toString();
            // cout << "done prog text 2" << endl;
        default:
            moduleList.push_back(newModule);
            moduleNum++;
            break;
        }
    }

    cout << "Symbol Table" << endl;

    for (const auto &kv : symbolTable)
    {
        cout << kv.first << "=" << kv.second;
        if (dupDefineList[kv.first] > 1)
        {
            cout << " Error: This variable is multiple times defined; first value used";
        }
        cout << endl;
    }
    cout << endl;
    cout << "Memory Map" << endl;

    int count = 0;
    int currOperationAddr;

    for (Module &m : moduleList)
    {
        int errorType = 0;
        string errorSymbol;
        // m.toString();
        // cout << "+" << m.getOffset() << endl;
        for (OpPair &opp : m.getProgText())
        {
            // cout << "OP Pair: " << opp.op << opp.addr << endl;
            // printf("%3d: %4d\n", count, opp.addr);
            if (opp.op.compare("R") == 0)
            {
                // cout << "R!" << endl;

                int opcode = opp.addr / 1000;
                int operand = opp.addr % 1000;
                int maxRelative = m.getSize() + m.getOffset();
                if (opcode >= 10)
                {
                    currOperationAddr = 9999;
                    errorType = 5;
                }
                // cout << "mod size: " << m.getSize();
                else if (operand < m.getSize() && operand >= 0)
                {
                    currOperationAddr = m.getOffset() + opp.addr;
                }
                else
                {
                    currOperationAddr = opcode * 1000 + m.getOffset();
                    errorType = 3;
                }
            }
            else if (opp.op.compare("A") == 0)
            {
                currOperationAddr = opp.addr;
                int opcode = opp.addr / 1000;
                int operand = opp.addr % 1000;
                if (operand > 512)
                {
                    currOperationAddr = opcode * 1000;
                    errorType = 2;
                }

                // cout << "A!" << endl;
            }
            else if (opp.op.compare("E") == 0)
            {
                
                int opcode = opp.addr / 1000;
                int operand = opp.addr % 1000;
                // cout << "operand: " << operand << endl;
                
                vector<string> useList = m.getUseList();
                // cout << "Use list size: " << useList.size() << endl;
                
                string tokenUsed;
                if (operand < useList.size()) {
                    tokenUsed = useList[operand];
                }
                // cout << "Token used here: " << tokenUsed << endl;
                int var;
                
                // cout << "E token: " << symbolTable.count(tokenUsed) << endl;
                if (operand >= useList.size())
                {
                    errorType = 4;
                    currOperationAddr = opp.addr;
                }
                else if (symbolTable.count(tokenUsed))
                {
                    
                    var = symbolTable[tokenUsed];
                    currOperationAddr = opcode * 1000 + var;
                    useTimes[tokenUsed] += 1;
                    usedSymbolList.push_back(tokenUsed);
                    
                }
                else
                {
                    
                    errorType = 1;
                    errorSymbol = tokenUsed;
                    useTimes[tokenUsed] += 1;
                    currOperationAddr = opcode * 1000;
                }
                // int var = 0;

                // cout << "E!" << endl;
            }
            else if (opp.op.compare("I") == 0)
            {
                if (opp.addr > 9999)
                {
                    currOperationAddr = 9999;
                    errorType = 6;
                }
                else
                {
                    currOperationAddr = opp.addr;
                }

                // cout << "I!" << endl;
            }
            else
            {
                cout << "ERROR: WRONG OPERATOR" << endl;
            }

            cout << setw(3) << setfill('0') << count << ": ";
            cout << setw(4) << setfill('0') << currOperationAddr;
            // cout << setw(10) << "??????????" << endl;
            switch (errorType)
            {
            case 1:
                // printf("errorrrrrrrrr");
                cout << " Error: " << errorSymbol << " is not defined; zero used";
                errorType = 0;
                break;
            case 2:
                cout << " Error: Absolute address exceeds machine size; zero used";
                errorType = 0;
                break;
            case 3:
                cout << " Error: Relative address exceeds module size; zero used";
                errorType = 0;
                break;
            case 4:
                cout << " Error: External address exceeds length of uselist; treated as immediate";
                errorType = 0;
                break;
            case 5:
                cout << " Error: Illegal opcode; treated as 9999";
                errorType = 0;
                break;
            case 6:
                cout << " Error: Illegal immediate value; treated as 9999";
                errorType = 0;
                break;
            default:
                errorType = 0;
                break;
            }

            cout << endl;
            // cout << ">?>>?????" << endl;
            // errorType = 0;
            count++;
        }

        for (auto kv : m.getUseList())
        {
            // if (!symbolTable.count(kv)) {
            //     continue;
            // }
            if (useTimes[kv] == 0)
            {
                cout << "Warning: Module " << m.getNumber() << ": "
                     << kv << " appeared in the uselist but was not actually used" << endl;
            }
        }
    }

    // Warning non-used symbols
    cout << endl;
    for (auto kv : symbolTable)
    {
        if (std::find(usedSymbolList.begin(), usedSymbolList.end(), kv.first) == usedSymbolList.end())
        {
            cout << "Warning: Module " << defList[kv.first] << ": " << kv.first << " was defined but never used" << endl;
        }
    }
}

string convertToString(char *a)
{
    int size = strlen(a);
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

void __parseerror(int errcode, int linenum, int lineoffset)
{
    static char *errstr[] = {
        "NUM_EXPECTED",           // Number expect, anything >= 2^30 is not a number either
        "SYM_EXPECTED",           // Symbol Expected
        "ADDR_EXPECTED",          // Addressing Expected which is A/E/I/R
        "SYM_TOO_LONG",           // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE", // > 16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "TOO_MANY_INSTR"};
    printf("Parse Error line %d offset %d: %s\n", linenum + 1, lineoffset + 1, errstr[errcode]);
}
