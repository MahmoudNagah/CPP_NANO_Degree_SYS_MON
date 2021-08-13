#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;
Process::Process(const int pid) {
  pid_ = pid;
  cpu_utilization_ = Process::CpuUtilization();
}

int Process::Pid() { return pid_; }

string Process::Uid() { return LinuxParser::Uid(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

string Process::Command() { return LinuxParser::Command(pid_); }

float Process::CpuUtilization() { return LinuxParser::CpuUtilization(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  return cpu_utilization_ > a.cpu_utilization_;
}
