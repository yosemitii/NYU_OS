//
// Created by Lubin Sun on 4/18/23.
//
#include <stdio.h>
#include <string>
#include <deque>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include "utils.h"
using namespace std;

#define MAX_FRAMES 128
#define MAX_VPAGES 64


typedef struct {
    unsigned int PRESENT:1;
    unsigned int REFERENCED:1;
    unsigned int MODIFIED:1;
    unsigned int WRITE_PROTECT:1;
    unsigned int PAGEDOUT:1;
    unsigned int NOT_HOLE:1;
    unsigned int FREE:19;
    unsigned int FRAME_TABLE:7;
} pte_t;

typedef struct {
    int PROCESS_ID;
    int VPAGE;
} frame_t;

typedef struct {
    std::string op;
    int vma_num;
} op_pair;

typedef enum PagerType
{
    FCFS,
    RANDOM,
    CLOCK,
    NRU,
    AGING,
    WORKING_SET,
} PagerType;

frame_t frame_table[MAX_FRAMES];
pte_t page_table[MAX_VPAGES];

class Pager {
    virtual frame_t* select_victim_frame() = 0; // virtual base class
};

//class FIFO: Pager {
//
//public:
//    FIFO() {
//
//    }
//    frame_t* select_victim_frame() override
//    {
//
//    }
//};

class Process {
public:
    int id;
    pte_t page_table[MAX_VPAGES];

    Process(int id) {
        this->id = id;
        page_table[MAX_VPAGES] = {0};
    }

    void display_pte() {
        for (int i = 0; i < 64; i++) {
            printf("PTE:%d, PRESENT:%d REFERENCED:%d MODIFIED:%d WRITE_PROTECT:%d PAGEDOUT:%d, NOT_HOLE:%d, FRAME_NUMBER:%d\n", i,
                   page_table[i].PRESENT, page_table[i].REFERENCED, page_table[i].MODIFIED, page_table[i].WRITE_PROTECT,
                   page_table[i].PAGEDOUT, page_table[i].NOT_HOLE, page_table[i].FRAME_TABLE);
        }
    }
};

class Simulator {
public:
    int num_frames = 0;
    PagerType pager_type;
    std::string infile_name;
    std::string rfile_name;
    std::deque<op_pair> operations;
    vector<Process *> *processes;
    ifstream inFile;
    ifstream rFile;

    Simulator(int num_frames, std::string infile_name, std::string rfile_name)
    {
        this->num_frames = num_frames;
//        this->pager_type = p_type;
        this->infile_name = infile_name;
        this->rfile_name = rfile_name;
        this->processes = new vector<Process *>();
    }

    void init() {
        printf("start init\n");
        inFile.open(infile_name);
        std::string line;
        int vma_num;
        char op;
        int readType = 0;
        int num_process = 0;
        int proc_cnt = 0;
        int num_vmas = 0;
        int vma_cnt = 0;
        char *lineArr;
        char *token;
        Process *curr_proc;
        const char *delim = "\t\n ";
        int index = 0;
        bool DONE_PROC_READ = false;
        while (!inFile.eof() && !DONE_PROC_READ)
        {
            getline(inFile, line);
            if (line[0] == '#') continue;
            else if (readType == 0)
            {
                //Type 0: Number of process
                num_process = stoi(line);
                readType++;
            }
            else if (readType == 1)
            {
                //Type 1: Number of VMAs
                num_vmas = stoi(line);
                curr_proc = new Process(proc_cnt);
                this->processes->push_back(curr_proc);
                readType++;
            }
            else if (readType == 2)
            {
                //Type 2: Detail of each VMA
                lineArr = new char[line.length() + 1];
                strcpy(lineArr, line.c_str());
                int start = 0;
                int end = 63;
                int write_protected = 0;
                int filemapped = 0;
                std::cout << line << std::endl;
                istringstream line_stream(line);
                for (int i = 0; i < 4; i++) {
//                    token = strtok(lineArr, delim);
//                    if (i == 0)  start = stoi(token);
//                    else if (i == 1) end = stoi(token);
//                    else if (i == 2) write_protected = stoi(token);
//                    else filemapped = stoi(token);
                    if (i == 0)  line_stream >> start;
                    else if (i == 1) line_stream >> end;
                    else if (i == 2) line_stream >> write_protected;
                    else line_stream >> filemapped;
                }
                for (int j = start; j <= end; j++) {
                    curr_proc->page_table[j].NOT_HOLE = 1;
                    curr_proc->page_table[j].WRITE_PROTECT = write_protected;
                    curr_proc->page_table[j].PRESENT = filemapped;
                }
                vma_cnt++;
                if (vma_cnt == num_vmas) {
                    vma_cnt = 0;
                    proc_cnt++;
                    readType = 1;
                    if (proc_cnt == num_process) DONE_PROC_READ = true;
                }
            }
        }
        printf("Done reading processes\n");
//        for (auto p: *processes){
//            printf("process id: %d\n", p->id);
//            p->display_pte();
//        }
    }
};

int main (int argc, char** argv) {
    int c;
    int num_frames;
    char pager_type;
    std::string infile_name;
    std::string rfile_name;
    while ((c = getopt(argc, argv, "f:a:o:")) != -1) {
        switch(c)
        {
            case 'f':
                num_frames = std::atoi(optarg);
                printf("OPT arg: %s\n", optarg);
                if (sscanf(argv[optind], "-f%d", &num_frames) == -1)
                {
                    printf("Error input");
                }
                break;
            case 'a':
                pager_type = *optarg;
                if (sscanf(argv[optind], "-a%c", &pager_type) == -1)
                {
                    printf("Error input");
                }
                break;
            case 'o':
                printf("o\n");
                break;
            case 'O':
                printf("O\n");
                break;
            case 'P':
                printf("P\n");
                break;
            case 'F':
                printf("F\n");
                break;
            case 'S':
                printf("S\n");
                break;
            case '?':
                printf("%s", c);
            default:
                printf("Abort from switching");
                abort();

        }
    }
    int fileCnt = 0;

    for (int index = optind; index < argc; index++)
    {
        if (fileCnt == 0)
        {
            printf("%s\n", argv[index]);
            infile_name = string(argv[index]);
            fileCnt++;
        }
        else if (fileCnt == 1)
        {
            printf("%s\n", argv[index]);
            rfile_name = string(argv[index]);
//            randFile = convertToString(argv[index]);
            fileCnt++;
        }
    }

    Simulator* s = new Simulator(num_frames, infile_name, rfile_name);
    s->init();
//    printf("num_frames: %d\n", num_frames);
//    printf("pager_type: %c\n", pager_type);
}