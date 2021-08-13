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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
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
  string key, value, kb, line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
      	std::replace(line.begin(), line.end(), ':', ' ');
  		std::istringstream linestream(line);
    	while (linestream >> key >> value >> kb) {
        	if (key == "MemTotal") { MemTotal = std::stof(value);}
            if (key == "MemFree") { MemFree = std::stof(value);}
        	}
    	}
  }
  return (MemTotal - MemFree)/MemTotal;
  }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime_long;
  string uptime, idletime;
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
    uptime_long = stol(uptime);
  }
  return uptime_long;
}
long LinuxParser::ActiveJiffies(int pid) { 
  string value, line;
  long total_clk;
  vector<string> stat_list;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
			stat_list.push_back(value);
        }
    }
  long int utime{std::stol(stat_list[13])};
  long int stime{std::stol(stat_list[14])};
  long int cutime{std::stol(stat_list[15])};
  long int cstime{std::stol(stat_list[16])};
  total_clk = (utime + stime + cutime + cstime);
  return total_clk;}

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
  int val;
  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
  		std::istringstream linestream(line);
    	while (linestream >> key >> value) {
        	if (key == "processes") { 
              val = std::stoi(value);
              return val;
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
  string cmd, line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
  		std::istringstream linestream(line);
      	std::replace(line.begin(), line.end(), ' ', '*');
    	while (linestream >> cmd) {
			std::replace(cmd.begin(), cmd.end(), '*', ' ');
          	return cmd;
        }
      }
    }
  return string();
}


string LinuxParser::Ram(int pid) { 
  string key, value, kb, line;
  int vmsize;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
  		std::istringstream linestream(line);
    	while (linestream >> key >> value >> kb) {
        	if (key == "VmSize:") { 
              	vmsize = std::stoi(value); 
              	vmsize = vmsize/1000;
              	return std::to_string(vmsize);
               }
        	}
      	}
    }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string key, value, uid, user, line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
  		std::istringstream linestream(line);
    	while (linestream >> key >> value) {
        	if (key == "Uid:") { 
              	uid = value;
               }
        	}
      	}
    }
  return uid;
}

string LinuxParser::User(int pid) {
  string usr, passwd, uid, line;
  string uid_ = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)){
        std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
    	while (linestream >> usr >> passwd >> uid) {
               if (uid==uid_){
                return usr;
               }
        	}
      	}
    }
  return string();
}


long LinuxParser::UpTime(int pid) { 
  long upTimeInClockTicks = 0;

  std::stringstream path;
  path << kProcDirectory << pid << kStatFilename;
    
  std::ifstream filestream(path.str());
  if (filestream.is_open()) {

    const int position = 22;
    std::string value;
    for (int i = 0; i < position; i++) {
      if (!(filestream >> value)) {
        return 10000;
      }
    }
    upTimeInClockTicks = std::stol(value);
  }
  
  return upTimeInClockTicks/sysconf(_SC_CLK_TCK);
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
