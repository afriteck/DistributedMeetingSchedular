#include <iostream>
#include <stdlib.h>
#include <string>

#include <libical/ical.h>



using namespace std;

void   promptHostForMeetingSchedule (int&, int&, int&, int&, int&, int&, unsigned int&);
void   initMeetingAttributes        (int&, int&, int&, int&, int&, int&, unsigned int&);
void   responseFromUser             (int&, int&, int&, int&, int&, int&, unsigned int&);
string sendMeetingInfoToAttendeesInANiceFormat(int&, int&, int&, int&, int&, int&, unsigned int&);

int main() {


	/* Make variables to prompt user and pass to it's respective parameters */
	int          year, month, day, hour, minute, seconds;
	unsigned int approxDuration;

	/* prompt host for meeting schedule */
	promptHostForMeetingSchedule(year, month, day, hour, minute, seconds, approxDuration);


	return 0;

}



/* Function to prompt nicely */
void promptHostForMeetingSchedule(int& year, int& month, int& day, int& hour, int& minute, int& second, unsigned int& durationTime) {

	int userResponse;


	cout << endl << endl;
	cout << "=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
	cout << "Welcome to the one and only all time Distributed Meeting Schedular System" << endl;
	cout << "=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl << endl;

	cout << "Choose from the category Operation to peform: " << endl;
	cout << "1. Schedule a Meeting" << endl;
	cout << "2. Exit from the system" << endl << endl;

	cout << "Selection: ";
	cin >> userResponse;

	/* Check for user's response */
	if(userResponse == 1) {

		/* Collect response from user */
		responseFromUser(year, month, day, hour, minute, second, durationTime);

		/* initialize meeting attributes now */
		initMeetingAttributes(year, month, day, hour, minute, second, durationTime);

		/* Call this function here and send this info using this string parameter over the network to each attendee*/
		string meetingInfo = sendMeetingInfoToAttendeesInANiceFormat(year, month, day, hour, minute, second, durationTime);		
		cout << meetingInfo;

	}

	exit(0);	

}


/* Ask user to enter response */
void responseFromUser(int& year, int& month, int& day, int& hour, int& minute, int& second, unsigned int& durationTime) {

	cout << endl << endl;
        cout << "Enter the suggested Meeting time:" << endl << endl;

        cout << "Enter year: ";
        cin >> year;
        cout << endl;

       	cout << "Enter month: ";
      	cin >> month;
      	cout << endl;

     	cout << "Enter day: ";
      	cin >> day;
      	cout << endl;

     	cout << "Enter hour: ";
       	cin >> hour;
      	cout << endl;

      	cout << "Enter minute: ";
      	cin >> minute;
      	cout << endl;

     	cout << "Enter second: ";
      	cin >> second;
     	cout << endl;

    	cout << "Enter duration(mins): ";
      	cin >> durationTime;
     	cout << endl;

}


/* Function to prompt and format stuffs nicely */
void initMeetingAttributes(int& year, int& month, int& day, int& hour, int& minute, int& second, unsigned int& durationTime) {

	/* Make a variable to init all my attrib */
	icaltimetype suggestedTimeByHost;
	
	/* Make a variable to init duration time attribute */
	icaldurationtype duration;


	const char* TZIDCanada = "America/Toronto";

	suggestedTimeByHost.year        = year;
	suggestedTimeByHost.month       = month;
	suggestedTimeByHost.day         = day;
	suggestedTimeByHost.hour        = hour;
	suggestedTimeByHost.minute      = minute;
	suggestedTimeByHost.second      = second;
	suggestedTimeByHost.is_date     = 0;
	suggestedTimeByHost.is_daylight = 0;
	suggestedTimeByHost.zone        = icaltimezone_get_builtin_timezone_from_tzid(TZIDCanada);


	/* Duration of the meeting in minutes */
	duration.minutes = durationTime; 

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
