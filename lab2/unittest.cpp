#include "olist.h"


bool olistTest() {
    OrderedList<int> o = OrderedList<int>();
    o.addData(2);
    o.addData(1);
    o.addData(5);
    o.addData(6);
    o.addData(2);
    // o.display();

    Event e1 = Event(0, 10, 0);
    Event e2 = Event(20, 50, 1);
    Event e3 = Event(5, 20, 2);
    Event e4 = Event(10, 20, 3);
    OrderedList<Event> ol = OrderedList<Event>();
    ol.addData(e1);
    ol.addData(e2);
    ol.addData(e3);
    ol.addData(e4);
    return true;
}

