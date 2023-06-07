#include <time.h>
#include <string.h>
#include <stdio.h>
#define DATE_STRING_LENGTH 30 // Maximum length of date/time/dayOfWeek string
// const char* ntpServer = "1.th.pool.ntp.org";
// const long  gmtOffset_sec = 7 * 3600;
// const int   daylightOffset_sec = 0;

char* getDateTime() {
    
  static char dateTimeString[DATE_STRING_LENGTH]; // Declare a static char array to hold the date/time/dayOfWeek string
  time_t now = time(NULL);
  struct tm* timeinfo = localtime(&now);

  strftime(dateTimeString, DATE_STRING_LENGTH, "%Y-%m-%d %H:%M:%S", timeinfo); // Format the date/time as a string and store it in the dateTimeString array

  int dayOfWeek = timeinfo->tm_wday; // Get the day of the week as an integer
  const char* dayNames[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  sprintf(dateTimeString + strlen(dateTimeString), " %s", dayNames[dayOfWeek]); // Append the day of the week to the dateTimeString array

  return dateTimeString;
}

char* getDayOfWeek() {
  time_t now = time(NULL);
  struct tm* timeinfo = localtime(&now);
  int dayOfWeek = timeinfo->tm_wday;
  switch (dayOfWeek) {
    case 0:
      return "Sunday";
    case 1:
      return "Monday";
    case 2:
      return "Tuesday";
    case 3:
      return "Wednesday";
    case 4:
      return "Thursday";
    case 5:
      return "Friday";
    case 6:
      return "Saturday";
    default:
      return "Invalid day of week";
  }
}

char* getMonth() {
  time_t now = time(NULL);
  struct tm* timeinfo = localtime(&now);
  int month = timeinfo->tm_mon;
  switch (month) {
    case 0:
      return "January";
    case 1:
      return "February";
    case 2:
      return "March";
    case 3:
      return "April";
    case 4:
      return "May";
    case 5:
      return "June";
    case 6:
      return "July";
    case 7:
      return "August";
    case 8:
      return "September";
    case 9:
      return "October";
    case 10:
      return "November";
    case 11:
      return "December";
    default:
      return "Invalid month";
  }
}