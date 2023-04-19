//
// Created by Lubin Sun on 4/18/23.
//
#include <stdio.h>
#include <string>
#include <deque>
#include <unistd.h>
#include <fstream>

using namespace std;
#define NUMBER_OF_PTE 64
#define MAX_FRAMES 128
#define MAX_VPAGES 64


typedef struct {
    unsigned int PRESENT:1;
    unsigned int REFERENCED:1;
    unsigned int MODIFIED:1;
    unsigned int WRITE_PROTECT:1;
    unsigned int PAGEDOUT:1;
    unsigned int FREE:20;
    unsigned int FRAME_TABLE:7;
} pte_t;

typedef struct {
//    unsigned int FRAME_TABLE:7;
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
    int id;
};

class Simulator {
    int num_frames = 0;
    PagerType pager_type;
    std::string infile_name;
    std::string rfile_name;
    std::deque<op_pair> operations;

    Simulator(int num_frames, PagerType p_type, std::string infile_name, std::string rfile_name)
    {
        this->num_frames = num_frames;
        this->pager_type = p_type;
        this->infile_name = infile_name;
        this->rfile_name = rfile_name;
    }

    void init() {
        ifstream inFile;
        ifstream rFile;
        inFile.open(infile_name);
        std::string line;
        int vma_num;
        char op;
        int readType = 0;
        int num_process = 0;
        const char *delim = "\t\n ";
        while (!inFile.eof())
        {
            getline(inFile, line);
            if (line[0] == '#') continue;
            else if (readType == 0)
            {
                //Type 0: Number of process
                num_process = stoi(line);
            }
            else if (readType == 1)
            {
                //Type 1: Number of VMAs
            }
            else if (readType == 2)
            {
                //Type 2: Detail of each VMA
            }
            else
            {
                //Operations:
            }
        }

    }
};

int main (int argc, char** argv) {
    int c;
    int num_frames;
    char pager_type;
    std::string infile_name;
    std::string rfile_name;
    while ((c = getopt(argc, argv, "f:a:oOPFS")) != -1) {
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
//                printf("OPT arg: %s\n", optarg);
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
//            inputFile = convertToString(argv[index]);
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

    Simulator s(num_frames, pager_type, infile_name, rfile_name);
//    printf("num_frames: %d\n", num_frames);
//    printf("pager_type: %c\n", pager_type);
}