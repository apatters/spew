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

#include <vector>

#include "TimeHack.h"
#include "JobStatisticsReadOnly.h"
#include "CumulativeStatisticsReadOnly.h"
#include "SpewProgressRow.h"


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

   virtual unsigned int getCurrentProgressRows() const = 0;
   virtual unsigned int getCurrentProgressColumns() const = 0;

	const vector<SpewProgressRow> &getProgressRows() const { return mProgressRows; };
   bool isStartOfProgressRow() const;
   bool isEndOfProgressRow(capacity_t numTrans) const;
   
   capacity_t setTotalTransfers(capacity_t numTrans) { mTotalTransfers = numTrans; };
   void addToTransfersCompleted(capacity_t numTrans);
   capacity_t getTransfersInNextHack();

   void addToProgress(capacity_t numTrans, 
                      bool foundError,
                      const JobStatistics *jobStats,
                      const CumulativeStatistics *cumStats);

   virtual void hack() = 0;
   virtual void endHack() = 0;
   virtual void errorHack() = 0;
   virtual void errorEndHack() = 0;
   virtual void noHack() = 0;
   virtual void noEndHack() = 0;
   virtual void nextProgressRow() = 0;

   virtual void intermediateStatistics(const JobStatistics *jobStats,
                                       const CumulativeStatistics *cumStats,
                                       const TimeHack& currentTime,
                                       const TimeHack& startTime) = 0;
   virtual void cumulativeStatistics(const JobStatistics *jobStats,
                                     const CumulativeStatistics *cumStats,
                                     const TimeHack& totalRunTime) = 0;

   virtual void startRun(const TimeHack &startTime);
   virtual void endRun() = 0;
   virtual void startJob(unsigned int iteration, IoDirection_t direction);
   virtual void endJob() = 0;
   virtual void nextJob() = 0;

   virtual ~SpewDisplay();

private:
   SpewDisplay(); // Hide default constructor.
   SpewDisplay(const SpewDisplay&); // Hide copy constructor.
   SpewDisplay& operator=(const SpewDisplay& rhs); //Hide assignment operator.


protected:
   unsigned int mIterationsToDo;
   Units_t mCurrentUnits;
   bool mShowProgress;
   Verbosity_t mVerbosity;
   TimeHack mStartTime;
   unsigned int mCurrentIteration;
   IoDirection_t mCurrentIoDirection;
   capacity_t mTotalTransfers;
   capacity_t mTransfersCompleted;
   unsigned int mCurrentProgressRow;
   vector<SpewProgressRow> mProgressRows;
};

#endif // SPEWDISPLAY_H
