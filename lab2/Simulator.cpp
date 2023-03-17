#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include "des.cpp"
#include "utils.h"
//#include <sort.h>

using namespace std;

extern std::string convertToString(char *a);

class Simulator
{
private:
    bool vflag, tflag, eflag, pflag, iflag, sflag;
    int quantum;
    int maxprio;
    int currTime;
    std::string inputFile;
    std::string randFile;
    SchedulerType sType;

public:
    Scheduler *scheduler;
    RandGenerator *rgen;
    vector<Process *> *procs;
    OrderedList<Event *> *eventQ;
    Process *CURRENT_RUNNING_PROCESS;
    Simulator()
    {
        procs = new vector<Process *>();
        eventQ = new OrderedList<Event *>();
        currTime = 0;

        // vector<Process *> *processes = buildProcess(rgen, inputFile);
    }

    void showRands()
    {
        int x;
        std::cout << "Display random numbers: " << endl;
        for (int i = 0; i < 20; i++)
        {
            x = rgen->myrandom(10);
            std::cout << x << endl;
        }
    }

    void showProcs()
    {
        for (Process *&p : *procs)
        {
            p->show();
        }
    }

    void init(int argc, char **argv)
    {
//        std::cout << "=========Initialization start=========" << std::endl;
        // cout << argv[0] << argv[1] << argv[2] << argv[3] << endl;
        // Option reader
        int c;
        int fileCnt = 0;
        int index;
        vflag = 0;
        tflag = 0;
        eflag = 0;
        pflag = 0;
        iflag = 0;
        sflag = 0;
        maxprio = 4;
        sType = FCFS;
        while ((c = getopt(argc, argv, ":svtepisFLSREP1234567890")) != -1)
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
                break;
            case 'L':
                sType = LCFS;
                break;
            case 'S':
                sType = SRTF;
                break;
            case 'R':
                sType = RR;
                if (sscanf(argv[optind], "-sR%d:%d", &quantum, &maxprio) == -1)
                {
                    if (sscanf(argv[optind], "-sP%d", &quantum) == -1)
                    {
                        std::cout << "Error: Need at least one argument.";
                    }
                };
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
//                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                break;
            default:
                abort();
            }
        }
//        std::cout << "quantum: " << quantum << " maxprio:" << maxprio << endl;
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

        // Random Generator Initialization.
        this->rgen = new RandGenerator(randFile);

        // Process file reader
        ifstream procFile;
        procFile.open(inputFile);
        this->procs = new vector<Process *>;
        const char *delim = "\t\n ";
        std::string line;
        char *lineArr;
        int id = 0;
        while (!procFile.eof())
        {
            getline(procFile, line);
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
            // cout << info[0] << info[1] << info[2] << info[3] << endl;
            while (token != NULL)
            {
                // cout << token << endl;
                token = strtok(NULL, delim);
                if (token != NULL)
                    try
                    {
                        info[index] = stoi(token);
                        // cout << info[index] << endl;
                        // cout << info[0] << info[1] << info[2] << info[3] << endl;
                    }
                    catch (exception &err)
                    {
                        __error(1);
                    }
                index++;
            }

            int prio = rgen->myrandom(maxprio);
            Process *process = new Process(id, info[0], info[1], info[2], info[3], prio);
            procs->push_back(process);
            Event *newevent = new Event(info[0], id, TRANS_TO_READY, process);
            eventQ->put(newevent, eflag);
            id++;
        }
//        cout << "Processes in simulator:" << endl;
        // showProcs();
        procFile.close();
        switch (sType)
        {
        case FCFS:
            scheduler = new FIFO();
            break;
        case LCFS:
            scheduler = new LIFO();
            break;
        case SRTF:
            scheduler = new class SRTF();
            break;
        case RR:
            scheduler = new class RR(quantum);
            break;
        case PRIO:
            scheduler = new class PRIO(quantum, maxprio);
            break;
        case PREPRIO:
            scheduler = new class PREPRIO(quantum, maxprio);
            break;
        default:
            scheduler = new FIFO();
//            std::cout << "WARNING: NO SCHEDULER TYPE. SET AS FIFO." << std::endl;
            break;
        }
    }

    void simulation()
    {
//        std::cout << "\n========Simulatio start:========" << endl;
        while (eventQ->size != 0)
        {
            Event *evt = eventQ->get();
            Process *process = evt->proc;
            currTime = evt->timestamp;
            int transition = evt->transition;
            int timeInPrevState = currTime - process->timestamp;
            int prevTime = process->timestamp;
            ProcState prevState = process->pState;
            delete evt;

            //Control parameter
            bool CALL_SCHEDULER = false;
            bool PREPRIO_PREEMT_FINISHED = false;
            Event *newEvent;
            int nextTimeStamp;

            // Handler: Set Time
            scheduler->timestamp = currTime;
            process->timestamp = currTime;

//            if (currTime >= 491) {
//                cout << "stop" << endl;
//            }
//            int preemptTime;
            switch (transition)
            {
            case TRANS_TO_READY:
                if (process->pState == CREATED || process->pState == BLOCKED)
                {
                    if(prevState == BLOCKED) {
                        process->ioWaitTime += timeInPrevState;
                        process->ioWaitPeriod->push_back(new int[]{prevTime, currTime});
                    }
//                    std::cout << "TRANS TO READY" << std::endl;
                    stateChangeLog(prevState, READY, timeInPrevState, process, 0, vflag);

                    scheduler->addProcess(process);
                    nextTimeStamp = 0;
                    CALL_SCHEDULER = true;

                }
                break;
            case TRANS_TO_PREEMPT:
                if (process->pState == RUNNG)
                {

                    //Need to calculate the remainning time
                    process->pState = READY;
                    process->totalTime -= timeInPrevState;
                    process->cpuBurstRemain -= timeInPrevState;
                    stateChangeLog(prevState, READY, timeInPrevState, process, 0, vflag);

                    process->dynamicPrio -= 1;
                    scheduler->addProcess(process);
                    CURRENT_RUNNING_PROCESS = nullptr;

                    CALL_SCHEDULER = true;
                } else {
                    std::cout << "ERROR: PREEMPT MUST COME FROM RUNNIGN" << std::endl;
                }
                break;
            case TRANS_TO_RUN:
//                cout << "TRANS TO RUN" << endl;
                // scheduler->run();
                if (prevState = READY) process->cpuWaitTime += timeInPrevState;
                CURRENT_RUNNING_PROCESS = process;
                process->pState = RUNNG;
                //The CPU burst this time, no matter it will be preempted or not. Only take random when the current one is exhauseted
//                    nextTimeStamp = rgen->myrandom(process->cpuBurst);
//                    nextTimeStamp = std::min(nextTimeStamp, process->totalTime);


                if (scheduler->isPreemptive()>0) {
                    //The case that the cpu burst is NOT exhausted
                    //Do not generate new burst
                    //Two sub-cases: 1. remainBurst > quantum: PREEMT
                    //               2. remainBurst <= quantum: BLOCK
                    if (process->cpuBurstRemain > 0) {
                        if (quantum < process->cpuBurstRemain) {
//                            process->cpuBurstRemain = nextTimeStamp;
                            newEvent = new Event(currTime + quantum,
                                                 process->id, TRANS_TO_PREEMPT, process);
                            stateChangeLog(prevState, RUNNG, timeInPrevState, process, quantum, vflag);
                        } else {
                            newEvent = new Event(currTime + process->cpuBurstRemain,
                                                 process->id, TRANS_TO_BLOCK, process);
                            stateChangeLog(prevState, RUNNG, timeInPrevState, process, process->cpuBurstRemain, vflag);
                        }
                        eventQ->put(newEvent, eflag);
                    }
                    //The case that the cpu burst is exhausted
                    //Two sub-cases: 1. burst > quantum: PREEMT
                    //               2. burst <= quantum: BLOCK
                    else {
                        nextTimeStamp = rgen->myrandom(process->cpuBurst);
                        nextTimeStamp = std::min(nextTimeStamp, process->totalTime);
                        process->cpuBurstRemain = nextTimeStamp;
                        if (quantum < nextTimeStamp) {
                            newEvent = new Event(currTime + quantum,
                                                 process->id, TRANS_TO_PREEMPT, process);
                            stateChangeLog(prevState, RUNNG, timeInPrevState, process, quantum, vflag);
                        } else {
                            newEvent = new Event(currTime + nextTimeStamp,
                                                 process->id, TRANS_TO_BLOCK, process);
                            stateChangeLog(prevState, RUNNG, timeInPrevState, process, nextTimeStamp, vflag);
                        }
                        eventQ->put(newEvent, eflag);
                    }

                } //The case of Non-preemptive.
                else {
                    nextTimeStamp = rgen->myrandom(process->cpuBurst);
                    nextTimeStamp = std::min(nextTimeStamp, process->totalTime);
                    newEvent = new Event(currTime + nextTimeStamp,
                                         process->id, TRANS_TO_BLOCK, process);
                    stateChangeLog(prevState, RUNNG, timeInPrevState, process, nextTimeStamp, vflag);
                    eventQ->put(newEvent, eflag);
                }
                break;
            case TRANS_TO_BLOCK:
//                std::cout << "TRANS TO BLOCK" << std::endl;
                if (process->pState == RUNNG)
                {
                    process->totalTime -= timeInPrevState;
                    process->cpuBurstRemain -= timeInPrevState;
                    process->pState = BLOCKED;
                    CURRENT_RUNNING_PROCESS = nullptr;
                    if (process->totalTime > 0)
                    {
                        nextTimeStamp = rgen->myrandom(process->ioBurst);
                        stateChangeLog(prevState, BLOCKED, timeInPrevState, process, nextTimeStamp, vflag);
                        newEvent = new Event(currTime + nextTimeStamp,
                                             process->id, TRANS_TO_READY, process);
                        eventQ->put(newEvent, eflag);
                    }
                    else
                    {
                        stateChangeLog(prevState, DONE, timeInPrevState, process, 0, vflag);
//                        std::cout << "DONE: " << process->id << std::endl;
                        process->finishTime = currTime;
                    }
                }
                else
                {
                   std::cout << "ERROR: TRANS_TO_BLOCKD, Process is not running" << std::endl;
                }
                // create an event for when process becomes READY again
                CALL_SCHEDULER = true;
                break;
            default:
                break;
            }

            if (CALL_SCHEDULER)
            {
                //for PREPRIO only

//                scheduler->runQueueLog();
                if (eventQ->getNextEventTime() == currTime){
                    if (sType == PREPRIO
                    && prevState == BLOCKED
                    && process->pState == READY
                    && CURRENT_RUNNING_PROCESS != nullptr
                    && !stopAt(CURRENT_RUNNING_PROCESS->id, currTime)
                    && eventQ->peek()->procID != process->id
                    && eventQ->peek()->procID != CURRENT_RUNNING_PROCESS->id
                    && process->dynamicPrio > eventQ->peek()->proc->dynamicPrio) {
                        if (CURRENT_RUNNING_PROCESS != nullptr && process->dynamicPrio > CURRENT_RUNNING_PROCESS->dynamicPrio) {
                            eventQ->remove(CURRENT_RUNNING_PROCESS->id, eflag);
                            newEvent = new Event(currTime, CURRENT_RUNNING_PROCESS->id, TRANS_TO_PREEMPT, CURRENT_RUNNING_PROCESS);
                            eventQ->put(newEvent, eflag);
                            CALL_SCHEDULER = false;
                            PREPRIO_PREEMT_FINISHED = true;
                        }
                    } else {
                        continue;
                    }

                }


                CALL_SCHEDULER = false;

                if (CURRENT_RUNNING_PROCESS == nullptr)
                {
                    CURRENT_RUNNING_PROCESS = scheduler->getNextProcess(tflag);
                    if (CURRENT_RUNNING_PROCESS == nullptr) {
                        continue;
                    } else {
                        // CURRENT_RUNNING_PROCESS->show();
                        //This case is for switching running process.
                        Event *newEvent = new Event(currTime, CURRENT_RUNNING_PROCESS->id, TRANS_TO_RUN, CURRENT_RUNNING_PROCESS);
                        eventQ->put(newEvent, eflag);
                    }
                } else {
                    if (sType == PREPRIO && process->pState == READY && process->dynamicPrio > CURRENT_RUNNING_PROCESS->dynamicPrio) {
                        if (!PREPRIO_PREEMT_FINISHED) {
                            eventQ->remove(CURRENT_RUNNING_PROCESS->id, eflag);
                            newEvent = new Event(currTime, CURRENT_RUNNING_PROCESS->id, TRANS_TO_PREEMPT, CURRENT_RUNNING_PROCESS);
                            eventQ->put(newEvent, eflag);
                        } else {
                            PREPRIO_PREEMT_FINISHED = false;
                        }
                        continue;
                    }
                }
            }
        }

        accounting();
    }

    bool stopAt(int procID, int time) {
        if (CURRENT_RUNNING_PROCESS == nullptr) return false;
        vector<Event *> *eventAtTime = eventQ->getAllEventByTime(time);
        if (eventAtTime->size() == 0) return false;
        for (auto e: *eventAtTime) {
            if (e->procID == CURRENT_RUNNING_PROCESS->id && e->timestamp == time
            && (e->transition == TRANS_TO_BLOCK || e->transition == TRANS_TO_PREEMPT)) {
                return true;
            }
        }
        return false;
    }



    void stateChangeLog(ProcState prev, ProcState now, int timeInPrevState, Process *p, int timeNeeded = 0, bool display = true)
    {
        if (!display) return;
        printf("%d %d %d: %s->%s\t", p->timestamp, p->id, timeInPrevState, ProcToString(prev), ProcToString(now));
        if (now == DONE) {
            printf("\n");
            return;
        }
        if (prev == READY && now == RUNNG)
        {
            printf("cb=%d rem=%d prio=%d",  p->cpuBurstRemain, p->totalTime, p->dynamicPrio);
        }
        else if (prev == RUNNG && now == BLOCKED)
        {
            printf("ib=%d rem=%d prio=%d", timeNeeded, p->totalTime, p->prio);
        } else if (prev == RUNNG && now == READY)
        {
            printf("cb=%d rem=%d prio=%d", p->cpuBurstRemain, p->totalTime, p->dynamicPrio);
        }
        printf("\n");
    }

    void accounting() {
        std::cout << StypeToString(sType);
        if (scheduler->isPreemptive()) std::cout << " " << quantum;
        std::cout << std::endl;
        int macroFinishTime = currTime;
        int macroCPUWaitTime = 0;
        int macroIOWaitTime = 0;
        int macroNumOfProcess = procs->size();
        int macroTurnAroundTime = 0;
        int macroCPUTotalTime = 0;
        vector<int *> periods = vector<int *>();
        Process *p;
        for (int i = 0; i < procs->size(); i++) {
            p = procs->at(i);
            p->turnAroundTime = p->finishTime - p->arrivTime;
            printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
                   p->id, p->arrivTime, p->statisticTT, p->cpuBurst, p->ioBurst, p->prio,
                   p->finishTime, p->turnAroundTime,p->ioWaitTime, p->cpuWaitTime);
            macroCPUTotalTime += p->statisticTT;
            macroCPUWaitTime += p->cpuWaitTime;
//            macroIOWaitTime += p->ioWaitTime;
            macroTurnAroundTime += p->turnAroundTime;

            periods.insert(periods.end(), p->ioWaitPeriod->begin(), p->ioWaitPeriod->end());
        }

        sort(periods.begin(), periods.end(),[] (int* a, int* b) -> bool {
            if (a[0] != b[0]) return a[0] < b[0];
            else return a[1] < b[1];
        });
        if (!periods.empty()) {
            int startTime = periods.front()[0];
            int endTime = periods.front()[1];
            for (auto i: periods) {
                if (i[0] <= endTime) {
                    endTime = max(endTime, i[1]);
                } else {
                    macroIOWaitTime += (endTime - startTime);
                    startTime = i[0];
                    endTime = i[1];
                }
            }
            macroIOWaitTime += (endTime - startTime);
        }


        double cpuUtil = 100.0 * (macroCPUTotalTime / (double) macroFinishTime);
        double ioUtil = 100.0 * (macroIOWaitTime / (double) macroFinishTime);
        double throughput = 100.0 * (macroNumOfProcess / (double) macroFinishTime);
        double turnarounRatio = (macroTurnAroundTime/ (double) macroNumOfProcess);
        double cpuWaitRaio = (macroCPUWaitTime/ (double) macroNumOfProcess);
        printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
               macroFinishTime, // last event finish
               cpuUtil, // percent cpu utilization
               ioUtil, // percent io wait
               turnarounRatio, // average turnaround
               cpuWaitRaio, //average waiting time
               throughput); // average something
    }
};