#include <stdio.h>
#include <string>

using namespace std;

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
