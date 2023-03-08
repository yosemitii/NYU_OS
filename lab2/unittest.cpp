#include "olist.h"

void olistTest()
{
    // OrderedList<int> o = OrderedList<int>();
    // o.addData(2);
    // o.addData(1);
    // o.addData(5);
    // o.addData(6);
    // o.addData(2);
    // o.display();

    Event *e1 = new Event(0, 10, 4);
    Event *e2 = new Event(20, 50, 3);
    Event *e3 = new Event(5, 20, 2);
    Event *e4 = new Event(10, 20, 1);
    Event *e5 = new Event(5, 31, 2);
    OrderedList<Event *> ol = OrderedList<Event *>();

    ol.put(e1);
    ol.put(e2);
    ol.put(e3);
    ol.put(e4);
    ol.put(e5);
    ol.display();

    ol.get();
    ol.get();
    // ol.display();
    ol.remove(10, 1);
    // ol.display();
    // return true;
}
