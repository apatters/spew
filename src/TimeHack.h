//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// TimeHack.h
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

#ifndef TIMEHACK_H
#define TIMEHACK_H



#include <time.h>
#include <sys/time.h>
#include <string>


///////////////////////////////////////////////////////////////////////////////
///////////////////////////  Class TimeHack  //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TimeHack
{
public:
   typedef long double timehack_t;

public:
   static timehack_t getCurrentTime();

public:
   TimeHack();
   TimeHack(timehack_t hack);
   TimeHack(const TimeHack& hack);

   TimeHack& operator=(const TimeHack& hack);
   TimeHack& operator=(timehack_t hack);
   timehack_t operator-(const TimeHack& hack);
   timehack_t operator-(timehack_t hack);
   TimeHack& operator-=(const TimeHack& hack);
   TimeHack& operator-=(timehack_t hack);
   timehack_t operator+(const TimeHack& hack);
   timehack_t operator+(timehack_t hack);
   TimeHack& operator+=(const TimeHack& hack);
   TimeHack& operator+=(timehack_t hack);

   timehack_t getTime() const;

   void setTimeNow();
   void setTime(const TimeHack& hack);
   void setTime(time_t time);
   void setTime(timehack_t time);
   void setTime(const struct timeval tv);

   string getTimeStampStr() const;
   string getTimeDiffStr(const TimeHack& elapsed) const;
   string getTimeDiffStr(timehack_t elapsed) const;
   string getElapsedTimeStr() const;
   
   ~TimeHack();

private:
   timehack_t mTime;
};

#endif // TIMEHACK_H
