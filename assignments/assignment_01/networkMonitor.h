//name: Musaddiqur Rahman
//id: 106932189
//networkMonitor.h

#include <iostream>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

//used to handle signals
static void signalHandler(int signum);