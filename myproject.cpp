#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

int Scheduler_Simulator(int scheduling_method, int mode);
int Scheduler = 0;
int Mode = 0;

template <class T>
class Queue{
    T* queue;
    int* priority;
    int size;
    int front, rear;

    void resize(){
        T* tempQ = new T[size*2];
        int* tempP = new int[size*2];
        for(int i=0; i<size; i++) {
            tempQ[i] = queue[i];
            tempP[i] = priority[i];
        }
        delete[] priority;
        delete[] queue;
        priority = tempP;
        queue = tempQ;
        size = size*2;
    }
public:
    Queue(int s=50)
    {
        this->size = s;
        queue = new T[size];
        priority = new int[size];
        front = 0;
        rear = 0;
    }

    bool isFull()
    {
        return (rear == size - 1);
    }

    bool isEmpty()
    {
        return (front == 0 && rear == 0);
    }

    void enqueue( T element, int p )
    {
        if( isFull() )
            resize();

        int i;
        for(i=front; i != rear && p >= priority[i]; i++ );

        for(int j = rear; j > i; j--) {
            queue[j] = queue[j - 1];
            priority[j] = priority[j-1];
        }

        queue[i] = element;
        priority[i] = p;
        rear++;
    }

    void addWaitTime(){
        int i;
        for(i=front; i<rear; i++) {
            queue[i]->incrementWaiting();
        }
    }

    void printQ(){
        for(int i = front; i < rear; i++)
            cout << *queue[i];
        system("pause");
    }

    T dequeue()
    {
        if( !isEmpty() )
        {
            T e = queue[front];
            front++;
            if( front == rear )
            {
                front = 0;
                rear = 0;
            }
            return e;
        }
        return NULL;
    }

    T See_First()
    {
        if( !isEmpty() )
        {
            T e = queue[front];
            return e;
        }
        return nullptr;
    }
    ~Queue(){
        delete[] priority;
        delete[] queue;
    }
};

class Process{
    int burst_time;
    int arrival_time;
    int priority;
    int waitingTime;

public:
    Process() {
        this->burst_time=0;
        this->arrival_time=0;
        this->priority=0;
        this->waitingTime = 0;
    }

    void setBurstTime(int burstTime) {
        burst_time = burstTime;
    }

    void setArrivalTime(int arrivalTime) {
        arrival_time = arrivalTime;
    }

    void setPriority(int p) {
        this->priority = p;
    }

    int getBurstTime() const {
        return burst_time;
    }

    int getArrivalTime() const {
        return arrival_time;
    }

    int getPriority() const {
        return priority;
    }

    void resetWatingTime(){
        waitingTime = 0;
    }

    int getWaitingTime() const{
        return waitingTime;
    }

    void incrementWaiting(){
        this->waitingTime++;
    }

    friend ostream& operator << (ostream& out, Process& p){
        out << p.burst_time << ',' << p.arrival_time << ',' << p.priority << " W: " << p.waitingTime <<  endl;
        return out;
    }
};

class CPU_Sheduler{
    Process **processes;
    int process_no;

public:
    explicit CPU_Sheduler(int size){
        processes = new Process*[size];
        process_no = 0;
    }

    void Add_Process(int b, int a, int p){
        Process *process;
        process = new Process;
        process->setBurstTime(b);
        process->setArrivalTime(a);
        process->setPriority(p);
        processes[process_no++] = process;
    }

    static bool Compare(Process p1, Process p2, int mode){
        if(mode == 1) {
            if (p1.getBurstTime() > p2.getBurstTime())
                return true;
        }

        else if(mode == 2){
            if (p1.getArrivalTime() > p2.getArrivalTime())
                return true;
        }
        return false;
    }

    static void insertionSort(Process **arr, int n, int mode)
    {
        int i, j;
        Process key;
        for (i = 1; i < n; i++){
            key = *arr[i];
            j = i - 1;

            // Move elements of arr[0..i-1],
            // that are greater than key, to one
            // position ahead of their
            // current position
            while (j >= 0 && Compare(*arr[j],key,mode)){
                //mode will tell what data member is to be comapred
                //in case of mode = 1, compare burst time
                //in case of mode = 2, compare arrival time

                *arr[j + 1] = *arr[j];
                j = j - 1;
            }
            *arr[j + 1] = key;
        }
    }

    int getSumBurstTime(){
        int sum = 0, i;
        for(i=0; i<process_no; i++){
            sum += processes[i]->getBurstTime();
        }
        return sum;
    }

    int getMaxArrivalTime(){
        int max = 0, i;
        for(i=0; i<process_no; i++){
            if(max < processes[i]->getArrivalTime())
                max = processes[i]->getArrivalTime();
        }
        return max;
    }

    int getArivingProcesses(int arr){
        int i;
        for(i=0; i<process_no && processes[i]->getArrivalTime() <= arr; i++);
        return i-1;
    }

    void resetWaitingTime(){
        int i;
        for(i=0; i<process_no; i++)
            processes[i]->resetWatingTime();
    }

    void FCFS(ostream& out){
        Queue<Process*> q;
        Process* p = nullptr;
        int i, maxCounter, index, j, burst = 0, start = 0;

        resetWaitingTime();
        insertionSort(processes,process_no,2);
        maxCounter = getMaxArrivalTime() + getSumBurstTime();

        for(i=0; i<maxCounter; i++){
            index = getArivingProcesses(i);

            if(index >= start) {
                for (j = start; j <= index; j++)
                    q.enqueue(processes[j], processes[j]->getArrivalTime());
                start = index + 1;
            }

            if(p == nullptr || p->getBurstTime() == burst) {
                p = q.dequeue();
                burst = 0;
            }
            q.addWaitTime();
            burst++;
        }
        Print_FCFS_waiting_time(out);
    }

    void Print_FCFS_waiting_time(ostream& out){
        int sum=0, wait;
        out << "Scheduling Method: First Come First Served\n";
        out << "Process Waiting Times:\n";
        for(int i=0;i<process_no;i++){
            wait = processes[i]->getWaitingTime();
            out << "P" << i+1 << ": " << wait << " ms\n";
            sum = sum+wait;
        }
        out << "Average Waiting Time: " << sum/(process_no*1.0) << " ms\n";
    }

    void SJF(bool isPreemptive, ostream& out){
        Queue<Process*> q;
        Process* p = nullptr;
        int i, maxCounter, index, j, start = 0;
        int burstBackup[process_no];

        for(i=0; i<process_no; i++)
            burstBackup[i] = processes[i]->getBurstTime();

        resetWaitingTime();
        insertionSort(processes,process_no,2);
        maxCounter = getMaxArrivalTime() + getSumBurstTime();

        for(i=0; i<maxCounter; i++){
            index = getArivingProcesses(i);

            if(p != nullptr){
                p->setBurstTime(p->getBurstTime()-1);
                if(isPreemptive  && p->getBurstTime() != 0) {
                    q.enqueue(p, p->getBurstTime());
                }
            }

            if(index >= start) {
                for (j = start; j <= index; j++)
                    q.enqueue(processes[j], processes[j]->getBurstTime());
                start = index + 1;
            }

            if(p == nullptr || p->getBurstTime() == 0 || isPreemptive) {
                p = q.dequeue();
            }

            q.addWaitTime();
        }

        for(i=0; i<process_no; i++)
            processes[i]->setBurstTime(burstBackup[i]);

        Print_SJF_waiting_time(isPreemptive, out);
    }

    void Print_SJF_waiting_time(bool isPreemptive, ostream& out){
        int sum=0, wait;
        out << "Scheduling Method: Shortest Job First ";
        if(isPreemptive)
            out << "- Preemptive\n";
        else
            out << "- Non-Preemptive\n";
        out << "Process Waiting Times:\n";
        for(int i=0;i<process_no;i++){
            wait = processes[i]->getWaitingTime();
            out << "P" << i+1 << ": " << wait << " ms\n";
            sum = sum+wait;
        }
        out << "Average Waiting Time: " << sum/(process_no*1.0) << " ms\n";
    }

    void PS(bool isPreemptive, ostream& out){
        Queue<Process*> q;
        Process* p = nullptr, *p1;
        int i, maxCounter, index, j, start = 0;
        int burstBackup[process_no];

        for(i=0; i<process_no; i++)
            burstBackup[i] = processes[i]->getBurstTime();

        resetWaitingTime();
        insertionSort(processes,process_no,2);
        maxCounter = getMaxArrivalTime() + getSumBurstTime();

        for(i=0; i<maxCounter; i++){
            index = getArivingProcesses(i);

            if(p != nullptr){
                p->setBurstTime(p->getBurstTime()-1);
            }

            if(index >= start) {
                for (j = start; j <= index; j++)
                    q.enqueue(processes[j], processes[j]->getPriority());
                start = index + 1;
            }

            if(p == nullptr || p->getBurstTime() == 0) {
                p = q.dequeue();
            }

            if(isPreemptive) {
                p1 = q.See_First();
                if (p1 != nullptr && p1->getPriority() < p->getPriority()) {
                    q.enqueue(p, p->getPriority());
                    p = q.dequeue();
                }
            }
            q.addWaitTime();
        }

        for(i=0; i<process_no; i++)
            processes[i]->setBurstTime(burstBackup[i]);

        Print_PS_waiting_time(isPreemptive, out);
    }

    void Print_PS_waiting_time(bool isPreemptive, ostream& out){
        int sum=0, wait;
        out << "Scheduling Method: Priority Scheduling ";
        if(isPreemptive)
            out << "- Preemptive\n";
        else
            out << "- Non-Preemptive\n";
        out << "Process Waiting Times:\n";
        for(int i=0;i<process_no;i++){
            wait = processes[i]->getWaitingTime();
            out << "P" << i+1 << ": " << wait << " ms\n";
            sum = sum+wait;
        }
        out << "Average Waiting Time: " << sum/(process_no*1.0) << " ms\n";
    }

    void RRS(int quantum, ostream& out){
        Queue<Process*> q;
        Process* p = nullptr;
        int i, maxCounter, index, j, start = 0, counter = 0, priority = 1;
        int burstBackup[process_no];

        for(i=0; i<process_no; i++)
            burstBackup[i] = processes[i]->getBurstTime();

        resetWaitingTime();
        insertionSort(processes,process_no,2);
        maxCounter = getMaxArrivalTime() + getSumBurstTime();

        for(i=0; i<maxCounter; i++){
            index = getArivingProcesses(i);

            if(index >= start) {
                for (j = start; j <= index; j++)
                    q.enqueue(processes[j], priority++);
                start = index + 1;
            }

            if(p != nullptr){
                p->setBurstTime(p->getBurstTime()-1);
                if(counter == quantum && p->getBurstTime() != 0) {
                    q.enqueue(p, process_no+priority++);
                }
            }

            if(p == nullptr || p->getBurstTime() == 0 || counter == quantum) {
                p = q.dequeue();
                counter = 0;
            }

            q.addWaitTime();
            counter++;
        }

        for(i=0; i<process_no; i++)
            processes[i]->setBurstTime(burstBackup[i]);

        Print_RRS_waiting_time(quantum, out);
    }

    void Print_RRS_waiting_time(int quantum, ostream& out){
        int sum=0, wait;
        out << "Scheduling Method: Round Robin Scheduling - time_quantum=" << quantum << endl;
        out << "Process Waiting Times:\n";
        for(int i=0;i<process_no;i++){
            wait = processes[i]->getWaitingTime();
            out << "P" << i+1 << ": " << wait << " ms\n";
            sum = sum+wait;
        }
        out << "Average Waiting Time: " << sum/(process_no*1.0) << " ms\n";
    }

    ~CPU_Sheduler(){
        int i;
        for(i=0; i<process_no; i++)
            delete processes[i];
        delete[] processes;
    }
};


int main(int argc, char* args[]) {
    fstream input, output;
    char filename[100];
    int quantum = 2;
    int arrival, burst, priority, count = 0;
    char e;
    bool isPreemptive = Mode == 1;
    CPU_Sheduler *scheduler;


    if(argc != 5) {
        cout << "Invalid Arguments. Terminating Program......" << endl;
        return 0;
    }

    //file opening
    if(!strcmp(args[1], "-f")){
        input.open(args[2], ios::in);
        strcpy(filename, args[2]);
    } else if( !strcmp(args[1], "-o" )){
        output.open(args[2], ios::out);
    } else {
        cout << "Invalid Arguments. Terminating Program......" << endl;
        return 0;
    }

    if(!strcmp(args[3], "-f")){
        input.open(args[4], ios::in);
        strcpy(filename, args[4]);
    } else if( !strcmp(args[3], "-o" )){
        output.open(args[4], ios::out);
    } else {
        cout << "Invalid Arguments. Terminating Program......" << endl;
        return 0;
    }

    if (input.fail()) { //if file does not open for certain reasons
        cout << "File cannot be open. Terminating Program......" << endl;
        return 0;
    }


    while (input >> burst) {
        input >> e;
        input >> arrival;
        input >> e;
        input >> priority;
        count++;
    }

    scheduler = new CPU_Sheduler(count);

    input.close();
    input.open(filename, ios::in);

    while (input >> burst) {
        input >> e;
        input >> arrival;
        input >> e;
        input >> priority;
        scheduler->Add_Process(burst, arrival, priority);
    }

    int option;
    do{
        option = Scheduler_Simulator(Scheduler,Mode);
        if (option == 1) {
            do { //sheduler method displayer
                cout << "\n\t\tScheduling Method\n";
                cout << "1) None\n";
                cout << "2) First Come, First Served Scheduling\n";
                cout << "3) Shortest-Job-First Scheduling\n";
                cout << "4) Priority Scheduling\n";
                cout << "5) Round-Robin Scheduling\n";
                cout << "Option > ";
                cin >> option;
                if (cin.fail() || option < 1 || option > 5) { //fail flag will be set when we give input other than integer.
                    cin.clear(); //clears buffer
                    cin.ignore(); //ignore input
                    cout << "\nInvalid Input:(\n";
                } else
                    break;
            } while (option < 1 || option > 5);
            Scheduler=option;
        }

        else if (option == 2) {
            do{
                cout << "Select a Preemptive Mode\n";
                cout << "0) OFF\n";
                cout << "1) ON\n";
                cout << "Option > ";
                cin >> option;
                if (cin.fail() || option < 0 || option > 1) { //fail flag will be set when we give input other than integer.
                    cin.clear(); //clears buffer
                    cin.ignore(); //ignore input
                    cout << "\nInvalid Input :(\n";
                } else
                    break;
            } while (option < 0 || option > 1);
            Mode=option;
        }
        else if(option == 3){
            if(Scheduler == 1){
                cout << "Select a Scheduling Method first.\n";
            } else if( Scheduler == 2 ){
                scheduler->FCFS(cout);
            } else if( Scheduler == 3 ){
                scheduler->SJF(isPreemptive, cout);
            } else if( Scheduler == 4 ){
                scheduler->PS(isPreemptive, cout);
            }  else if( Scheduler == 5 ) {
                do {
                    cout << "Enter the Time Quantum: ";
                    cin >> quantum;
                    if (cin.fail() || quantum <= 0) {
                        cin.clear();
                        cin.ignore();
                        cout << "\nInvalid Input :(\n";
                    }
                } while (quantum <= 0);
                scheduler->SJF(quantum, cout);
            }
        }
    }while(option!=4);

    scheduler->FCFS(cout);
    scheduler->FCFS(output);

    scheduler->SJF(false, cout);
    scheduler->SJF(false, output);
    scheduler->SJF(true, cout);
    scheduler->SJF(true, output);

    scheduler->PS(true, cout);
    scheduler->PS(true, output);
    scheduler->PS(false, cout);
    scheduler->PS(false, output);

    scheduler->RRS(quantum, cout);
    scheduler->RRS(quantum, output);

    output.close();

    delete scheduler;

    return 0;
}
int Scheduler_Simulator(int scheduling_method, int mode){ //Assigns particular sheduler and shows if mode is on or off
    string Method="None";
    string Mode="Off";
    if(scheduling_method==2)
        Method = "First Come, First Served Scheduling";
    else if(scheduling_method==3)
        Method = "Shortest-Job-First Scheduling";
    else if(scheduling_method == 4)
        Method = "Priority Scheduling";
    else if(scheduling_method == 5)
        Method = "Round-Robin Scheduling";

    if(mode == 1)
        Mode = "ON";
    else
        Mode = "OFF";

    int option;
    do{
        cout << "\n\t\t\tCPU Scheduler Simulator\n";
        cout << "1) Scheduling Method(" << Method << ")\n";
        cout << "2) Preemptive Mode (" << Mode << ")\n";
        cout << "3) Show Result\n";
        cout << "4) End Program\n";
        cout << "Option > ";
        cin >> option;
        if (cin.fail() || option < 1 || option > 4) { //fail flag will be set when we give input other than integer.
            cin.clear(); //clears buffer
            cin.ignore(); //ignore input
            cout << "\nInvalid Input :(\n";
        }
        else
            break;
    }while(option < 1 || option > 4);
    return option;



}

