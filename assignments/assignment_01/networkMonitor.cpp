#include <iostream>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>


using namespace std;

char socket_path[]="/tmp/networkMon";
const int BUF_LEN=100;
const int MAX_CLIENTS=5;
bool is_running;

static void signalHandler(int signum) {
       	switch(signum) {
		case SIGINT:
			cout<<"signalHandler("<<signum<<"): SIGINT"<<endl;
			is_running=false;
		       	break;
	       	default:
		       	cout<<"signalHandler("<<signum<<"): unknown"<<endl;
	}
}


int main(){


	//ask user input
	int numOfInterfaces;
	
	string *intNames = nullptr;

	cout << "Number of interfaces to be monitored: ";

	cin >> numOfInterfaces;

	intNames = new string[numOfInterfaces];
	

	for(int i = 0; i < numOfInterfaces; i++){
		cout << endl  << "Interface " << i+1 << " name: ";		
		cin >> intNames[i]; 
	}

	for(int i = 0; i < numOfInterfaces; i++){
		cout << intNames[i]; 
	}
	

	//setup socket connection
	




	//use select to monitor communication




	





}



