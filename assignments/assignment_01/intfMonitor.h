//name: Musaddiqur Rahman
//id: 106932189
//intfMonitor.h
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
#include <signal.h>
#include <sys/un.h>

//used to set interface flags
int set_if_flags(char *ifname, short flags);

//used to turn on network interface
int set_if_up(char *ifname, short flags);

//gets all the required stats from the interface files and stores to 'data' variable
void getInterfaceStat(char *interface, std::string &data);
