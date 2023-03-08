#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

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
} SchedulerType;

typedef enum EventType
{
    CPU_BURST,
    IO_BURST
} EventType;

typedef enum ProcState
{
    RUNNG,
    READY,
    BLOCK,
    CREATED
} ProcState;
/*Class Definition*/
class RandGenerator
{
private:
    int *randvals;
    int ofs;
    int size;

public:
    RandGenerator() {}
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
        inFile.close();
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
    void show()
    {
        printf("ID: %d, ArrTime: %d, Total: %d, CPU: %d, IO: %d, Prio: %d\n", id, at, tc, cb, io, prio);
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
};

class FIFO : public Scheduler
{
public:
    // FIFO(RandGenerator &rgen, string inputFile, int maxprio) : Scheduler(rgen, inputFile, 0, maxprio){};
};

class Event
{

public:
    int startTime;
    int endTime;
    int procID;
    EventType eType;
    Process *proc;
    Event(){
    }
    Event(int st, int et, int id){
        this->startTime = st;
        this->endTime = et;
        this->procID = id;
    };

    void toString() {
        printf("[%d, %d, %d]", startTime, endTime, procID);
    }

    bool operator >= (Event *b){
        return this->startTime >= b->startTime;
    }

    bool operator > (Event *b){
        return this->startTime > b->startTime;
    }

    bool operator <= (Event *b){
        return this->startTime <= b->startTime;
    }

    bool operator < (Event *b){
        return this->startTime < b->startTime;
    }

    bool operator = (Event *b){
        return this->startTime = b->startTime;
    }
};

class EventQueue
{
private:
    queue<Event> eventq;

public:
    EventQueue(){};

   
};