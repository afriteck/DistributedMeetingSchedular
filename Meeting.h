#include <unordered_set>

#include <libical/ical.h>

class Person;

class Meeting {
public:
  enum MessageOptions: int {
    POSSIBLE_TIMES,
    INVITATION,
    AWARD,
    REJECT
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
  string topic_as_string_noastericks(string&);
  string topic_as_string_astericks() const;
  string topic_as_string_noastericks();
  icalcomponent * to_icalcomponent();
  friend ostream& operator<<(ostream& out, const Meeting& obj);
  friend istream& operator>>(istream& in, Meeting& obj);
};
