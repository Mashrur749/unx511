

#include <iostream>
#include <netinet/in.h>
#include <queue>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace std;

bool is_running;
const int BUF_LEN=1024;
const int MAX_CLIENTS=3;
void *recv_func(void *arg);
queue<string> message;
int  numClients;
int  master_fd, cl[MAX_CLIENTS];

/* shared mutex between receive thread and send */
pthread_mutex_t lock_x;

int main(int argc, char *argv[])
{
    //Create the socket for inter-process communications
    struct sockaddr_in addr;
    char buf[BUF_LEN];
    int len;
    int master_fd,rc;
    int ret;

    if(argc<2) {
        cout<<"usage: server <port number>"<<endl;
	return -1;
    }

    //Create the socket
    memset(&addr, 0, sizeof(addr));
    if ( (master_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "server: " << strerror(errno) << endl;
        exit(-1);
    }

    //Set the socket to this port
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(atoi(argv[1]));

    //Bind the socket to this port
    if (bind(master_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        cout << "server: " << strerror(errno) << endl;
        close(master_fd);
        exit(-1);
    }

    cout<<"Waiting for the client..."<<endl;
    //Listen for a client to connect to this port
    if (listen(master_fd, 5) == -1) {
        cout << "server: " << strerror(errno) << endl;
        close(master_fd);
        exit(-1);
    }

    cout<<"client(s) connected to the server"<<endl;

    /* initialize pthread mutex */
    pthread_mutex_init(&lock_x, NULL);

    /* start the receive thread */
    pthread_t tid;
    ret = pthread_create(&tid, NULL, recv_func, &master_fd);
    if(ret!=0) {
        cout<<"Cannot create receive thread"<<endl;
        cout<<strerror(errno)<<endl;
        close(master_fd);
        return -1;
    }
    is_running = true;
    while(is_running){
        while(message.size()>0) {
	    pthread_mutex_lock(&lock_x);
            cout<<message.front()<<endl;
	    message.pop();
	    pthread_mutex_unlock(&lock_x);
	}
        sleep(1);
    }
    memset(buf, 0, BUF_LEN);
    len = sprintf(buf, "Quit")+1;
    for(int i=0; i<numClients; ++i) {
        ret = write(cl[i], buf, len);
        if(ret==-1) {
            cout<<"server: Write Error"<<endl;
            cout<<strerror(errno)<<endl;
        }
    }
    pthread_join(tid, NULL);
    close(master_fd);
    return 0;
}

void *recv_func(void *arg)
{
    int master_fd = *(int *)arg;
    char buf[BUF_LEN];
    int  max_fd=master_fd;
    struct sockaddr_in cli_addr[MAX_CLIENTS];
    socklen_t clilen = sizeof(cli_addr[0]);
    fd_set activefds;
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_ZERO(&activefds);
    //Add the master_fd to the socket set
    FD_SET(master_fd, &activefds);

    numClients = 0;
    while(is_running) {
        readfds = activefds;
        struct timeval tv;
        /* Wait up to five seconds. */
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        int ret = select(max_fd+1, &readfds, NULL, NULL, &tv);
        if(FD_ISSET(master_fd, &readfds)){//incoming connection
            cl[numClients] = accept(master_fd, (struct sockaddr *) &cli_addr[numClients], &clilen);
            cout<<"server: incoming connection "<<cl[numClients]<<endl;
            FD_SET(cl[numClients], &activefds);
            if(max_fd<cl[numClients]) max_fd=cl[numClients];
            ++numClients;
        } else if(FD_ISSET(cl[0], &readfds)) {
            memset(buf, 0, BUF_LEN);
            ret=read(cl[0], buf, BUF_LEN);
	    pthread_mutex_lock(&lock_x);
	    message.push(buf);
	    pthread_mutex_unlock(&lock_x);
        } else if(FD_ISSET(cl[1], &readfds)) {
            memset(buf, 0, BUF_LEN);
            ret=read(cl[1], buf, BUF_LEN);
	    pthread_mutex_lock(&lock_x);
	    message.push(buf);
	    pthread_mutex_unlock(&lock_x);
        } else if(FD_ISSET(cl[2], &readfds)) {
            memset(buf, 0, BUF_LEN);
            ret=read(cl[2], buf, BUF_LEN);
	    pthread_mutex_lock(&lock_x);
	    message.push(buf);
	    pthread_mutex_unlock(&lock_x);
        } else {
            is_running=false;
        }
    }
    pthread_exit(NULL);
}