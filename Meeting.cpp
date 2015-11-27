#include <iostream>
#include <string>

#include "Entity.h"

Meeting::Meeting(){}

Meeting::~Meeting(){
 if(host)
   delete host;
 if(duration)
   delete duration;
 if(deadline)
  delete deadline;
}

Meeting::Meeting(int id,string tp,unordered_set<Person*> att, Person* h, icaldurationtype* dur, float pr,unordered_set<icalperiodtype*> pt, icaltimetype* dl): meetingID(id), topic(tp), attendees(att), host(h), duration(dur), priority(pr), possible_times(pt), deadline(dl) {}

ostream& operator<<(ostream& os, const Meeting& m) {
  // Spaces are needed to separate elements
  os << m.meetingID << " " << m.topic << " " << " ";
  os << icaldurationtype_as_ical_string(*m.duration) << " ";
  unordered_set<icalperiodtype *>::const_iterator itr;
  for (itr = m.possible_times.begin(); itr != m.possible_times.end(); ++itr) {
    os << icalperiodtype_as_ical_string(**itr);
  }
  os << " " << icaltime_as_ical_string(*m.deadline);
  return os;
}
