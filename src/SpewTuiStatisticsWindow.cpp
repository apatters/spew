//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewTuiStatisticsWindow.cpp
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

#include <stdio.h>
#include <stdarg.h>
#include <ncurses.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common.h"
#include "SpewTui.h"
#include "SpewTuiWindow.h"
#include "SpewTuiStatisticsWindow.h"

///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Local Constants()  ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const int COLUMN_1_ST   = 0;
static const int COLUMN_1_END =  15;
static const int COLUMN_2_ST =  17;
static const int COLUMN_2_END = 36;
static const int COLUMN_3_ST =  45;
static const int COLUMN_3_END=  58;
static const int COLUMN_4_ST =  60;
static const int COLUMN_4_END=  70;

static const char ITERATION_LABEL[] = "Iteration:";
static const int ITERATION_LABEL_STARTX = COLUMN_1_ST;
static const int ITERATION_LABEL_STARTY = 0;
static const int ITERATION_LABEL_WIDTH = COLUMN_1_END - COLUMN_1_ST + 1;
static const char ITERATION_FIELD_WRITING_IND[] = "writing";
static const char ITERATION_FIELD_READING_IND[] = "reading";
static const int ITERATION_FIELD_STARTX = COLUMN_2_ST;
static const int ITERATION_FIELD_STARTY = ITERATION_LABEL_STARTY;
static const int ITERATION_FIELD_WIDTH = COLUMN_2_END - COLUMN_2_ST + 1;

static const char RUNTIME_LABEL[] = "Total run time:";
static const int RUNTIME_LABEL_STARTX = COLUMN_3_ST;
static const int RUNTIME_LABEL_STARTY = 0;
static const int RUNTIME_LABEL_WIDTH = COLUMN_3_END - COLUMN_3_ST + 1;
static const int RUNTIME_FIELD_STARTX = COLUMN_4_ST;
static const int RUNTIME_FIELD_STARTY = RUNTIME_LABEL_STARTY;
static const int RUNTIME_FIELD_WIDTH = COLUMN_4_END - COLUMN_4_ST + 1;

static const char ITER_READ_TRANSRATE_LABEL[]  = "Iteration RTR:";
static const char ITER_WRITE_TRANSRATE_LABEL[] = "Iteration WTR:";
static const int ITER_TRANSRATE_LABEL_STARTX = COLUMN_1_ST;
static const int ITER_TRANSRATE_LABEL_STARTY = 2;
static const int ITER_TRANSRATE_LABEL_WIDTH = COLUMN_1_END - COLUMN_1_ST + 1;
static const int ITER_TRANSRATE_FIELD_STARTX = COLUMN_2_ST;
static const int ITER_TRANSRATE_FIELD_STARTY = ITER_TRANSRATE_LABEL_STARTY;
static const int ITER_TRANSRATE_FIELD_MAX_DIGITS = 12;
static const int ITER_TRANSRATE_FIELD_PRECISION = 2;
static const int ITER_TRANSRATE_FIELD_UNITS_WIDTH = 5;
static const int ITER_TRANSRATE_FIELD_WIDTH = COLUMN_2_END - COLUMN_2_ST + 1;

static const char ITER_READ_TRANSTIME_LABEL[] =  "Iteration RTT:";
static const char ITER_WRITE_TRANSTIME_LABEL[] = "Iteration WTT:";
static const int ITER_TRANSTIME_LABEL_STARTX = COLUMN_3_ST;
static const int ITER_TRANSTIME_LABEL_STARTY = 2;
static const int ITER_TRANSTIME_LABEL_WIDTH = COLUMN_3_END - COLUMN_3_ST + 1;
static const int ITER_TRANSTIME_FIELD_STARTX = COLUMN_4_ST;
static const int ITER_TRANSTIME_FIELD_STARTY = ITER_TRANSTIME_LABEL_STARTY;
static const int ITER_TRANSTIME_FIELD_WIDTH = COLUMN_4_END - COLUMN_4_ST + 1;

static const char WRITE_TRANSRATE_LABEL[] = "Cumulative WTR:";
static const int WRITE_TRANSRATE_LABEL_STARTX = COLUMN_1_ST;
static const int WRITE_TRANSRATE_LABEL_STARTY = 3;
static const int WRITE_TRANSRATE_LABEL_WIDTH = COLUMN_1_END - COLUMN_1_ST + 1;
static const int WRITE_TRANSRATE_FIELD_STARTX = COLUMN_2_ST;
static const int WRITE_TRANSRATE_FIELD_STARTY = WRITE_TRANSRATE_LABEL_STARTY;
static const int WRITE_TRANSRATE_FIELD_MAX_DIGITS = 12;
static const int WRITE_TRANSRATE_FIELD_PRECISION = 2;
static const int WRITE_TRANSRATE_FIELD_UNITS_WIDTH = 5;
static const int WRITE_TRANSRATE_FIELD_WIDTH = COLUMN_2_END - COLUMN_2_ST + 1;

static const char WRITE_TRANSTIME_LABEL[] = "Cumulative WTT:";
static const int WRITE_TRANSTIME_LABEL_STARTX = COLUMN_3_ST;
static const int WRITE_TRANSTIME_LABEL_STARTY = 3;
static const int WRITE_TRANSTIME_LABEL_WIDTH = COLUMN_3_END - COLUMN_3_ST + 1;
static const int WRITE_TRANSTIME_FIELD_STARTX = COLUMN_4_ST;
static const int WRITE_TRANSTIME_FIELD_STARTY = WRITE_TRANSTIME_LABEL_STARTY;
static const int WRITE_TRANSTIME_FIELD_WIDTH = COLUMN_4_END - COLUMN_4_ST + 1;

static const char READ_TRANSRATE_LABEL[] = "Cumulative RTR:";
static const int READ_TRANSRATE_LABEL_STARTX = COLUMN_1_ST;
static const int READ_TRANSRATE_LABEL_STARTY = 4;
static const int READ_TRANSRATE_LABEL_WIDTH = COLUMN_1_END - COLUMN_1_ST + 1;
static const int READ_TRANSRATE_FIELD_STARTX = COLUMN_2_ST;
static const int READ_TRANSRATE_FIELD_STARTY = READ_TRANSRATE_LABEL_STARTY;
static const int READ_TRANSRATE_FIELD_MAX_DIGITS = 12;
static const int READ_TRANSRATE_FIELD_PRECISION = 2;
static const int READ_TRANSRATE_FIELD_UNITS_WIDTH = 5;
static const int READ_TRANSRATE_FIELD_WIDTH = COLUMN_2_END - COLUMN_2_ST + 1;

static const char READ_TRANSTIME_LABEL[] = "Cumulative RTT:";
static const int READ_TRANSTIME_LABEL_STARTX = COLUMN_3_ST;
static const int READ_TRANSTIME_LABEL_STARTY = 4;
static const int READ_TRANSTIME_LABEL_WIDTH = COLUMN_3_END - COLUMN_3_ST + 1;
static const int READ_TRANSTIME_FIELD_STARTX =  COLUMN_4_ST;
static const int READ_TRANSTIME_FIELD_STARTY = READ_TRANSTIME_LABEL_STARTY;
static const int READ_TRANSTIME_FIELD_WIDTH = COLUMN_4_END - COLUMN_4_ST + 1;


/////////  SpewTuiStatisticsWindow::SpewTuiStatisticsWindow()  ////////////
SpewTuiStatisticsWindow::SpewTuiStatisticsWindow(const SpewTui *spewTui,
                                                 int height,
                                                 int width,
                                                 int startx,
                                                 int starty,
                                                 Units_t units):
   SpewTuiWindow(spewTui, height, width, startx, starty)

{
   mJobBytesTransferred = 0;
   mJobTransferTime = 0.0;
   mBytesInJob = 0;
   mTotalBytesRead = 0;
   mTotalReadTransferTime = 0.0;
   mTotalBytesWritten = 0;
   mTotalWriteTransferTime = 0.0;
   mTotalRunTime = 0.0;
}


/////////////////  SpewTuiStatisticsWindow::show()  ///////////////////////////
int SpewTuiStatisticsWindow::show()
{
   mvwprintw(mWindow,
             ITERATION_LABEL_STARTY,
             ITERATION_LABEL_STARTX,
             "%-*s",
             ITERATION_LABEL_WIDTH,
             ITERATION_LABEL);

   mvwprintw(mWindow,
             RUNTIME_LABEL_STARTY,
             RUNTIME_LABEL_STARTX,
             "%-*s",
             RUNTIME_LABEL_WIDTH,
             RUNTIME_LABEL);

   // ITER_TRANSRATE_LABEL and ITER_TRANSTIME_LABEL are displayed in 
   // startJob().

   mvwprintw(mWindow,
             READ_TRANSRATE_LABEL_STARTY,
             READ_TRANSRATE_LABEL_STARTX,
             "%-*s",
             READ_TRANSRATE_LABEL_WIDTH,
             READ_TRANSRATE_LABEL);

   mvwprintw(mWindow,
             READ_TRANSTIME_LABEL_STARTY,
             READ_TRANSTIME_LABEL_STARTX,
             "%-*s",
             READ_TRANSTIME_LABEL_WIDTH,
             READ_TRANSTIME_LABEL);

   mvwprintw(mWindow,
             WRITE_TRANSRATE_LABEL_STARTY,
             WRITE_TRANSRATE_LABEL_STARTX,
             "%-*s",
             WRITE_TRANSRATE_LABEL_WIDTH,
             WRITE_TRANSRATE_LABEL);

   mvwprintw(mWindow,
             WRITE_TRANSTIME_LABEL_STARTY,
             WRITE_TRANSTIME_LABEL_STARTX,
             "%-*s",
             WRITE_TRANSTIME_LABEL_WIDTH,
             WRITE_TRANSTIME_LABEL);

   this->refresh();

   return 0;
}


/////////////////  SpewTuiStatisticsWindow::refresh()  //////////////////////
int SpewTuiStatisticsWindow::refresh()
{
   this->updateIterations();
   this->updateJobTransferRate();
   this->updateJobTransferTime();
   this->updateTotalReadTransferRate();
   this->updateTotalReadTransferTime();
   this->updateTotalWriteTransferRate();
   this->updateTotalWriteTransferTime();
   this->updateTotalRunTime();

   wrefresh(mWindow);
   return 0;
}


////////  SpewTuiStatisticsWindow::updateIterations()  //////////////////////
void SpewTuiStatisticsWindow::updateIterations()
{
   mvwprintw(mWindow, 
             ITERATION_FIELD_STARTY, 
             ITERATION_FIELD_STARTX,
             "%*s",
             ITERATION_FIELD_WIDTH,
             "");

   unsigned int iteration = mSpewTui->getCurrentIteration();
   if (iteration > 0) 
   {
      long double percentage = (long double)mJobBytesTransferred/(long double)mBytesInJob*100.0;
      if (percentage >= 1.0 && percentage <= 99.0)
      {
         const char *directionStr;
         switch (mSpewTui->getCurrentIoDirection())
         {
         case READING:
            directionStr = ITERATION_FIELD_READING_IND;
            break;
         case WRITING:
            directionStr = ITERATION_FIELD_WRITING_IND;
            break;
         }

         int len = snprintf(NULL, 0, "%d (%.Lf%% %s)",
                            iteration, percentage, directionStr);
         mvwprintw(mWindow, 
                   ITERATION_FIELD_STARTY, 
                   ITERATION_FIELD_STARTX + ITERATION_FIELD_WIDTH - len,
                   "%d (%.Lf%% %s)",
                   iteration,
                   percentage,
                   directionStr);
      }
      else
      {
         int len = snprintf(NULL, 0, "%d", iteration);
         mvwprintw(mWindow, 
                   ITERATION_FIELD_STARTY, 
                   ITERATION_FIELD_STARTX + ITERATION_FIELD_WIDTH - len,
                   "%d",
                   iteration);
      }
   }
}


////////  SpewTuiStatisticsWindow::updateRunTime()  /////////////////////////
void SpewTuiStatisticsWindow::updateTotalRunTime()
{
   mvwprintw(mWindow, 
             RUNTIME_FIELD_STARTY, 
             RUNTIME_FIELD_STARTX,
             "%*s",
             RUNTIME_FIELD_WIDTH,
             "");
   if (mSpewTui->getCurrentIteration() > 0)
   {
      int len = snprintf(NULL, 0, (char *)mTotalRunTime.getElapsedTimeStr().c_str());
      mvwprintw(mWindow, 
                RUNTIME_FIELD_STARTY, 
                RUNTIME_FIELD_STARTX + RUNTIME_FIELD_WIDTH - len,
                "%s",
                mTotalRunTime.getElapsedTimeStr().c_str());
   }
}


////////  SpewTuiStatisticsWindow::updateJobTransferRate()  ///////////////////
void SpewTuiStatisticsWindow::updateJobTransferRate()
{

   mvwprintw(mWindow, 
             ITER_TRANSRATE_FIELD_STARTY, 
             ITER_TRANSRATE_FIELD_STARTX,
             "%*s",
             ITER_TRANSRATE_FIELD_WIDTH,
             "");
   if (mSpewTui->getCurrentIteration() > 0 && mJobBytesTransferred != 0)
   {
      long double transferRate = convertCapacity(mJobBytesTransferred, mSpewTui->getCurrentUnits())/(long double)mJobTransferTime.getTime();
      int len = snprintf(NULL, 0, "%*.*Lf %-*s",
                         ITER_TRANSRATE_FIELD_MAX_DIGITS,
                         ITER_TRANSRATE_FIELD_PRECISION,
                         transferRate,
                         ITER_TRANSRATE_FIELD_UNITS_WIDTH,
                         getTransferRateUnitsStr(mSpewTui->getCurrentUnits()));
      mvwprintw(mWindow, 
                ITER_TRANSRATE_FIELD_STARTY, 
                ITER_TRANSRATE_FIELD_STARTX + ITER_TRANSRATE_FIELD_WIDTH - len, 
                "%*.*Lf %-*s",
                ITER_TRANSRATE_FIELD_MAX_DIGITS,
                ITER_TRANSRATE_FIELD_PRECISION,
                transferRate,
                ITER_TRANSRATE_FIELD_UNITS_WIDTH,
                getTransferRateUnitsStr(mSpewTui->getCurrentUnits()));
   }
}


////////  SpewTuiStatisticsWindow::updateJobTransferTime()  ///////////////////
void SpewTuiStatisticsWindow::updateJobTransferTime()
{
   mvwprintw(mWindow, 
             ITER_TRANSTIME_FIELD_STARTY, 
             ITER_TRANSTIME_FIELD_STARTX,
             "%*s",
             ITER_TRANSTIME_FIELD_WIDTH,
             "");
   if (mSpewTui->getCurrentIteration() > 0)
   {
      int len = snprintf(NULL, 0, "%s", 
                         mJobTransferTime.getElapsedTimeStr().c_str());
      mvwprintw(mWindow, 
                ITER_TRANSTIME_FIELD_STARTY, 
                ITER_TRANSTIME_FIELD_STARTX + ITER_TRANSTIME_FIELD_WIDTH - len,
                "%s",
                mJobTransferTime.getElapsedTimeStr().c_str());
   }
}


////////  SpewTuiStatisticsWindow::updateTotalReadTransferRate()  /////////////
void SpewTuiStatisticsWindow::updateTotalReadTransferRate()
{
   mvwprintw(mWindow, 
             READ_TRANSRATE_FIELD_STARTY, 
             READ_TRANSRATE_FIELD_STARTX,
             "%*s",
             READ_TRANSRATE_FIELD_WIDTH,
             "");
   if (mSpewTui->getCurrentIteration() > 0 && mTotalBytesRead > 0)
   {
      long double transferRate = convertCapacity(mTotalBytesRead, mSpewTui->getCurrentUnits())/(long double)mTotalReadTransferTime.getTime();
      int len = snprintf(NULL, 0, "%*.*Lf %-*s",
                         READ_TRANSRATE_FIELD_MAX_DIGITS,
                         READ_TRANSRATE_FIELD_PRECISION,
                         transferRate,
                         READ_TRANSRATE_FIELD_UNITS_WIDTH,
                         getTransferRateUnitsStr(mSpewTui->getCurrentUnits()));      mvwprintw(mWindow, 
                READ_TRANSRATE_FIELD_STARTY, 
                READ_TRANSRATE_FIELD_STARTX + READ_TRANSRATE_FIELD_WIDTH - len,
                "%*.*Lf %-*s",
                READ_TRANSRATE_FIELD_MAX_DIGITS,
                READ_TRANSRATE_FIELD_PRECISION,
                transferRate,
                READ_TRANSRATE_FIELD_UNITS_WIDTH,
                getTransferRateUnitsStr(mSpewTui->getCurrentUnits()));
   }
}


////////  SpewTuiStatisticsWindow::updateTotalReadTransferTime()  ////////////
void SpewTuiStatisticsWindow::updateTotalReadTransferTime()
{
   mvwprintw(mWindow, 
             READ_TRANSTIME_FIELD_STARTY, 
             READ_TRANSTIME_FIELD_STARTX,
             "%*s",
             READ_TRANSTIME_FIELD_WIDTH,
             "");
   if (mSpewTui->getCurrentIteration() > 0)
   {
      int len = snprintf(NULL, 0, "%s", 
                         mTotalReadTransferTime.getElapsedTimeStr().c_str());
      mvwprintw(mWindow, 
                READ_TRANSTIME_FIELD_STARTY, 
                READ_TRANSTIME_FIELD_STARTX + READ_TRANSTIME_FIELD_WIDTH - len,
                "%s",
                mTotalReadTransferTime.getElapsedTimeStr().c_str());
   }
}


///////  SpewTuiStatisticsWindow::updateTotalWriteTransferRate()  /////////////
void SpewTuiStatisticsWindow::updateTotalWriteTransferRate()
{
   mvwprintw(mWindow, 
             WRITE_TRANSRATE_FIELD_STARTY, 
             WRITE_TRANSRATE_FIELD_STARTX,
             "%*s",
             WRITE_TRANSRATE_FIELD_WIDTH,
             "");
   if (mSpewTui->getCurrentIteration() > 0 && mTotalBytesWritten > 0)
   {
      long double transferRate = convertCapacity(mTotalBytesWritten, mSpewTui->getCurrentUnits())/(long double)mTotalWriteTransferTime.getTime();
      int len = snprintf(NULL, 0, "%*.*Lf %-*s",
                         WRITE_TRANSRATE_FIELD_MAX_DIGITS,
                         WRITE_TRANSRATE_FIELD_PRECISION,
                         transferRate,
                         WRITE_TRANSRATE_FIELD_UNITS_WIDTH,
                         getTransferRateUnitsStr(mSpewTui->getCurrentUnits()));      mvwprintw(mWindow, 
                WRITE_TRANSRATE_FIELD_STARTY, 
                WRITE_TRANSRATE_FIELD_STARTX + WRITE_TRANSRATE_FIELD_WIDTH - len,
                "%*.*Lf %-*s",
                WRITE_TRANSRATE_FIELD_MAX_DIGITS,
                WRITE_TRANSRATE_FIELD_PRECISION,
                transferRate,
                WRITE_TRANSRATE_FIELD_UNITS_WIDTH,
                getTransferRateUnitsStr(mSpewTui->getCurrentUnits()));
   }
}


////////  SpewTuiStatisticsWindow::updateTotalWriteTransferTime()  ////////////
void SpewTuiStatisticsWindow::updateTotalWriteTransferTime()
{
   mvwprintw(mWindow, 
             WRITE_TRANSTIME_FIELD_STARTY, 
             WRITE_TRANSTIME_FIELD_STARTX,
             "%*s",
             WRITE_TRANSTIME_FIELD_WIDTH,
             "");
   if (mSpewTui->getCurrentIteration() > 0)
   {
      int len = snprintf(NULL, 0, "%s", 
                         mTotalWriteTransferTime.getElapsedTimeStr().c_str());
      mvwprintw(mWindow, 
                WRITE_TRANSTIME_FIELD_STARTY, 
                WRITE_TRANSTIME_FIELD_STARTX + WRITE_TRANSTIME_FIELD_WIDTH - len,
                "%s",
                mTotalWriteTransferTime.getElapsedTimeStr().c_str());
   }
}


/////////////////  SpewTuiStatisticsWindow::startJob()  ///////////////////////
void SpewTuiStatisticsWindow::startJob()
{
   unsigned int iteration = mSpewTui->getCurrentIteration();

   const char *directionStr;
   switch (mSpewTui->getCurrentIoDirection())
   {
   case READING:
      directionStr = ITERATION_FIELD_READING_IND;
      break;
   case WRITING:
      directionStr = ITERATION_FIELD_WRITING_IND;
      break;
   }
   int len = snprintf(NULL, 0, "%d (0%% %s)",  iteration, directionStr);
   mvwprintw(mWindow, 
             ITERATION_FIELD_STARTY, 
             ITERATION_FIELD_STARTX + ITERATION_FIELD_WIDTH - len,
             "%d (0%% %s)",
             iteration,
             directionStr);

   switch (mSpewTui->getCurrentIoDirection())
   {
   case READING:
      mvwprintw(mWindow,
                ITER_TRANSRATE_LABEL_STARTY,
                ITER_TRANSRATE_LABEL_STARTX,
                "%-*s",
                ITER_TRANSRATE_LABEL_WIDTH,
                ITER_READ_TRANSRATE_LABEL);
      mvwprintw(mWindow,
                ITER_TRANSTIME_LABEL_STARTY,
                ITER_TRANSTIME_LABEL_STARTX,
                "%-*s",
                ITER_TRANSTIME_LABEL_WIDTH,
                ITER_READ_TRANSTIME_LABEL);
      break;
      break;
   case WRITING:
      mvwprintw(mWindow,
                ITER_TRANSTIME_LABEL_STARTY,
                ITER_TRANSTIME_LABEL_STARTX,
                "%-*s",
                ITER_TRANSTIME_LABEL_WIDTH,
                ITER_WRITE_TRANSTIME_LABEL);
      mvwprintw(mWindow,
                ITER_TRANSRATE_LABEL_STARTY,
                ITER_TRANSRATE_LABEL_STARTX,
                "%-*s",
                ITER_TRANSRATE_LABEL_WIDTH,
                ITER_WRITE_TRANSRATE_LABEL);
      break;
   }

   mvwprintw(mWindow, 
             ITER_TRANSRATE_FIELD_STARTY, 
             ITER_TRANSRATE_FIELD_STARTX,
             "%*s",
             ITER_TRANSRATE_FIELD_WIDTH,
             "");

   mvwprintw(mWindow, 
             ITER_TRANSTIME_FIELD_STARTY, 
             ITER_TRANSTIME_FIELD_STARTX,
             "%*s",
             ITER_TRANSTIME_FIELD_WIDTH,
             "");

   wrefresh(mWindow);
}


//////////  SpewTuiStatisticsWindow::~SpewTuiStatisticsWindow()  //////////
SpewTuiStatisticsWindow::~SpewTuiStatisticsWindow()
{
}

