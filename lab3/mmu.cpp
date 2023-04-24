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
    unsigned int FILE_MAPPED:1;
    unsigned int FREE:18;
    unsigned int FRAME_NUMBER:7;

    void reset() {
        PRESENT = 0;
        REFERENCED = 0;
        MODIFIED = 0;
        FRAME_NUMBER = 0;
    }
} pte_t;

typedef struct {
    int PROCESS_ID = -1;
    int VPAGE = -1;
    int INDEX = -1;
} frame_t;


//typedef enum PagerType
//{
//    FCFS,
//    RANDOM,
//    CLOCK,
//    NRU,
//    AGING,
//    WORKING_SET,
//} PagerType;


//pte_t page_table[MAX_VPAGES];
frame_t frame_table[MAX_FRAMES];
int victim_frame_index = -1;
class Pager {
public:
    virtual frame_t* select_victim_frame() = 0; // virtual base class
};

class FIFOPager: public Pager {
    int num_frames;
//    int size;
    int victim_index;
public:
    FIFOPager(int num_frames) {
//        this->size = 0;
//        this->frames = deque<frame_t*> (num_frames);l
        this->num_frames = num_frames;
        this->victim_index = -1;
    }
    frame_t* select_victim_frame() override {
        victim_index = (victim_index + 1) % num_frames;
        return &frame_table[victim_index];
//        return frames[victim_index];
    }
};

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
                   page_table[i].PAGEDOUT, page_table[i].NOT_HOLE, page_table[i].FRAME_NUMBER);
        }
    }

    void display_pagetable() {
        printf("PT[%d]:", id);
        for (int i = 0; i < 64; i++) {
            pte_t pte = page_table[i];
            if (!pte.PRESENT) {
//                printf("%d", pte.PAGEDOUT);
//                printf((pte.PAGEDOUT && !pte.FILE_MAPPED)? " #" : " *");
                if (pte.PAGEDOUT && !pte.FILE_MAPPED) {
                    printf(" #");
                }
                else {
                    printf(" *");
                }
            }
            else {
                printf(" %d:%c%c%c", i, pte.REFERENCED ? 'R' : '-',
                       pte.MODIFIED ? 'M' : '-', pte.PAGEDOUT ? 'S' : '-');
            }
        }
        printf("\n");
    }
};

class Simulator {
public:
    int num_frames;
    std::string infile_name;
    std::string rfile_name;
    std::deque<int> free_list;
    vector<Process *> *processes;
    ifstream inFile;
    ifstream rFile;
    Pager *THE_PAGER;
    bool Oflag;
    bool Pflag;
    bool Fflag;
//    frame_t *frame_table;
    Simulator(char pager_type, int num_frames, std::string infile_name, std::string rfile_name)
    {
        this->THE_PAGER = new FIFOPager(num_frames);
        this->num_frames = num_frames;
        this->infile_name = infile_name;
        this->rfile_name = rfile_name;
        this->processes = new vector<Process *>;
//        this->frame_table = new frame_t[num_frames];
    }

    void init() {
        inFile.open(infile_name);
        std::string line;
        int readType = 0;
        //For reading processes
        int num_process = 0;
        int proc_cnt = 0;
        // For reading vmas
        int num_vmas = 0;
        int vma_cnt = 0;
        char *lineArr;
        Process *curr_proc;
//        const char *delim = "\t\n ";
//        char *token;
//        int index = 0;

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
                int file_mapped = 0;
                std::cout << line << std::endl;
                istringstream line_stream(line);
                for (int i = 0; i < 4; i++) {
//                    token = strtok(lineArr, delim);
//                    if (i == 0)  start = stoi(token);
//                    else if (i == 1) end = stoi(token);
//                    else if (i == 2) write_protected = stoi(token);
//                    else file_mapped = stoi(token);
                    if (i == 0)  line_stream >> start;
                    else if (i == 1) line_stream >> end;
                    else if (i == 2) line_stream >> write_protected;
                    else line_stream >> file_mapped;
                }
                for (int j = start; j <= end; j++) {
                    curr_proc->page_table[j].NOT_HOLE = 1;
                    curr_proc->page_table[j].WRITE_PROTECT = write_protected;
                    curr_proc->page_table[j].FILE_MAPPED = file_mapped;
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
        printf("--------------Done reading processes--------------\n");
        for (int i = 0; i < num_frames; i++) {
            free_list.push_back(i);
            frame_table[i].VPAGE = -1;
            frame_table[i].PROCESS_ID = -1;
            frame_table[i].INDEX = i;
        }
//        for (auto p: *processes){
//            printf("process id: %d\n", p->id);
//            p->display_pte();
//        }
    }

    bool  get_next_instruction(char &operation, int &vpage) {
        std::string line;
        if (!inFile.eof()) {
            getline(inFile, line);
            while (line[0] == '#' && !inFile.eof()) {
                getline(inFile, line);
            }
            if (inFile.eof()) return false;
            istringstream line_stream(line);
            line_stream >> operation;
            line_stream >> vpage;
            return true;
        } else {
            inFile.close();
            return false;
        }
    }

    frame_t *allocate_frame_from_free_list() {
        if (free_list.size() == 0) return NULL;
        else {
            frame_t *res = &frame_table[free_list.front()];
            free_list.pop_front();
            return res;
        }
    }

    frame_t *get_frame() {
        frame_t *frame = allocate_frame_from_free_list();
        if (frame == NULL) frame = THE_PAGER->select_victim_frame();
        return frame;
    }

    void simulation() {
        char operation;
        int vpage;
        Process *current_process;
        int instruct_cnt = 0;
        int frame_index = -1;
        while (get_next_instruction(operation, vpage)) {
            // handle special case of “c” and “e” instruction
            printf("%d: ==> %c %d\n", instruct_cnt, operation, vpage);
            if (operation == 'c') {
                current_process = processes->at(vpage);
                instruct_cnt++;
                continue;
            } else if (operation == 'e') {
                //Exit handle
                printf("EXIT current process %d\n", vpage);
                current_process = processes->at(vpage);
                for (int i = 0; i < MAX_VPAGES; i++) {
                    pte_t *del_pte = &current_process->page_table[i];
                    if (del_pte->PRESENT) {
                        free_list.push_back(del_pte->FRAME_NUMBER);
                        exit_unmap(&frame_table[del_pte->FRAME_NUMBER]);
                        if (del_pte->MODIFIED) {
                            if (del_pte->FILE_MAPPED) {
                                printf(" FOUT\n");
                            }
//                            } else {
//                                printf(" OUT\n");
//                            }
                        }
                    }
                    del_pte->reset();
                    del_pte->PAGEDOUT = 0;
                }
                current_process = nullptr;
                instruct_cnt++;
                continue;
            }
            // now the real instructions for read and write

            pte_t *pte = &current_process->page_table[vpage];
            if (!pte->NOT_HOLE) {
                printf(" SEGV\n");
                instruct_cnt++;
                continue;
            }
            if (!pte->PRESENT) {
                // this in reality generates the page fault exception and now you execute
                // verify this is actually a valid page in a vma if not raise error and next inst
                frame_t *newframe = get_frame();
//                frame_t *newframe = get_frame();
                //-> figure out if/what to do with old frame if it was mapped
                int pid = newframe->PROCESS_ID;
                int vpn = newframe->VPAGE;
                //Swap out
                if (pid != -1 || vpn != -1) {
                    printf(" UNMAP %d:%d\n", pid, vpn);
                    Process *victim = processes->at(pid);
                    if (victim->page_table[vpn].MODIFIED) {
                        victim->page_table[vpn].PAGEDOUT = 1;
                        if (victim->page_table[vpn].FILE_MAPPED) {
                            printf(" FOUT\n");
                        } else {
                            printf(" OUT\n");
                        }
                    }
                    victim->page_table[vpn].reset();
//                    processes->at(pid)->page_table[vpn].PRESENT = 0;
                    victim->page_table[vpn].PRESENT = 0;
                }
                newframe->PROCESS_ID = current_process->id;
                newframe->VPAGE = vpage;
                // see general outline in MM-slides under Lab3 header and writeup below
                // see whether and how to bring in the content of the access page.
                if (pte->FILE_MAPPED) {
                    printf(" FIN\n");
                } else {
                    if (pte->PAGEDOUT) {
                        printf(" IN\n");
                    } else {
                        printf(" ZERO\n");
                    }
                }
//                if (!pte->PAGEDOUT && !pte->FILE_MAPPED) {
//                    printf(" ZERO\n");
//                } else if (pte->FILE_MAPPED) {
//                    printf(" FIN\n");
//                } else if (pte->PAGEDOUT) {
//                    printf(" IN\n");
//                }
                pte->FRAME_NUMBER = newframe->INDEX;
                pte->PRESENT = 1;
                printf(" MAP %d\n", newframe->INDEX);
            }
//            } else {
//                instruct_cnt++;
//                continue;
//            }
            // now the page is definitely present // check write protection
            if (operation == 'w') {
                if (pte->WRITE_PROTECT) {
                    printf(" SEGPROT\n");
                } else {
                    pte->MODIFIED = 1;
                }
            }
            pte->REFERENCED = 1;

            // simulate instruction execution by hardware by updating the R/M PTE bits
//            update_pte(read/modify) bits based on operations.
            instruct_cnt++;
        }

        if (Pflag) {
            for (auto p: *processes) {
                p->display_pagetable();
            }
        }

        if (Fflag) {
            printf("FT:");
            for (int i = 0; i < num_frames; i++) {
                if (frame_table[i].PROCESS_ID == -1) printf("*");
                else {
                    printf(" %d:%d", frame_table[i].PROCESS_ID, frame_table[i].VPAGE);
                }
            }
        }


    }

    void exit_unmap(frame_t* frameEntry) {
        if (frameEntry->PROCESS_ID == -1 || frameEntry->VPAGE == -1) {
            printf("Unable to unmap\n");
            return;
        }
        printf(" UNMAP %d:%d\n", frameEntry->PROCESS_ID, frameEntry->VPAGE);
        frameEntry->PROCESS_ID = -1;
        frameEntry->VPAGE = -1;
//        free_list.push_back(frameEntry.INDEX);
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

    Simulator* s = new Simulator(pager_type, num_frames, infile_name, rfile_name);
    s->init();
    s->Pflag = true;
    s->Fflag = true;
    s->simulation();
}