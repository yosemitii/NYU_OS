#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <list>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include <sstream>
#include <iostream>

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
        ifstream rFile;
        rFile.open(filename);
        std::string line;
        size = 0;
        vector<int> numVec;
        getline(rFile, line);
        try
        {
            size = stoi(line);
        }
        catch (exception &err)
        {
            std::cout << "ERROR: Open file error:" << filename << " Line: " << line << std::endl;
            exit(1);
        }
        randvals = new int[size];
        int index = 0;
        while (!rFile.eof())
        {
            getline(rFile, line);
            try
            {
                // numVec.push_back(stoi(line));
                randvals[index] = stoi(line);
//                cout << line << endl;
                index++;
            }
            catch (exception &err)
            {
                break;
            }
        }
        // randvals = &numVec[0];
        rFile.close();
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
    int cpuBurstRemain;
    int ioBurst;
    int prio;
    int dynamicPrio;
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
        this->dynamicPrio = this->prio - 1;
    }
    void show()
    {
        printf("ID: %d, ArrTime: %d, Total: %d, CPU: %d, IO: %d, Prio: %d, TS:%d, State:%d\n",
               id, arrivTime, totalTime, cpuBurst, ioBurst, prio, timestamp, pState);
    }
    std::string toString() {
        ostringstream os;
        os << "ID: " << id << "TimeStamp: " << timestamp << "Static Prio: " << prio << "Dynamic Prio: " << dynamicPrio;
        return os.str();
    }
};
#endif
#ifndef SCHEDULER
#define SCHEDULER
class Scheduler
{

public:
    int timestamp;
    int CPULimit;
    explicit Scheduler()
    {
        CPULimit = 1;
    }
    virtual void addProcess(Process *p) = 0;
    virtual Process *getNextProcess() = 0;
    virtual bool isPreemptive() = 0;
    virtual void runQueueLog(){};
    // virtual void runQueue() = 0;
};

class FIFO : public Scheduler
{
private:
    std::deque<Process *> *runQueue;

public:
    FIFO() : Scheduler(){
        runQueue = new std::deque<Process*>();
    };
    
    virtual void addProcess(Process *p)
    {
        p->pState = READY;
        p->dynamicPrio = p->prio - 1;
        runQueue->push_back(p);
    };

    virtual Process *getNextProcess()
    {
        runQueueLog();
        Process *p = nullptr;
        if (!runQueue->empty())
        {
            p = runQueue->front();
            runQueue->pop_front();

        }

        return p;
    }

    bool isPreemptive()
    {
        return false;
    }

    void runQueueLog(){
        printf("FIFO SCHED (%d):", runQueue->size());
        if (runQueue->size() == 0) return;
        Process *head = runQueue->front();
        std::cout << head->id << ":" << head->timestamp << " ";
        runQueue->pop_front();
        runQueue->push_back(head);
        Process *curr = runQueue->front();
        while (curr != head) {
            std::cout << curr->id << ":" << curr->timestamp << " ";
            runQueue->pop_front();
            runQueue->push_back(curr);
            curr = runQueue->front();
        }
        std::cout << std::endl;
    }
};


class LIFO : public Scheduler
{
private:
    std::deque<Process *> *runQueue;

public:
    LIFO(): Scheduler()
    {
        runQueue = new std::deque<Process*>();
    };

    virtual void addProcess(Process *p)
    {
        p->dynamicPrio = p->prio - 1;
        runQueue->push_front(p);
    };

    virtual Process *getNextProcess()
    {
        runQueueLog();
        Process *p = nullptr;
        if (!runQueue->empty())
        {
            p = runQueue->front();
            runQueue->pop_front();
        }
        return p;
    }

    bool isPreemptive()
    {
        return false;
    }

    void runQueueLog(){
        printf("LIFO SCHED (%d):", runQueue->size());
        if (runQueue->size() == 0) {
            std::cout << endl;
            return;
        }
        Process *head = runQueue->front();
        std::cout << head->id << ":" << head->timestamp << " ";
        runQueue->pop_front();
        runQueue->push_back(head);
        Process *curr = runQueue->front();
        while (curr != head) {
            std::cout << curr->id << ":" << curr->timestamp << " ";
            runQueue->pop_front();
            runQueue->push_back(curr);
            curr = runQueue->front();
        }
        std::cout << std::endl;
    }
};

class SRTF : public FIFO
{
private:
    std::deque<Process *> *runQueue;

public:
    SRTF() : FIFO()
    {
        runQueue = new std::deque<Process*>();
    };

    void addProcess(Process *p)
    {
        p->pState = READY;
        p->dynamicPrio = p->prio - 1;
        runQueue->push_back(p);
        sortQueue();
    };

    virtual Process *getNextProcess()
    {
        runQueueLog();
        Process *p = nullptr;
        if (!runQueue->empty())
        {
            p = runQueue->front();
            runQueue->pop_front();
        }
        return p;
    }

    bool isPreemptive()
    {
        return false;
    }

    int minIndex(int sortedIndex)
    {
        int min_index = -1;
        int min_val = INT_MAX;
        int n = runQueue->size();
        for (int i=0; i < n; i++)
        {
            Process *curr = runQueue->front();
            runQueue->pop_front();  // This is dequeue() in C++ STL

            // we add the condition i <= sortedIndex
            // because we don't want to traverse
            // on the sorted part of the queue,
            // which is the right part.
            if (curr->totalTime <= min_val && i <= sortedIndex)
            {
                min_index = i;
                min_val = curr->totalTime;
            }
            runQueue->push_back(curr);  // This is enqueue() in
            // C++ STL
        }
        return min_index;
    }

// Moves given minimum element to rear of
// queue
    void insertMinToRear(int min_index)
    {
        Process * min_process;

        int n = runQueue->size();
        for (int i = 0; i < n; i++)
        {
            Process *curr = runQueue->front();
            runQueue->pop_front();
            if (i != min_index)
                runQueue->push_back(curr);
            else
                min_process = curr;
        }
        runQueue->push_back(min_process);
    }

    void sortQueue()
    {
        for (int i = 1; i <= runQueue->size(); i++)
        {
            int min_index = minIndex(runQueue->size() - i);
            insertMinToRear(min_index);
        }
    }

    void runQueueLog(){
        printf("FIFO SCHED (%d):", runQueue->size());
        if (runQueue->size() == 0) return;
        Process *head = runQueue->front();
        std::cout << head->id << ":" << head->timestamp << " ";
        runQueue->pop_front();
        runQueue->push_back(head);
        Process *curr = runQueue->front();
        while (curr != head) {
            std::cout << curr->id << ":" << curr->timestamp << " ";
            runQueue->pop_front();
            runQueue->push_back(curr);
            curr = runQueue->front();
        }
        std::cout << std::endl;
    }
};


class RR : public Scheduler
{
private:
    std::deque<Process *> *runQueue;
    int quantum;

public:
    RR(int q) {
        quantum = q;
        runQueue = new std::deque<Process *>();
    }
    virtual void addProcess(Process *p) {
        p->pState = READY;
        p->dynamicPrio = p->prio-1;
        runQueue->push_back(p);
    };
    virtual Process *getNextProcess() {
        runQueueLog();
        Process *p = nullptr;
        if (!runQueue->empty())
        {
            p = runQueue->front();
            runQueue->pop_front();
        }
        return p;
    };
    virtual bool isPreemptive() {
        return true;
    };

    void runQueueLog(){
        printf("RR SCHED (%d):", runQueue->size());
        if (runQueue->size() == 0) return;
        Process *head = runQueue->front();
        std::cout << head->id << ":" << head->timestamp << " ";
        runQueue->pop_front();
        runQueue->push_back(head);
        Process *curr = runQueue->front();
        while (curr != head) {
            std::cout << curr->id << ":" << curr->timestamp << " ";
            runQueue->pop_front();
            runQueue->push_back(curr);
            curr = runQueue->front();
        }
        std::cout << std::endl;
    }
};

class PRIO : public Scheduler
{
private:
//    std::vector<std::deque<Process *>*>* activeQ;
//    std::vector<std::deque<Process *>*>* expiredQ;
    std::vector<std::deque<Process *> >* activeQ;
    std::vector<std::deque<Process *> >* expiredQ;
    int quantum;
    int maxprio;

public:
    PRIO(int q, int mp) : Scheduler(){
        this->quantum = q;
        this->maxprio = mp;
        this->activeQ =  new vector<deque<Process *> >();
        this->expiredQ =  new vector<deque<Process *> >();
        for (int i = 0; i < maxprio; i++) {
            activeQ->push_back(std::deque<Process *>());
            expiredQ->push_back(std::deque<Process *>());
        }
    }
    virtual void addProcess(Process *p) {
//        std::deque<Process *>* d = activeQ->at(1);
        p->pState = READY;
        if (p->dynamicPrio < 0) {
            p->dynamicPrio = p->prio-1;
            expiredQ->at(p->dynamicPrio).push_back(p);
        } else {
            activeQ->at(p->dynamicPrio).push_back(p);
        }

    };

    virtual Process *getNextProcess() {
        runQueueLog();
        Process *res = nullptr;
        for (int i = maxprio - 1; i >=0; i--) {
            if (activeQ->at(i).empty()) continue;
            res = activeQ->at(i).front();
            activeQ->at(i).pop_front();
            return res;
        }
        if (res == nullptr) {
            std::cout << "switch queue" << std::endl;
            switchQ();
        }
        for (int i = maxprio - 1; i >=0; i--) {
            if (activeQ->at(i).empty()) continue;
            res = activeQ->at(i).front();
            activeQ->at(i).pop_front();
            return res;
        }
        return res;
    };

    virtual bool isPreemptive() {
        return true;
    };

    void switchQ() {
        vector<std::deque<Process *> >* temp = activeQ;
        activeQ = expiredQ;
        expiredQ = temp;
    };

    void runQueueLog() {
        std::cout << traverseQ(activeQ) << " | " << traverseQ(expiredQ) << endl;
    }

    std::string traverseQ(vector<std::deque<Process *> > *q) {
        ostringstream os;
        os << "{";
        for (int i = q->size() - 1; i >=0; i--) {
            os << "[";
            Process * p;
            for (int j = 0; j < q->at(i).size(); j++) {
//                if (q->at(j).empty()) continue;
                //front will return nullptr
                p = q->at(i).front();
                os << p->id;
                q->at(i).pop_front();
                q->at(i).push_back(p);
            }
            os << "]";
        }
        os << "}";
        return os.str();
    };

};

class PREPRIO : public Scheduler
{
private:
    std::vector<std::deque<Process *> >* activeQ;
    std::vector<std::deque<Process *> >* expiredQ;
    int quantum;
    int maxprio;

public:
    PREPRIO(int q, int mp) : Scheduler(){
        this->quantum = q;
        this->maxprio = mp;
        this->activeQ =  new vector<deque<Process *> >();
        this->expiredQ =  new vector<deque<Process *> >();
        for (int i = 0; i < maxprio; i++) {
            activeQ->push_back(std::deque<Process *>());
            expiredQ->push_back(std::deque<Process *>());
        }
    }
    virtual void addProcess(Process *p) {
        if (p->pState == BLOCKED) p->dynamicPrio = p->prio-1;
        p->pState = READY;
        if (p->dynamicPrio < 0) {
            p->dynamicPrio = p->prio-1;
            expiredQ->at(p->dynamicPrio).push_back(p);
        } else {
            activeQ->at(p->dynamicPrio).push_back(p);
        }

    };

    virtual Process *getNextProcess() {
        runQueueLog();
        Process *res = nullptr;
        for (int i = maxprio - 1; i >=0; i--) {
            if (activeQ->at(i).empty()) continue;
            res = activeQ->at(i).front();
            activeQ->at(i).pop_front();
            return res;
        }
        if (res == nullptr) {
            std::cout << "switch queue" << std::endl;
            switchQ();
        }
        for (int i = maxprio - 1; i >=0; i--) {
            if (activeQ->at(i).empty()) continue;
            res = activeQ->at(i).front();
            activeQ->at(i).pop_front();
            return res;
        }
        return res;
    };

    virtual bool isPreemptive() {
        return true;
    };

    void switchQ() {
        vector<std::deque<Process *> >* temp = activeQ;
        activeQ = expiredQ;
        expiredQ = temp;
    };

    void runQueueLog() {
        std::cout << traverseQ(activeQ) << " | " << traverseQ(expiredQ) << endl;
    }

    std::string traverseQ(vector<std::deque<Process *> > *q) {
        ostringstream os;
        os << "{";
        for (int i = q->size() - 1; i >=0; i--) {
            os << "[";
            Process * p;
            for (int j = 0; j < q->at(i).size(); j++) {
//                if (q->at(j).empty()) continue;
                //front will return nullptr
                p = q->at(i).front();
                os << p->id;
                q->at(i).pop_front();
                q->at(i).push_back(p);
            }
            os << "]";
        }
        os << "}";
        return os.str();
    };
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

    std::string toString()
    {
        ostringstream os;
        // printf("[%d, %d]", startTime, procID);
        os << "(" << timestamp << "," << procID << "," << EventToString(transition) << ")";
        return os.str();
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