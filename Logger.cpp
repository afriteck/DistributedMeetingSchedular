#include "Logger.h"

#include <fstream>
using namespace std;

mutex logMutex;

Logger::Logger(string f) : filename(f) { }

void Logger::log(string msg)
{
  logMutex.lock();
  ofstream ofs;
  ofs.open(filename);
  ofs << msg;
  ofs.close();
  logMutex.unlock();
}
