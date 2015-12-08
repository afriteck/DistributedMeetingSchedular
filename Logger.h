#include <string>
using namespace std;

class Logger {
public:
  Logger(string filename);
  void log(string msg);

private:
  string filename;
};
