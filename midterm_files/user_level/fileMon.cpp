//fileMon.cpp - log file monitor
//
// 11-Feb-21  M. Watler         Created.
//

#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "memoryUsage.h"

using namespace std;
bool isRunning;
const int MAXBUF=64;
const int THRESHOLD=40000;

static void sigHandler(int sig) {
    switch(sig) {
        case SIGINT:
            cout<<"fileMon: SIGINT received"<<endl;
            isRunning = false;
            break;
    }
}

int main(int argc, char *argv[])
{
    char key1[MAXBUF], key2[MAXBUF];
    strncpy(key1, argv[1], MAXBUF);
    int fd=open(keyFile, O_RDWR);
    read(fd, key2, MAXBUF);
    close(fd);
    //Question 18: Where is key1 coming from?
    //from command line arguments 
    //Question 19: Where is key2 coming from?
    //from the /dev/keygen file descriptor
    //Question 20: Why are they being compared?
    //the kernel code is generating a random number, the commandline argument sent has to match that
    //number for the key to be valid

    int valid=strcmp(key1, key2);
    if(valid!=0) {
        cout<<"Invalid key, aborting..."<<endl;
    } else {
        struct sigaction action;
        action.sa_handler = sigHandler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction(SIGINT, &action, NULL);

        isRunning = true;
        struct stat st;
        while(isRunning) {
            stat(logFile, &st);
            if(st.st_size>THRESHOLD) {
                int openFlags = O_RDWR | O_CREAT | O_TRUNC;
                mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
		//Question 21: What do the following two lines effectively do to the log file?
        //opens the log file with flags mentioned in openFlags, with the permissions set in filePerms
                fd=open(logFile, openFlags, filePerms);
		        close(fd);
            }
            sleep(1);
        }
    }
    return 0;
}
