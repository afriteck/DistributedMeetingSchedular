#include <iomanip>
#include <iostream>
#include <string>
#include <istream>
#include <ostream>

#include "Entity.h"

using namespace std;


 

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














