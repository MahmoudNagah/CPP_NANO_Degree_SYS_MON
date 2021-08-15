#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel,VER;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> VER >>kernel;
  }
  return VER +" "+ kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
  
    if (file->d_type == DT_DIR) {
      
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
    float MemTotal, MemFree;
  string key, value, line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      	std::replace(line.begin(), line.end(), ':', ' ');
		//std::remove(line.begin(), line.end(), ' ');
  		std::istringstream linestream(line);
    	while (linestream >> key >> value ) {
        	if (key == "MemTotal") {
				MemTotal = std::stof(value);
				}
            else if (key == "MemFree") { 
				MemFree = std::stof(value);
				break;
			}
        	}
    	}
  }
  return (MemTotal - MemFree)/MemTotal;
 
  }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
 long uptime_l;
  string uptime, line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    uptime_l = stol(uptime);
  }
  return uptime_l;
}
long LinuxParser::ActiveJiffies(int pid) { 
  string value, line;
  int counter =0;
  long total_clk ,utime ,stime ,cutime ,cstime;
  std::ifstream stream(kProcDirectory + "/"+ std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while ((linestream >> value)) {
			if(counter == 12)
				utime =std::stol(value);
			else if (counter == 13)
				stime = std::stol(value);
			else if (counter ==14)
				cutime= std::stol(value);
			else if (counter == 15)
				cstime=std::stol(value);
			else if (counter > 15)
				break;
		counter ++;
        }
    }
  total_clk = (utime + stime + cutime + cstime);
  return total_clk / sysconf(_SC_CLK_TCK) ;
  }

// TODO: Read and return CPU utilization
vector<long> LinuxParser::CpuUtilization() { 
  std::vector<long> cpuJiffies;
  
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::string cpu;
    filestream >> cpu;
    if (cpu == "cpu") {
      for (int i = 0; i < CPUStates::END; i++) {
        long value;
        filestream >> value;
        cpuJiffies.push_back(value);
      }
    }
  }

  return cpuJiffies;
}


// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key = "", value = "", line = "";
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)){
  		std::istringstream linestream(line);
    	while (linestream >> key >> value) {
        	if (key == "processes") { 
              return std::stoi(value);
              }
        	}
      	}
    }
  return 0;

}
int LinuxParser::RunningProcesses() {
  int num_running_processes;
  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          num_running_processes = stoi(value);
        }
      }
    }
  }
  return num_running_processes;
}



string LinuxParser::Command(int pid) { 
   string value = "" ;
  std::ifstream filestream(kProcDirectory + "/"+std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, value);
    return value;
  }
  else
  {
   return "";
  }
  return "";
}


string LinuxParser::Ram(int pid) { 
   string line, kb, key, value ;
  std::ifstream filestream(kProcDirectory +"/"+ std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      
      while (linestream >> key >> value >> kb ) {
        if (key == "VmSize") {
          return std::to_string(std::stoi(value) / 1024);
        }
      }
    }
  }
  else 
  {
	return "";
  }
  return "";
  	
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
string line, kb, key, value = "";
  std::ifstream filestream(kProcDirectory +"/"+ std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value>>kb) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  else
  {
  return value;
  }
return "";
}

string LinuxParser::User(int pid) {
  string uid ,line,value ,kb,key;
  
  uid = Uid(pid);
  // find user name for this user ID in /etc/passwd
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> value >> kb >> key) {
        if (key == uid) {
          return value;
        }
      }
    }
  }
  else 
  {
  return "";
  }	
  return "";
}


long LinuxParser::UpTime(int pid) { 
   string line, value;
  long uptime = 0;
 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      for (int i = 1; i <= 22; i++) {
        linestream >> value;
        if (i == 22) {

            uptime = std::stol(value) / sysconf(_SC_CLK_TCK);
            return uptime;
        }
      }
    }
  }
  return uptime;
}
float LinuxParser::CpuUtilization(int pid) {
 const int systemUpTimeSeconds = LinuxParser::UpTime();
    const int totalTimeActiveSeconds = LinuxParser::ActiveJiffies(pid);
    const int processUpTimeSeconds = LinuxParser::UpTime(pid);
    
    const int totalTimeSiceStartUp = systemUpTimeSeconds - processUpTimeSeconds;

   float  _cpuUtilization = (1.0*totalTimeActiveSeconds)/totalTimeSiceStartUp;
    return (_cpuUtilization);
}

long LinuxParser::Jiffies() { return 0; }



// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  std::vector <long> cpuJiffies = CpuUtilization();
  
  return cpuJiffies[kUser_] + cpuJiffies[kNice_] + cpuJiffies[kSystem_] + 
         cpuJiffies[kIRQ_] + cpuJiffies[kSoftIRQ_] + cpuJiffies[kSteal_];
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  std::vector <long> cpuJiffies = CpuUtilization();
  return cpuJiffies[kIdle_] + cpuJiffies[kIOwait_];
}
