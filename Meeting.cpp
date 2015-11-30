#include "Entity.h"
#include "Meeting.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

Meeting::Meeting() : meetingID(0) {}

Meeting::~Meeting(){
 if(host)
   delete host;
 if(duration)
   delete duration;
 if(deadline)
  delete deadline;
}

Meeting::Meeting(int id,string tp,unordered_set<Person*> att, Person* h, icaldurationtype* dur, float pr,unordered_set<icalperiodtype*> pt, icaltimetype* dl): meetingID(id), topic(tp), attendees(att), host(h), duration(dur), priority(pr), possible_times(pt), deadline(dl) {}

string Meeting::possible_times_as_string() const {
  stringstream ss;
  unordered_set<icalperiodtype *>::const_iterator itr;
  for (itr = possible_times.begin(); itr != possible_times.end(); ++itr) {
    // Asterisks used to separate individual periods
    ss << icalperiodtype_as_ical_string(**itr) << "*";
  }
  string str = ss.str();
  str.pop_back(); // Delete final asterisk
  return str;
}

unordered_set<icalperiodtype *> string_to_possible_times(string str) {
  unordered_set<icalperiodtype *> possible_times;
  replace(str.begin(), str.end(), '*', ' ');
  string possible_time;
  istringstream iss(str);
  while (iss >> possible_time) {
    possible_times.insert(new icalperiodtype(icalperiodtype_from_string(possible_time.c_str())));
  }
  return possible_times;
}

ostream& operator<<(ostream& os, const Meeting& m) {
  os << m.meetingID << " ";
  os << m.option << " ";
  os << icaldurationtype_as_ical_string(*m.duration) << " ";
  os << m.possible_times_as_string() << " ";
  os << icaltime_as_ical_string(*m.deadline);
  return os;
}

istream& operator>>(istream &in, Meeting& obj) {
  in >> obj.meetingID;

  int optionAsInt;
  in >> optionAsInt;
  obj.option = (Meeting::MessageOptions)optionAsInt;

  string durationStr;
  in >> durationStr;
  obj.duration = new icaldurationtype(icaldurationtype_from_string(durationStr.c_str()));

  string possibleTimesStr;
  in >> possibleTimesStr;
  obj.possible_times = string_to_possible_times(possibleTimesStr);

  string deadlineStr;
  in >> deadlineStr;
  obj.deadline = new icaltimetype(icaltime_from_string(deadlineStr.c_str()));
  return in;
}
