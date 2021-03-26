//name: Musaddiqur Rahman
//id: 106932189
//intfMonitor.cpp - An interface monitor.

#include "intfMonitor.h"

using namespace std;

const int MAXBUF = 64;
bool isRunning = true;
char socket_path[] = "/tmp/networkMon";
bool is_running;
const int BUF_LEN = 200;
string data;
int skfd = -1; /* AF_INET socket for ioctl() calls.*/
int set_if_flags(char *ifname, short flags)
{
	struct ifreq ifr;
	int res = 0;
	ifr.ifr_flags = flags;
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("socket error %s\n", strerror(errno));
		res = 1;
		goto out;
	}

	res = ioctl(skfd, SIOCSIFFLAGS, &ifr);
	if (res < 0)
	{
		printf("Interface '%s': Error: SIOCSIFFLAGS failed: %s\n",
			   ifname, strerror(errno));
	}
	else
	{

		printf("Interface '%s': LINK UP\n", ifname);
	}
out:
	return res;
}

int set_if_up(char *ifname, short flags)
{
	return set_if_flags(ifname, flags | IFF_UP);
}


//gets all the required interface info
void getInterfaceStat(char *interface, string &data)
{
	//gather some stats
	string operstate;
	int carrier_up_count = 0;
	int tx_bytes = 0;
	int rx_bytes = 0;
	int rx_dropped = 0;
	int rx_errors = 0;
	int tx_packets = 0;
	int rx_packets = 0;
	int tx_dropped = 0;
	int tx_errors = 0;
	char statPath[2 * MAXBUF];

	ifstream infile;

	sprintf(statPath, "/sys/class/net/%s/operstate", interface);
	infile.open(statPath);
	if (infile.is_open())
	{
		infile >> operstate;
		infile.close();
	}

	sprintf(statPath, "/sys/class/net/%s/carrier_up_count", interface);
	infile.open(statPath);
	if (infile.is_open())
	{
		infile >> carrier_up_count;
		infile.close();
	}

	sprintf(statPath, "/sys/class/net/%s/statistics/tx_bytes", interface);
	infile.open(statPath);
	if (infile.is_open())
	{
		infile >> tx_bytes;
		infile.close();
	}

	sprintf(statPath, "/sys/class/net/%s/statistics/rx_bytes", interface);
	infile.open(statPath);
	if (infile.is_open())
	{
		infile >> rx_bytes;
		infile.close();
	}

	sprintf(statPath, "/sys/class/net/%s/statistics/rx_dropped", interface);
	infile.open(statPath);
	if (infile.is_open())
	{
		infile >> rx_dropped;
		infile.close();
	}

	sprintf(statPath, "/sys/class/net/%s/statistics/rx_errors", interface);
	infile.open(statPath);
	if (infile.is_open())
	{
		infile >> rx_errors;
		infile.close();
	}

	sprintf(statPath, "/sys/class/net/%s/statistics/tx_packets", interface);
	infile.open(statPath);
	if (infile.is_open())
	{
		infile >> tx_packets;
		infile.close();
	}

	sprintf(statPath, "/sys/class/net/%s/statistics/tx_dropped", interface);
	infile.open(statPath);
	if (infile.is_open())
	{
		infile >> tx_dropped;
		infile.close();
	}

	sprintf(statPath, "/sys/class/net/%s/statistics/tx_errors", interface);
	infile.open(statPath);
	if (infile.is_open())
	{
		infile >> tx_errors;
		infile.close();
	}

	sprintf(statPath, "/sys/class/net/%s/statistics/rx_packets", interface);
	infile.open(statPath);
	if (infile.is_open())
	{
		infile >> rx_packets;
		infile.close();
	}

	if (operstate == "down")
	{
		set_if_up(interface, 1);
	}

	//communicating through socket

	string tmpInterface = string(interface);
	string tmpOperstate = string(operstate);

	data = "Interface: " + tmpInterface + " state: " + tmpOperstate + " up_count: " + to_string(carrier_up_count) + "\n" +
		   +" rx_bytes: " + to_string(rx_bytes) + " rx_dropped: " + to_string(rx_dropped) + " rx_errors: " + to_string(rx_errors) +
		   " rx_packets: " + to_string(rx_packets) + " \n tx_bytes: " + to_string(tx_bytes) +
		   " tx_dropped: " + to_string(tx_dropped) + " tx_errors: " + to_string(tx_errors) + " \n\n";
}

int main(int argc, char *argv[])
{
	//start socket communication
	//gathering the stats

	char interface[MAXBUF];
	int retVal = 0;

	strncpy(interface, argv[1], MAXBUF);
	//Set up socket communications
	struct sockaddr_un addr;
	char buf[BUF_LEN];
	int len, ret;
	int fd, rc;

	cout << "client(" << getpid() << "): running..." << endl;
	memset(&addr, 0, sizeof(addr));
	//Create the socket
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		cout << "client(" << getpid() << "): " << strerror(errno) << endl;
		exit(-1);
	}

	addr.sun_family = AF_UNIX;
	//Set the socket path to a local socket file
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

	//Connect to the local socket
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		cout << "client(" << getpid() << "): " << strerror(errno) << endl;
		close(fd);
		exit(-1);
	}

	is_running = true;

	len = sprintf(buf, "Ready");
	bzero(buf, BUF_LEN);
	ret = write(fd, buf, BUF_LEN);

	ret = read(fd, buf, BUF_LEN); //Read something from the server
	if (ret < 0)
	{
		cout << "client(" << getpid() << "): error reading the socket" << endl;
		cout << strerror(errno) << endl;
	}

	cout << "Received message from Network Monitor: " << buf << endl;
	while (is_running)
	{

		if (strcmp(buf, "Quit") == 0)
		{ //Server requests the client to terminate
			cout << "client(" << getpid() << "): received request to quit" << endl;
			is_running = false;
		}

		//gets all the required interface info and stores them in 'data' variable
		getInterfaceStat(interface, data);


		bzero(buf, BUF_LEN);
		strcpy(buf, data.c_str());
		cout << buf;
		buf[len] = '\0';
		ret = write(fd, buf, BUF_LEN); //The client writes the data
		if (ret == -1)
		{
			cout << "client: data Write Error" << endl;
			cout << strerror(errno) << endl;
		}

		sleep(1);
	}

	return 0;
}
