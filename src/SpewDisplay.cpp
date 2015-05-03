//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewDisplay.cpp
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

#include "common.h"
#include "SpewDisplay.h"

//////////////////////////  SpewDisplay::SpewDisplay()  ///////////////////////
SpewDisplay::SpewDisplay(unsigned int iterationsToDo,
                         Units_t units,
                         bool showProgress,
                         Verbosity_t verbosity): 
   mIterationsToDo(iterationsToDo),
   mCurrentUnits(units),
   mShowProgress(showProgress),
   mVerbosity(verbosity)
{
   mCurrentIteration = 0;
   mCurrentIoDirection = WRITING;
   mTotalTransfers = 0;
   mTransfersCompleted = 0;
   mCurrentProgressRow = 0;
   mProgressRows.clear();
}


///////////////  SpewDisplay::isStartOfProgressRow()  /////////////////////////
bool SpewDisplay::isStartOfProgressRow() const
{
   if (mCurrentProgressRow == 0)
      return false;

   const SpewProgressRow *currRow = &(mProgressRows[mCurrentProgressRow - 1]);
   if (currRow->getTransfersCompletedInRow() == 0)
      return true;
   else
      return false;
};


///////////////  SpewDisplay::isEndOfProgressRow()  ///////////////////////////
bool SpewDisplay::isEndOfProgressRow(capacity_t numTrans) const
{
   if (mCurrentProgressRow == 0)
      return false;

   const SpewProgressRow *currRow = &(mProgressRows[mCurrentProgressRow - 1]);
   capacity_t transComp = currRow->getTransfersCompletedInRow() + numTrans;
   if (transComp >= currRow->getTotalTransfersInRow())
      return true;
   else
      return false;  
};


//////////////////////////  SpewDisplay::addToTransfersCompleted()  ///////////
void SpewDisplay::addToTransfersCompleted(capacity_t numTrans)
{
   mTransfersCompleted += numTrans; 
   mProgressRows[mCurrentProgressRow - 1].addToTransfersCompletedInRow(numTrans);
};


//////////////////////////  SpewDisplay::getTransfersInNextHack()  ////////////
capacity_t SpewDisplay::getTransfersInNextHack()
{
   SpewProgressRow *currRow = (SpewProgressRow *)NULL;
   unsigned int rowsLeft;
   capacity_t transPerRow;
   unsigned int rows = this->getCurrentProgressRows();
   unsigned int cols = this->getCurrentProgressColumns();


   capacity_t transLeft = mTotalTransfers - mTransfersCompleted;
   if (transLeft == 0)
      return 0;

   if (mCurrentProgressRow == 0)
   {
      if (mTotalTransfers <= cols)
         transPerRow = 1;
      else
         transPerRow = mTotalTransfers/rows;
      mProgressRows.push_back(SpewProgressRow(transPerRow, cols));
      mCurrentProgressRow++;
      currRow = &(mProgressRows[mCurrentProgressRow - 1]);
   }
   else
   {
      capacity_t totalTrans;
      capacity_t transComp;
      currRow = &(mProgressRows[mCurrentProgressRow - 1]);
      totalTrans = currRow->getTotalTransfersInRow();
      transComp = currRow->getTransfersCompletedInRow();
      if (transComp == totalTrans)
      {
         // We are at the end, add a new row to list.
         if (rows <= mCurrentProgressRow + 1)
         {
            // One or less rows left, so dump everything in this row.
            mProgressRows.push_back(SpewProgressRow(transLeft, cols));
         }
         else
         {
            rowsLeft = rows - mCurrentProgressRow;
            transPerRow = mTotalTransfers/rows;
            if (transPerRow < 1)
               transPerRow = 1;
            mProgressRows.push_back(SpewProgressRow(transPerRow, cols));
         }
         mCurrentProgressRow++;
         currRow = &(mProgressRows[mCurrentProgressRow - 1]);
      }
   }

   return currRow->getTransfersInNextHack();
}


//////////////////////////  SpewDisplay::addToProgress()  ////////////////////
void SpewDisplay::addToProgress(capacity_t numTrans, 
                                bool foundError,
                                const JobStatistics *jobStats,
                                const CumulativeStatistics *cumStats)
{
   SpewProgressRow *currRow = (SpewProgressRow *)NULL;
   currRow = &(mProgressRows[mCurrentProgressRow - 1]);
   capacity_t transComp = currRow->getTransfersCompletedInRow() + numTrans;
   capacity_t totalTrans = currRow->getTotalTransfersInRow();

   if (transComp >= totalTrans)
   {
      // End of row
      if (foundError)
         this->errorEndHack();
      else
         this->endHack();

      for (unsigned int i = currRow->getHacksCompletedInRow();
           i < currRow->getTotalHacksInRow() - 1;
           i++)
         this->noHack();

      TimeHack rowTime(jobStats->getIntervalEndTime());
      rowTime -= jobStats->getIntervalStartTime();
      currRow->setTransferTimeForRow(rowTime);
      this->intermediateStatistics(jobStats,
                                   cumStats, 
                                   TimeHack::getCurrentTime(),
                                   mStartTime);
      this->nextProgressRow();
   }
   else
   {
      // Just a hack
      if (foundError)
         this->errorHack();
      else
         this->hack();
   }
   
   this->addToTransfersCompleted(numTrans);
}


//////////////////////////  SpewDisplay::setCurrentUnits()  ///////////////////
void SpewDisplay::setCurrentUnits(Units_t units)
{
   mCurrentUnits = units;
}


//////////////////////////  SpewDisplay::startRun()  //////////////////////////
void SpewDisplay::startRun(const TimeHack &startTime)
{
   mStartTime = startTime;
}


//////////////////////////  SpewDisplay::startJob()  //////////////////////////
void SpewDisplay::startJob(unsigned int iteration, IoDirection_t direction)
{
   mCurrentIteration = iteration;
   mCurrentIoDirection = direction;
   mTransfersCompleted = 0;
   mCurrentProgressRow = 0;
   mProgressRows.clear();
}


//////////////////////////  SpewDisplay~SpewDisplay()  ////////////////////////
SpewDisplay::~SpewDisplay()
{
}
