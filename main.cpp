#include <iostream>
#include <list>
#include <sstream>
#include <thread>
#include <libical/ical.h>
#include <libical/icalss.h>
#include <unistd.h>

#include "Agent.h"
#include "TimeSlotFinder.h"
#include "CompareTimeSets.h"
#include "networking.h"
#include "Entity.h"
using namespace std;

#define NETWORK_DEBUG 0

string sendMeetingInfoToAttendeesInANiceFormat(int&, int&, int&, int&, int&, int&, unsigned int&);
list<Person *>* promptForInvitees();
Meeting * askHostForMeetingInfo();
void listen(int port);
int displayMainMenu();
void doWork(int descriptor);
void invitePeopleToMeeting(list<Person *> *people, Meeting *m);
bool findOpenTimeSlots(Meeting *m, icalset *set);

int main(int argc, char *argv[]) {
  if (argc < 3) {
    cout << "usage: ./agent [path-to-ics-file] [port-number]";
    return -1;
  }

  icalset *fileset = icalfileset_new_reader(argv[1]);
  if (fileset == NULL) {
    cout << "Can't create icalfileset" << endl;
    return -1;
  }

  thread t1(listen, atoi(argv[2]));
  t1.detach();

  const char *path = argv[1];

  while (displayMainMenu() != 2) {
    Meeting *meeting = askHostForMeetingInfo();
    list<Person *> *people = promptForInvitees();

    if (findOpenTimeSlots(meeting, fileset)) {
      invitePeopleToMeeting(people, meeting);
    } else {
      continue;
    }

    /** Checking the attendee's free times **/
    path = "test-data/spanlist2.ics";
    fileset = icalfileset_new(path);
    if (fileset == NULL) {
      cout << "Can't create icalfileset" << endl;
      return -1;
    }

    CompareTimeSets handler;
    unordered_set<icalperiodtype *> free_times;
    handler.CompareSets(meeting, fileset, &free_times);

    cout << "\nFree times with deadline:"
    << icaltime_as_ical_string(*meeting->deadline) << endl;
    for (unordered_set<icalperiodtype *>::iterator it = free_times.begin();
     it != free_times.end();
     ++it) {
      cout << "- " << icalperiodtype_as_ical_string(**it) << endl;
    }

  }
  return 0;
}

void invitePeopleToMeeting(list<Person *> *people, Meeting *meeting) {
  for (list<Person*>::iterator it = people->begin(); it != people->end(); ++it) {
    Person *person = *it;
    cout << "* Sending invitation to " << *person << endl;
    int descriptor = -1;
    connectToServer(const_cast<char*>(person->IP_ADDRESS.c_str()), person->PORT_NUMBER, &descriptor);
  }
}

bool findOpenTimeSlots(Meeting *meeting, icalset *set) {
  TimeSlotFinder finder;
  finder.findAvailabilityForMeeting(meeting, set);
  cout << "Suggested times for meeting with deadline "
  << icaltime_as_ical_string(*meeting->deadline) << endl;

cout <<*meeting<<endl;
  /* Check if the deadline for the meeting is backwards or doesn't exist */
  int deadlineCheck = icaltime_compare(*meeting->deadline, icaltime_today());

  if (meeting->deadline->month < 1 || meeting->deadline->month > 12 ||
      meeting->deadline->day < 1 || meeting->deadline->day > 31 ||
      meeting->deadline->hour < 0 || meeting->deadline->hour > 23 ||
      meeting->deadline->minute < 0 || meeting->deadline->minute > 59 ||
      meeting->deadline->second < 0 || meeting->deadline->second > 59) {
    cout << "Can't schedule meeting because of an invalid date" << endl;
  }

  else if (deadlineCheck == -1) {
    cout << "This meeting cannot be scheduled due to invalid date!" << endl;
  }

  else if (deadlineCheck == 0) {
    cout << "Schdeuling same day doesn't guarantee other invitees to make the meeting, therefore it can't be scheduled" << endl;
  }

  else {
    for (unordered_set<icalperiodtype *>::iterator it = meeting->possible_times.begin();
        it != meeting->possible_times.end();
        ++it) {
         cout << "- " << icalperiodtype_as_ical_string(**it) << endl;
    }
    return true;
  }
  return false;
}

Meeting * askHostForMeetingInfo() {
  string year, month, day, hour, minute;
  int durationInMinutes;

  cout << "Please enter the latest possible date that the meeting can be held." << endl;
  cout << "  Year: ";
  cin >> year;
  cout << "  Month: ";
  cin >> month;
  cout << "  Day: ";
  cin >> day;
  cout << "  Hour: ";
  cin >> hour;
  cout << "  Minute: ";
  cin >> minute;

  cout << endl << "Desired meeting length (minutes): ";
  cin >> durationInMinutes;

  stringstream ss;
  ss << year << month << day << "T" << hour << minute << "00";
  string iso_str = ss.str();

  const char* TZID = "/freeassociation.sourceforge.net/America/Toronto";
  icaltimezone *tz = icaltimezone_get_builtin_timezone_from_tzid(TZID);

  Meeting *meeting = new Meeting();
  meeting->duration = new icaldurationtype(icaldurationtype_from_int(durationInMinutes * 60));
  meeting->deadline = new icaltimetype(icaltime_from_string(iso_str.c_str()));
  icaltime_set_timezone(meeting->deadline, tz);
  return meeting;
}

string sendMeetingInfoToAttendeesInANiceFormat(int& year, int& month, int& day, int& hour, int& minute, int& second, unsigned int& durationTime) {
  /* make a new instance of icaltimetype and retrieve user info */
  string intro  = "Meeting schdule information:";
  intro         += '\n';
  string sYears = "Scheduled Year: " + to_string(year);
  sYears        += '\n';
  string sMonth = "Scheduled Month: " + to_string(month);
  sMonth        += '\n';
  string sDay   = "Scheduled Day: " + to_string(day);
  sDay          += '\n';
  string sHour  = "Scheduled Hour: " + to_string(hour);
  sHour         += '\n';
  string sMin   = "Scheduled Minute: " + to_string(minute);
  sMin          += '\n';
  string sSec   = "Scheduled Seconds: " + to_string(second);
  sSec          += '\n';
  string sDur   = "Scheduled Duration Time: " + to_string(durationTime);
  sDur          += '\n';

  /* This should be the string to send to attendees over the network */
  string deployStringToAttendee = intro + sYears + sMonth + sDay + sHour + sMin + sSec + sDur;
  return deployStringToAttendee;
}

/* Prompt the user to know how many invitees to intvite to the meeting */
list<Person *> * promptForInvitees() {
  int    numberOfInvitees;
  string inviteesIPAddress;
  int portNumber;

  cout << "Enter number of invitees: ";
  cin >> numberOfInvitees;

  list<Person *> *l = new list<Person *>();

  for (int i = 0; i < numberOfInvitees; i++) {
    stringstream ss;
    ss << "Invitee " << i + 1;
    string name = ss.str();

    cout << "  Enter " << name << " IP address: ";
    cin >> inviteesIPAddress;
    cout << "  Enter " << name << " port number: ";
    cin >> portNumber;
    cout << endl;

    Person *person = new Person(name, inviteesIPAddress, portNumber);
    l->push_back(person);
  }
  return l;
}

int displayMainMenu() {
  cout << "==== Distributed Meeting Scheduler ====" << endl;
  cout << "1. Schedule a meeting" << endl;
  cout << "2. Exit" << endl;
  cout << "Enter option: ";

  int option;
  cin >> option;
  return option;
}

void listen(int port) {
    int listenSocket = -1, acceptSocket = -1;
    setupListenSocket(port, &listenSocket);

    #ifdef NETWORK_DEBUG
    cout << "Accepting connections on port " << port << endl;
    #endif

    while (1) {
      #ifdef NETWORK_DEBUG
      cout << "Ready to accept an incoming connection!" << endl;
      #endif

      acceptIncomingConnection(&listenSocket, &acceptSocket);

      #ifdef NETWORK_DEBUG
      cout << "Connection accepted!" << endl;
      #endif

      thread t1(doWork, acceptSocket);
      t1.detach();
    }
}

void doWork(int descriptor) {
  int i = 0;
  while (i < 5) {
    sleep(1);
    #ifdef NETWORK_DEBUG
    cout << "Work #" << i << endl;
    #endif
    ++i;
  }
  close(descriptor);
}
