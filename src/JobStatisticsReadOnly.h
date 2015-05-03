//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// JobStatisticsReadOnly.h
//
// Spew
//
// Copyright (C) 2006 Hewlett-Packard Corp.
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

#ifndef JOBSTATISTICSREADONLY_H
#define JOBSTATISTICSREADONLY_H

#include "common.h"
#include "Statistics.h"
#include "TimeHack.h"
#include "JobStatistics.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////  Class JobStatisticsReadOnly  /////////////////////
///////////////////////////////////////////////////////////////////////////////
class JobStatisticsReadOnly: public JobStatistics
{
public:
   JobStatisticsReadOnly();
   JobStatisticsReadOnly(const JobStatisticsReadOnly &rhs);

   virtual void init();
   
   virtual TimeHack getJobStartTime() const { return mJobStartTime; };
   virtual TimeHack getJobEndTime() const { return mJobEndTime; };
	capacity_t getBytesInJob() const { return mBytesInJob; }
   virtual capacity_t getJobBytesTransferred() const { return mJobBytesTransferred; };
   virtual TimeHack getIntervalStartTime() const { return mIntervalStartTime; };
   virtual TimeHack getIntervalEndTime() const { return mIntervalEndTime; };
   virtual capacity_t getIntervalBytesTransferred() const { return mIntervalBytesTransferred; };
   virtual TimeHack getTransferStartTime() const { return mTransferStartTime; };
   virtual TimeHack getTransferEndTime() const { return mTransferEndTime; };
   virtual capacity_t getTransferBytesTransferred() const { return mTransferBytesTransferred; };
   capacity_t getNumTransfersWithDataIntegrityErrors() const { return mNumTransfersWithDataIntegrityErrors; };

   virtual ~JobStatisticsReadOnly();

private:
   JobStatisticsReadOnly& operator=(const JobStatisticsReadOnly &rhs);

};

#endif // JOBSTATISTICSREADONLY_H
