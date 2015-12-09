#include "Logger.h"
#include "Meeting.h"
#include "Entity.h"

#include <fstream>
#include <sstream>
using namespace std;

mutex logMutex;

Logger::Logger(string f) : filename(f) { }

void Logger::log(Meeting *meeting, Person *person, MessageType type)
{
  string msg;
  switch (type) {
    case FOUND_TIME_SLOTS:
      msg = foundTimeSlotsMessage(meeting);
      break;
    case SEND_INVITATION:
      msg = sendInvitationMessage(meeting, person);
      break;
    case RECEIVED_INVITATION:
      msg = receivedInvitationMessage(meeting);
      break;
    case SEND_INVITATION_REPLY:
      msg = sendInvitationReplyMessage(meeting);
      break;
    case RECEIVED_INVITATION_REPLY:
      msg = receivedInvitationReplyMessage(meeting, person);
      break;
  }

  logMutex.lock();
  writeToFile(msg);
  writeToFile("\n");
  logMutex.unlock();
}

void Logger::writeToFile(string msg)
{
  ofstream ofs;
  ofs.open(filename, ios_base::app | ios_base::out);
  ofs << msg;
  // Don't call close() explicitly; stream is closed when it goes out of scope
}

string periodtype_set_to_string(unordered_set<icalperiodtype *> set)
{
  stringstream ss;
  unordered_set<icalperiodtype *>::iterator it;
  for (it = set.begin(); it != set.end(); ++it) {
    ss << icalperiodtype_as_ical_string(**it) << endl;
  }
  return ss.str();
}

string Logger::foundTimeSlotsMessage(Meeting *meeting)
{
  stringstream ss;
  ss << "Found " << meeting->possible_times.size() << " open time slots in host's calendar for "
     << meeting->meeting_as_log_string() << endl;

  ss << "====FOUND TIME SLOTS====" << endl;
  ss << periodtype_set_to_string(meeting->possible_times);
  ss << "========================" << endl;
  return ss.str();
}

string Logger::sendInvitationMessage(Meeting *meeting, Person *person)
{
  stringstream ss;
  ss << "Inviting " << *person << " to " << meeting->meeting_as_log_string() << endl;

  ss << "====SUGGESTED TIMES FROM HOST====" << endl;
  ss << periodtype_set_to_string(meeting->possible_times);
  ss << "=================================" << endl;
  return ss.str();
}

string Logger::receivedInvitationMessage(Meeting *meeting)
{
  stringstream ss;
  ss << "Received invitation for " << meeting->meeting_as_log_string() << endl;

  ss << "====TIME SLOTS SENT BY HOST====" << endl;
  ss << periodtype_set_to_string(meeting->possible_times);
  ss << "===============================" << endl;
  return ss.str();
}

string Logger::sendInvitationReplyMessage(Meeting *meeting)
{
  stringstream ss;
  ss << "Replied to invitation for " << meeting->meeting_as_log_string() << endl;

  ss << "====SLOTS WHERE INVITEE WAS AVAILABLE====" << endl;
  ss << periodtype_set_to_string(meeting->possible_times);
  ss << "=========================================" << endl;
  return ss.str();
}

string Logger::receivedInvitationReplyMessage(Meeting *meeting, Person *person)
{
  stringstream ss;
  ss << "Received invitation reply from " << *person << " for " << meeting->meeting_as_log_string() << endl;

  ss << "====SLOTS WHERE INVITEE WAS AVAILABLE====" << endl;
  ss << periodtype_set_to_string(meeting->possible_times);
  ss << "=========================================" << endl;
  return ss.str();
}
