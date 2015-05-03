//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewerTuiStatisticsWindow.h
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

#ifndef SPEWERTUISTATISTICSWINDOW_H
#define SPEWERSTUITATISTICSWINDOW_H

#include <string>
#include <ncurses.h>

class SpewerTuiStatisticsWindow
{

public:
   SpewerTuiStatisticsWindow();  
   
   void show();
   void refresh();
   void clear();

   void setIterations(int iterations) { mIterations = iterations; };
   void setRunTime(timehack_t runTime) { mRunTime = runTime; };
   void setNumJobs(int numJobs) { mNumJobs = numJobs; };
   void setUnits(Units_t units) { mUnits = units; };
   void setCumulativeWriteTransferRate(double CumulativeWriteTransferRate) { mCumulativeWriteTransferRate = CumulativeWriteTransferRate; };
   void setCumulativeReadTransferRate(double CumulativeReadTransferRate) { mCumulativeReadTransferRate = CumulativeReadTransferRate; };
   
   ~SpewerTuiStatisticsWindow();

private:
   void updateIterations();
   void updateRunTime();
   void updateNumJobs();
   void updateCumulativeWriteTransferRate();
   void updateCumulativeReadTransferRate();

private:
   WINDOW *mWindow;
   int mIterations;
   timehack_t mRunTime;
   int mNumJobs;
   Units_t mUnits;
   double mCumulativeWriteTransferRate;
   double mCumulativeReadTransferRate;
};

#endif // SPEWERTUISTATISTICSWINDOW_H
