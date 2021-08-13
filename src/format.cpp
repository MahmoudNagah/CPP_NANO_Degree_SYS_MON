#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long total_time) {
  long hours = total_time / 3600;
  long remaining_seconds = total_time % 3600;
  long minutes = remaining_seconds / 60;
  long seconds = remaining_seconds % 60;
  string hours_str = std::to_string(hours);
  string minutes_str = std::to_string(minutes);
  string seconds_str = std::to_string(seconds);
  string time = string(2 - hours_str.length(), '0') + hours_str + ':' +
                string(2 - minutes_str.length(), '0') + minutes_str + ':' +
                string(2 - seconds_str.length(), '0') + seconds_str;
  return time;
}