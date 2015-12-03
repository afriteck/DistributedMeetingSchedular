#include "Entity.h"
#include "Meeting.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

Meeting::Meeting() : meetingID(0), topic("") {}

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


/*Append an astericks to spaces string */
string Meeting::topic_as_string_astericks(const string& topic) const {

  string appendAstericks = topic;
  replace(appendAstericks.begin(), appendAstericks.end(), ' ', '*');

  return appendAstericks;
}


string Meeting::topic_as_string_noastericks(string& topic) {

  string removeAstericks = topic;
  replace(removeAstericks.begin(), removeAstericks.end(), '*', ' ');

  return removeAstericks;
}


ostream& operator<<(ostream& os, const Meeting& m) {

  os << m.meetingID << " ";
  os << m.topic_as_string_astericks(m.topic) <<  " ";
  os << m.option << " ";
  os << icaldurationtype_as_ical_string(*m.duration) << " ";
  os << m.possible_times_as_string() << " ";
  os << icaltime_as_ical_string(*m.deadline);
  return os;
}

istream& operator>>(istream &in, Meeting& obj) {
  

  in >> obj.meetingID;

  string meetingTopicWithoutAstericks;
  in >> meetingTopicWithoutAstericks;
  obj.topic = obj.topic_as_string_noastericks(meetingTopicWithoutAstericks);

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

icalcomponent * Meeting::to_icalcomponent()
{
  icalcomponent *event = icalcomponent_new_vevent();
  // Prevent setting summary to empty string since this generates an X-LIC-ERROR
  icalcomponent_set_summary(event, topic.empty() ? "Untitled Event" : topic.c_str());
  icalcomponent_set_dtstart(event, (*possible_times.begin())->start);
  icalcomponent_set_dtend(event, (*possible_times.begin())->end);
  return event;
}
