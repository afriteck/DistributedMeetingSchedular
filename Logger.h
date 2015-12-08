#include <string>
using namespace std;

class Meeting;
class Person;

class Logger {
public:
  enum MessageType: int {
    FOUND_TIME_SLOTS,
    SEND_INVITATION
  };
  Logger(string filename);
  void log(Meeting *meeting, Person *person, MessageType type);

private:
  string filename;
  string foundTimeSlotsMessage(Meeting *meeting);
  string sendInvitationMessage(Meeting *meeting, Person *person);
  void writeToFile(string msg);
};
