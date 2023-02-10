#include <vector>
using namespace std;

class OpPair
{
public:
    string op;
    int addr;
    OpPair(string o, int ad) {
        op = o;
        addr = ad;
    }
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

    void toString() {
        cout << "--- Module info: ---" << endl;
        cout << "Offset: " << offset << endl;
        cout << "Use List: ";
        for (string& s: useList) {
            cout << s << " ";
        }
        cout << endl;
        cout << "Prog Text: " << endl;
        for (OpPair& opp: progText) {
            cout << opp.op << " " << opp.addr << endl;
        }
    }
};