#include "olist.h"

bool olistTest()
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
    OrderedList<Event *> ol = OrderedList<Event *>();

    // cout << "e1 >= e2? " << (e1 >= e2) << endl;
    // cout << "e1 <= e2? " << (e1 <= e2) << endl;
    // e1->toString();
    // e2->toString();

    ol.addData(e1);
    ol.addData(e2);
    ol.addData(e3);
    ol.addData(e4);
    ol.display();
    return true;
}
