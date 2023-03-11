#include "olist.h"
void olistTest()
{

    Event *e1 = new Event(0, 4);
    Event *e2 = new Event(20, 3);
    Event *e3 = new Event(5, 2);
    Event *e4 = new Event(10, 1);
    Event *e5 = new Event(5, 2);
    OrderedList<Event *> ol = OrderedList<Event *>();

    ol.put(e1);
    ol.put(e2);
    ol.put(e3);
    ol.put(e4);
    ol.put(e5);
    ol.display();

    ol.get();
    ol.get();
    ol.display();
    ol.remove(10, 1);
    ol.display();
    
}


void queueTest() {
    //Queue Property test
    std::queue<Process *> *readyq = new std::queue<Process *>();
    Process *p1 = new Process(0, 10, 20, 10, 10, 4);
    Process *p2 = new Process(1, 15, 25, 15, 10, 4);
    Process *p3 = new Process(2, 25, 35, 45, 10, 4);
    readyq->push(p1);
    readyq->push(p2);
    readyq->push(p3);
    Process *t = readyq->front();
    readyq->pop();
    t->show();
    readyq->push(t);
    t = nullptr;
    delete t;
    readyq->pop();
    readyq->pop();
    t = readyq->front();
    t->show();
}
