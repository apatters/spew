//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewConsole.cpp
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

using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "common.h"
#include "SpewConsole.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Local Constants()  ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const char WRITE_HACK_STR[] = "w-";
static const char END_WRITE_HACK_STR[] = "w";
static const char READ_HACK_STR[] = "r-";
static const char END_READ_HACK_STR[] = "r";
static const char ERROR_HACK_STR[] = "E-";
static const char END_ERROR_HACK_STR[] = "E";
static const char NO_HACK_STR[] = "  ";
static const char END_NO_HACK_STR[] = " ";

static const unsigned int NON_VERTICAL_HACK_ROWS = 5;
static const unsigned int MIN_VERTICAL_HACK_ROWS = 1;
static const unsigned int NON_HORIZONTAL_HACK_COLUMNS = 22;
static const unsigned int MIN_HORIZONTAL_HACK_COLUMNS = 2;


//////////////////////////  SpewConsole::SpewConsole()  ///////////////////////
SpewConsole::SpewConsole(unsigned int iterationsToDo,
                         Units_t units,
                         bool showProgress,
                         Verbosity_t verbosity): 
   SpewDisplay(iterationsToDo, 
               units,
               showProgress,
               verbosity)
{
   mCurrentScreenRows = DEFAULT_SCREEN_ROWS;
   mCurrentScreenColumns = DEFAULT_SCREEN_COLUMNS;
}


//////////////////////////  SpewConsole::init()  //////////////////////////////
int SpewConsole::init()
{
   mCurrentScreenRows = this->getScreenRows();
   mCurrentScreenColumns = this->getScreenColumns();

   return 0;
}


//////////////////////////  SpewConsole::close() //////////////////////////////
int SpewConsole::close()
{
   fflush(stdout);
   fflush(stderr);
   return 0;
}


//////////////////////////  SpewConsole::resize()  ////////////////////////////
int SpewConsole::resize()
{
   return 0;
}


////////////////  SpewConsole::getCurrentNumVerticalHacks()  //////////////////
unsigned int SpewConsole::getCurrentNumVerticalHacks() const 
{
   int rows;
   rows = max((int)mCurrentScreenRows - (int)NON_VERTICAL_HACK_ROWS,
              (int)MIN_VERTICAL_HACK_ROWS);  

   return rows;
}


////////////////  SpewConsole::getCurrentNumHorizontalHacks()  ////////////////
unsigned int SpewConsole::getCurrentNumHorizontalHacks() const 
{
   int cols;
   cols = max((int)mCurrentScreenColumns - (int)NON_HORIZONTAL_HACK_COLUMNS,
              (int)MIN_HORIZONTAL_HACK_COLUMNS);
   return cols/strlen(NO_HACK_STR);
}


//////////////////////////  SpewConsole::hack()  //////////////////////////////
void SpewConsole::hack()
{
   switch (mCurrentIoDirection)
   {
   case READING:
      printf("%s", READ_HACK_STR);
      break;
   case WRITING:
      printf("%s", WRITE_HACK_STR);
      break;
   }
   fflush(stdout);
}


//////////////////////////  SpewConsole::endHack()  ///////////////////////////
void SpewConsole::endHack()
{
   switch (mCurrentIoDirection)
   {
   case READING:
      printf("%s", END_READ_HACK_STR);
      break;
   case WRITING:
      printf("%s", END_WRITE_HACK_STR);
      break;
   }
   fflush(stdout);
}


//////////////////////////  SpewConsole::errorHack()  /////////////////////////
void SpewConsole::errorHack()
{
   printf("%s", ERROR_HACK_STR);
   fflush(stdout);
}


//////////////////////////  SpewConsole::errorEndHack()  //////////////////////
void SpewConsole::errorEndHack()
{
   printf("%s", END_ERROR_HACK_STR);
   fflush(stdout);
}


//////////////////////////  SpewConsole::noHack()  ////////////////////////////
void SpewConsole::noHack()
{
   printf("%s", NO_HACK_STR);
   fflush(stdout);
}


//////////////////////////  SpewConsole::noEndHack()  /////////////////////////
void SpewConsole::noEndHack()
{
   printf("%s", END_NO_HACK_STR);
   fflush(stdout);
}


//////////////////////////  SpewConsole::nextHackRow()  ///////////////////////
void SpewConsole::nextHackRow() 
{
   printf("\n");
   fflush(stdout);
}


///////////////  SpewConsole::intermediateStatistics()  ///////////////////////
void SpewConsole::intermediateStatistics(
   capacity_t hackRowBytesTransferred,
   const TimeHack& hackRowTransferTime,
   capacity_t jobBytesTransferred,
   const TimeHack& jobTransferTime,
   capacity_t bytesInJob,
   capacity_t totalBytesRead,
   const TimeHack& totalReadTransferTime,
   capacity_t totalBytesWritten,
   const TimeHack& totalWriteTransferTime,
   const TimeHack& totalRunTime)
{
   long double transferRate = convertCapacity(hackRowBytesTransferred, mCurrentUnits)/(long double)hackRowTransferTime.getTime();
   long double percentage = (long double)jobBytesTransferred/(long double)bytesInJob*100.0;

   printf(" %3.Lf%%", percentage);
   printf(" %11.2Lf %-5s", 
          transferRate, getTransferRateUnitsStr(mCurrentUnits));
   fflush(stdout);
}


/////////////////  SpewConsole::cumulativeStatistics()  ///////////////////////
void SpewConsole::cumulativeStatistics(capacity_t jobBytesTransferred,
                                       const TimeHack& jobTransferTime,
                                       capacity_t totalBytesRead,
                                       const TimeHack& totalReadTransferTime,
                                       capacity_t totalReadOps,
                                       capacity_t totalBytesWritten,
                                       const TimeHack& totalWriteTransferTime,
                                       capacity_t totalWriteOps,
                                       const TimeHack& totalRunTime)
{
   if (mVerbosity == VERBOSITY_NONE)
      return;

   long double transferRate = convertCapacity((long double)jobBytesTransferred, mCurrentUnits)/(long double)jobTransferTime.getTime();

   if (mIterationsToDo != 1)
   {
      printf("Itereration: %8d    Total runtime: %s\n",
             mCurrentIteration, totalRunTime.getElapsedTimeStr().c_str());     
   }
   switch (mCurrentIoDirection)
   {
   case READING:
      printf("RTR: %11.2Lf %-5s   Tranfser time: %s    IOPS: %11.2Lf\n",
             transferRate, 
             getTransferRateUnitsStr(mCurrentUnits), 
             jobTransferTime.getElapsedTimeStr().c_str(),
             (long double)totalReadOps/(long double)jobTransferTime.getTime());  
      break;
   case WRITING:
      printf("WTR: %11.2Lf %-5s   Transfer time: %s    IOPS: %11.2Lf\n",
             transferRate, 
             getTransferRateUnitsStr(mCurrentUnits), 
             jobTransferTime.getElapsedTimeStr().c_str(),
             (long double)totalWriteOps/(long double)jobTransferTime.getTime());  
      break;
   }
   fflush(stdout);
}


//////////////////////////  SpewConsole::startJob()  //////////////////////////
void SpewConsole::startJob(unsigned int iteration, IoDirection_t direction) 
{
   SpewDisplay::startJob(iteration, direction);
}


//////////////////////////  SpewConsole::nextJob()  ///////////////////////////
void SpewConsole::nextJob() 
{
   if (mVerbosity == VERBOSITY_NONE && !mShowProgress)
      return;

   if (mIterationsToDo != 1 || mShowProgress)
   {
      printf("\n");
      fflush(stdout);
   }
}


//////////////////////////  SpewConsole::~SpewConsole()  //////////////////////
SpewConsole::~SpewConsole()
{
}


///////////////////  SpewConsole::getScreenRows()  ////////////////////////////
unsigned int SpewConsole::getScreenRows() const
{
   unsigned int screenRows = DEFAULT_SCREEN_ROWS;
   char *errPtr;

   const char *lines = getenv("LINES");
   if (lines)
   {
      long int tmpval = strtol(lines, &errPtr, 10);
      if (*errPtr == '\0')
          screenRows = tmpval;
   }

   return screenRows;
}


////////////////  SpewConsole::getScreenColumns()  ////////////////////////////
unsigned int SpewConsole::getScreenColumns() const 
{
   unsigned int screenCols = DEFAULT_SCREEN_COLUMNS;
   char *errPtr;

   const char *cols = getenv("COLUMNS");
   if (cols)
   {
      long int tmpval = strtol(cols, &errPtr, 10);
      if (*errPtr == '\0')
          screenCols = tmpval;
   }
   return screenCols;
}
