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
#include "utils.h"

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
#ifndef RAND_GENERATOR
#define RAND_GENERATOR
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
            std::cout << "ERROR: File:" << filename << " Line: " << line << std::endl;
            // __error(0);
            exit(1);
        }
        randvals = new int[size];
        int index = 0;
        while (!inFile.eof())
        {
            getline(inFile, line);
            try
            {
                // numVec.push_back(stoi(line));
                randvals[index] = stoi(line);
                index++;
            }
            catch (exception &err)
            {
                // TODO: There could be empty line problem.
                break;
            }
        }
        // randvals = &numVec[0];
        inFile.close();
    }

    int myrandom(int burst)
    {
        // cout << randvals[ofs] << endl;
        // cout << "randvals: " << randvals[ofs] << endl;
        int res = 1 + (randvals[ofs] % burst);
        this->ofs = (this->ofs + 1) % size;
        return res;
    }
};
#endif
#ifndef PROCESS
#define PROCESS
class Process
{
private:
public:
    int id;
    int arrivTime;
    int totalTime;
    int cpuBurst;
    int ioBurst;
    int prio;
    int timestamp;
    ProcState pState;
    Process(int id, int at, int tc, int cb, int io, int prio)
    {
        this->id = id;
        this->arrivTime = at;
        this->totalTime = tc;
        this->cpuBurst = cb;
        this->ioBurst = io;
        this->prio = prio;
        this->timestamp = this->arrivTime;
        this->pState = CREATED;
    }
    void show()
    {
        printf("ID: %d, ArrTime: %d, Total: %d, CPU: %d, IO: %d, Prio: %d, TS:%d, State:%d\n",
               id, arrivTime, totalTime, cpuBurst, ioBurst, prio, timestamp, pState);
    }
};
#endif
#ifndef SCHEDULER
#define SCHEDULER
class Scheduler
{
protected:
    std::queue<Process *> *readyQueue;
    std::queue<Process *> *runQueue;
    std::queue<Process *> *expiredQueue;
    int timestamp;
    int CPULimit;

public:
    Scheduler()
    {
        CPULimit = 1;
        readyQueue = new std::queue<Process *>();
        runQueue = new std::queue<Process *>();
        expiredQueue = new std::queue<Process *>();
    }
    virtual void addProcess(Process *p) = 0;
    virtual Process *getNextProcess() = 0;
    virtual Process *preempt() = 0;
    virtual Process *run() = 0;
    virtual Process *block() = 0;
    virtual Process *getCurrRunngProc() = 0;
    virtual void setTimestamp(int t)
    {
        this->timestamp = t;
    }
    virtual void runQueueLog()
    {
        printf("SCHED(%d)", runQueue->size());
        Process *head = runQueue->front();
        if (runQueue->size() == 1) {
            std::cout << head->id << ":" << head->id << std::endl;
        }
    }
    // virtual void runQueue() = 0;
};

class FIFO : public Scheduler
{
public:
    FIFO() : Scheduler(){};

    void addProcess(Process *p)
    {
        readyQueue->push(p);
    };
    Process *getNextProcess()
    {
        if (readyQueue->empty())
            return nullptr;
        Process *p = readyQueue->front();
        readyQueue->pop();
        return p;
    };
    Process *preempt()
    {
        Process *p = runQueue->front();
        runQueue->pop();
        expiredQueue->push(p);
        return p;
    }
    Process *run()
    {
        Process *p;
        if (!readyQueue->empty())
        {
            p = readyQueue->front();
            if (runQueue->size() < CPULimit) {
                readyQueue->pop();
                runQueue->push(p);
                p->pState = RUNNG;
            }
            return p;
        }
        else
        {
            std::cout << "WARNING: Ready Queue is empty." << endl;
            return nullptr;
        }
    }

    virtual Process *block()
    {
        Process *p;
        if (!runQueue->empty())
        {
            p = runQueue->front();
            runQueue->pop();

            // if (!readyQueue->empty()) {
            //     run();
            // }
            // int timeUsed = timestamp - p->timestamp;
            // cout << "Time used: " << timeUsed << endl;
            // p->totalTime -= timeUsed;
            // cout << "Time left: " << p->totalTime << endl;
            if (p->totalTime > 0)
            {
                expiredQueue->push(p);
            }
            return p;
        }
        else
        {
            cout << "WARNING: Run Queue is empty." << endl;
            return nullptr;
        }
    }

    Process *getCurrRunngProc() {
        if (runQueue->empty()){
            return nullptr;
        }
        else {
            return runQueue->front();
        }
    }

    // FIFO(RandGenerator &rgen, string inputFile, int maxprio) : Scheduler(rgen, inputFile, 0, maxprio){};
};

#endif

#ifndef EVENT
#define EVENT
class Event
{

public:
    int timestamp;
    // int endTime;
    int procID;
    EventType transition;
    Process *proc;
    Event()
    {
    }
    Event(int ts, int id)
    {
        this->timestamp = ts;
        // this->endTime = et;
        this->procID = id;
    };

    Event(int ts, int id, EventType etype, Process *p)
    {
        this->timestamp = ts;
        // this->endTime = et;
        this->procID = id;
        this->transition = etype;
        this->proc = p;
    };

    void toString()
    {
        // printf("[%d, %d]", startTime, procID);
        std::cout << "[" << timestamp << "," << procID << "," << EventToString(transition) << "]";
    }

    inline bool operator>=(Event *b)
    {
        return this->timestamp >= b->timestamp;
    }

    inline bool operator>(Event *b)
    {
        return this->timestamp > b->timestamp;
    }

    inline bool operator<=(Event *b)
    {
        return this->timestamp <= b->timestamp;
    }

    inline bool operator<(Event *b)
    {
        return this->timestamp < b->timestamp;
    }

    inline bool operator=(Event *b)
    {
        return this->timestamp = b->timestamp;
    }
};
#endif