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
#include <cstring>
#include "utils.h"
using namespace std;

#define MAX_FRAMES 128
#define MAX_VPAGES 64
#define COST_MAPS 350
#define COST_UNMAPS 410
#define COST_INS 3200
#define COST_OUTS 2750
#define COST_FINS 2350
#define COST_FOUTS 2800
#define COST_ZEROS 150
#define COST_SEGV 440
#define COST_SEGPROT 410
#define COST_RW 1
#define COST_CONTEXT_SWITCH 130
#define COST_EXIT 1230


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

    void init() {
        PRESENT = 0;
        REFERENCED = 0;
        MODIFIED = 0;
        WRITE_PROTECT = 0;
        PAGEDOUT = 0;
        NOT_HOLE = 0;
        FILE_MAPPED = 0;
        FREE = 0;
        FRAME_NUMBER = 0;
    }

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
    unsigned int AGE = 0;
    unsigned int TIME_LAST_USED = 0;
} frame_t;

//pte_t page_table[MAX_VPAGES];
frame_t frame_table[MAX_FRAMES];

class Process {
public:
    typedef struct {
        unsigned long unmaps = 0;
        unsigned long maps = 0;
        unsigned long ins = 0;
        unsigned long outs = 0;
        unsigned long fins = 0;
        unsigned long fouts = 0;
        unsigned long zeros = 0;
        unsigned long segv = 0;
        unsigned long segprot = 0;
    } proc_stats;

    int id;
    pte_t page_table[MAX_VPAGES];
    proc_stats pstats;

    Process(int id) {
        this->id = id;
        this->page_table[MAX_VPAGES] = {0};
        this->pstats;
    }

    void display_pagetable() {
        printf("PT[%d]:", id);
        for (int i = 0; i < MAX_VPAGES; i++) {
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

    void diplay_stats() {
        printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n", id,
               pstats.unmaps, pstats.maps, pstats.ins, pstats.outs,
               pstats.fins, pstats.fouts, pstats.zeros,
               pstats.segv, pstats.segprot);
    }

};

vector<Process *> *processes = new vector<Process *>();

class Pager {
public:
    virtual frame_t* select_victim_frame(int instruct_cnt) = 0; // virtual base class
//
    virtual void map_update(int frame_id) = 0;
};

class FIFOPager: public Pager {
    int num_frames;
//    int size;
    int hand;
public:
    FIFOPager(int num_frames) {
//        this->size = 0;
//        this->frames = deque<frame_t*> (num_frames);l
        this->num_frames = num_frames;
        this->hand = -1;
    }
    frame_t* select_victim_frame(int instruct_cnt) override {
        hand = (hand + 1) % num_frames;
        return &frame_table[hand];
//        return frames[hand];
    }

    virtual void map_update(int frame_id) {return;};
};

class RandomPager: public Pager {
    int num_frames;
    int hand;
    RandGenerator *randGenerator;
public:
    RandomPager(int num_frames, std::string rfile_name) {
//        this->size = 0;
//        this->frames = deque<frame_t*> (num_frames);l
        this->num_frames = num_frames;
        this->hand = -1;
        this->randGenerator = new RandGenerator(rfile_name);
    }

    frame_t* select_victim_frame(int instruct_cnt) override {
        hand = randGenerator->myrandom(num_frames);
        return &frame_table[hand];
    }

    virtual void map_update(int frame_id) {return;};
};

class ClockPager: public Pager {
    int num_frames;
//    int size;
    int hand;
    RandGenerator *randGenerator;
public:
    ClockPager(int num_frames) {
        this->num_frames = num_frames;
        this->hand = -1;
    }

    frame_t* select_victim_frame(int instruct_cnt) override {
        hand = (hand + 1) % num_frames;
        int cnt = 0;
        int pid = frame_table[hand].PROCESS_ID;
        int vpn = frame_table[hand].VPAGE;
        while (processes->at(pid)->page_table[vpn].REFERENCED) {
            processes->at(pid)->page_table[vpn].REFERENCED = false;
            hand = (hand + 1) % num_frames;
            pid = frame_table[hand].PROCESS_ID;
            vpn = frame_table[hand].VPAGE;
            cnt++;
        }
        return &frame_table[hand];
    }

    virtual void map_update(int frame_id) {return;};

};

class NRUPager: public Pager {
    int num_frames;
    int hand;
    int reset_period;
    int last_rest;
//    frame_t *class_0;
//    frame_t *class_1;
//    frame_t *class_2;
//    frame_t *class_3;

public:
    NRUPager(int num_frames) {
        this->num_frames = num_frames;
        this->hand = 0;
        this->reset_period = 50;
        this->last_rest = -1;
    }

    frame_t* select_victim_frame(int instruct_cnt) override {
        frame_t *class_0 = nullptr;
        frame_t *class_1 = nullptr;
        frame_t *class_2 = nullptr;
        frame_t *class_3 = nullptr;

        bool RESET_FLAG = false;
        if (instruct_cnt - last_rest >= 50) {
            RESET_FLAG = true;
        }
        frame_t *curr_frame;
        int pid;
        int vpn;
        for (int i = 0; i < num_frames; i++) {
            curr_frame = &frame_table[hand];
            pid = curr_frame->PROCESS_ID;
            vpn = curr_frame->VPAGE;
            pte_t *curr_pte = &processes->at(pid)->page_table[vpn];

            if (!curr_pte->REFERENCED && !curr_pte->MODIFIED && class_0 == nullptr) {
                class_0 = curr_frame;
                if (!RESET_FLAG) {
                    hand = (hand + 1) % num_frames;
                    return curr_frame;
                }
            } else if (!curr_pte->REFERENCED && curr_pte->MODIFIED && class_1 == nullptr) {
                class_1 = curr_frame;
            } else if (curr_pte->REFERENCED && !curr_pte->MODIFIED && class_2 == nullptr) {
                class_2 = curr_frame;
            } else if (curr_pte->REFERENCED && curr_pte->MODIFIED && class_3 == nullptr) {
                class_3 = curr_frame;
            }

            if (RESET_FLAG) {
                curr_pte->REFERENCED = 0;
            }
            hand = (hand + 1) % num_frames;
        }
        if (RESET_FLAG) {
            RESET_FLAG = false;
            last_rest = instruct_cnt;
        }
        if (class_0 != nullptr) {
            hand = class_0->INDEX;
            hand = (hand + 1) % num_frames;
            return class_0;
        } else if (class_1 != nullptr) {
            hand = class_1->INDEX;
            hand = (hand + 1) % num_frames;
            return class_1;
        } else if (class_2 != nullptr) {
            hand = class_2->INDEX;
            hand = (hand + 1) % num_frames;
            return class_2;
        } else {
            hand = class_3->INDEX;
            hand = (hand + 1) % num_frames;
            return class_3;
        }
    }

    virtual void map_update(int frame_id) {return;};
};

class AgingPager : public Pager {
    int num_frames;
    int hand;
public:
    AgingPager(int num_frames) {
        this->num_frames = num_frames;
        this->hand = 0;
    }

    virtual frame_t* select_victim_frame(int instruct_cnt) {
        int lowest = 0xffffffff;
        frame_t *hand_frame;
        int victim_index = hand;
        for (int i = 0; i < num_frames; i++) {
            hand_frame = &frame_table[hand];
            hand_frame->AGE = hand_frame->AGE >> 1;
//            iter->age = iter->age >> 1;
            int pid = hand_frame->PROCESS_ID;
            int vpn = hand_frame->VPAGE;
            if (processes->at(pid)->page_table[vpn].REFERENCED) {
                hand_frame->AGE = (hand_frame->AGE | 0x80000000);
            }
            processes->at(pid)->page_table[vpn].REFERENCED = 0;
            if (hand_frame->AGE < lowest) {
                victim_index = hand;
                lowest = hand_frame->AGE;
            }
            hand = (hand + 1) % num_frames;

        }
        hand = (victim_index + 1) % num_frames;
        return &frame_table[victim_index];
    }

    virtual void map_update(int frame_id) {
        if (frame_id >= num_frames) {
            printf("ERROR");
        } else {
            frame_t *fp = &frame_table[frame_id];
            fp->AGE = 0;
        }
    };
};

class WSPager : public Pager {

    int hand;
    int num_frames;

public:
    WSPager(int num_frames) {
        this->hand = 0;
        this->num_frames = num_frames;
    }

    frame_t* select_victim_frame(int instruct_cnt) {
        int max_age = 0;
        int victim_index = hand;
        int start = hand;
        do {
            frame_t *curr_frame = &frame_table[hand];
            int pid = curr_frame->PROCESS_ID;
            int vpn = curr_frame->VPAGE;
            pte_t *pte = &processes->at(pid)->page_table[vpn];
            if (pte->REFERENCED) {
                pte->REFERENCED = false;
                curr_frame->TIME_LAST_USED = instruct_cnt;
            }
            else {
                int age = instruct_cnt - curr_frame->TIME_LAST_USED;
                if (age >= 50) {
                    victim_index = hand;
                    hand = (victim_index + 1) % num_frames;
                    return &frame_table[victim_index];
                }
                if (age > max_age) {
                    victim_index = hand;
                    max_age = age;
                }
            }
            hand = (hand + 1) % num_frames;
//            cnt++;
        } while (hand != start);
        hand = (victim_index + 1) % num_frames;
        return &frame_table[victim_index];
    }

    virtual void map_update(int frame_id) {return;};
};

class Simulator {
public:
    int num_frames;
    std::string infile_name;
    std::string rfile_name;
    std::deque<int> free_list;

    ifstream inFile;
    ifstream rFile;
    Pager *pager;
    bool Oflag;
    bool Pflag;
    bool Fflag;
    bool Sflag;
    bool xflag;
    bool yflag;
    bool aflag;
    bool fflag;
//    frame_t *frame_table;
    Simulator(int num_frames)
    {

        this->pager = new FIFOPager(num_frames);
        this->num_frames = num_frames;
//        this->processes = new vector<Process *>;
//        this->frame_table = new frame_t[num_frames];
    }

    void init(char pager_type, std::string infile_name, std::string rfile_name) {
//        this->infile_name = infile_name;
//        this->rfile_name = rfile_name;
        switch (pager_type)
        {
            case 'r':
                this->pager = new RandomPager(num_frames, rfile_name);
                break;
            case 'c':
                this->pager = new ClockPager(num_frames);
                break;
            case 'e':
                this->pager = new NRUPager(num_frames);
                break;
            case 'a':
                this->pager = new AgingPager(num_frames);
                break;
            case 'w':
                this->pager = new WSPager(num_frames);
            default:
                break;
        }
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
                processes->push_back(curr_proc);
//                this->processes->push_back(curr_proc);
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
//                std::cout << line << std::endl;
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
                    curr_proc->page_table[j].init();
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
//        printf("--------------Done reading processes--------------\n");
        for (int i = 0; i < num_frames; i++) {
            free_list.push_back(i);
            frame_table[i].VPAGE = -1;
            frame_table[i].PROCESS_ID = -1;
            frame_table[i].INDEX = i;
        }
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

//    frame_t *allocate_frame_from_free_list() {
//
//    }

    frame_t *get_frame(int instruct_cnt) {
        frame_t *frame;
        if (free_list.size() != 0) {
            frame = &frame_table[free_list.front()];
            frame->AGE = 0;
            free_list.pop_front();
            return frame;
        } else {
            frame = pager->select_victim_frame(instruct_cnt);
            frame->AGE = 0;
            return frame;
        }
//        return frame;
    }

    void simulation() {
        char operation;
        int vpage;
        Process *current_process = nullptr;
        int instruct_cnt = 0;
        int ctx_switches = 0, process_exits= 0;
        unsigned long long cost = 0;
        while (get_next_instruction(operation, vpage)) {
            // handle special case of “c” and “e” instruction
            printf("%d: ==> %c %d\n", instruct_cnt, operation, vpage);
            if (operation == 'c') {
//                Process *old_process = current_process;
                current_process = processes->at(vpage);
//                if (current_process->id != old_process->id) ctx_switches++;
                ctx_switches++;
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
                        current_process->pstats.unmaps++;
                        if (del_pte->MODIFIED) {
                            if (del_pte->FILE_MAPPED) {
                                printf(" FOUT\n");
                                current_process->pstats.fouts++;
                            }
//                            } else {
//                                printf(" OUT\n");
//                            }
                        }
                    }
                    del_pte->reset();
                    del_pte->PAGEDOUT = 0;
                }
//                current_process = nullptr;
                instruct_cnt++;
                process_exits++;
                continue;
            }
                // now the real instructions for read and write
            else {
                pte_t *pte = &current_process->page_table[vpage];
                cost += COST_RW;
                if (!pte->NOT_HOLE) {
                    printf(" SEGV\n");
                    instruct_cnt++;
                    current_process->pstats.segv++;
                    continue;
                }
                if (!pte->PRESENT) {
                    // this in reality generates the page fault exception and now you execute
                    // verify this is actually a valid page in a vma if not raise error and next inst
                    frame_t *newframe = get_frame(instruct_cnt);
//                frame_t *newframe = get_frame();
                    //-> figure out if/what to do with old frame if it was mapped
                    int pid = newframe->PROCESS_ID;
                    int vpn = newframe->VPAGE;
                    //Swap out
                    if (pid != -1 || vpn != -1) {
                        printf(" UNMAP %d:%d\n", pid, vpn);
                        Process *victim = processes->at(pid);
                        victim->pstats.unmaps++;
                        if (victim->page_table[vpn].MODIFIED) {

                            if (victim->page_table[vpn].FILE_MAPPED) {
                                printf(" FOUT\n");
                                victim->pstats.fouts++;
                            } else {
                                victim->page_table[vpn].PAGEDOUT = 1;
                                printf(" OUT\n");
                                victim->pstats.outs++;
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
                        current_process->pstats.fins++;
                    } else {
                        if (pte->PAGEDOUT) {
                            printf(" IN\n");
                            current_process->pstats.ins++;
                        } else {
                            printf(" ZERO\n");
                            current_process->pstats.zeros++;
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
//                    pager->map_update(newframe->INDEX);
                    current_process->pstats.maps++;
                }
//            } else {
//                instruct_cnt++;
//                continue;
//            }
                // now the page is definitely present // check write protection
                if (operation == 'w') {
                    if (pte->WRITE_PROTECT) {
                        printf(" SEGPROT\n");
                        current_process->pstats.segprot++;
                    } else {
                        pte->MODIFIED = 1;
                    }
                }
                pte->REFERENCED = 1;


                // simulate instruction execution by hardware by updating the R/M PTE bits
//            update_pte(read/modify) bits based on operations.
                instruct_cnt++;
            }
        }

        if (Pflag) {
            for (auto p: *processes) {
                p->display_pagetable();
            }
        }

        if (Fflag) {
            printf("FT:");
            for (int i = 0; i < num_frames; i++) {
                if (frame_table[i].PROCESS_ID == -1) printf(" *");
                else {
                    printf(" %d:%d", frame_table[i].PROCESS_ID, frame_table[i].VPAGE);
                }
            }
            printf("\n");
        }

        if (Sflag) {
            for (auto p: *processes) {
                p->diplay_stats();
                cost += COST_UNMAPS * p->pstats.unmaps;
                cost += COST_MAPS * p->pstats.maps;
                cost += COST_INS * p->pstats.ins;
                cost += COST_OUTS * p->pstats.outs;
                cost += COST_FINS * p->pstats.fins;
                cost += COST_FOUTS * p->pstats.fouts;
                cost += COST_ZEROS * p->pstats.zeros;
                cost += COST_SEGV * p->pstats.segv;
                cost += COST_SEGPROT * p->pstats.segprot;
            }
            cost += COST_CONTEXT_SWITCH * ctx_switches;
            cost += COST_EXIT * process_exits;
            printf("TOTALCOST %lu %lu %lu %llu %lu\n",
                   instruct_cnt, ctx_switches, process_exits, cost, sizeof(pte_t));
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
//                printf("OPT arg: %s\n", optarg);
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
//                printf("o\n");
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
//            printf("%s\n", argv[index]);
            infile_name = string(argv[index]);
            fileCnt++;
        }
        else if (fileCnt == 1)
        {
//            printf("%s\n", argv[index]);
            rfile_name = string(argv[index]);
//            randFile = convertToString(argv[index]);
            fileCnt++;
        }
    }

    Simulator* s = new Simulator(num_frames);
    s->init(pager_type, infile_name, rfile_name);
    s->Oflag = true;
    s->Pflag = true;
    s->Fflag = true;
    s->Sflag = true;
    s->simulation();
}