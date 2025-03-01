#include<iostream>
#include<queue>
#include<algorithm>

#define TIME_QUANTUM_Q 20 // time quantum for each queue
#define TIME_QUANTUM_RR 5 // time quantum for each process in q0

using namespace std;

class process
{
    protected:
        static short counter;
        short burstTime;
        short remainingTime;
        short processId;
        short priorityLevel;
        short completionTime;

    public:
        process(int bTime,short pLevel) : burstTime(bTime),priorityLevel(pLevel) 
        {
            remainingTime = bTime;
            processId = counter++;
            completionTime = 0;
        }

        void getData()
        {
            cout << "-> process Id = " << processId <<endl 
                 << "-> Burst Time = " << burstTime << "s" <<endl
                 << "-> priority Level = " << priorityLevel <<endl 
                 << "-> Remaining Time = " << remainingTime << "s" <<endl << endl;
        }

        void setRemainingTime(short t)
        {
            remainingTime = t;
        }
        
        short getRemainingTime() const
        {
            return remainingTime;
        }

        short getProcessId() const
        {
            return processId;
        }

        void setCompletionTime(short t)
        {
            completionTime = t;
        }

        void getTurnaroundTimeAndWaitingTime()
        {
            cout << "<< Turnaround time = " << completionTime << "s >>" << endl // turnaound time = completion time -> arrival time is set to 0
                 << "<<  Waiting time = " << completionTime - burstTime << "s >>" << endl <<endl;
        }
};

queue<process> q0;
queue<process> q1;
queue<process> q2;
queue<process> q3;

void generateUserProcesses()
{
    short numberOfProcesses;
    cout << "Enter the number of processes: ";
    cin >> numberOfProcesses;
    
    short time;
    short level;
    for(short number = 0; number < numberOfProcesses; number++)
    {
        cout << "Enter burst time from seconds for process " << number + 1 << ": ";
        cin >> time;
        cout << "Enter priority level for process " << number + 1 << " (0 to 3): ";
        cin >> level;

        process p(time, level);
        switch(level)
        {
            case 0:
                q3.push(p);
                break;
            case 1:
                q2.push(p);
                break;
            case 2:
                q1.push(p);
                break;
            case 3:
                q0.push(p);
                break;
            default:
                cout << "Invalid priority level! " << endl;
        }
    }
}

short process::counter = 1;

void printQ(queue<process> show)
{
    while(!show.empty())
    {
        show.front().getData();
        show.pop();
    }
}

void generateProcesses()
{   
    short numberOfProcesses = rand() % 14 + 1; //this will decide the number of process to be generated
    int time;
    short level;
    for(int number = 0 ; number < numberOfProcesses ; number++)
    {
        time = rand() % 20 + 1; // this will decide the burst time of each process
        level = rand() % 4; // this will decide the priority level of each process
        process p(time,level);
        switch(level)
        {
            case 0:
                q3.push(p);
                break;
            case 1:
                q2.push(p);
                break;
            case 2:
                q1.push(p);
                break;
            case 3:
                q0.push(p);
        }
    }   
}

bool compare(process &p1,process &p2)
{   
    return p1.getRemainingTime() < p2.getRemainingTime(); //compares burst time of process in order to sort 
}

void fifo(queue<process> &q,short& cTime)
{
    short remaining = q.front().getRemainingTime(); 
    short processCount = q.size(); // in order to track the number of process that has been completed
    short timeForQueue = TIME_QUANTUM_Q;
    while(processCount != 0 && timeForQueue > 0) 
    {
        if(remaining > timeForQueue) 
        {
            cTime += timeForQueue;
            remaining -= timeForQueue; 
            q.front().setRemainingTime(remaining);
            timeForQueue = 0; 
        }
        else if(remaining <= timeForQueue) 
        {
            cTime += remaining;
            q.front().setRemainingTime(0);
            cout << "<< Process " << q.front().getProcessId() << " completed. >>" << endl;
            q.front().setCompletionTime(cTime);
            q.front().getTurnaroundTimeAndWaitingTime();q.push(q.front());
            q.pop();
            timeForQueue -= remaining;
            remaining = q.front().getRemainingTime();
            processCount--;
        }
    }    
}

void sjf(queue<process>& q,short& cTime)
{   
    vector<process> tempQ1;
    while(!q.empty())
    {   
        tempQ1.push_back(q.front());
        q.pop();
    }    
    sort(tempQ1.begin(),tempQ1.end(),compare);
    for(process &p : tempQ1)
    {
        q.push(p);
    }    
    fifo(q,cTime);
    
    return;
} 

void execQ0(queue<process>&q)
{
    short timeQuantum = TIME_QUANTUM_RR;
    short remaining;
    short timeForQueue = TIME_QUANTUM_Q;
    static short processCount = q.size(); // Static variables to track total process count and current execution time
    static short currentTime = 0;

    do
    {   
        remaining = q.front().getRemainingTime(); 
        if(remaining > timeQuantum  && remaining != 0 && timeForQueue >= timeQuantum) // Case 1: Process requires more time than the time quantum
        {
            q.front().setRemainingTime(remaining - timeQuantum); 
            q.push(q.front());
            q.pop();
            timeForQueue -= timeQuantum; 
            currentTime += timeQuantum;
        }
        else if(remaining <= timeQuantum && remaining != 0 && timeForQueue >= timeQuantum)// Case 2: Process can complete within the time quantum
        {
            currentTime += remaining;
            timeForQueue -= remaining;
            q.front().setRemainingTime(0);
            cout << "<< Process " << q.front().getProcessId() << " completed. >>" << endl;
            q.front().setCompletionTime(currentTime);
            q.front().getTurnaroundTimeAndWaitingTime();
            q.push(q.front());
            q.pop();
            processCount--;
        }
        else if(processCount != 0 && remaining == 0) // Case 3: Process has already completed execution
        {
            q.push(q.front());
            q.pop();
        }
        else if(timeForQueue < timeQuantum && remaining != 0)// Case 4: Remaining time in the queue is less than time quantum
        {
            if(remaining <= timeForQueue)// If process can complete within the remaining queue time
            {
                currentTime += remaining;
                q.front().setRemainingTime(0);
                cout << "<< Process " << q.front().getProcessId() << " completed. >>" << endl;
                q.front().setCompletionTime(currentTime);
                q.front().getTurnaroundTimeAndWaitingTime();
                q.push(q.front());
                q.pop();
                processCount--;
                timeForQueue -= remaining; 
            }
            else if(remaining > timeForQueue)// If process needs more time than reamining queue time
            {
                currentTime += timeQuantum;
                q.front().setRemainingTime(remaining - timeForQueue);
                q.push(q.front());
                q.pop();   
                timeForQueue = 0;       
            }
        }
    }while(timeForQueue > 0 && processCount > 0);
}

bool isQueueOver(queue<process>& q) // to check whether the all processes in the queue has been completed its task or not
{
    queue<process> tempQ = q; 
    while (!tempQ.empty()) 
    {
        if (tempQ.front().getRemainingTime() > 0) 
        {
            return false;
        }
        tempQ.pop();
    }
    return true;
}

void execQ1(queue<process>&q1)
{
    static short currentTimeQ1 = 0; // to track the completion time of queue 1
    sjf(q1,currentTimeQ1);
}

void execQ2(queue<process>&q2)
{
    static short currentTimeQ2 = 0;
    sjf(q2,currentTimeQ2);
}    

void execQ3(queue<process>&q3)
{
    static short currentTimeQ3 = 0;
    fifo(q3,currentTimeQ3);
}

int main()
{
    srand(time(NULL));
    generateUserProcesses();
    bool q0Over = false;
    bool q1Over = false;
    bool q2Over = false;
    bool q3Over = false;
    do
    {   
        if(!q0Over)
        {
            cout << endl << "BEFORE EXECUTING q0" << endl << endl;
            printQ(q0);
            execQ0(q0);
            q0Over = isQueueOver(q0);
            cout << endl << "AFTER EXECUTING q0" << endl << endl;
            printQ(q0);
        }
        if(!q1Over)
        {
            cout << endl << "BEFORE EXECUTING q1" << endl << endl;
            printQ(q1);
            execQ1(q1);
            q1Over = isQueueOver(q1);
            cout << endl << "AFTER EXECUTING q1" << endl << endl;
            printQ(q1);
        }
        if(!q2Over)
        {
            cout << endl << "BEFORE EXECUTING q2" << endl << endl;
            printQ(q2);
            execQ2(q2);
            q2Over = isQueueOver(q2);
            cout << endl << "AFTER EXECUTING q2" << endl << endl;
            printQ(q2);
        }
        if(!q3Over)
        {
            cout << endl << "BEFORE EXECUTING q3" << endl << endl;
            printQ(q3);    
            execQ3(q3);
            q3Over = isQueueOver(q3);
            cout << endl << "AFTER EXECUTING q3" << endl << endl;
            printQ(q3);    
        }
    }while(!(q0Over && q1Over && q2Over && q3Over)); // this loop will run until all the processes are done

    return 0;
}
