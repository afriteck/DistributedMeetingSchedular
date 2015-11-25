#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <unordered_set>
//#include <iostream>
#include <istream>
#include <ostream>
//#include "ThorSerialize/Traits.h"
//#include "ThorSerialize/SerUtil.h"
//#include "ThorSerialize/JsonThor.h"
//#include <json/json.h>
//#include "Serializable.h"
#include <libical/icalperiod.h>

using namespace std;

class Person{
public:
	string 	name;
	string 	IP_ADDRESS;
	int				PORT_NUMBER;

	Person(string name = "NULL", string ip = "NULL", int port = -1);
	Person(Person& p);
	~Person();
        friend ostream& operator<< (ostream& out,Person& obj) {
       out <<obj.name <<" " <<obj.IP_ADDRESS <<" "<<obj.PORT_NUMBER;

        }
};

class Meeting{
public:

        int meetingID;
        string topic;
        unordered_set<Person*> attendees;
        Person* host;
        icaldurationtype* duration; 
        float priority;
        unordered_set<icalperiodtype*> possible_times;
        icaltimetype* deadline;
	
	Meeting();
	Meeting(int, string, unordered_set<Person*>, Person*, icaldurationtype*, float, unordered_set<icalperiodtype*>, icaltimetype*);
	~Meeting();
        friend ostream& operator<< (ostream& out, Meeting& obj) {
        
 out << obj.meetingID << " " << obj.topic<< " "
  <<" "<<obj.duration<< " ";
  for(unordered_set<icalperiodtype*>:: iterator itr=obj.possible_times.begin();itr!=obj.possible_times.end(); ++itr) out<<icalperiodtype_as_ical_string(**itr);
out<< " "<<icaltime_as_ical_string(*obj.deadline);   
  //The space (" ") is necessary for separete elements
   return out;
}

/*friend istream& operator>> (istream& in, Meeting& obj) {
 in >> obj.meetingID >> " " >> obj.topic>> " "
  >>" ">>obj.duration>> " ";
  for(unordered_set<icalperiodtype*>:: iterator itr=obj.possible_times.begin();itr!=obj.possible_times.end(); ++itr) in>>icalperiodtype_as_ical_string(**itr);
in>> " "<<icaltime_as_ical_string(*obj.deadline); 
}*/
        

};

struct Message{
	Meeting* meeting;
	unordered_set<icalperiodtype*> proposal; //preferred time intervals proposed by the host for the meeting
	unordered_set<icalperiodtype*> response; //time intervals available for an attendee
};
typedef struct Message Message;

#endif
