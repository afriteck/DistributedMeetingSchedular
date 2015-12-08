#include <string>
using namespace std;

class Meeting;

class Logger {
public:
  enum MessageType: int {
    FOUND_TIME_SLOTS
  };
  Logger(string filename);
  void log(Meeting *meeting, MessageType type);

private:
  string filename;
  string foundTimeSlotsMessage(Meeting *meeting);
  void writeToFile(string msg);
};
