//memMonitor.cpp - memory monitor
//
// 11-Jan-21  M. Watler         Created.
//
#include <iostream>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include "memoryUsage.h"

using namespace std;

const int MAXBUF=64;
bool isRunning;

static void sigHandler(int sig) {
    switch(sig) {
        case SIGINT:
            cout<<"memMonitor: SIGINT received"<<endl;
            isRunning = false;
            break;
        case SIGKILL:
            cout<<"memMonitor: SIGKILL ignored"<<endl;
            break;
    }
}

int main(int argc, char *argv[])
{
    char key1[MAXBUF], key2[MAXBUF];
    int fd, fdOut;
    int valid;

    strncpy(key1, argv[1], MAXBUF);
    fd=open(keyFile, O_RDWR);
    //Question 13: This appears to be reading a file under /dev. What is it really doing?
    //this accesses the /dev/keygen file, and tries to read MAXBUF bytes data from that file descriptor and store it in key2 variable 
    read(fd, key2, MAXBUF);
    close(fd);
    valid=strcmp(key1, key2);
    if(valid!=0) {
        cout<<"Invalid key, aborting..."<<endl;
    } else {
        //Question 14: What happens if memMonitor receives a SIGKILL?
        //SIGKILL and SIGSTOP signals cannot be handled hence, if memMonitor receives SIGKILL it'll by default kill the process 
        //Question 15: What happens if memMonitor receives a ctrl-C?
        //receives a SIGINT signal
        //and prints "memMonitor: SIGINT received"
        struct sigaction action;
        action.sa_handler = sigHandler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction(SIGINT, &action, NULL);

        //Question 16: Describe these open flags
        //O_RDWR -> read write
        //O_CREAT -> create the file if it doesn't exist
        //O_APPEND -> the file gets openned in append mode, so write function will append content
        int openFlags = O_RDWR | O_CREAT | O_APPEND;
        //Question 17: Describe these file permissions
        // S_IRUSR ->  grants read permission to the users
        // S_IWUSR ->  grants write permission to the users
        // S_IRGRP ->  grants read permission to groups
        // S_IWGRP ->  grants write permission to groups
        // S_IROTH ->  grants read permission to others
        // S_IWOTH ->  grants write permission to others
        mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
        fdOut = open(logFile, openFlags, filePerms);
        string modelName, cpuMhz;
        string MemTotal, MemFree, MemAvailable;
        string procRunning, procBlocked;
        string uptime, idle;
        ifstream infile;
	isRunning = true;

        while(isRunning) {
            // The file /proc/cpuinfo contains a collection of CPU and
            // system architecture dependent items
            infile.open("/proc/cpuinfo");
            if(infile.is_open()) {
                string data;
                bool dataFound = false;
                while(!dataFound && !infile.eof()) {
                    infile >> data;
                    // Search for model name
                    if(data=="model") {
                        infile >> data;
                        if(data=="name") {
                            infile >> data;
                            getline(infile, modelName);
                        }
                    // Search for cpu speed
                    } else if(data=="cpu") {
                        infile >> data;
                        if(data=="MHz") {
                            infile >> cpuMhz;
                            infile >> cpuMhz;
                            dataFound = true;
                        }
                    }
                }
                infile.close();
            }

            // The file /proc/meminfo reports statistics about memory usage on the system.
            infile.open("/proc/meminfo");
            if(infile.is_open()) {
                string data;
                infile >> data;
                infile >> MemTotal;
                infile >> data;
                infile >> data;
                infile >> MemFree;
                infile >> data;
                infile >> data;
                infile >> MemAvailable;
                infile.close();
            }

            // The file /proc/stat reports kernel/system statistics
            infile.open("/proc/stat");
            if(infile.is_open()) {
            string data;
                bool dataFound = false;
                while(!dataFound && !infile.eof()) {
                    infile >> data;
                    if(data=="procs_running") {
                        infile >> procRunning;
                    } else if(data=="procs_blocked") {
                        infile >> procBlocked;
                        dataFound = true;
                    }
                }
                infile.close();
            }
    
            // The file /proc/uptime contains the uptime of the system (seconds),
            // and the amount of time spent in idle process (seconds)
            infile.open("/proc/uptime");
            if(infile.is_open()) {
                infile >> uptime;
                infile >> idle;
                infile.close();
            }
    
            // Display all stats and then sleep
            char buf[MAXBUF];
	    int len=sprintf(buf, "Model Name: %s CPU MHz: %s\n", modelName.c_str(), cpuMhz.c_str());
	    write(fdOut, buf, len);
	    len=sprintf(buf, "Mem Total: %s Mem Free: %s Mem Avail:%s\n", MemTotal.c_str(), MemFree.c_str(), MemAvailable.c_str());
	    write(fdOut, buf, len);
	    len=sprintf(buf, "Procs Running: %s Procs Blocked: %s\n", procRunning.c_str(), procBlocked.c_str());
	    write(fdOut, buf, len);
	    len=sprintf(buf, "System Uptime: %s System Idle: %s\n\n", uptime.c_str(), idle.c_str());
	    write(fdOut, buf, len);
            sleep(1);
        }
        close(fdOut);
    }

    return 0;
}
