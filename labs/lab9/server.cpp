//Musaddiqur Rahman
//106932189



/* 

What is the difference between synchronous and asynchronous communication?

There are a few differences between synchronous and asynchronous communication. 
The main difference is that synchronous communications happen in the moment, meaning that it’s faster and more dynamic. 
This means that it’s also good for participation and active discussions with smaller groups.

Conversely, asynchronous communication happens over a period of time, so there’s no need to schedule anything. 
You can go at your own pace, without as many disruptions. 

Do you prefer socket reads in a receive thread or do you prefer both socket read and write to be in the main() function? Why?

Sockets are not thread safe, because operations included in sockets are not atomic, which means socket read and write operations are independant. And if socket reads
are done in threads, there will be close() functions as well in those threads which creates the possibility of attempting to run close() operation multiple times, 
which may cause complications related to file descriptor errors. 


*/

#include <iostream>
#include <signal.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <queue>

using namespace std;

bool is_running;
const int BUF_LEN = 1024;
const int MAX_CLIENTS = 3;
queue < string > message;
int numClients;
int fd, cl[MAX_CLIENTS];

pthread_mutex_t lock_x;

void * recv_func(void * fdArg) {
        int fd = * (int * ) fdArg;
        char buffer[BUF_LEN];
        int max_fd = fd;
        struct sockaddr_in cli_addr[MAX_CLIENTS];
        socklen_t clientSize = sizeof(cli_addr[0]);
        fd_set activefds;
        fd_set readfds;

        FD_ZERO( & readfds);
        FD_ZERO( & activefds);
        FD_SET(fd, & activefds);

        numClients = 0;
        while (is_running) {
                readfds = activefds;
                struct timeval tv;

                tv.tv_sec = 5;
                tv.tv_usec = 0;
                int ret = select(max_fd + 1, & readfds, NULL, NULL, & tv);
                if (FD_ISSET(fd, & readfds)) { 
                        cl[numClients] = accept(fd, (struct sockaddr * ) & cli_addr[numClients], & clientSize);
                        cout << "server: incoming connection " << cl[numClients] << endl;
                        FD_SET(cl[numClients], & activefds);
                        if (max_fd < cl[numClients]) max_fd = cl[numClients];
                        ++numClients;
                } else if (FD_ISSET(cl[0], & readfds)) {
                        memset(buffer, 0, BUF_LEN);
                        ret = read(cl[0], buffer, BUF_LEN);
                        pthread_mutex_lock( & lock_x);
                        message.push(buffer);
                        pthread_mutex_unlock( & lock_x);
                } else if (FD_ISSET(cl[1], & readfds)) {
                        memset(buffer, 0, BUF_LEN);
                        ret = read(cl[1], buffer, BUF_LEN);
                        pthread_mutex_lock( & lock_x);

                        message.push(buffer);

                        pthread_mutex_unlock( & lock_x);
                } else if (FD_ISSET(cl[2], & readfds)) {
                        memset(buffer, 0, BUF_LEN);
                        ret = read(cl[2], buffer, BUF_LEN);
                        pthread_mutex_lock( & lock_x);
                        message.push(buffer);
                        pthread_mutex_unlock( & lock_x);
                } else {
                        is_running = false;
                }
        }
        pthread_exit(NULL);
}

int main(int argc, char * argv[]) {
        struct sockaddr_in addr;
        char buffer[BUF_LEN];
        int len;
        int fd, rc;
        int ret;

        if (argc < 2) {
                cout << "usage: server <port number>" << endl;
                return -1;
        }

        //Create the socket
        memset( & addr, 0, sizeof(addr));
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                cout << "server: " << strerror(errno) << endl;
                exit(-1);
        }

        //Set the socket to this port
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(atoi(argv[1]));
        if (bind(fd, (struct sockaddr * ) & addr, sizeof(addr)) == -1) {
                cout << "server: " << strerror(errno) << endl;
                close(fd);
                exit(-1);
        }

        cout << "Waiting for the client..." << endl;
        
        if (listen(fd, 5) == -1) {
                cout << "server: " << strerror(errno) << endl;
                close(fd);
                exit(-1);
        }

        cout << "client(s) connected to the server" << endl;

        pthread_mutex_init( & lock_x, NULL);

        pthread_t tid;
        ret = pthread_create( & tid, NULL, recv_func, & fd);
        if (ret != 0) {
                cout << "Cannot create receive thread" << endl;
                cout << strerror(errno) << endl;
                close(fd);
                return -1;
        }
        is_running = true;
        while (is_running) {
                while (message.size() > 0) {
                        pthread_mutex_lock( & lock_x);
                        cout << message.front() << endl;
                        message.pop();
                        pthread_mutex_unlock( & lock_x);
                }
                sleep(1);
        }
        memset(buffer, 0, BUF_LEN);
        len = sprintf(buffer, "Quit") + 1;
        for (int i = 0; i < numClients; ++i) {
                ret = write(cl[i], buffer, len);
                if (ret == -1) {
                        cout << "server: Write Error" << endl;
                        cout << strerror(errno) << endl;
                }
        }
        pthread_join(tid, NULL);
        close(fd);
        return 0;
}