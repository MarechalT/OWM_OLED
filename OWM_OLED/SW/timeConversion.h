#ifndef TIMECONVERSION_H_
#define TIMECONVERSION_H_
#include <time.h>

char* getHumanTime(const char* timestamp){
  time_t ts = (time_t) strtol(timestamp,NULL,10);
  struct tm *date;
  char buf[20];
  // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
  date = localtime(&ts);
  strftime(buf, sizeof(buf), "%R", date);
  return buf;
}

#endif
