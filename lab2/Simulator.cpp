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

using namespace std;

extern std::string convertToString(char *a);

class Simulator
{
private:
    int vflag, tflag, eflag, pflag, iflag, sflag;
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
        std::cout << "=========Initialization start=========" << std::endl;
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
                if (sscanf(argv[optind], "-sP%d:%d", &quantum, &maxprio) == -1)
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

            int prio = rgen->myrandom(maxprio) - 1;
            Process *process = new Process(id, info[0], info[1], info[2], info[3], prio);
            procs->push_back(process);
            Event *newevent = new Event(info[0], id, TRANS_TO_READY, process);
            eventQ->put(newevent);
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
            std::cout << "WARNING: NO SCHEDULER TYPE. SET AS FIFO." << std::endl;
            break;
        }
    }

    void simulation()
    {
        std::cout << "\n========Simulatio start:========" << endl;

        while (eventQ->size != 0)
        {
//            eventQ->display();
            //Parse event
            Event *evt = eventQ->get();
            Process *process = evt->proc;
            currTime = evt->timestamp;
            int transition = evt->transition;
            int timeInPrevState = currTime - process->timestamp;
            ProcState prevStatet = process->pState;
            delete evt;

            //Control parameter
            bool CALL_SCHEDULER = false;
            Event *newEvent;
            int nextTimeStamp;

            // Handler: Set Time
            scheduler->timestamp = currTime;
            process->timestamp = currTime;

            //Definition for control flow:
            int preemptTime;
            switch (transition)
            {
            case TRANS_TO_READY:
                if (process->pState == CREATED || process->pState == BLOCKED)
                {
//                    std::cout << "TRANS TO READY" << std::endl;
                    stateChangeLog(prevStatet, READY, timeInPrevState, process);
                    process->pState = READY;
                    scheduler->addProcess(process);
                    nextTimeStamp = 0;
                    CALL_SCHEDULER = true;
                    // newEvent = new Event(currTime, process->id, TRANS_TO_RUN, process);
                    // eventQ->put(newEvent);
                }
                break;
            case TRANS_TO_PREEMPT:
                if (process->pState == RUNNG)
                {
                    process->pState = READY;
                    stateChangeLog(prevStatet, READY, timeInPrevState, process);
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
                CURRENT_RUNNING_PROCESS = process;
                process->pState = RUNNG;
                nextTimeStamp = rgen->myrandom(process->cpuBurst);
                nextTimeStamp = std::min(nextTimeStamp, process->totalTime);
                if (scheduler->isPreemptive()) {
                    preemptTime = quantum - (currTime % quantum);
                    if (preemptTime < nextTimeStamp) {
                        newEvent = new Event(currTime + preemptTime,
                                             process->id, TRANS_TO_PREEMPT, process);
                        stateChangeLog(prevStatet, RUNNG, timeInPrevState, process, preemptTime);
                    } else {
                        newEvent = new Event(currTime + nextTimeStamp,
                                             process->id, TRANS_TO_BLOCK, process);
                        stateChangeLog(prevStatet, RUNNG, timeInPrevState, process, nextTimeStamp);
                    }
                    eventQ->put(newEvent);
                } else {

                    newEvent = new Event(currTime + nextTimeStamp,
                                         process->id, TRANS_TO_BLOCK, process);
                    stateChangeLog(prevStatet, RUNNG, timeInPrevState, process, nextTimeStamp);
                    eventQ->put(newEvent);
                }
                break;
            case TRANS_TO_BLOCK:
//                std::cout << "TRANS TO BLOCK" << std::endl;
                if (process->pState == RUNNG)
                {
                    process->totalTime -= timeInPrevState;
                    process->pState = BLOCKED;
                    CURRENT_RUNNING_PROCESS = nullptr;
                    if (process->totalTime > 0)
                    {
                        nextTimeStamp = rgen->myrandom(process->ioBurst);
                        stateChangeLog(prevStatet, BLOCKED, timeInPrevState, process, nextTimeStamp);
                        newEvent = new Event(currTime + nextTimeStamp,
                                             process->id, TRANS_TO_READY, process);
                        eventQ->put(newEvent);
                    }
                    else
                    {
                        stateChangeLog(prevStatet, BLOCKED, timeInPrevState, process);
                        std::cout << "DONE: " << process->id << std::endl;
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
//                scheduler->runQueueLog();
                if (eventQ->getNextEventTime() == currTime)
                    continue;

                CALL_SCHEDULER = false;

                if (CURRENT_RUNNING_PROCESS == nullptr)
                {
                    CURRENT_RUNNING_PROCESS = scheduler->getNextProcess();
                    if (CURRENT_RUNNING_PROCESS == nullptr) {
                        continue;
                    } else {
                        // CURRENT_RUNNING_PROCESS->show();
                        //This case is for switching running process.
                        Event *newEvent = new Event(currTime, CURRENT_RUNNING_PROCESS->id, TRANS_TO_RUN, CURRENT_RUNNING_PROCESS);
                        eventQ->put(newEvent);
                    }
                }
            }
        }
    }

    void stateChangeLog(ProcState prev, ProcState now, int timeInPrevState, Process *p, int timeNeeded = 0)
    {
        printf("%d %d %d: %s->%s\t", p->timestamp, p->id, timeInPrevState, ProcToString(prev), ProcToString(now));
        if (prev == READY && now == RUNNG)
        {
            printf("cb=%d rem=%d prio=%d", timeNeeded, p->totalTime, p->prio);
        }
        else if (prev == RUNNG && now == BLOCKED)
        {
            printf("ib=%d rem=%d prio=%d", timeNeeded, p->totalTime, p->prio);
        }
        printf("\n");
    }
};