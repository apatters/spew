//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewTuiStatisticsWindow.h
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

#ifndef SPEWTUISTATISTICSWINDOW_H
#define SPEWTUISTATISTICSWINDOW_H

#include <string>
#include <ncurses.h>

#include "SpewTui.h"
#include "SpewTuiWindow.h"

class SpewTuiStatisticsWindow: public SpewTuiWindow
{

public:
   SpewTuiStatisticsWindow(const SpewTui *spewTui,
                           int height,
                           int width,
                           int startx,
                           int starty,
                           Units_t units);  
   
   int show();
   int refresh();

   void setJobBytesTransferred(capacity_t bytesTransferred) { mJobBytesTransferred = bytesTransferred; };
   void setJobTransferTime(const TimeHack& transferTime) { mJobTransferTime = transferTime; };
   void setBytesInJob(capacity_t jobBytes) {mBytesInJob = jobBytes; };
   void setTotalBytesRead(capacity_t bytesRead) {mTotalBytesRead = bytesRead; };
   void setTotalReadTransferTime(const TimeHack& transferTime) {mTotalReadTransferTime = transferTime; };
   void setTotalWriteTransferTime(const TimeHack& transferTime) {mTotalWriteTransferTime = transferTime; };
void setTotalBytesWritten(capacity_t bytesWritten) {mTotalBytesWritten = bytesWritten; };
   void setTotalRunTime(const TimeHack& runTime) { mTotalRunTime = runTime; };

   void startRun() {};
   void endRun() {};
   void startJob();
   void endJob() {};
   void nextJob() {};

   virtual ~SpewTuiStatisticsWindow();

private:
   SpewTuiStatisticsWindow();  // Hide default constructor.
   void updateIterations();
   void updateJobTransferTime();
   void updateJobTransferRate();
   void updateTotalReadTransferRate();
   void updateTotalReadTransferTime();
   void updateTotalWriteTransferRate();
   void updateTotalWriteTransferTime();
   void updateTotalRunTime();


private:
   capacity_t mJobBytesTransferred;
   TimeHack mJobTransferTime;
   capacity_t mBytesInJob;
   capacity_t mTotalBytesRead;
   TimeHack mTotalReadTransferTime;
   capacity_t mTotalBytesWritten;
   TimeHack  mTotalWriteTransferTime;
   TimeHack mTotalRunTime;
};

#endif // SPEWTUISTATISTICSWINDOW_H
