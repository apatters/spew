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
#include <string.h>

#include "common.h"
#include "SpewConsole.h"
#include "TimeHack.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Local Constants()  ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const char WRITE_HACK_STR[] = "w-";
static const char END_WRITE_HACK_STR[] = "w";
static const char READ_HACK_STR[] = "r-";
static const char END_READ_HACK_STR[] = "r";
static const char ERROR_HACK_STR[] = "E-";
static const char END_ERROR_HACK_STR[] = "E";
static const char NO_HACK_STR[] = "Aa";
static const char END_NO_HACK_STR[] = "B";

static const unsigned int NON_VERTICAL_PROGRESS_ROWS = 5;
static const unsigned int MIN_VERTICAL_PROGRESS_ROWS = 1;
static const unsigned int NON_HORIZONTAL_PROGRESS_COLUMNS = 22;
static const unsigned int MIN_HORIZONTAL_PROGRESS_COLUMNS = 2;


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


////////////////  SpewConsole::getCurrentProgressRows()  //////////////////////
unsigned int SpewConsole::getCurrentProgressRows() const 
{
   int rows;
   rows = max((int)mCurrentScreenRows - (int)NON_VERTICAL_PROGRESS_ROWS,
              (int)MIN_VERTICAL_PROGRESS_ROWS);  

   return rows;
}


////////////////  SpewConsole::getCurrentProgressColumns()  ///////////////////
unsigned int SpewConsole::getCurrentProgressColumns() const 
{
   int cols;
   cols = max((int)mCurrentScreenColumns - (int)NON_HORIZONTAL_PROGRESS_COLUMNS, (int)MIN_HORIZONTAL_PROGRESS_COLUMNS);
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


//////////////////////////  SpewConsole::nextProgressRow()  ///////////////////
void SpewConsole::nextProgressRow() 
{
   printf("\n");
   fflush(stdout);
}


///////////////  SpewConsole::intermediateStatistics()  ///////////////////////
void SpewConsole::intermediateStatistics(const JobStatistics *jobStats,
                                         const CumulativeStatistics *cumStats,
                                         const TimeHack& currentTime,
                                         const TimeHack& startTime)
{

   long double progressRowTransferTime = jobStats->getIntervalEndTime() - jobStats->getIntervalStartTime();
   long double transferRate = convertCapacity(jobStats->getIntervalBytesTransferred(), mCurrentUnits)/progressRowTransferTime;
   long double percentage = (long double)jobStats->getJobBytesTransferred()/(long double)jobStats->getBytesInJob()*100.0;

   printf(" %3.Lf%%", percentage);
   printf(" %11.2Lf %-5s", 
          transferRate, getTransferRateUnitsStr(mCurrentUnits));
   fflush(stdout);
}


/////////////////  SpewConsole::cumulativeStatistics()  ///////////////////////
void SpewConsole::cumulativeStatistics(const JobStatistics *jobStats,
                                       const CumulativeStatistics *cumStats,
                                       const TimeHack& totalRunTime)
{
   if (mVerbosity == VERBOSITY_NONE)
      return;

   TimeHack jobTransferTime = jobStats->getJobEndTime() - jobStats->getJobStartTime();

   long double transferRate = convertCapacity((long double)jobStats->getJobBytesTransferred(), mCurrentUnits)/jobTransferTime.getTime();

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
             (long double)cumStats->getTotalReadOps()/(long double)jobTransferTime.getTime());  
      break;
   case WRITING:
      printf("WTR: %11.2Lf %-5s   Transfer time: %s    IOPS: %11.2Lf\n",
             transferRate, 
             getTransferRateUnitsStr(mCurrentUnits), 
             jobTransferTime.getElapsedTimeStr().c_str(),
             (long double)cumStats->getTotalWriteOps()/(long double)jobTransferTime.getTime());  
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
