#include <iostream>
#include <sstream>
#include <libical/ical.h>
#include <libical/icalss.h>

#include "Agent.h"
#include "TimeSlotFinder.h"
#include "CompareTimeSets.h"
using namespace std;

string  sendMeetingInfoToAttendeesInANiceFormat(int&, int&, int&, int&, int&, int&, unsigned int&);
void    promptForInvitees();
void    askHostForMeetingInfo(Meeting* m);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << "usage: ./agent [path-to-ics-file]";
    return -1;
  }

  cout << "==== Distributed Meeting Scheduler ====" << endl;
  cout << "1. Schedule a meeting" << endl;
  cout << "2. Exit" << endl;
  cout << "Enter option: ";

  int option;
  cin >> option;
  cout << endl;

  if (option != 1) {
    return 0;
  }

  Meeting *meeting = new Meeting();
  askHostForMeetingInfo(meeting);
  stringstream ss;
  ss << meeting; //serialize


  
  
  promptForInvitees();

  

  //write (socket, ss.str().c_str(), 20); //send - the buffer size must be adjusted

  const char *path = argv[1];
  icalset *fileset = icalfileset_new(path);
  if (fileset == NULL) {
    cout << "Can't create icalfileset" << endl;
    return -1;
  }

  TimeSlotFinder finder;
  finder.findAvailabilityForMeeting(meeting, fileset);
  cout << "Suggested times for meeting with deadline "
       << icaltime_as_ical_string(*meeting->deadline) << endl;

cout <<*meeting<<endl;
  /* Check if the deadline for the meeting is backwards or doesn't exist */
  int deadlineCheck = icaltime_compare(*meeting->deadline, icaltime_today());

  if(deadlineCheck == -1) {
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
  return 0;
}

void askHostForMeetingInfo(Meeting* m) {
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
  const icaltimezone *tz = icaltimezone_get_builtin_timezone_from_tzid(TZID);
  m->deadline = new icaltimetype(icaltime_from_string(iso_str.c_str()));
  icaltime_set_timezone(m->deadline, tz);
  m->duration = new icaldurationtype(icaldurationtype_from_int(durationInMinutes * 60));
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
void promptForInvitees() {
	int    numberOfInvitees;
	string inviteesIPAddress;

	cout << endl << endl;
	cout << "Enter number of invitees: ";
	cin >> numberOfInvitees;
	cout << endl;

	int    inviteeCounter = 1;
	string inviteesArray[numberOfInvitees];

	for(int i = 0; i < numberOfInvitees; i++) {
		cout << "Enter invitee " << inviteeCounter++ << " IP address: ";
		cin >> inviteesIPAddress;
		cout << endl;

		inviteesArray[i] = inviteesIPAddress;
	}

	cout << "List of invitees IP Addresses: " << endl;

	/* Loop through this array get the string at each index and parse it as a network address and send over the network */
	for (int i = 0; i < sizeof(inviteesArray)/sizeof(inviteesArray[0]); i++) {
		cout << inviteesArray[i] << endl;
	}

	cout << endl << endl;
}
