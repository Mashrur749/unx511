//name: Musaddiqur Rahman
//id: 106932189
//networkMonitor.cpp

#include "./networkMonitor.h"

using namespace std;

char socket_path[] = "/tmp/networkMon";
const int BUF_LEN = 200;
const int MAX_CLIENTS = 5;
bool is_running;


static void signalHandler(int signum)
{
    switch (signum)
    {
    case SIGINT:
        cout << "signalHandler(" << signum << "): SIGINT" << endl;
        is_running = false;
        break;
    default:
        cout << "signalHandler(" << signum << "): unknown" << endl;
    }
}

int main()
{

    int numOfInterfaces = 2;
    string *intNames = nullptr;

    cout << "Number of interfaces to be monitored: ";
    cin >> numOfInterfaces;

    intNames = new string[numOfInterfaces];
    for (int i = 0; i < numOfInterfaces; i++)
    {
        cout << endl
             << "Interface " << i + 1 << " name: ";
        cin >> intNames[i];
    }


    //Create the socket for inter-process communications
    struct sockaddr_un addr;
    char buf[BUF_LEN];
    int len;
    int master_fd, max_fd, cl[MAX_CLIENTS], rc;
    fd_set active_fd_set;
    fd_set read_fd_set;
    int ret;
    int numClients = 0;

    //Set up a signal handler to terminate the program gracefully
    struct sigaction action;
    action.sa_handler = signalHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);

    //Create the socket
    memset(&addr, 0, sizeof(addr));
    if ((master_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        cout << "server: " << strerror(errno) << endl;
        exit(-1);
    }

    addr.sun_family = AF_UNIX;
    //Set the socket path to a local socket file
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    unlink(socket_path);

    //Bind the socket to this local socket file
    if (bind(master_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        cout << "server: " << strerror(errno) << endl;
        close(master_fd);
        exit(-1);
    }


    if (fork() == 0)
    {
        for(int i = 0; i < numOfInterfaces; i++){
            if(fork() == 0){
                cout << intNames[i] << endl;
                execlp("./intfMon", "./intfMon", intNames[i].c_str(), NULL);
            }
        }
    }
    else
    {
        cout << "Waiting for the client..." << endl;
        //Listen for a client to connect to this local socket file
        if (listen(master_fd, 5) == -1)
        {
            cout << "NetworkMonitor: " << strerror(errno) << endl;
            unlink(socket_path);
            close(master_fd);
            exit(-1);
        }

        FD_ZERO(&read_fd_set);
        FD_ZERO(&active_fd_set);
        //Add the master_fd to the socket set
        FD_SET(master_fd, &active_fd_set);
        max_fd = master_fd; //We will select from max_fd+1 sockets (plus one is due to a new connection)
        is_running = true;

        while (is_running)
        {
            //Block until an input arrives on one or more sockets
            read_fd_set = active_fd_set;
            ret = select(max_fd + 1, &read_fd_set, NULL, NULL, NULL); //Select from up to max_fd+1 sockets
            if (ret < 0)
            {
                cout << "NetworkMonitor: " << strerror(errno) << endl;
            }
            else
            { //Service all the sockets with input pending
                if (FD_ISSET(master_fd, &read_fd_set))
                {                                                   //Connection request on the master socket
                    cl[numClients] = accept(master_fd, NULL, NULL); //Accept the new connection
                    if (cl[numClients] < 0)
                    {
                        cout << "NetworkMonitor: " << strerror(errno) << endl;
                    }
                    else
                    {

                        cout << "NetworkMonitor: incoming connection " << cl[numClients] << endl;
                        FD_SET(cl[numClients], &active_fd_set); //Add the new connection to the set
                        ret = read(cl[numClients], buf, BUF_LEN);

                        cout << "Received message from interface monitor: " << buf << endl;

                        len = sprintf(buf, "Monitor") + 1;
                        ret = write(cl[numClients], buf, BUF_LEN);
                        if (ret == -1)
                        {
                            cout << "NetworkMonitor: Write Error" << endl;
                            cout << strerror(errno) << endl;
                        }

                        if (max_fd < cl[numClients])
                            max_fd = cl[numClients]; //Update the maximum fd
                        ++numClients;
                    }
                }
                else //Data arriving on an already-connected socket
                {
                    for (int i = 0; i < numClients; ++i)
                    { //Find which client sent the data
                        if (FD_ISSET(cl[i], &read_fd_set))
                        {
                            bzero(buf, BUF_LEN);
                            ret = read(cl[i], buf, BUF_LEN); //Read the data from that client
                            cout << buf;

                            if (ret == -1)
                            {
                                cout << "NetworkMonitor: Read Error" << endl;
                                cout << strerror(errno) << endl;
                            }
                        }
                    }
                }
            }

            sleep(1);
        }

        for (int i = 0; i < numClients; ++i)
        { //Request each client to quit
            cout << "NetworkMonitor: request client " << i + 1 << " to quit" << endl;
            len = sprintf(buf, "Quit") + 1;
            ret = write(cl[i], buf, len);
            if (ret == -1)
            {
                cout << "NetworkMonitor: Write Error" << endl;
                cout << strerror(errno) << endl;
            }
            sleep(1);                      //Give the clients a change to quit
            FD_CLR(cl[i], &active_fd_set); //Remove the socket from the set of active sockets
            close(cl[i]);                  //Close the socket connection
        }



        //waits for all the processes to end
        while(wait(NULL) > 0);

        //Close the master socket
        close(master_fd);
        //Remove the socket file from /tmp
        unlink(socket_path);
    }

    return 0;
}
