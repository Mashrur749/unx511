//processMon.cpp - A process monitor for memory usage
//
// 11-Feb-21  M. Watler         Created.

#include <fcntl.h>
#include <fstream>
#include <cstring>
#include <iostream>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "memoryUsage.h"

using namespace std;
const int NUM=2;
const int MAXBUF=64;

int processMonitor();
bool isRunning;
bool isParent;

pid_t childPid[NUM];
char key[MAXBUF];
int fd;

char args[NUM][MAXBUF] = {"./fileMon", "./memMonitor"};

static void sigHandler(int sig) {
    switch(sig) {
        case SIGINT:
            cout<<"processMonitor: SIGINT received"<<endl;
	    isRunning = false;
	    break;
    }
}

int main()
{
    //Question 2: What happens if the following is executed from the command line: "pkill -2 processMon"



    struct sigaction action;
    action.sa_handler = sigHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
    //Question 3: What function in the kernel is called when open is called here?
    //key_generator_open

    std::cout << "not in the loop";
    fd=open(keyFile, O_RDWR);
    isParent = true;
    for(int i=0; i<NUM && isParent; ++i) {
        std::cout << "in the loop";
        childPid[i] = fork();
	//Question 4: How many processes are running at this point?
    //three processes parent and two children
        if(childPid[i]==0) {//the child
            isParent=false;
            //Question 5: What function in the kernel is called when read is called here?
            //key_generator_read
            read(fd, key, MAXBUF);
	        close(fd);
            execlp(args[i], args[i], (const char*)key, NULL);
            //Question 6: Why is it an error if the program continues beyond execlp()?

            //because exec replaces the current process image and with a new process image 
            //and terminates the process afterwards, hence the program never reaches the line afterwards
            //but in the case of error while running exec, the process will return control to the parent

	        cout<<strerror(errno)<<endl;
        }
    }
    if(isParent) {
        sleep(1);
        //Question 7: What function in the kernel is called when ioctl is called here?
        //key_generator_ioctl

        ioctl(fd, KEY_GENERATOR_CHANGE_KEY, NULL);
        processMonitor();
    }

    for(int i=0; i<NUM; ++i) {
        //Question 8: What function in the child processes is called when they receive a SIGINT?
        //  sigHandler function
        kill(childPid[i], SIGINT);
    }
    cout << "pid:" << getpid() << " Finished!" << endl;

    close(fd);

    return 0;
}

int processMonitor()
{
    int status=-1;
    pid_t pid=0;

    isRunning = true;
    while(isRunning && pid>=0) {
        //Question 9: What are we waiting on here?
        //waiting on the status change of the child process id 
        pid=wait(&status);
	    char buf[MAXBUF];
        //Question 10: What is processMon attempting to do with the log file?
        //attempting to open log file with read and write flags, if the file doesn't exist the 
        //create flag allows the file to be created and append flag allows the writes 
        //on the file to be appended
        //fcntl(fdLog,F_GETFL) retuns the file access mode and the file status flags
        //fcntl(fdLog, F_SETFL, flags) sets the file status flags to the value specified

        int openFlags = O_RDWR | O_CREAT | O_APPEND;
        mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        int fdLog = open(logFile, openFlags, filePerms);
        int flags = fcntl(fdLog, F_GETFL);
        flags = flags | O_EXCL;
        fcntl(fdLog, F_SETFL, flags);
        int len=sprintf(buf, "\nprocessMonitor: status:%d The child pid:%d has finished\n", status, pid);
        write(fdLog, buf, len);
        bool isFound=false;
        for(int i=0; i<NUM && isParent && !isFound; ++i) {
                if(pid==childPid[i]) {
                    isFound=true;
                len=sprintf(buf, "processMonitor: Restart the child process\n\n");
            write(fdLog, buf, len);
            flags = flags & ~O_EXCL;
                    //Question 11: What is processMon attempting to do with the log file now?
                    //setting the file status flags to the values specified in flags
                fcntl(fdLog, F_SETFL, flags);
            close(fdLog);
                    childPid[i] = fork();
                    if(childPid[i]==0) {//the child
                        isParent=false;
                        read(fd, key, MAXBUF);
                        close(fd);
                //Question 12: Why are we invoking execlp() again here?
                //first execlp gets ran inside the child process and this one gets ran in the parent process
                        execlp(args[i], args[i], (const char*)key, NULL);
                        cout<<strerror(errno)<<endl;
                    }
                }	
            }
        sleep(1);
            ioctl(fd, KEY_GENERATOR_CHANGE_KEY, NULL);
    }

    return 0;
}
