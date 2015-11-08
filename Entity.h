#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <unordered_set>
#include <libical/icalperiod.h>

class Person{
public:
	std::string 	name;
	std::string 	IP_ADDRESS;
	int				PORT_NUMBER;

	Person(std::string name = "NULL", std::string ip = "NULL", int port = -1);
	Person(Person& p);
	~Person();
};

class Meeting{
public:
	int meetingID; //meeting ID
	std::string topic; //topic of the meeting
	std::unordered_set<Person*> attendees; //set of attendees
	Person* host; //host of the meeting
	icaldurationtype* duration; //length of the meeting
	float priority; //priority of the meeting btw 0 - 1
	std::unordered_set<icalperiodtype*>  possible_times; /* possible starting times on the calendar for the meeting */
	icaltimetype* deadline; //deadline of the meeting
	
	/* Constructor for meeting */
	/* parameters are : meetingID
						topic
						attendees
						host
						duration
						priority
						possible_times
						deadline
	*/
	Meeting(int, std::string, std::unordered_set<Person*>, Person*, icaldurationtype*, float, std::unordered_set<icalperiodtype*>, icaltimetype*);
	~Meeting();

};

struct Message{
	Meeting* meeting;
	std::unordered_set<icalperiodtype*> proposal; //preferred time intervals proposed by the host for the meeting
	std::unordered_set<icalperiodtype*> response; //time intervals available for an attendee
};
typedef struct Message Message;
	
#endif
