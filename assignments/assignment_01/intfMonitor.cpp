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
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

using namespace std;

const int MAXBUF=64;
bool isRunning=true;



int skfd = -1;      /* AF_INET socket for ioctl() calls.*/
int set_if_flags(char *ifname, short flags)
{
	struct ifreq ifr;
        int res = 0;
	ifr.ifr_flags = flags;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	       	printf("socket error %s\n", strerror(errno));
	        res = 1;
	        goto out; 
	}
	
	res = ioctl(skfd, SIOCSIFFLAGS, &ifr);	
	if (res < 0) {         
		printf("Interface '%s': Error: SIOCSIFFLAGS failed: %s\n",
				ifname, strerror(errno));
	} else {
		
		printf("Interface '%s': flags set to %04X.\n", ifname, flags);
	}
out:
       	return res;
}


int set_if_up(char *ifname, short flags)
{	    
	return set_if_flags(ifname, flags | IFF_UP);
}



int main(int argc, char *argv[])
{
    char interface[MAXBUF];
    char statPath[2*MAXBUF];
    int retVal=0;

    strncpy(interface, argv[1], MAXBUF);

    while(isRunning) {
	    //gather some stats
	    	std::string operstate;
	    	int carrier_up_count=0;	
		int tx_bytes = 0;	
		int rx_bytes = 0;		
		int rx_dropped = 0;
		int rx_errors = 0;
      		int tx_packets = 0;
       		int rx_packets = 0;
		int tx_dropped = 0;
		int tx_errors = 0;

		ifstream infile;
      
	      	sprintf(statPath, "/sys/class/net/%s/operstate", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>operstate;
			infile.close();
		}

       
	       	sprintf(statPath, "/sys/class/net/%s/carrier_up_count", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>carrier_up_count;
			infile.close();
		}
       
	       	sprintf(statPath, "/sys/class/net/%s/statistics/tx_bytes", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>tx_bytes;
			infile.close();
		}
		
		sprintf(statPath, "/sys/class/net/%s/statistics/rx_bytes", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>rx_bytes;
			infile.close();
		}

		sprintf(statPath, "/sys/class/net/%s/statistics/rx_dropped", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>rx_dropped;
			infile.close();
		}
				
		sprintf(statPath, "/sys/class/net/%s/statistics/rx_errors", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>rx_errors;
			infile.close();
		}		
		
		sprintf(statPath, "/sys/class/net/%s/statistics/tx_packets", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>tx_packets;
			infile.close();
		}
		
		sprintf(statPath, "/sys/class/net/%s/statistics/tx_dropped", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>tx_dropped;
			infile.close();
		}

		sprintf(statPath, "/sys/class/net/%s/statistics/tx_errors", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>tx_errors;
			infile.close();
		}

		sprintf(statPath, "/sys/class/net/%s/statistics/rx_packets", interface);
		infile.open(statPath);
		if(infile.is_open()) {
			infile>>rx_packets;
			infile.close();
		}
		

		if(operstate == "down"){
			set_if_up(interface, 1);
		}

		
    		cout<<"interface: "<<interface
			<< " state: " << operstate 
			<< " up_count: " << carrier_up_count
			<< endl
			<< " rx_bytes: " << rx_bytes
			<< " rx_dropped: " << rx_dropped
			<< " rx_errors: " << rx_errors
			<< " rx_packets: " << rx_packets
			<< endl 
			<< " tx_bytes" << tx_bytes
			<< " tx_dropped" << tx_dropped
			<< " tx_Errors: " << tx_errors
			<< endl <<  endl;



//		char data[3*MAXBUF];

		//int len=sprintf(data, "%s: tx_bytes:%d rx_bytes:%d tx_packets:%d rx_packets: %d\n", interface, tx_bytes, rx_bytes, tx_packets, rx_packets);
		
		//cout << data;

		sleep(1);
    }

    return 0;
}
