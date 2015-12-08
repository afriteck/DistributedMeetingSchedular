#include "Logger.h"
#include "Meeting.h"

#include <fstream>
#include <sstream>
using namespace std;

mutex logMutex;

Logger::Logger(string f) : filename(f) { }

void Logger::log(Meeting *meeting, MessageType type)
{
  string msg;
  switch (type) {
    case FOUND_TIME_SLOTS:
      msg = foundTimeSlotsMessage(meeting);
    break;
  }

  logMutex.lock();
  writeToFile(msg);
  logMutex.unlock();
}

void Logger::writeToFile(string msg)
{
  ofstream ofs;
  ofs.open(filename);
  ofs << msg;
  // Don't call close() explicitly; stream is closed when it goes out of scope
}

string Logger::foundTimeSlotsMessage(Meeting *meeting)
{
  stringstream ss;
  ss << "Found " << meeting->possible_times.size() << " open time slots in host's calendar for \""
     << meeting->topic << "\" (duration: " << icaldurationtype_as_ical_string(*meeting->duration) << ", "
     << "deadline " << icaltime_as_ical_string(*meeting->deadline) << ")" << endl;
  ss << "====FOUND TIME SLOTS====" << endl;
  unordered_set<icalperiodtype *>::iterator it;
  for (it = meeting->possible_times.begin(); it != meeting->possible_times.end(); ++it) {
    ss << icalperiodtype_as_ical_string(**it) << endl;
  }
  ss << "========================" << endl;
  return ss.str();
}
