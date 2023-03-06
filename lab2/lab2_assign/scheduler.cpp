#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

class RandGenerator
{
private:
    int* randvals;
    int ofs;
public:
    RandGenerator(string filename) {
        ifstream inFile;
        inFile.open(filename);
        string line;
        int count = 0;
        vector<int> numVec;
        while (!inFile.eof())
        {
            getline(inFile, line);
            numVec.push_back(stoi(line));
            count++;
        }
        randvals = &numVec[0];
    }

    int myrandom(int burst) { 
        return 1 + (randvals[ofs] % burst);
    }
};

int main(int argc, char **argv)
{
    RandGenerator rgen = RandGenerator("rfile");
    int x = rgen.myrandom(100);
    cout << x << endl;
}

// int myrandom(int burst) { return 1 + (randvals[ofs] % burst); }