//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewConsole.h
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

#ifndef SPEWCONSOLE_H
#define SPEWCONSOLE_H

#include "TimeHack.h"
#include "SpewDisplay.h"

class SpewConsole: public SpewDisplay
{
public:
   SpewConsole(unsigned int iterationsToDo,
               Units_t units,
               bool showProgress,
               Verbosity_t verbosity);
   
   virtual int init();
   virtual int close(); 
   virtual int resize();

   virtual unsigned int getCurrentNumVerticalHacks() const;
   virtual unsigned int getCurrentNumHorizontalHacks() const;

   virtual void hack();
   virtual void endHack();
   virtual void errorHack();
   virtual void errorEndHack();
   virtual void noHack();
   virtual void noEndHack();
   virtual void nextHackRow();

   virtual void intermediateStatistics(capacity_t hackRowBytesTransferred,
                                       const TimeHack& hackRowTransferTime,
                                       capacity_t jobBytesTransferred,
                                       const TimeHack& jobTransferTime,
                                       capacity_t bytesInJob,
                                       capacity_t totalBytesRead,
                                       const TimeHack& totalReadTransferTime,
                                       capacity_t totalBytesWritten,
                                       const TimeHack& totalWriteTransferTime,
                                       const TimeHack& totalRunTime);
   virtual void cumulativeStatistics(capacity_t jobBytesTransferred,
                                     const TimeHack& jobTransferTime,
                                     capacity_t totalBytesRead,
                                     const TimeHack& totalReadTransferTime,
                                     capacity_t totalReadOps,
                                     capacity_t totalBytesWritten,
                                     const TimeHack& totalWriteTransferTime,
                                     capacity_t totalWriteOps,
                                     const TimeHack& totalRunTime);
   virtual void startRun() {};
   virtual void endRun() {};
   virtual void startJob(unsigned int iteration, IoDirection_t direction);
   virtual void endJob() {};
   virtual void nextJob();

   virtual ~SpewConsole();

protected:
   virtual unsigned int getScreenRows() const;
   virtual unsigned int getScreenColumns() const;

private:
   SpewConsole();  // Hide default constructor.

private:
   unsigned int mCurrentScreenRows;
   unsigned int mCurrentScreenColumns;
};

#endif // SPEWCONSOLE_H
