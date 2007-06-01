/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "Date.h"
#include "System.h"
#include "StringTools.h"

using namespace std;
using namespace db::util;

Date::Date()
{
   // get the current time
   mSecondsSinceEpoch = time(NULL);
   gmtime_r(&mSecondsSinceEpoch, &mBrokenDownTime);
}

Date::~Date()
{
}
#include <iostream>
string& Date::format(string& str, const string& format, TimeZone* tz)
{
   string f = format;
   
   // year (4 digit)
   StringTools::regexReplaceAll(f, "\\by{4}\\b", "%Y");
   
   // year (2 digit)
   StringTools::regexReplaceAll(f, "\\by{1,2}\\b", "%y");
   
   // month in year (full)
   StringTools::regexReplaceAll(f, "\\bM{4,}\\b", "%B");
   
   // month in year (abbreviated)
   StringTools::regexReplaceAll(f, "\\bM{1,3}\\b", "%b");
   
   // week in year
   StringTools::regexReplaceAll(f, "\\bw{1,2}\\b", "%U");
   
   // day in year
   StringTools::regexReplaceAll(f, "\\bD{1,3}\\b", "%j");
   
   // day in month
   StringTools::regexReplaceAll(f, "\\bd{1,2}\\b", "%d");
   
   // day in week (full)
   StringTools::regexReplaceAll(f, "\\bE{4,}\\b", "%A");
   
   // day in week (abbreviated)
   StringTools::regexReplaceAll(f, "\\bE{1,3}\\b", "%a");
   
   // AM/PM
   StringTools::regexReplaceAll(f, "\\ba\\b", "%p");
   
   // hour in day (0-24)
   StringTools::regexReplaceAll(f, "\\bH{1,2}\\b", "%H");
   
   // hour in day (1-12)
   StringTools::regexReplaceAll(f, "\\bh{1,2}\\b", "%I");
   
   // minute in hour
   StringTools::regexReplaceAll(f, "\\bm{1,2}\\b", "%M");
   
   // second in minute
   StringTools::regexReplaceAll(f, "\\bs{1,2}\\b", "%S");
   
   struct tm time;
   
   // apply time zone
   if(tz == NULL)
   {
      // get local time
      localtime_r(&mSecondsSinceEpoch, &time);
   }
   else if(tz->getMinutesWest() != 0)
   {
      // remove minutes west and get time
      time_t seconds = mSecondsSinceEpoch - tz->getMinutesWest() * 60UL;
      gmtime_r(&seconds, &time);
   }
   else
   {
      // use stored time
      time = mBrokenDownTime;
   }
   
   // print the time to a string
   unsigned int size = format.length() + 100;
   char out[size];
   memset(out, '\0', size);
   strftime(out, size, f.c_str(), &time);
   str.append(out);
   
   return str;
}

bool Date::parse(const string& str, const string& format)     
{
   // FIXME: implement me
   return false;
}
