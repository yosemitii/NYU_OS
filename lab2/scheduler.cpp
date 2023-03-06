#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

std::string convertToString(char *a);

class RandGenerator
{
private:
    int *randvals;
    int ofs;

public:
    RandGenerator(std::string filename)
    {
        ifstream inFile;
        inFile.open(filename);
        std::string line;
        int count = 0;
        vector<int> numVec;

        while (!inFile.eof())
        {
            getline(inFile, line);
            // std::cout << line << endl;
            try
            {
                numVec.push_back(stoi(line));
            }
            catch (exception &err)
            {
                // TODO: There coule be empty line problem.
                break;
            }
            count++;
        }
        randvals = &numVec[0];
    }

    int myrandom(int burst)
    {
        return 1 + (randvals[ofs] % burst);
    }
};

int main(int argc, char **argv)
{
    int vflag = 0;
    int tflag = 0;
    int eflag = 0;
    int pflag = 0;
    int iflag = 0;
    int sflag = 0;
    int Rflag = 0;
    int Pflag = 0;
    int Eflag = 0;
    char *cvalue = NULL;
    int index;
    int c;
    int quantum;
    int maxprio = 4;
    int optionIndex = 0;
    int fileCnt = 0;
    std::string inputFile;
    std::string randFile;

    while ((c = getopt(argc, argv, ":svtepisREP1234567890")) != -1)
    {
        switch (c)
        {
        case 'v':
            vflag = 1;
            break;
        case 't':
            tflag = 1;
            break;
        case 'e':
            eflag = 1;
            break;
        case 'p':
            pflag = 1;
            break;
        case 'i':
            iflag = 1;
            break;
        case 's':
            sflag = 1;
            break;
        case 'R':
            Rflag = 1;
            sscanf(argv[optind], "-sR%d", &quantum);
            break;
        case 'P':
            Pflag = 1;
            if (sscanf(argv[optind], "-sP%d:%d", &quantum, &maxprio) == -1)
            {
                if (sscanf(argv[optind], "-sP%d", &quantum) == -1)
                {
                    std::cout << "Error: Need at least one argument.";
                }
            };
            break;
        case 'E':
            Eflag = 1;
            if (sscanf(argv[optind], "-sE%d:%d", &quantum, &maxprio) == -1)
            {
                if (sscanf(argv[optind], "-sE%d", &quantum) == -1)
                {
                    std::cout << "Error: Need at least one argument.";
                }
            };
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case ':':
            break;
        case '?':
            fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            break;
        default:
            abort();
        }
    }
    std::cout << "quantum: " << quantum << " maxprio:" << maxprio << endl;
    for (index = optind; index < argc; index++)
    {
        if (fileCnt == 0)
        {
            inputFile = convertToString(argv[index]);
            fileCnt++;
        }
        else if (fileCnt == 1)
        {
            randFile = convertToString(argv[index]);
            fileCnt++;
        }
    }
    // std::cout << convertToString(argv[index]) << "." << endl;
    // printf("Non-option argument: %s.\n", convertTostd::string(argv[index]));
    std::cout << "input file name: " << inputFile << "." << endl;
    std::cout << "rand file name: " << randFile << "." << endl;
    RandGenerator rgen = RandGenerator("rfile");
    int x = rgen.myrandom(100);
    // std::cout << x << endl;
}

std::string convertToString(char *a)
{
    int size = strlen(a);
    int i;
    std::string s = "";
    for (i = 0; i < size; i++)
    {
        s = s + a[i];
    }
    return s;
}