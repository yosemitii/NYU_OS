#include <iostream>
#include <fstream>
#include <sstream>
#include <getopt.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <sys/stat.h>

using namespace std;


class Request {
public:
    int iop;
    int arr_time;
    int start_time;
    int end_time;

    Request(int iop, int at) {
        this->iop = iop;
        this->arr_time = at;
        this->start_time = INT_MAX;
        this->end_time = INT_MAX;
    }

    void toString() {
        printf("%5d: %5d %5d %5d\n", iop, arr_time, start_time, end_time);
    }

};
class IOScheduler {
public:
    int head;
    IOScheduler(int h) {
        this->head = h;
    }
    virtual void schedule(std::vector<std::pair<int, int>>& requests) = 0;
};

class FIFO : public IOScheduler {

public:
    using IOScheduler::IOScheduler;
//    FIFO(int headTrack) : IOScheduler(int headTrack) {
//        this->head = headTrack;
//    }

    struct pairCompare{
        bool operator() (pair<int, int> &p1, pair<int, int> &p2) {
            return p1.first > p2.first;
        }
    };

    void schedule(std::vector<std::pair<int, int>>& requests) {
        std::sort(requests.begin(), requests.end(), pairCompare());
    }
};

class SSTF : public IOScheduler {
public:
    using IOScheduler::IOScheduler;
    struct pairCompare{
        int headTrack;
        pairCompare(int h) {
            headTrack = h;
        }
        bool operator() (const pair<int, int> &p1, const pair<int, int> &p2) {
//            printf("Compare %d and %d\n", abs(p1.second-headTrack), abs(p2.first-headTrack));
            return abs(p1.second-headTrack) < abs(p2.second-headTrack);
        }
    };

    void schedule(std::vector<std::pair<int, int>>& requests) {
        std::sort(requests.begin(), requests.end(), pairCompare(head));
    }
};

class Simulator {
public:
    ifstream inFile;
    int headTrack;
    int time;
    IOScheduler *IOSched;
//    std::map<int, int> requests;
    std::vector<std::pair<int, int>> requests;
    Simulator() {
        this->headTrack = 0;
        this->time = 0;
    }

    void init(char algoType) {
        switch (algoType){
            case 'N':
                printf("FIFO");
                IOSched = new FIFO(headTrack);
                break;
            case 'S':
                IOSched = new SSTF(headTrack);
                printf("SSTF");
                break;
            case 'L':
                printf("LOOK");
                break;
            case 'C':
                printf("CLOOK");
                break;
            case 'F':
                printf("FLOOK");
                break;
            default:
                IOSched = new FIFO(headTrack);
                break;
        }
    }

    void read(std::string inputfileName) {
        cout << "File name:" << inputfileName << endl;
        //Test stat to check if file exists
//        struct stat buffer;
//        if (stat(inputfileName.c_str(), &buffer) != 0) {
//            printf("File does not exist. Exit.");
//            abort();
//        }
        inFile.open(inputfileName);
        if (!inFile) {
            printf("File does not exist. Exit.");
            abort();
        }
        std::string line;
        int requestTime;
        int requestTrack;
        while (!inFile.eof()) {
            getline(inFile, line);
            if (line[0] == '#' || line.length() == 0) continue;
            istringstream linestream(line);
            linestream>>requestTime;
            linestream>>requestTrack;
            requests.emplace_back(requestTime,requestTrack);
        }

        for (auto p : requests) {
            printf("%d:%d\n", p.first, p.second);
        }
        inFile.close();
    }

    void simulate(){
//        IOSched = new SSTF(200);
//        printf("HEAD:%d", IOSched->head);
//        IOSched->schedule(requests);
//        printf("===========");
//        for (auto p : requests) {
//            printf("%d:%d\n", p.first, p.second);
//        }
        while(!requests.empty()) {
            int nextTrack = requests.front().second;
            printf
            time++;
        }
    }
};


int main(int argc, char** argv) {
    int c;
    bool vflag = false;
    bool qflag = false;
    bool fflag = false;
    char algo_type;
    std::string infile_name;
    while ((c = getopt(argc, argv, "s:vqf")) != -1) {
        switch(c) {
            case 's':
                printf("OPT ARG: %c\n", *optarg);
                algo_type = *optarg;
                if (algo_type != 'N' && algo_type != 'S' && algo_type != 'L' && algo_type != 'C' && algo_type != 'F') abort();
                break;
            case 'v':
                vflag = true;
                break;
            case 'q':
                qflag = true;
                break;
            case 'f':
                fflag = true;
                break;
            case '?':
                printf("%s", c);
            default:
                printf("Abort from switching");
                abort();
        }
    }
    if (argv[optind] == nullptr) {
        printf("Wrong Iput\n");
        abort();
    }
    infile_name = string(argv[optind]);
    Simulator* s = new Simulator();
    s->init(algo_type);
    s->read(infile_name);
    s->simulate();
    return 0;
}
