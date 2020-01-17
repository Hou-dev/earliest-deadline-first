/*This is Earliest Deadline First
Some assumptions were taken on the input, 
the rooms will not update if the names 
does not match the name of input.
Semaphore was coded with help
http://www.cs.tufts.edu/comp/111/examples/Locks/semop.c
as a skeleton but modified. Accessed on Nov 4, 2018
*/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <sys/types.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <queue>
#include <stdlib.h>
#include <signal.h>

using namespace std;
//varibles for parsing inputs.
int rooms;
int customers;
vector<int> customernum;
vector<int> reserve;
vector<int> cancel;
vector<int> check;
vector<int> pay;
//struct for parsed commands
struct datamine
{
    string cmd;
    string resnum;
    string datenum;
    string numofdays;
    string nofcust;
    string dline;
    string nameofcust;
};
//vectors containing structs
vector<datamine> vaildcmds;
//array containing structs
datamine customerarray[10][10];
//varaibles used to get starting date and make array of rooms
vector<int> comandcounter;
int *roomMarker = NULL;
int startdate;
int enddate;
vector<int> sortdline;

void readinput()
{
    int vcmdsctr = 0;
    int cmdscounter = 0;
    ifstream inpfile("input");
    string inpstr;
    //Get the number of Customers and Rooms
    getline(inpfile, inpstr);
    rooms = atoi(inpstr.c_str());
    getline(inpfile, inpstr);
    customers = atoi(inpstr.c_str());
    while (getline(inpfile, inpstr))
    {
        if (inpstr.substr(0, 9) == "customer_")
        {
            //Regular Parsing
            //Parsing the four lines with delay for each operation
            inpstr.erase(inpstr.length() - 1, 1);
            customernum.push_back(atoi(inpstr.substr(9).c_str()));
            getline(inpfile, inpstr);
            reserve.push_back(atoi(inpstr.substr(8).c_str()));
            getline(inpfile, inpstr);
            cancel.push_back(atoi(inpstr.substr(7).c_str()));
            getline(inpfile, inpstr);
            check.push_back(atoi(inpstr.substr(6).c_str()));
            getline(inpfile, inpstr);
            pay.push_back(atoi(inpstr.substr(4).c_str()));
            //Parsing Valid Operations
            while (getline(inpfile, inpstr))
            {
                vaildcmds.push_back(datamine());
                if (inpstr.substr(0, 4) == "end.")
                {
                    vcmdsctr++;
                    comandcounter.push_back(cmdscounter);
                    cmdscounter = 0;
                    break;
                }
                stringstream p1(inpstr);
                string transtream;
                getline(p1, transtream, ' ');
                //parse command with reserve
                if (transtream.substr(0, 7) == "reserve")
                {
                    customerarray[vcmdsctr][cmdscounter].cmd = transtream;
                    getline(p1, transtream, ' ');
                    transtream.erase(std::remove(transtream.begin(), transtream.end(), '('), transtream.end());
                    transtream.erase(std::remove(transtream.begin(), transtream.end(), ')'), transtream.end());
                    customerarray[vcmdsctr][cmdscounter].resnum = transtream;
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].datenum = transtream;
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].numofdays = transtream;
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].nameofcust = transtream;
                    getline(p1, transtream, ' ');
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].dline = transtream;
                    cmdscounter++;
                }
                //parse and command with cancel
                if (transtream.substr(0, 6) == "cancel")
                {
                    customerarray[vcmdsctr][cmdscounter].cmd = transtream;
                    getline(p1, transtream, ' ');
                    transtream.erase(std::remove(transtream.begin(), transtream.end(), '('), transtream.end());
                    transtream.erase(std::remove(transtream.begin(), transtream.end(), ')'), transtream.end());
                    customerarray[vcmdsctr][cmdscounter].resnum = transtream;
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].datenum = transtream;
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].numofdays = transtream;
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].nameofcust = transtream;
                    getline(p1, transtream, ' ');
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].dline = transtream;
                    cmdscounter++;
                }
                //parse command with pay
                if (transtream.substr(0, 3) == "pay")
                {
                    customerarray[vcmdsctr][cmdscounter].cmd = transtream;
                    getline(p1, transtream, ' ');
                    transtream.erase(std::remove(transtream.begin(), transtream.end(), '('), transtream.end());
                    transtream.erase(std::remove(transtream.begin(), transtream.end(), ')'), transtream.end());
                    customerarray[vcmdsctr][cmdscounter].resnum = transtream;
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].datenum = transtream;
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].numofdays = transtream;
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].nameofcust = transtream;
                    getline(p1, transtream, ' ');
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].dline = transtream;
                    cmdscounter++;
                }
                //parse command with check
                if (transtream.substr(0, 5) == "check")
                {
                    customerarray[vcmdsctr][cmdscounter].cmd = transtream;
                    getline(p1, transtream, ' ');
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].nameofcust = transtream;
                    getline(p1, transtream, ' ');
                    getline(p1, transtream, ' ');
                    customerarray[vcmdsctr][cmdscounter].dline = transtream;
                    cmdscounter++;
                }
            }
        }
    }
}

//sort deadlines by shortest
void sortdeadline()
{
    for (int i = 0; i < customers; i++)
    {
        for (int j = 0; j < comandcounter[i]; j++)
        {
            int a = atoi(customerarray[i][j].dline.c_str());
            sortdline.push_back(a);
        }
    }
    for (int ix = 0; ix < sortdline.size() - 1; ix++)
    {
        int sdf;
        int temp;
        int r;
        sdf = sortdline[ix];
        temp = sortdline[ix + 1];
        if (sdf == temp)
        {
            r = rand() % 2;
            if (r == 0)
            {
                sortdline[ix] = sdf + 1;
            }
            if (r == 1)
            {
                sortdline[ix + 1] = temp + 1;
            }
        }
    }
    sort(sortdline.begin(), sortdline.end());
}

#define BUFSIZE 100
// semaphore meanings
#define FREE 0
#define USED 1
#define MUTEX 2
int group; // identity of semaphore group

int main(int argc, char const *argv[])
{
    // create shared memory arrays
    void *memory = NULL;
    int shmid = shmget((key_t)1234, 6, IPC_CREAT | 0666);
    memory = shmat(shmid, NULL, 0);
    string *p = (string *)memory;

    // create shared memory arrays
    int *roomray;
    int shmid2 = shmget((key_t)1235, rooms + 1, IPC_CREAT | 0666);
    roomray = (int *)shmat(shmid2, 0, 0);

    // create shared memory arrays
    string *nameray;
    int shmid3 = shmget((key_t)1236, rooms + 1, IPC_CREAT | 0666);
    nameray = (string *)shmat(shmid3, 0, 0);

    //parse and sort deadline
    readinput();
    sortdeadline();

    group = semget(IPC_PRIVATE, 3, 0755);
    //struct for operations
    struct sembuf sb[3] = {
        {FREE, BUFSIZE - 1, 0}, // free space in buffer
        {USED, 0, 0},           // elements in queue
        {MUTEX, 1, 0}           // mutual exclusion lock
    };
    semop(group, sb, 3); // initial setup
    //make new process and fork
    pid_t newproc;
    newproc = fork();
    switch (newproc)
    {
    case -1:
        perror("Fork Error");
        exit(1);
    //child
    case 0:
        int delay;
        while (1)
        {
            //child responsible to decrement the number of days in room
            struct sembuf s;
            s.sem_num = USED;
            s.sem_op = -1;
            s.sem_flg = 0;
            semop(group, &s, 1);
            s.sem_num = MUTEX;
            s.sem_op = -1;
            s.sem_flg = 0;
            semop(group, &s, 1);
            for (int t = 0; t < rooms; t++)
            {
                if (roomray[t] > 0)
                {
                    roomray[t] = roomray[t] - 1;
                }
            }

            s.sem_num = MUTEX;
            s.sem_op = 1;
            s.sem_flg = 0;
            semop(group, &s, 1);
            s.sem_num = FREE;
            s.sem_op = 1;
            s.sem_flg = 0;
            semop(group, &s, 1);
        }
    //Parent
    default:
        while (1)
        {
            struct sembuf s;
            //parent loops while not at the end of sorted deadlines
            for (int p1 = 0; p1 < sortdline.size(); p1++)
            {
                int p2 = sortdline[p1];
                for (int i = 0; i < customers; i++)
                {
                    for (int j = 0; j < comandcounter[i]; j++)
                    {
                        if (atoi(customerarray[i][j].dline.c_str()) == p2)
                        {
                            //Prints deadline and prints if there is an operation
                            cout << "DEADLINE " << customerarray[i][j].dline << endl;
                            if (customerarray[i][j].cmd == "reserve")
                            {

                                string a;
                                a = customerarray[i][j].resnum;
                                int a1;
                                int b1;
                                a1 = atoi(a.substr(0, 1).c_str());
                                a.erase(1, 1);
                                b1 = atoi(a.substr(1, 1).c_str());
                                //this helps parsing in the days for reservation
                                if (b1 == 0)
                                {
                                    s.sem_num = FREE;
                                    s.sem_op = -1;
                                    s.sem_flg = 0;
                                    semop(group, &s, 1);
                                    s.sem_num = MUTEX;
                                    s.sem_op = -1;
                                    s.sem_flg = 0;
                                    semop(group, &s, 1);
                                    roomray[a1] = atoi(customerarray[i][j].numofdays.c_str());
                                    //semaphore used to reserve room
                                    cout << "RESERVING ROOM: " << a1 << " FOR " << customerarray[i][j].nameofcust << endl;
                                    s.sem_num = MUTEX;
                                    s.sem_op = 1;
                                    s.sem_flg = 0;
                                    semop(group, &s, 1);
                                    s.sem_num = USED;
                                    s.sem_op = 1;
                                    s.sem_flg = 0;
                                    semop(group, &s, 1);
                                }
                                else if (b1 != 0)
                                {
                                    for (int ih = a1; ih <= b1; ih++)
                                    {
                                        if (roomray[ih] == 0)
                                        {
                                            s.sem_num = FREE;
                                            s.sem_op = -1;
                                            s.sem_flg = 0;
                                            semop(group, &s, 1);
                                            s.sem_num = MUTEX;
                                            s.sem_op = -1;
                                            s.sem_flg = 0;
                                            semop(group, &s, 1);
                                            //semaphore used to reserve room
                                            cout << "RESERVING ROOM: " << ih << " FOR " << customerarray[i][j].nameofcust << endl;
                                            roomray[ih] = atoi(customerarray[i][j].numofdays.c_str());
                                            nameray[ih] = customerarray[i][j].nameofcust;
                                            s.sem_num = MUTEX;
                                            s.sem_op = 1;
                                            s.sem_flg = 0;
                                            semop(group, &s, 1);
                                            s.sem_num = USED;
                                            s.sem_op = 1;
                                            s.sem_flg = 0;
                                            semop(group, &s, 1);
                                        }
                                        else
                                        {
                                            cout << "ROOM:" << ih << " ALREADY RESERVED" << endl;
                                        }
                                    }
                                }
                                //delay of command
                                delay = reserve[i];
                                usleep(delay * 1000);
                            }
                            if (customerarray[i][j].cmd == "pay")
                            {
                                for (int c = 0; c <= rooms; c++)
                                {
                                    if (roomray[c] > 0)
                                    {
                                        //if the room is reserved for more than 0 days
                                        //check if the name matches who is reserved room
                                        if (nameray[c] == customerarray[i][j].nameofcust)
                                        {
                                            string sa;
                                            sa = customerarray[i][j].resnum;
                                            int a2;
                                            int b2;

                                            a2 = atoi(sa.substr(0, 1).c_str());
                                            sa.erase(1, 1);
                                            b2 = atoi(sa.substr(1, 1).c_str());
                                            if (b2 == 0)
                                            {
                                                s.sem_num = FREE;
                                                s.sem_op = -1;
                                                s.sem_flg = 0;
                                                semop(group, &s, 1);
                                                s.sem_num = MUTEX;
                                                s.sem_op = -1;
                                                s.sem_flg = 0;
                                                semop(group, &s, 1);
                                                roomray[a2] = atoi(customerarray[i][j].numofdays.c_str());
                                                nameray[a2] = "paid";
                                                //semaphore paying for room.

                                                cout << "PAYING FOR ROOM: " << a2 << " FOR " << customerarray[i][j].nameofcust << endl;
                                                s.sem_num = MUTEX;
                                                s.sem_op = 1;
                                                s.sem_flg = 0;
                                                semop(group, &s, 1);
                                                s.sem_num = USED;
                                                s.sem_op = 1;
                                                s.sem_flg = 0;
                                                semop(group, &s, 1);
                                                break;
                                            }
                                            else if (b2 != 0)
                                            {
                                                for (int ij = a2; ij <= b2; ij++)
                                                {
                                                    s.sem_num = FREE;
                                                    s.sem_op = -1;
                                                    s.sem_flg = 0;
                                                    semop(group, &s, 1);
                                                    s.sem_num = MUTEX;
                                                    s.sem_op = -1;
                                                    s.sem_flg = 0;
                                                    semop(group, &s, 1);
                                                    roomray[ij] = atoi(customerarray[i][j].numofdays.c_str());
                                                    nameray[ij] = "paid";
                                                    //semaphore paying for room.
                                                    cout << "PAYING FOR ROOM: " << ij << " FOR " << customerarray[i][j].nameofcust << endl;
                                                    s.sem_num = MUTEX;
                                                    s.sem_op = 1;
                                                    s.sem_flg = 0;
                                                    semop(group, &s, 1);
                                                    s.sem_num = USED;
                                                    s.sem_op = 1;
                                                    s.sem_flg = 0;
                                                    semop(group, &s, 1);
                                                }
                                            }
                                        }
                                        //if room is already paid for try another
                                        else if (nameray[c] != customerarray[i][j].nameofcust)
                                        {
                                            if (nameray[c] == "paid")
                                            {
                                                cout << "ROOM: " << c << " UNAVAILABLE, TRY ANOTHER" << endl;
                                            }
                                        }
                                    }
                                }
                                //delay for command.
                                delay = pay[i];
                                usleep(delay * 1000);
                            }
                            if (customerarray[i][j].cmd == "cancel")
                            {
                                for (int dc = 0; dc <= rooms; dc++)
                                {
                                    //cannot cancel paid room
                                    if (nameray[dc] == "paid")
                                    {
                                        cout << "ALREADY PAID, CANNOT CANCEL" << endl;
                                    }
                                    //if room is not paid and name matches then you are able to cancel
                                    else if (nameray[dc] == customerarray[i][j].nameofcust)
                                    {
                                        string sb;
                                        sb = customerarray[i][j].resnum;
                                        int a3;
                                        int b3;
                                        a3 = atoi(sb.substr(0, 1).c_str());
                                        sb.erase(1, 1);
                                        b3 = atoi(sb.substr(1, 1).c_str());
                                        if (b3 == 0)
                                        {
                                            s.sem_num = FREE;
                                            s.sem_op = -1;
                                            s.sem_flg = 0;
                                            semop(group, &s, 1);
                                            s.sem_num = MUTEX;
                                            s.sem_op = -1;
                                            s.sem_flg = 0;
                                            semop(group, &s, 1);
                                            roomray[a3] = 0;
                                            nameray[a3] = "-";
                                            cout << "CANCELING: " << a3 << " FOR " << customerarray[i][j].nameofcust << endl;
                                            s.sem_num = MUTEX;
                                            s.sem_op = 1;
                                            s.sem_flg = 0;
                                            semop(group, &s, 1);
                                            s.sem_num = USED;
                                            s.sem_op = 1;
                                            s.sem_flg = 0;
                                            semop(group, &s, 1);
                                            break;
                                        }
                                        else if (b3 != 0)
                                        {
                                            for (int tj = a3; tj <= b3; tj++)
                                            {
                                                s.sem_num = FREE;
                                                s.sem_op = -1;
                                                s.sem_flg = 0;
                                                semop(group, &s, 1);
                                                s.sem_num = MUTEX;
                                                s.sem_op = -1;
                                                s.sem_flg = 0;
                                                semop(group, &s, 1);
                                                roomray[tj] = 0;
                                                nameray[tj] = "-";
                                                cout << "CANCELING: " << tj << " FOR " << customerarray[i][j].nameofcust << endl;
                                                s.sem_num = MUTEX;
                                                s.sem_op = 1;
                                                s.sem_flg = 0;
                                                semop(group, &s, 1);
                                                s.sem_num = USED;
                                                s.sem_op = 1;
                                                s.sem_flg = 0;
                                                semop(group, &s, 1);
                                            }
                                            break;
                                        }
                                    }
                                }
                                //delay for command
                                delay = cancel[i];
                                usleep(delay * 1000);
                            }
                            //check room depending on name
                            if (customerarray[i][j].cmd == "check")
                            {
                                //delay for command
                                delay = check[i];
                                usleep(delay * 1000);
                                for (int t = 0; t < rooms; t++)
                                {
                                    if (nameray[t] == customerarray[i][j].nameofcust)
                                    {
                                        cout << "CHECKING: " << customerarray[i][j].nameofcust << "HERE" << endl;
                                    }
                                    else
                                    {
                                        goto ed;
                                    }
                                }
                            ed:
                                cout << "CHECKING: " << customerarray[i][j].nameofcust << " NOT IN ANY ROOM" << endl;
                            }
                        }
                    }
                }
                if (p2 == sortdline.back())
                {
                    //output the rooms and who is in it
                    cout << "---------------------------------------------------" << endl;
                    cout << "ROOMS" << endl;
                    for (int cemu = 1; cemu < rooms + 1; cemu++)
                    {
                        if (nameray[cemu] == "-" || nameray[cemu] == "")
                        {
                            cout << "ROOM: " << cemu << " EMPTY" << endl;
                        }
                        else
                        {
                            cout << "ROOM: " << cemu << " PAID FOR: " << nameray[cemu] << endl;
                        }
                    }
                    //detach and  clear shared memory
                    shmdt(roomray);
                    shmctl(shmid2, IPC_RMID, NULL);

                    shmdt(nameray);
                    shmctl(shmid3, IPC_RMID, NULL);

                    shmctl(group, IPC_RMID, NULL);
                    //cancel process
                    kill(0, SIGKILL);
                }
            }
        }
    }
    return 0;
}
