#include <iostream>
#include <fstream>
#include <sstream>
#include <getopt.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <sys/stat.h>
#include <algorithm>
#include <climits>
#include <deque>

using namespace std;

bool DEBUG_MODE = false;
class Request {
public:
    int iop;
    int arr_time;
    int track;
    int start_time;
    int end_time;
    int disk_start_move_time;

    Request(int iop, int at, int trk) {
        this->iop = iop;
        this->arr_time = at;
        this->track = trk;
        this->start_time = INT_MAX;
        this->end_time = INT_MAX;
        this->disk_start_move_time = INT_MAX;
    }

    void toString() {
        if (DEBUG_MODE) printf("%5d: %5d %5d %5d\n", iop, arr_time, start_time, end_time);
    }

};
class IOScheduler {
public:
    int head;
    int target;
    std::deque<Request*> *waitingQ;
    std::deque<Request*> *activeQ;

    IOScheduler(int h, std::deque<Request*> *requests) {
        this->head = h;
        this->target = -1;
        this->waitingQ = requests;
        this->activeQ = new std::deque<Request*>();
    }

    IOScheduler() {}

    virtual void schedule() = 0;

    virtual Request* getNextRequest() {
        if (activeQ->empty()) {
            return nullptr;
        } else {
            schedule();
            Request* res = activeQ->front();
            activeQ->pop_front();
            return res;
        }
    };

    virtual void timeUpdate(int time, int head) {
        this->head = head;
        if (waitingQ->empty()) return;
        if (time >= waitingQ->front()->arr_time) {
            if (DEBUG_MODE) printf("LOG: [%d] Adding Reuqest #%d to activeQ\n", time, waitingQ->front()->iop);
            activeQ->push_back(waitingQ->front());
            waitingQ->pop_front();
        }
    }

//    virtual void finishRequest() {
//        schedule();
////        Request* req = activeQ->front();
////        activeQ->pop_front();
////
////        return req;
//    }

    virtual bool queueEmpty() = 0;

};

class FIFO : public IOScheduler {

public:
    using IOScheduler::IOScheduler;

    struct pairCompare{
        bool operator() (const Request *r1, const Request *r2) {
            return r1->arr_time < r2->arr_time;
        }
    };

    void schedule() {
        std::sort(activeQ->begin(), activeQ->end(), pairCompare());
        if (DEBUG_MODE) {
            printf("LOG: ActiveQ(:");
            for (auto p: *activeQ) {
                printf("%d: %d %d ", p->iop, p->arr_time, p->track);
            }
            printf(")\n");
        }
    }

    bool queueEmpty() {
        return activeQ->empty() && waitingQ->empty();
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
        bool operator() (const Request *r1, const Request *r2) {
            return abs(r1->track - headTrack) < abs(r2->track - headTrack);
        }
    };

    void schedule() {
        std::sort(activeQ->begin(), activeQ->end(), pairCompare(head));
    }

    bool queueEmpty() {
        return activeQ->empty() && waitingQ->empty();
    }
};

class LOOK : public IOScheduler {
public:
    std::deque<Request*> *queue_0;
    std::deque<Request*> *queue_1;
    int direction;
    int target;

    LOOK(int h, std::deque<Request *> *requests) {
        head = h;
        waitingQ = requests;
        queue_0 = new std::deque<Request*>();
        queue_1 = new std::deque<Request*>();
        activeQ = queue_0;
        direction = 1;
        target = -1;
    }

    virtual void timeUpdate(int time, int head) {
        this->head = head;
        if (waitingQ->empty()) return;
        if (time >= waitingQ->front()->arr_time) {
            if (DEBUG_MODE) printf("LOG: [%d] Adding Reuqest #%d to %s\n", time, waitingQ->front()->iop, activeQ == queue_0 ? "queue_0" : "queue_1");
            if (target == -1) {
                if (direction == 1) {
                    if (waitingQ->front()->track >= head) {
                        queue_0->push_back(waitingQ->front());
                    } else {
                        queue_1->push_back(waitingQ->front());
                    }
                } else {
                    if (waitingQ->front()->track <= head) {
                        queue_1->push_back(waitingQ->front());
                    } else {
                        queue_0->push_back(waitingQ->front());
                    }
                }
                waitingQ->pop_front();
            } else {
                if (direction == 1) {
                    if (waitingQ->front()->track >= target) {
                        queue_0->push_back(waitingQ->front());
                    } else {
                        queue_1->push_back(waitingQ->front());
                    }
                } else {
                    if (waitingQ->front()->track <= target) {
                        queue_1->push_back(waitingQ->front());
                    } else {
                        queue_0->push_back(waitingQ->front());
                    }
                }
                waitingQ->pop_front();
            }
            if (DEBUG_MODE) {
                printf("UpQ = [");
                for (auto r: *queue_0) {
                    printf("%d: %d ", r->iop, r->track);
                }
                printf("] ");
                printf("DownQ = [");
                for (auto r: *queue_1) {
                    printf("%d: %d ", r->iop, r->track);
                }
                printf("]\n");
            }
        }
    }

    struct pairCompare{
        int headTrack;
        pairCompare(int h) {
            headTrack = h;
        }
        bool operator() (const Request *r1, const Request *r2) {
            return abs(r1->track - headTrack) < abs(r2->track - headTrack);
        }
    };

    virtual void schedule() {
        std::sort(activeQ->begin(), activeQ->end(), pairCompare(head));
        if (DEBUG_MODE) {
            printf("LOG: ActiveQ: (");
            for (auto p: *activeQ) {
                printf("%d: %d %d ", p->iop, p->arr_time, p->track);
            }
            printf(")\n");
        }
    }

    virtual bool queueEmpty() {
        return queue_0->empty() && queue_1->empty() && waitingQ->empty();
    }

    virtual Request* getNextRequest() {
        if (!activeQ->empty()) {
            schedule();
            Request *res = activeQ->front();
            activeQ->pop_front();
            target = res->track;
            return res;
        } else {
            if (queue_0->empty() && queue_1->empty()) return nullptr;
            else {
                if (DEBUG_MODE) printf("Change queues and direction\n");
                if (activeQ == queue_0) activeQ = queue_1;
                else activeQ = queue_0;
                direction = -1 * direction;
                target = -1;
                schedule();
                Request *res = activeQ->front();
                activeQ->pop_front();

                if (DEBUG_MODE) printf("dir = %d\n", direction);
                target = res->track;
                return res;
            }
        }
    }

};

class FLOOK : public LOOK {

    using LOOK::LOOK;

    virtual void timeUpdate (int time, int head) override{
        this->head = head;
        if (waitingQ->empty()) return;
        if (time >= waitingQ->front()->arr_time) {
            if (DEBUG_MODE) printf("LOG: [%d] Adding Reuqest #%d to %s\n", time, waitingQ->front()->iop, activeQ == queue_0 ? "queue_0" : "queue_1");
            if (target == -1) {
                activeQ->push_back(waitingQ->front());
            } else {
                if (activeQ == queue_0) {
                    queue_1->push_back(waitingQ->front());
                } else {
                    queue_0->push_back(waitingQ->front());
                }
            }
            waitingQ->pop_front();
            if (DEBUG_MODE) {
                if (activeQ == queue_0) {

                    printf("Q[0] = [");
                    for (auto r: *queue_0) {
                        printf("%d: %d ", r->iop, r->track);
                    }
                    printf("] ");
                    printf("Q[1] = [");
                    for (auto r: *queue_1) {
                        printf("%d: %d ", r->iop, r->track);
                    }
                    printf("]\n");
                } else {
                    printf("Q[1] = [");
                    for (auto r: *queue_1) {
                        printf("%d: %d ", r->iop, r->track);
                    }
                    printf("] ");
                    printf("Q[0] = [");
                    for (auto r: *queue_0) {
                        printf("%d: %d ", r->iop, r->track);
                    }
                    printf("]\n");
                }
            }
        }
    }

    virtual Request* getNextRequest() {
        if (queue_0->empty() && queue_1->empty()) return nullptr;
        if (activeQ->empty()){
            if (DEBUG_MODE) printf("LOG: change queues\n");
            if (activeQ == queue_0) activeQ = queue_1;
            else activeQ = queue_0;
        }
        int distance = INT_MAX;
        int reDistance = INT_MAX;
        Request *res = nullptr;
        Request *revRes = nullptr;
        Request *requestPtr;
        auto tomRemove = activeQ->begin();
        auto tomRemoveRev = activeQ->begin();
        if (DEBUG_MODE) printf("ActiveQ: (");
        for (auto i = activeQ->begin(); i < activeQ->end(); i++){
            requestPtr = *i;
            int directionalDis = (requestPtr->track - head) * direction;
            if (DEBUG_MODE) printf("%d:%d %d ", requestPtr->iop, requestPtr->track, directionalDis);
            if (directionalDis >= 0 && directionalDis < distance) {
                distance = directionalDis;
                res = requestPtr;
                tomRemove = i;
            }
            int reDirectionalDis = (-1) * directionalDis;
            if (reDirectionalDis > 0 && reDirectionalDis < reDistance) {
                reDistance = reDirectionalDis;
                revRes = requestPtr;
                tomRemoveRev = i;
            }
        }
        if (DEBUG_MODE) printf(")\n");
        // There is no positive directionalDis
        if (res == nullptr) {
            direction = (-1) * direction;
            if (DEBUG_MODE) printf("Change direction. dir = %d\n", direction);
            activeQ->erase(tomRemoveRev);
            target = revRes->track;
            return revRes;
        } else {
            activeQ->erase(tomRemove);
            target = res->track;
            return res;
        }

    }
};

class CLOOK: public IOScheduler {
public:
    int direction;
    CLOOK(int h, deque<Request*> *requests) {
        this->activeQ = new deque<Request*>();
        this->head = h;
        this->target = -1;
        this->waitingQ = requests;
        this->activeQ = new std::deque<Request*>();
        this->direction = 1;
    };

    void schedule() {
        return;
    }

    bool queueEmpty() {
        return waitingQ->empty() && activeQ->empty();
    }

    virtual Request* getNextRequest() {
        if (activeQ->empty()) {
            return nullptr;
        }
        int distance = INT_MAX;
        int reDistance = 0;
        Request *res = nullptr;
        Request *revRes = nullptr;
        Request *requestPtr;
        auto tomRemove = activeQ->begin();
        auto tomRemoveRev = activeQ->begin();
        if (direction == -1) {
            if (DEBUG_MODE) printf("Change direction\n");
            direction = 1;
        }
        if (DEBUG_MODE) printf("ActiveQ: (");
        for (auto i = activeQ->begin(); i < activeQ->end(); i++) {
            requestPtr = *i;
            int directionalDis = requestPtr->track - head;
            if (DEBUG_MODE) printf("%d:%d %d ", requestPtr->iop, requestPtr->track, directionalDis);
            if (directionalDis >= 0 && directionalDis < distance) {
                distance = directionalDis;
                res = requestPtr;
                tomRemove = i;
            }
            if (directionalDis < 0 && directionalDis < reDistance) {
                reDistance = directionalDis;
                revRes = requestPtr;
                tomRemoveRev = i;
            }
        }
        if (DEBUG_MODE) printf(")\n");

        if (res != nullptr) {
            activeQ->erase(tomRemove);
            target = res->track;
            return res;
        } else {
            direction = -1;
            if (DEBUG_MODE) printf("Go to bottom. dir=%d", direction);
            activeQ->erase(tomRemoveRev);
            target = revRes->track;
            return revRes;
        }

    }
};


class Simulator {
public:
    ifstream inFile;
    int headTrack;
    int time;
    int io_idle_time;
    IOScheduler *IOSched;
    std::deque<Request*> *requests;
    std::deque<Request*> *finishedRequests;
    Simulator() {
        this->headTrack = 0;
        this->time = 0;
        this->requests = new std::deque<Request*>();
        this->finishedRequests = new std::deque<Request*>();
        this->io_idle_time = 0;
    }

    void init(char algoType) {
        switch (algoType){
            case 'N':
                IOSched = new FIFO(headTrack, requests);
                break;
            case 'S':
                IOSched = new SSTF(headTrack, requests);
                break;
            case 'L':
                IOSched = new LOOK(headTrack, requests);
                break;
            case 'C':
                IOSched = new CLOOK(headTrack, requests);
                break;
            case 'F':
                IOSched = new FLOOK(headTrack, requests);
                break;
            default:
                IOSched = new FIFO(headTrack, requests);
                break;
        }
    }

    void read(std::string inputfileName) {
        if (DEBUG_MODE) cout << "File name:" << inputfileName << endl;
        inFile.open(inputfileName);
        if (!inFile) {
            printf("File does not exist. Exit.");
            abort();
        }
        std::string line;
        int requestTime;
        int requestTrack;
        int id = 0;
        while (!inFile.eof()) {
            getline(inFile, line);
            if (line[0] == '#' || line.length() == 0) continue;
            istringstream linestream(line);
            linestream>>requestTime;
            linestream>>requestTrack;
            this->requests->emplace_back(new Request(id, requestTime, requestTrack));
//            this->finishedRequests->emplace_back(new Request(id, requestTime, requestTrack));
            id++;
        }

//        for (auto p : *requests) {
//            printf("%d: %d %d\n", p->iop, p->arr_time, p->track);
//        }
        inFile.close();
        if (DEBUG_MODE) printf("Done file reading\n");
    }

    void simulate(){
        for (auto p : *requests) {
            if (DEBUG_MODE) printf("%d: %d %d\n", p->iop, p->arr_time, p->track);
        }
        Request* currReq = nullptr;
        while (!IOSched->queueEmpty() || currReq != nullptr)
        {
            IOSched->timeUpdate(time, headTrack);
            if (currReq == nullptr) {
                if (DEBUG_MODE) printf("LOG: [%d]: Getting new task\n", time);
                currReq = IOSched->getNextRequest();
                if (currReq == nullptr) {
                    if (DEBUG_MODE) printf("LOG: [%d]: Stay pending\n", time);
                    io_idle_time++;
                    time++;
                    continue;
                } else {
                    currReq->start_time = time;
                    if (DEBUG_MODE) printf("LOG: [%d]: New task = Req#%d. Target track = %d\n", time, currReq->iop, currReq->track);
                }
            }
            // Request is not active

            if (headTrack < currReq->track) {
                if (DEBUG_MODE) printf("LOG: [%d]: %d -> %d\n", time, headTrack, headTrack+1);
                headTrack++;
                time++;
            } else if (headTrack > currReq->track) {
                if (DEBUG_MODE) printf("LOG: [%d]: %d -> %d\n", time, headTrack, headTrack-1);
                headTrack--;
                time++;
            } else {
                if (DEBUG_MODE) printf("LOG: [%d]: Finished task of Request #%d\n", time, currReq->iop);
                currReq->end_time = time;
                finishedRequests->push_back(currReq);
                IOSched->schedule();
                currReq = nullptr;
            }
        }
        statisticResult();
    }

    void statisticResult() {
        struct RequestIopCompare {
            bool operator() (const Request *r1, const Request *r2) {
                return r1->iop < r2->iop;
            }
        };
        std::sort(finishedRequests->begin(), finishedRequests->end(), RequestIopCompare());
        int total_time = 0;
        int total_movement = 0;
        int total_trunaround = 0;
        int total_waittime = 0;
        int max_waittime = 0;
        for (auto r: *finishedRequests) {
            total_time = max(r->end_time, total_time);
            total_movement += r->end_time - r->start_time;
            total_trunaround += r->end_time - r->arr_time;
            int wait_time = r->start_time - r->arr_time;
            total_waittime += wait_time;
            max_waittime = max(wait_time, max_waittime);
            printf("%5d: %5d %5d %5d\n", r->iop, r->arr_time, r->start_time, r->end_time);
        }
        double io_utilization = static_cast<double>(total_time - io_idle_time) / static_cast<double> (total_time);
        double avg_turnaround = static_cast<double>(total_trunaround) / static_cast<double>(finishedRequests->size());
        double avg_waittime = static_cast<double>(total_waittime) / static_cast<double>(finishedRequests->size());


        printf("SUM: %d %d %.4lf %.2lf %.2lf %d\n",
               total_time, total_movement, io_utilization, avg_turnaround, avg_waittime, max_waittime);
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
                abort();
        }
    }
    if (argv[optind] == nullptr) {
        printf("Wrong Input\n");
        abort();
    }
    infile_name = string(argv[optind]);
    Simulator* s = new Simulator();
    s->init(algo_type);
    s->read(infile_name);
    s->simulate();
    return 0;
}
