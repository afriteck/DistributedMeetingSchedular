#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include <string>
#include <unordered_set>
#include <libical/ical.h>

using namespace std;

class Person{
public:
  string  name;
  string  IP_ADDRESS;
  int     PORT_NUMBER;

  Person(string name = "NULL", string ip = "NULL", int port = -1);
  Person(Person& p);
  ~Person();
  friend ostream& operator<<(ostream& out, const Person& obj);
};

class Meeting{
public:
  enum MessageOptions: int {
    POSSIBLE_TIMES,
    INVITATION
  };
  int meetingID;
  MessageOptions option;
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
  string possible_times_as_string() const;
  friend ostream& operator<<(ostream& out, const Meeting& obj);
  friend istream& operator>>(istream& in, Meeting& obj);
};

struct Message{
  Meeting* meeting;
  unordered_set<icalperiodtype*> proposal; //preferred time intervals proposed by the host for the meeting
  unordered_set<icalperiodtype*> response; //time intervals available for an attendee
};
typedef struct Message Message;

#endif
