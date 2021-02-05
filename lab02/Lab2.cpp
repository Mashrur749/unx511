//Name: Musaddiqur Rahman
//ID: 106932189

#include <iostream>
#include "pidUtil.h"
#include <vector>
#include <string.h>
using namespace std;

int main(){
	vector <int> pids;

	string tmpPidName;

	cout << "Part A - 1"  << endl  << "----------" << endl << endl;

	
	if(GetAllPids(pids) != 0){
		cout << GetErrorMsg(GetAllPids(pids));
	}


	//Part A - 1

	for(vector<int>::iterator it = pids.begin(); it != pids.end(); ++it){
		
		GetNameByPid(*it, tmpPidName);
		
		cout << *it << ": " << tmpPidName << endl; 

	}

	//Part A - 2

	cout << endl << "Part A - 2"  << endl << "----------" << endl<< endl;


	int pid = 1;
	
	if(GetNameByPid(pid, tmpPidName) != 0) {
		cout << GetErrorMsg(GetNameByPid(pid, tmpPidName)) << endl;
	}

	cout << "pid " << pid << ": " << tmpPidName << endl;


	//Part A - 3 
	
	cout << endl << "Part A - 3"  << endl  << "----------"  << endl << endl;

	tmpPidName = "Lab2";

	if(GetPidByName(tmpPidName, pid) != 0){
		cout << GetErrorMsg(GetPidByName(tmpPidName, pid)) << endl;
	}

	cout << tmpPidName << ": " << pid << endl;
	
	//Part A - 4
	
	cout << endl << "Part A - 4" << endl << "----------" << endl << endl;
	
	tmpPidName = "Lab22";

	if(GetPidByName(tmpPidName, pid) != 0){
		cout << GetErrorMsg(GetPidByName(tmpPidName, pid)) << endl;
	}



	
	
	


}

