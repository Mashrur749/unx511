
//intfMonitor.cpp - An interface monitor
//
// 05-Feb-20  M. Watler         Created.

#include <fcntl.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

const int MAXBUF=64;
bool isRunning=true;

int main(int argc, char *argv[])
{
    char interface[MAXBUF];
    char statPath[2*MAXBUF];
    const char logfile[]="Network.log";//store network data in Network.log
    int retVal=0;

    strncpy(interface, argv[1], MAXBUF);
    int fd=open(logfile, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    cout<<"intfMonitor:main: interface:"<<interface<<":  pid:"<<getpid()<<endl;

    while(isRunning) {
        //gather some stats
        int tx_bytes=0;
        int rx_bytes=0;
        int tx_packets=0;
        int rx_packets=0;
		ifstream infile;
        sprintf(statPath, "/sys/class/net/%s/statistics/tx_bytes", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile >> tx_bytes;
			infile.close();
		}
		
		sprintf(statPath, "/sys/class/net/%s/statistics/rx_bytes", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>rx_bytes;
			infile.close();
		}
		
		sprintf(statPath, "/sys/class/net/%s/statistics/tx_packets", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>tx_packets;
			infile.close();
		}
		
		sprintf(statPath, "/sys/class/net/%s/statistics/rx_packets", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>rx_packets;
			infile.close();
		}
		char data[3*MAXBUF];
		//write the stats into Network.log
		int len=sprintf(data, "%s: tx_bytes:%d rx_bytes:%d tx_packets:%d rx_packets: %d\n", interface, tx_bytes, rx_bytes, tx_packets, rx_packets);
		//write(fd, data, len);
		cout << data;
		sleep(1);
    }
    close(fd);

    return 0;
}

