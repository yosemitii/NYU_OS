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

    Node *sentinem;
    int size;

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

    void addData(T data)
    {
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
        if (std::is_same<T, int>::value)
        {
            while (curr != sentinem && data >= curr->data)
            {
                // cout << "curr->data: " << curr->data << endl;
                curr = curr->next;
            }
        }

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

    void display()
    {
        Node *curr = sentinem->next;
        while (curr != sentinem)
        {
            if (std::is_same<T, int>::value)
            {
                cout << curr->data << "->";
            }
            else if (std::is_same<T, Event>::value)
            {
                cout << curr->data->startTime << "->";
            }
            curr = curr->next;
        }
        cout << endl;
        curr = sentinem->next;
        while (curr != sentinem)
        {
            if (std::is_same<T, int>::value)
            {
                cout << curr->data << "<-";
            }
            else if (std::is_same<T, Event>::value)
            {
                cout << curr->data->startTime << "<-";
            }
            curr = curr->next;
        }
        cout << endl;
        return;
    }
};
// OrderedList()
// {
//     Node *sentinem = new Node(-999);
// }

// T getData()
// {
//     return *tlist.begin();
// }

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