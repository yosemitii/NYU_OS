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
#include "unittest.cpp"
using namespace std;

/*Function declaration*/
extern void __error(int errcode);

/*Variable Declaration*/
std::string convertToString(char *a);
// vector<Process *> *buildProcess(RandGenerator &rgen, string inputFile);

int main(int argc, char **argv)
{
    // Error?: show segmentation error. 
    // olistTest();
    // Simulator simul = Simulator(argc, argv);
    Simulator *simul = new Simulator();
    simul->init(argc, argv);
    // Process* p = (* simul->procs)[0];
    // simul->showProcs();
    // ?:
    olistTest();
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