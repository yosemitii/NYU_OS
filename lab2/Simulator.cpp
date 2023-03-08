#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "olist.h"

using namespace std;

extern std::string convertToString(char *a);

class Simulator
{
private:
    int vflag, tflag, eflag, pflag, iflag, sflag;
    int quantum;
    int maxprio;
    std::string inputFile;
    std::string randFile;
    SchedulerType sType;

public:
    Scheduler *sched;
    RandGenerator rgen;
    vector<Process *> *procs;
    OrderedList<Event> des;
    Simulator()
    {

        // switch (sType)
        // {
        // default:
        //     sched = new FIFO(rgen, inputFile, maxprio);
        //     break;
        // }

        // vector<Process *> *processes = buildProcess(rgen, inputFile);
    }

    void showRands()
    {
        int x;
        std::cout << "Display random numbers: " << endl;
        for (int i = 0; i < 20; i++)
        {
            x = rgen.myrandom(10);
            std::cout << x << endl;
        }
    }

    void showProcs(){
        for(Process* & p: *procs) {
            p->show();
        }
    }

    void init(int argc, char **argv)
    {
        cout << argv[0] << argv[1] << argv[2] << argv[3] << endl;
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
        this->rgen = RandGenerator(randFile);
        
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
            cout << "checkpoint 0" <<endl;
            int prio = this->rgen.myrandom(maxprio);
            cout << "checkpoint 1" <<endl;
            Process *process = new Process(id, info[0], info[1], info[2], info[3], prio);
            // cout << "here 2" <<endl;
            this->procs->push_back(process);
            
            id++;
        }
        procFile.close();
    }
};