//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// TimeHack.cpp
//
// Spew
//
// Copyright (C) 2004 Hewlett-Packard Corp.
//
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation version 2 of the License.
// 
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.

namespace std {} using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <time.h>
#include <string>

#include "common.h"
#include "TimeHack.h"

static const size_t MAX_STR_CHARS = 1024;
static const size_t TIMESTAMP_STR_LEN = 19;  // Does not includes space for \0.

///////////////////////  TimeHack::getCurrentTime()  //////////////////////////
TimeHack::timehack_t TimeHack::getCurrentTime()
{
   struct timezone tz;
   struct timeval nowTime;
   gettimeofday(&nowTime, &tz);

   return (timehack_t)nowTime.tv_sec + (timehack_t)nowTime.tv_usec/1000000.0;
}

//////////////////////////  TimeHack::TimeHack()  /////////////////////////////
TimeHack::TimeHack()
{
   mTime = 0.0;
}


//////////////////////////  TimeHack::TimeHack()  /////////////////////////////
TimeHack::TimeHack(timehack_t hack)
{
   mTime = hack;
}


//////////////////////////  TimeHack::TimeHack()  /////////////////////////////
TimeHack::TimeHack(const TimeHack& hack)
{
   mTime = hack.mTime;
}


//////////////////////////  TimeHack::operator=() /////////////////////////////
TimeHack& TimeHack::operator=(const TimeHack& hack)
{
   mTime = hack.mTime;
   return *this;
}


//////////////////////////  TimeHack::operator=() /////////////////////////////
TimeHack& TimeHack::operator=(timehack_t hack)
{
   mTime = hack;
   return *this;
}


///////////////////////  TimeHack::operator-()  ///////////////////////////////
TimeHack::timehack_t TimeHack::operator-(const TimeHack& hack)
{
   return mTime - hack.mTime;
}


///////////////////////  TimeHack::operator-()  ///////////////////////////////
TimeHack::timehack_t TimeHack::operator-(timehack_t hack)
{
   return mTime - hack;
}


///////////////////////  TimeHack::operator-=()  //////////////////////////////
TimeHack& TimeHack::operator-=(const TimeHack& hack)
{
   mTime -= hack.mTime;
   return *this;
}


///////////////////////  TimeHack::operator-=()  //////////////////////////////
TimeHack& TimeHack::operator-=(timehack_t hack)
{
   mTime -= hack;
   return *this;
}


///////////////////////  TimeHack::operator+()  ///////////////////////////////
TimeHack::timehack_t TimeHack::operator+(const TimeHack& hack)
{
   return mTime + hack.mTime;
}


///////////////////////  TimeHack::operator+()  ///////////////////////////////
TimeHack::timehack_t TimeHack::operator+(timehack_t hack)
{
   return mTime + hack;
}


///////////////////////  TimeHack::operator+=()  /////////////////////////////
TimeHack& TimeHack::operator+=(const TimeHack& hack)
{
   mTime += hack.mTime;
   return *this;
}


///////////////////////  TimeHack::operator+=()  /////////////////////////////
TimeHack& TimeHack::operator+=(timehack_t hack)
{
   mTime += hack;
   return *this;
}


///////////////////////  TimeHack::getTime()  /////////////////////////////////
TimeHack::timehack_t TimeHack::getTime() const
{
   return mTime;
}


///////////////////////  TimeHack::setTimeNow()  //////////////////////////////
void TimeHack::setTimeNow()
{
   mTime = getCurrentTime();
}


///////////////////////  TimeHack::setTime()  /////////////////////////////////
void TimeHack::setTime(const TimeHack& hack)
{
   mTime = hack.mTime;
}


///////////////////////  TimeHack::setTime()  /////////////////////////////////
void TimeHack::setTime(time_t time)
{
   mTime = (long double)time;
}


///////////////////////  TimeHack::setTime()  /////////////////////////////////
void TimeHack::setTime(timehack_t time)
{
   mTime = time;
}


///////////////////////  TimeHack::setTime()  /////////////////////////////////
void TimeHack::setTime(const struct timeval tv)
{
   mTime = (timehack_t)tv.tv_sec + (timehack_t)tv.tv_usec/1000000.0;
}


///////////////////////  TimeHack::getTimeStampStr()  /////////////////////////
string TimeHack::getTimeStampStr() const
{
   char tmpStr[MAX_STR_CHARS + 1];

   struct tm currentBrokenDownTime;
   time_t t = (time_t)mTime;
   localTime(&t, &currentBrokenDownTime);
   strftime(tmpStr, 
            TIMESTAMP_STR_LEN + 1,
            "%Y/%m/%d %T", 
            &currentBrokenDownTime);

   return string(tmpStr);
}


///////////////////////  TimeHack::getTimeDiffStr()  //////////////////////////
string TimeHack::getTimeDiffStr(timehack_t elapsed) const
{
   char tmpStr[MAX_STR_CHARS + 1];
   int hours, minutes, seconds;
   timehack_t rem;
   timehack_t diff = elapsed - mTime;

   if (diff < 0.0L)
      diff = -diff;

   hours = (int)(diff/3600);
   rem =  diff - (hours * 3600.0);
   minutes = (int)(rem/60);
   rem -= minutes * 60;
   seconds = (int)rem;

   sprintf(tmpStr, "%02d:%02d:%02d", hours, minutes, seconds);

   return string(tmpStr);
}


///////////////////////  TimeHack::getTimeDiffStr()  //////////////////////////
string TimeHack::getTimeDiffStr(const TimeHack& elapsed) const
{
   return this->getTimeDiffStr(elapsed.mTime);
}


///////////////////////  TimeHack::getElapsedTimeStr()  ///////////////////////
string TimeHack::getElapsedTimeStr() const
{
   return this->getTimeDiffStr(0.0);
}


//////////////////////////  TimeHack::~TimeHack()  ////////////////////////////
TimeHack::~TimeHack()
{
}


