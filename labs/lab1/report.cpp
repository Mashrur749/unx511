//name: Musaddiqur Rahman
//id: 106932189




#include <iostream>
#include <dirent.h>
#include <ctype.h>
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>

using namespace std;

bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}


int main(int argc, const char**argv) {

	struct dirent *entry = nullptr; 
	DIR *dp = nullptr;
	dp = opendir(argc > 1 ? argv[1] : "/proc/");
	if (dp != nullptr) {
		while ((entry = readdir(dp))){

			string pName, VmRSS, VmSize;

			bool isValid = false;

			if(is_number(entry->d_name)){
				string fileLocation = string("/proc/") + string(entry->d_name)  + string("/status");
				
					
				ifstream statusFile(fileLocation);
			
				string txt;
				size_t found;


				while (getline(statusFile, txt)){

					if(txt.find("Name") != string::npos){
						pName = txt;	
					}



					if(txt.find("VmRSS") != string::npos){
						int i = 0;
						for(; i < txt.length(); i++){
							if(isdigit(txt[i])){
								break;
							}
						}

						VmRSS = txt.substr(i, txt.length() - (i + 2));
						

						if(stoi(VmRSS) > 10000 ){
							isValid = true;
						}
					}


					if(txt.find("VmSize") != string::npos){
						int i = 0;
						for(; i < txt.length(); i++){
							if(isdigit(txt[i])){
								break;
							}
						}
						

						VmSize = txt.substr(i, txt.length() - (i + 2));
					}


				}

				if(isValid){
					cout << "PID: " << entry->d_name << endl
						<< pName << endl
						<< "VmRSS: " << VmRSS << endl 
						<< "VmSize: " << VmSize << endl<< endl;
				}

			}
		}
	}
    

	closedir(dp);
	return 0;
}


