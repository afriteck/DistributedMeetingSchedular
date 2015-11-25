#include "Entity.h"

using namespace std;

Person::Person(string n, string ip, int port): name(n), IP_ADDRESS(ip), PORT_NUMBER(port){}

Person::Person(Person& p){
	this->name = p.name;
	this->IP_ADDRESS = p.IP_ADDRESS;
	this->PORT_NUMBER = p.PORT_NUMBER;
}
Person::~Person(){}



/*Meeting::Meeting() {
}

Meeting::Meeting(int id, std::string tp, std::unordered_set<Person*> att, Person* h, icaldurationtype* dur, float pr, std::unordered_set<icalperiodtype*> pt, icaltimetype* dl){
	meetingID = id;
	topic = tp;
	attendees = att;
	host = h;
	duration = dur;
	priority = pr;
	possible_times = pt;
	deadline = dl;

       
}



Meeting::~Meeting(){
	if(host)
		delete host;
	if(duration)
		delete duration;
	if(deadline)
		delete deadline;
}*/


