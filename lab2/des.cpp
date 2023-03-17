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
        sentinem->next = sentinem;
        sentinem->prev = sentinem;
        size = 0;
    };

    void put(T &data)
    {
        std::string evtQbefore = this->toString();
        printf("\tAddEvent(%d, %d, %s)", data->timestamp, data->proc->id, EventToString(data->transition));
//        std::cout << "AddEvent" << data->toString();

        // data->proc->show();
//        printf("\n");
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
            std::string evtQafter = this->toString();
            std::cout << " " << evtQbefore << " ==> " << evtQafter << std::endl;
            // display();
            return;
        }
        while (curr != sentinem)
        {
            if (data->timestamp < curr->data->timestamp) {
                break;
            }
            curr = curr->next;

        }

        Node *n = new Node(data);
        n->next = curr;
        n->prev = curr->prev;
        curr->prev->next = n;
        curr->prev = n;
        size++;
        std::string evtQafter = this->toString();
        std::cout << " " << evtQbefore << " ==> " << evtQafter << std::endl;
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

    int remove(int procID){
        Node *curr = sentinem->next;
        while (curr != sentinem) {
            if (curr->data->procID == procID) {
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
            cout << "->" << curr->data->toString();
            curr = curr->next;
        }
        cout << endl;
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

    std::string toString() {
        ostringstream os;
        Node *curr = sentinem->next;
        while (curr != sentinem) {
//            os << "[";
            os << curr->data->timestamp;
            os << ":";
            os << curr->data->procID;
            os << " ";
//            os << ":";
//            os << EventToString(curr->data->transition);
//            os << "]";
            curr = curr->next;
        }
        return os.str();
    }
};

#endif

