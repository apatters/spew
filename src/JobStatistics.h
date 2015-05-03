//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// JobStatistics.h
//
// Spew
//
// Copyright (C) 2005 Hewlett-Packard Corp.
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

#ifndef JOBSTATISTICS_H
#define JOBSTATISTICS_H

#include "common.h"
#include "Statistics.h"
#include "TimeHack.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////  Class JobStatistics  /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class JobStatistics: public Statistics
{
public:
   JobStatistics();
   JobStatistics(const JobStatistics &rhs);

   virtual void init();
   
   virtual TimeHack getJobStartTime() const = 0;
   virtual TimeHack getJobEndTime() const = 0;
   virtual capacity_t getBytesInJob() const = 0;
   virtual capacity_t getJobBytesTransferred() const = 0;
   virtual TimeHack getIntervalStartTime() const = 0;
   virtual TimeHack getIntervalEndTime() const = 0;
   virtual capacity_t getIntervalBytesTransferred() const = 0;
   virtual TimeHack getTransferStartTime() const = 0;
   virtual TimeHack getTransferEndTime() const = 0;
   virtual capacity_t getTransferBytesTransferred() const = 0;
   virtual capacity_t getNumTransfersWithDataIntegrityErrors() const = 0;

   virtual ~JobStatistics();

private:
   JobStatistics& operator=(const JobStatistics &rhs);

protected:
   TimeHack mJobStartTime;
   TimeHack mJobEndTime;
   capacity_t mBytesInJob;
   capacity_t mJobBytesTransferred;

   TimeHack mIntervalStartTime;
   TimeHack mIntervalEndTime;
   capacity_t mIntervalBytesTransferred;

   TimeHack mTransferStartTime;
   TimeHack mTransferEndTime;
   capacity_t mTransferBytesTransferred;

   capacity_t mNumTransfersWithDataIntegrityErrors; // Number of transfers
                                                    // with any number of data 
                                                    // integrity errors.
};

#endif // JOBSTATISTICS_H
