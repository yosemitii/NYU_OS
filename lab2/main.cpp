#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "Scheduler.cpp"
#include "Simulator.cpp"
using namespace std;

/*Function declaration*/
extern void __error(int errcode);

/*Variable Declaration*/
std::string convertToString(char *a);
// vector<Process *> *buildProcess(RandGenerator &rgen, string inputFile);

int main(int argc, char **argv)
{
    SchedulerType sType = FCFS;
    int vflag = 0;
    int tflag = 0;
    int eflag = 0;
    int pflag = 0;
    int iflag = 0;
    int sflag = 0;
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
        case 'F':
            sType = FCFS;
        case 'L':
            sType = LCFS;
        case 'S':
            sType = SRTF;
        case 'R':
            sType = RR;
            sscanf(argv[optind], "-sR%d", &quantum);
            break;
        case 'P':
            sType = PRIO;
            if (sscanf(argv[optind], "-sP%d:%d", &quantum, &maxprio) == -1)
            {
                if (sscanf(argv[optind], "-sP%d", &quantum) == -1)
                {
                    std::cout << "Error: Need at least one argument.";
                }
            };
            break;
        case 'E':
            sType = PREPRIO;
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
    if (inputFile == "" || randFile == "")
    {
        __error(1);
    }
    // std::cout << convertToString(argv[index]) << "." << endl;
    // printf("Non-option argument: %s.\n", convertTostd::string(argv[index]));
    std::cout << "input file name: " << inputFile << "." << endl;
    std::cout << "rand file name: " << randFile << "." << endl;
    RandGenerator rgen = RandGenerator(randFile);

    // vector<Process *> *processes = buildProcess(rgen, inputFile);

    int x;

    for (int i = 0; i < 20; i++)
    {
        x = rgen.myrandom(10);
        std::cout << x << endl;
    }
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

void __error(int errcode)
{
    static string errstr[] = {
        "VARIABLE_EXPECTED",      // Number expect, anything >= 2^30 is not a number either
        "FILE_EXPECTED",          // Symbol Expected
        "_EXPECTED",              // Addressing Expected which is A/E/I/R
        "SYM_TOO_LONG",           // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE", // > 16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "UNKNOWN"};
    // printf("Error: %s\n", errstr[errcode]);
    cout << "Error: " << errstr[errcode] << endl;
    exit(1);
    // printf("Parse Error line %d offset %d: %s\n", linenum + 1, lineoffset + 1, errstr[errcode]);
}