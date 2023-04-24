#ifndef RAND_GENERATOR
#define RAND_GENERATOR

#include <string>
#include <fstream>
#include <vector>
#include <iostream>

/*Class Definition*/
class RandGenerator
{
private:
    int *randvals;
    int ofs;
    int size;
    std::string rfile_name;

public:
    RandGenerator(std::string rname) {
        this->rfile_name = rname;
        init();
    }
    void init()
    {
        std::ifstream rFile;
        rFile.open(rfile_name);
        std::string line;
        size = 0;
        std::vector<int> numVec;
        getline(rFile, line);
        try
        {
            size = stoi(line);
        }
        catch (std::exception &err)
        {
            std::cout << "ERROR: Open file error:" << rfile_name << " Line: " << line << std::endl;
            exit(1);
        }
        randvals = new int[size];
        int index = 0;
        while (!rFile.eof())
        {
            getline(rFile, line);
            try
            {
                randvals[index] = stoi(line);
                index++;
            }
            catch (std::exception &err)
            {
                break;
            }
        }
        rFile.close();
    }

    int myrandom(int burst)
    {
        int res = (randvals[ofs] % burst);
        this->ofs = (this->ofs + 1) % size;
        return res;
    }
};
#endif
