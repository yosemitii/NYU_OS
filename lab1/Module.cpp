#include <vector>
using namespace std;

class OpPair
{
public:
    string op;
    int addr;
};

class Module
{
private:
    int offset;
    vector<string> useList;
    vector<OpPair> progText;

public:
    Module()
    {
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

    vector<OpPair> getProgText() {
        return progText;
    }

    void setProgText(string op, int addr) {
        OpPair toAdd = OpPair(op, addr);
        progText.push_back(toAdd);
    }
};