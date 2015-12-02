#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <thread>
#include <libical/ical.h>
#include <libical/icalss.h>
#include <unistd.h>
#include <mutex>

#include "Agent.h"
#include "TimeSlotFinder.h"
#include "CompareTimeSets.h"
#include "networking.h"
#include "Meeting.h"
using namespace std;

mutex invitationResponse;

const string MEETING_SCHEDULED = "MEETING_SCHEDULED";
const string MEETING_NOT_SCHEDULED = "MEETING_NOT_SCHEDULED";
const string STILL_WORKING = "STILL_WORKING";

string sendMeetingInfoToAttendeesInANiceFormat(int&, int&, int&, int&, int&, int&, unsigned int&);
list<Person *>* promptForInvitees();
Meeting * askHostForMeetingInfo();
void listen(int port, icalset* PATH);
int displayMainMenu();
void doWork(int descriptor, icalset* set);
void sendAllInvitations(list<Person *> *people, Meeting *m, icalset *set);
void invitePersonToMeeting(Person *person, Meeting *meeting, vector<Meeting *> *v);
bool findOpenTimeSlots(Meeting *m, icalset *set);
void saveMeeting(Meeting *meeting, icalset *set);

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

  thread t1(listen, atoi(argv[2]), fileset);
  t1.detach();

  while (displayMainMenu() != 2) {
    Meeting *meeting = askHostForMeetingInfo();
    list<Person *> *people = promptForInvitees();

    if (findOpenTimeSlots(meeting, fileset)) {
      sendAllInvitations(people, meeting, fileset);
    } else {
      continue;
    }

    cout << endl << "Free times with deadline:" << icaltime_as_ical_string(*meeting->deadline) << endl;
  }
  return 0;
}

void sendAllInvitations(list<Person *> *people, Meeting *meeting, icalset *set) {
  vector<thread*> threads;
  vector<Meeting *> *v = new vector<Meeting *>;

  // Send out invitations on separate threads
  for (list<Person*>::iterator it = people->begin(); it != people->end(); ++it) {
    Person *person = *it;
    thread *t = new thread(invitePersonToMeeting, person, meeting, v);
    threads.push_back(t);
  }

  // Wait for replies
  vector<thread *>::iterator it;
  for (it = threads.begin(); it != threads.end(); ++it) {
    thread *t = *it;
    t->join();
  }

  // Find intersection of all replies
  CompareTimeSets c;
  vector<Meeting *>::iterator meeting_it;
  Meeting *m = v->front();
  unordered_set<icalperiodtype *> *free_times = &m->possible_times;
  for (meeting_it = v->begin(); meeting_it != v->end(); ++meeting_it) {
    c.findIntersection(free_times, &(*meeting_it)->possible_times, free_times);
  }

  // Check host calendar to see if intersecting time slot is still available
  TimeSlotFinder finder;
  finder.findAvailabilityForMeeting(meeting, set);
  c.findIntersection(free_times, &meeting->possible_times, free_times);


  while(1) {

    bool notFree = false;

    // Send back rejections or awards
    meeting->possible_times.clear();
    if (!free_times->empty()) {
      meeting->possible_times.insert(*free_times->begin());
    }

    for (list<Person *>::iterator it = people->begin(); it != people->end(); ++it) {
      meeting->option = free_times->empty() ? Meeting::REJECT : Meeting::AWARD;
      Person *person = *it;
      sendMeeting(*meeting, person->descriptor);

      string *messageBackFromAttendee = new string;
      receiveMessage(*messageBackFromAttendee, person->descriptor);

      if(messageBackFromAttendee->compare("free") == 0) {
        cout << endl << "We can hold a meeting" << endl << endl;
      }

      else if(messageBackFromAttendee->compare("not free") == 0) {
        notFree = true;
      }
    }

    if (notFree) {
      icalperiodtype *temp = *free_times->begin();

      string msg = free_times->empty() ? MEETING_NOT_SCHEDULED : STILL_WORKING;
      if (msg == STILL_WORKING) {
        free_times->erase(temp);
      }

      for (list<Person *>::iterator it = people->begin(); it != people->end(); ++it) {
        Person *person = *it;
        sendMessage(msg, person->descriptor);
      }
    } else {
      for(list<Person *>::iterator it = people->begin(); it != people->end(); ++it) {
        Person *person = *it;
        string msg = MEETING_SCHEDULED;
        sendMessage(msg, person->descriptor);
        saveMeeting(meeting, set);
      }
      break;
    }
  }
}

void invitePersonToMeeting(Person *person, Meeting *meeting, vector<Meeting *> *v) {
  cout << "* Sending invitation to " << *person << "...";

  int descriptor = -1;
  connectToServer(const_cast<char*>(person->IP_ADDRESS.c_str()), person->PORT_NUMBER, &descriptor);
  person->descriptor = descriptor;
  sendMeeting(*meeting, descriptor);

  Meeting *responseFromAttendee = new Meeting();
  receiveMeeting(*responseFromAttendee, descriptor);
  invitationResponse.lock();
  v->push_back(responseFromAttendee);
  invitationResponse.unlock();
}

bool findOpenTimeSlots(Meeting *meeting, icalset *set) {

  /* Write results to a file */
  ofstream outfile;
  outfile.open("possibleHostTimes.txt");

  TimeSlotFinder finder;
  finder.findAvailabilityForMeeting(meeting, set);

  outfile << "Suggested times for meeting with deadline: " << icaltime_as_ical_string(*meeting->deadline) << endl << endl;

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

    cout << endl << "My possible free time has been written to a file ..." << endl << endl;
    outfile << "Possible Suggested times by the host: " << endl << endl;

    for (unordered_set<icalperiodtype *>::iterator it = meeting->possible_times.begin();
        it != meeting->possible_times.end();
        ++it) {
    	outfile << icalperiodtype_as_ical_string(**it) << endl;
    }

    /* Close file after writing to file */
    outfile.close();

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
  meeting->option = Meeting::INVITATION;
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

void listen(int port, icalset* PATH) {
    int listenSocket = -1, acceptSocket = -1;
    setupListenSocket(port, &listenSocket);

    NETWORKING_LOG("Accepting connections on port " << port);

    while (1) {
      NETWORKING_LOG("Ready to accept an incoming connection!");
      acceptIncomingConnection(&listenSocket, &acceptSocket);
      NETWORKING_LOG("Connection accepted!");

      thread t1(doWork, acceptSocket, PATH);
      t1.detach();
    }
}

void doWork(int descriptor, icalset* set) {
  /* Open a file in a write mode */
  ofstream outfile;
  outfile.open("hostAndAttendeeFreeTimes.txt");

  Meeting *meeting = new Meeting();
  receiveMeeting(*meeting, descriptor);

  if (meeting->option == Meeting::INVITATION) {
    // Find free times for invitee that are before the deadline
    CompareTimeSets handler;
    unordered_set<icalperiodtype *> free_times;
    handler.CompareSets(meeting, set, &free_times);

    // Send those times back
    meeting->option = Meeting::POSSIBLE_TIMES;
    meeting->possible_times = free_times;

    unordered_set<icalperiodtype *>::iterator it;
    string freeTimes;

    cout << endl << "Writing free time between the host and attendee to file ...." << endl;
    outfile << "Free times between host and attendee: " << endl << endl;

    for (it = free_times.begin(); it != free_times.end(); ++it) {
      string freeTime = icalperiodtype_as_ical_string(**it);

      outfile << freeTime << endl;
      freeTimes += freeTime;
    }

    sendMeeting(*meeting, descriptor);
  }

  while (1) {
    // Wait for award or rejection
    Meeting *meeting2 = new Meeting();
    receiveMeeting(*meeting2, descriptor);
    string result = meeting2->option == Meeting::AWARD ? "Award" : "Rejection";
    //cout << "Meeting " << result << " received" << flush;

    CompareTimeSets handler;
    unordered_set<icalperiodtype *> free_times_final;
    handler.CompareSets(meeting2, set, &free_times_final);

    bool isFree = meeting2->option == Meeting::AWARD && !(free_times_final.empty());
    string msg = isFree ? "free" : "not free";
    sendMessage(msg, descriptor);

    /* Check what we received back from the host */
    string *messageReceivedFromHost = new string;

    receiveMessage(*messageReceivedFromHost, descriptor);

    if (messageReceivedFromHost->compare(STILL_WORKING) == 0) {
      continue;
    }

    else if (messageReceivedFromHost->compare(MEETING_NOT_SCHEDULED) == 0) {
      break;
    }

    else if (messageReceivedFromHost->compare(MEETING_SCHEDULED) == 0) {
      saveMeeting(meeting2, set);
      break;
    }
  }

  /* close file when done writing to file */
  outfile.close();
  close(descriptor);
}

void saveMeeting(Meeting *meeting, icalset *set)
{
  // Open a second set because the original icalset is read-only.
  icalerrorenum error;
  icalset *readWriteSet = icalfileset_new(icalfileset_path(set));
  icalcomponent *component = meeting->to_icalcomponent();
  icalfileset_add_component(set, component);
  icalfileset_commit(set);
  icalfileset_free(readWriteSet);
}

