#include <iostream>
#include <libical/ical.h>
#include <libical/icalvalue_cxx.h>
using namespace std;

int main() {
  LibICal::ICalValue *value = new LibICal::ICalValue();
  cout << "libical works!" << endl;
  delete value;
  return 0;
}
