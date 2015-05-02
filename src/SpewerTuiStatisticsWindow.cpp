//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewerTuiStatisticsWindow.cpp
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

#include <stdio.h>
#include <stdarg.h>
#include <ncurses.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common.h"
#include "SpewerTuiStatisticsWindow.h"


static const int WINDOW_HEIGHT = 6;
static const int WINDOW_WIDTH = 80;
static const int WINDOW_STARTX = 0;
static const int WINDOW_STARTY = 0;

static const char ITERATIONS_LABEL[] = "Iterations:";
static const int ITERATIONS_LABEL_STARTX = 0;
static const int ITERATIONS_LABEL_STARTY = 0;
static const int ITERATIONS_LABEL_WIDTH = 11;
static const int ITERATIONS_FIELD_STARTX = ITERATIONS_LABEL_STARTX +
                                           ITERATIONS_LABEL_WIDTH + 1;
static const int ITERATIONS_FIELD_STARTY = ITERATIONS_LABEL_STARTY;
static const int ITERATIONS_FIELD_WIDTH = 5;

static const char RUNTIME_LABEL[] = "Run time:";
static const int RUNTIME_LABEL_STARTX = 40;
static const int RUNTIME_LABEL_STARTY = 0;
static const int RUNTIME_LABEL_WIDTH = 9;
static const int RUNTIME_FIELD_STARTX = RUNTIME_LABEL_STARTX +
                                        RUNTIME_LABEL_WIDTH + 1;
static const int RUNTIME_FIELD_STARTY = RUNTIME_LABEL_STARTY;
static const int RUNTIME_FIELD_WIDTH = 8;

static const char NUMJOBS_LABEL[] = "Running jobs:";
static const int NUMJOBS_LABEL_STARTX = 0;
static const int NUMJOBS_LABEL_STARTY = 1;
static const int NUMJOBS_LABEL_WIDTH = 13;
static const int NUMJOBS_FIELD_STARTX = NUMJOBS_LABEL_STARTX +
                                        NUMJOBS_LABEL_WIDTH + 1;
static const int NUMJOBS_FIELD_STARTY = NUMJOBS_LABEL_STARTY;
static const int NUMJOBS_FIELD_WIDTH = 5;


static const char WRITETRANS_LABEL[] = "Write transfer rate:";
static const int WRITETRANS_LABEL_STARTX = 0;
static const int WRITETRANS_LABEL_STARTY = 2;
static const int WRITETRANS_LABEL_WIDTH = 20;
static const int WRITETRANS_FIELD_STARTX = WRITETRANS_LABEL_STARTX +
                                           WRITETRANS_LABEL_WIDTH + 1;
static const int WRITETRANS_FIELD_STARTY = WRITETRANS_LABEL_STARTY;
static const int WRITETRANS_FIELD_MAX_DIGITS = 12;
static const int WRITETRANS_FIELD_PRECISION = 2;
static const int WRITETRANS_FIELD_UNITS_WIDTH = 4;
static const int WRITETRANS_FIELD_WIDTH = WRITETRANS_FIELD_MAX_DIGITS + 1 +
                                          WRITETRANS_FIELD_UNITS_WIDTH;

static const char READTRANS_LABEL[] = "Read transfer rate:";
static const int READTRANS_LABEL_STARTX = 0;
static const int READTRANS_LABEL_STARTY = 3;
static const int READTRANS_LABEL_WIDTH = 20;
static const int READTRANS_FIELD_STARTX = READTRANS_LABEL_STARTX +
                                          READTRANS_LABEL_WIDTH + 1;
static const int READTRANS_FIELD_STARTY = READTRANS_LABEL_STARTY;
static const int READTRANS_FIELD_MAX_DIGITS = WRITETRANS_FIELD_MAX_DIGITS;
static const int READTRANS_FIELD_PRECISION = WRITETRANS_FIELD_PRECISION;
static const int READTRANS_FIELD_UNITS_WIDTH = WRITETRANS_FIELD_UNITS_WIDTH;
static const int READTRANS_FIELD_WIDTH = READTRANS_FIELD_MAX_DIGITS + 1 +
                                         READTRANS_FIELD_UNITS_WIDTH;

static const int JOBS_HEADER_STARTX = 0;
static const int JOBS_HEADER_STARTY = 5;

static const char JOB_LABEL[] = "Job";
static const int JOB_LABEL_WIDTH = 7;

static const char DIRECTORY_LABEL[] = "Directory";
static const int  DIRECTORY_LABEL_WIDTH = 14;

static const char PROGRESS_LABEL[] = "Progress";
static const int  PROGRESS_LABEL_WIDTH = 44;

static const char TRANSRATE_LABEL[] = "Transfer rate";
static const int  TRANSRATE_LABEL_WIDTH = 15;


/////////  SpewerTuiStatisticsWindow::SpewerTuiStatisticsWindow()  ////////////
SpewerTuiStatisticsWindow::SpewerTuiStatisticsWindow()
{
   mIterations = 0;
   mNumJobs = 0;
   mRunTime = 0.0;
   mUnits = MEGABYTES;
   mCumulativeWriteTransferRate = 0.0;
   mCumulativeReadTransferRate = 0.0;
}


/////////////////  SpewerTuiStatisticsWindow::show()  /////////////////////////
void SpewerTuiStatisticsWindow::show()
{
   mWindow = newwin(WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_STARTY, WINDOW_STARTX);

   mvwprintw(mWindow, 
             ITERATIONS_LABEL_STARTY, 
             ITERATIONS_LABEL_STARTX,
             "%-*s",
             ITERATIONS_LABEL_WIDTH,
             ITERATIONS_LABEL);

   mvwprintw(mWindow, 
             RUNTIME_LABEL_STARTY, 
             RUNTIME_LABEL_STARTX,
             "%-*s",
             RUNTIME_LABEL_WIDTH,
             RUNTIME_LABEL);


   mvwprintw(mWindow, 
             NUMJOBS_LABEL_STARTY, 
             NUMJOBS_LABEL_STARTX,
             "%-*s",
             NUMJOBS_LABEL_WIDTH,
             NUMJOBS_LABEL);


   mvwprintw(mWindow, 
             WRITETRANS_LABEL_STARTY, 
             WRITETRANS_LABEL_STARTX,
             "%-*s",
             WRITETRANS_LABEL_WIDTH,
             WRITETRANS_LABEL);


   mvwprintw(mWindow, 
             READTRANS_LABEL_STARTY, 
             READTRANS_LABEL_STARTX,
             "%-*s",
             READTRANS_LABEL_WIDTH,
             READTRANS_LABEL);

   wattron(mWindow, A_REVERSE);
   mvwprintw(mWindow, 
             JOBS_HEADER_STARTY,
             JOBS_HEADER_STARTX,
             "%-*s%-*s%-*s%-*s",
             JOB_LABEL_WIDTH, 
             JOB_LABEL, 
             DIRECTORY_LABEL_WIDTH, 
             DIRECTORY_LABEL, 
             PROGRESS_LABEL_WIDTH, 
             PROGRESS_LABEL, 
             TRANSRATE_LABEL_WIDTH,
             TRANSRATE_LABEL);
   wattroff(mWindow, A_REVERSE);

   this->refresh();
}


/////////////////  SpewerTuiStatisticsWindow::refresh()  //////////////////////
void SpewerTuiStatisticsWindow::refresh()
{
   this->updateIterations();
   this->updateRunTime();
   this->updateNumJobs();
   this->updateCumulativeWriteTransferRate();
   this->updateCumulativeReadTransferRate();

   wrefresh(mWindow);
}


/////////////////  SpewerTuiStatisticsWindow::clear()  ////////////////////////
void SpewerTuiStatisticsWindow::clear()
{
   wclear(mWindow);
   wrefresh(mWindow);
}


////////  SpewerTuiStatisticsWindow::updateIterations()  //////////////////////
void SpewerTuiStatisticsWindow::updateIterations()
{
   if (mIterations == 0)
   {
      mvwprintw(mWindow, 
                ITERATIONS_FIELD_STARTY, 
                ITERATIONS_FIELD_STARTX,
                "%*s",
                ITERATIONS_FIELD_WIDTH,
                "");
   }
   else
   {
      mvwprintw(mWindow, 
                ITERATIONS_FIELD_STARTY, 
                ITERATIONS_FIELD_STARTX,
                "%-*d",
                ITERATIONS_FIELD_WIDTH,
                mIterations);
   }
}


////////  SpewerTuiStatisticsWindow::updateRunTime()  /////////////////////////
void SpewerTuiStatisticsWindow::updateRunTime()
{
   if (mIterations == 0.0)
   {
      mvwprintw(mWindow, 
                RUNTIME_FIELD_STARTY, 
                RUNTIME_FIELD_STARTX,
                "%*s",
                RUNTIME_FIELD_WIDTH,
                "");
   }
   else
   {
      mvwprintw(mWindow, 
                RUNTIME_FIELD_STARTY, 
                RUNTIME_FIELD_STARTX,
                "%-*s",
                RUNTIME_FIELD_WIDTH,
                getElapsedTimeStr(mRunTime));
   }
}


////////  SpewerTuiStatisticsWindow::updateNumJobs()  /////////////////////////
void SpewerTuiStatisticsWindow::updateNumJobs()
{
   if (mIterations == 0)
   {
      mvwprintw(mWindow, 
                NUMJOBS_FIELD_STARTY, 
                NUMJOBS_FIELD_STARTX,
                "%*s",
                NUMJOBS_FIELD_WIDTH,
                "");
   }
   else
   {
      mvwprintw(mWindow, 
                NUMJOBS_FIELD_STARTY, 
                NUMJOBS_FIELD_STARTX,
                "%-*d",
                NUMJOBS_FIELD_WIDTH,
                mNumJobs);
   }
}


////////  SpewerTuiStatisticsWindow::updateCumulativeWriteTransferRate()  /////
void SpewerTuiStatisticsWindow::updateCumulativeWriteTransferRate()
{
   if (mIterations == 0)
   {
      mvwprintw(mWindow, 
                WRITETRANS_FIELD_STARTY, 
                WRITETRANS_FIELD_STARTX,
                "%*s",
                WRITETRANS_FIELD_WIDTH,
                "");
   }
   else
   {
      mvwprintw(mWindow, 
                WRITETRANS_FIELD_STARTY, 
                WRITETRANS_FIELD_STARTX,
                "%*.*f %-*s",
                WRITETRANS_FIELD_MAX_DIGITS,
                WRITETRANS_FIELD_PRECISION,
                mCumulativeWriteTransferRate,
                WRITETRANS_FIELD_UNITS_WIDTH,
                getTransferRateUnitsStr(mUnits));
   }
}


////////  SpewerTuiStatisticsWindow::updateCumulativeReadTransferRate()  //////
void SpewerTuiStatisticsWindow::updateCumulativeReadTransferRate()
{
   if (mIterations == 0)
   {
      mvwprintw(mWindow, 
                READTRANS_FIELD_STARTY, 
                READTRANS_FIELD_STARTX,
                "%*s",
                READTRANS_FIELD_WIDTH,
                "");
   }
   else
   {
      mvwprintw(mWindow, 
                READTRANS_FIELD_STARTY, 
                READTRANS_FIELD_STARTX,
                "%*.*f %-*s",
                READTRANS_FIELD_MAX_DIGITS,
                READTRANS_FIELD_PRECISION,
                mCumulativeReadTransferRate,
                READTRANS_FIELD_UNITS_WIDTH,
                getTransferRateUnitsStr(mUnits));
   }
}


//////////  SpewerTuiStatisticsWindow::~SpewerTuiStatisticsWindow()  //////////
SpewerTuiStatisticsWindow::~SpewerTuiStatisticsWindow()
{
}

