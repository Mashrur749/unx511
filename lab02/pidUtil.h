//pidUtil.h - Header file for process utility functions
//
// 10-Jan-19  M. Watler         Created.
//

#ifndef PIDUTIL_H
#define PIDUTIL_H

#include <vector>
#include <iostream>

using namespace std;

//Error types. These are indices to GetErrorMsg()
enum ErrStatus {
    Err_OK=0,
    Err_DirOpen,
    Err_NoPid,
    Err_NoName
};

//Returns a list of pids in pidList
ErrStatus GetAllPids(vector<int> &pidList);

//Returns a name for a given pid
ErrStatus GetNameByPid(int pid, string &name);

//Returns a pid for a given name
ErrStatus GetPidByName(string name, int &pid);

//Returns an error message for a given error status
string GetErrorMsg(ErrStatus index);

#endif//PIDUTIL_H
