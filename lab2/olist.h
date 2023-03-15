#ifndef ORDEREDLIST
#define ORDEREDLIST
template <class T>
class OrderedList
{
private:
public:
    class Node
    {
    private:
    public:
        T data;
        Node *prev;
        Node *next;
        Node()
        {
            // if (std::is_same<T, Event>::value) {
            //     this->data = Event(-9999, -1, -1);
            // } else if (std::is_same<T, int>::value) {
            //     this->data = -9999;
            // }
            this->prev = nullptr;
            this->next = nullptr;
        }
        explicit Node(T data)
        {
            this->data = data;
            this->prev = nullptr;
            this->next = nullptr;
        }
    };
    //Variable attributes
    Node *sentinem;
    int size;
    //Methods
    OrderedList()
    {
        sentinem = new Node();
        // if (std::is_same<T, Event>::value)
        // {
        //     sentinem = new Node(Event(-9999, -1, -1));
        // }
        // else if (std::is_same<T, int>::value)
        // {
        //     sentinem = new Node((int) -9999);
        // };
        sentinem->next = sentinem;
        sentinem->prev = sentinem;
        size = 0;
    };

    void put(T &data)
    {
        printf("Add Event(Time: %d, ID: %d, State Now: %s, Time Remained: %d)",
               data->proc->timestamp, data->proc->id, ProcToString(data->proc->pState), data->proc->totalTime);
        // data->proc->show();
        printf("\n");
        // cout << "Data: " << data << endl;
        Node *curr = sentinem->next;
        if (size == 0)
        {
            // cout << "curr is sentinem" << endl;
            Node *n = new Node(data);
            sentinem->next = n;
            sentinem->prev = n;
            n->next = sentinem;
            n->prev = sentinem;
            size++;
            // display();
            return;
        }
        // if (std::is_same<T, int>::value)
        // {
        //     while (curr != sentinem && data >= curr->data)
        //     {
        //         cout << "curr->event: " << curr->data << endl;
        //         curr = curr->next;
        //     }
        // }
        while (curr != sentinem)
        {
            
            // cout << "data->startTime: " << data->startTime << endl;
            // cout << "curr->data->startTime: " << curr->data->startTime << endl;
            // cout << "data >= curr->data" << (data->startTime >= curr->data->startTime) << endl;
            // cout << "curr->event: " << curr->data.toString() << endl;
            // curr->data->toString();
            // cout << endl;
            if (data->timestamp < curr->data->timestamp) {
                break;
            }
            curr = curr->next;
            // cout << "Curr: " << curr->data->startTime << endl;
            // curr->data->toString();
        }
        
        // if (std::is_same<T, Event>::value)
        // {
        //     while (curr != sentinem && data >= curr->data)
        //     {
        //         // cout << "curr->event: " << curr->data.toString() << endl;
        //         // curr->data.toString();
        //         curr = curr->next;
        //     }
        // }

        Node *n = new Node(data);
        // n->prev = curr;
        // n->next = curr->next;
        // curr->next->prev = n;
        // curr->next = n;

        n->next = curr;
        n->prev = curr->prev;
        curr->prev->next = n;
        curr->prev = n;
        size++;
        // display();
        return;
    }

    T get(){
        Node *res = sentinem->next;
        sentinem->next = res->next;
        sentinem->next->prev = sentinem;
        size--;
        // T data = res->data;
        return res->data;
    }

    int remove(int time, int procID){
        Node *curr = sentinem->next;
        while (curr != sentinem) {
            if (curr->data->timestamp == time && curr->data->procID == procID) {
                break;
            }
            curr = curr->next;
        }
        if (curr == sentinem) {
            return -1;
        }
        Node *prev = curr->prev;
        Node *next = curr->next;
        prev->next = next;
        next->prev = prev;
        curr->~Node();
        size--;
        return 1;
    }

    void display()
    {
        Node *curr = sentinem->next;
        cout << "EventQ:";
        while (curr != sentinem)
        {
            // if (std::is_same<T, int>::value)
            // {
            //     cout << curr->data << "->";
            // }
            // else if (std::is_same<T, Event>::value)
            // {
            cout << "->";
            curr->data->toString();
            
            // cout << curr->data.startTime << "->";
            // }
            curr = curr->next;
        }
        cout << endl;
        // curr = sentinem->next;
        // while (curr != sentinem)
        // {
        //     // if (std::is_same<T, int>::value)
        //     // {
        //     //     cout << curr->data << "<-";
        //     // }
        //     // else if (std::is_same<T, Event>::value)
        //     // {
        //     // cout << curr->data.startTime << "<-";
        //     // }
        //     curr->data->toString();
        //     cout << "<-";
        //     curr = curr->next;
        // }
        // cout << "sentinem" << endl;
        return;
    }

    int getNextEventTime() {
        if (size > 0) {
            Node *head = sentinem->next;
            return head->data->timestamp;
        }
        else
        {
            std::cout << "Event queue is empty" << endl;
        }
    }
};

// void addData(T *data)
// {
//     tlist.push_back(data);
// }

// OrderedList()
// {
//     Node *sentinem = new Node();
// }

// T getData()
// {
//     return *tlist.begin();
// }

// void addData(T *data)
// {
//     tlist.push_back(data);
// }

#endif

