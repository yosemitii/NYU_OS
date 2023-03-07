#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
// #include "scheduler.cpp"
extern void __error(int errcode);

using namespace std;
/*Variable Definition*/
typedef enum SchedulerType
{
    FCFS,
    LCFS,
    SRTF,
    RR,
    PRIO,
    PREPRIO,
}SchedulerType;
/*Class Definition*/
class RandGenerator
{
private:
    int *randvals;
    int ofs;
    int size;

public:
    RandGenerator(std::string filename)
    {
        ifstream inFile;
        inFile.open(filename);
        std::string line;
        size = 0;
        vector<int> numVec;
        getline(inFile, line);
        try
        {
            size = stoi(line);
        }
        catch (exception &err)
        {
            // TODO: There could be empty line problem.
            cout << "File:" << filename << " Line: " << line << endl;
            __error(0);
        }
        while (!inFile.eof())
        {
            getline(inFile, line);
            try
            {
                numVec.push_back(stoi(line));
            }
            catch (exception &err)
            {
                // TODO: There could be empty line problem.
                break;
            }
        }
        randvals = &numVec[0];
    }

    int myrandom(int burst)
    {
        // cout << randvals[ofs] << endl;
        int res = 1 + (randvals[ofs] % burst);
        this->ofs = (this->ofs + 1) % size;
        return res;
    }
};

class Process
{
private:
    int id;
    int at;
    int tc;
    int cb;
    int io;
    int prio;

public:
    Process(int id, int at, int tc, int cb, int io, int prio)
    {
        this->id = id;
        this->at = at;
        this->tc = tc;
        this->cb = cb;
        this->io = io;
        this->prio = prio;
    }
};

class Scheduler
{
private:
    SchedulerType type;
    vector<Process *> *procs;

public:
    // virtual void addProcess() = 0;
    // virtual Process *getNextProc() = 0;
    // virtual void runQueue() = 0;
    Scheduler(RandGenerator &rgen, string inputFile)
    {
        ifstream inFile;
        inFile.open(inputFile);
        this->procs = new vector<Process *>;
        const char *delim = "\t\n ";
        std::string line;
        char *lineArr;
        int id = 0;
        while (!inFile.eof())
        {

            getline(inFile, line);
            // cout << "Line:" << line << endl;
            lineArr = new char[line.length() + 1];
            strcpy(lineArr, line.c_str());
            int info[4];
            char *token = strtok(lineArr, delim);
            if (token != NULL)
            {
                try
                {
                    info[0] = stoi(token);
                }
                catch (exception &err)
                {
                    __error(1);
                }
            }
            else
            {
                break;
            }
            int index = 1;
            while (token != NULL)
            {
                // cout << token << endl;
                token = strtok(NULL, delim);
                if (token != NULL)
                    try
                    {
                        info[index] = stoi(token);
                        // cout << info[index] << endl;
                    }
                    catch (exception &err)
                    {
                        __error(1);
                    }
            }
            int prio = rgen.myrandom(100);
            Process *process = new Process(id, info[0], info[1], info[2], info[3], prio);
            this->procs->push_back(process);
            id++;
        }
    }
};

class FIFO : public Scheduler
{
    public:
        FIFO(RandGenerator &rgen, string inputFile): Scheduler(rgen, inputFile){};

};
