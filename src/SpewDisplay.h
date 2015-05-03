//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewDisplay.h
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

#ifndef SPEWDISPLAY_H
#define SPEWDISPLAY_H

#include "TimeHack.h"

class SpewDisplay
{
public:
   static const unsigned int INFINITE_ITERATIONS = 0;

public:
   SpewDisplay(unsigned int iterationsToDo, 
               Units_t units, 
               bool showProgress,
               Verbosity_t verbosity); 

   virtual int init() = 0;
   virtual int close() = 0;
   virtual int resize() = 0; 
   
   virtual unsigned int getCurrentIteration() const { return mCurrentIteration; };

   virtual void setCurrentUnits(Units_t units);
   virtual IoDirection_t getCurrentIoDirection() const { return mCurrentIoDirection; };
   virtual Units_t getCurrentUnits() const { return mCurrentUnits; };

   virtual unsigned int getCurrentNumVerticalHacks() const = 0;
   virtual unsigned int getCurrentNumHorizontalHacks() const = 0;

   virtual void hack() = 0;
   virtual void endHack() = 0;
   virtual void errorHack() = 0;
   virtual void errorEndHack() = 0;
   virtual void noHack() = 0;
   virtual void noEndHack() = 0;
   virtual void nextHackRow() = 0;

   virtual void intermediateStatistics(capacity_t hackRowBytesTransferred,
                                       const TimeHack& hackRowTransferTime,
                                       capacity_t jobBytesTransferred,
                                       const TimeHack& jobTransferTime,
                                       capacity_t bytesInJob,
                                       capacity_t totalBytesRead,
                                       const TimeHack& totalReadTransferTime,
                                       capacity_t totalBytesWritten,
                                       const TimeHack& totalWriteTransferTime,
                                       const TimeHack& totalRunTime) = 0;
   virtual void cumulativeStatistics(capacity_t jobBytesTransferred,
                                     const TimeHack& jobTransferTime,
                                     capacity_t totalBytesRead,
                                     const TimeHack& totalReadTransferTime,
                                     capacity_t totalReadOps,
                                     capacity_t totalBytesWritten,
                                     const TimeHack& totalWriteTransferTime,
                                     capacity_t totalWriteOps,
                                     const TimeHack& totalRunTime) = 0;

   virtual void startRun() = 0;
   virtual void endRun() = 0;
   virtual void startJob(unsigned int iteration, IoDirection_t direction);
   virtual void endJob() = 0;
   virtual void nextJob() = 0;

   virtual ~SpewDisplay();

private:
   SpewDisplay(); // Hide default constructor.
   SpewDisplay(const SpewDisplay&); // Hide copy constructor.

protected:
   unsigned int mIterationsToDo;
   Units_t mCurrentUnits;
   bool mShowProgress;
   Verbosity_t mVerbosity;
   unsigned int mCurrentIteration;
   IoDirection_t mCurrentIoDirection;

};

#endif // SPEWDISPLAY_H
