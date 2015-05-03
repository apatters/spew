//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// JobStatisticsReadWrite.h
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

#ifndef JOBSTATISTICSREADWRITE_H
#define JOBSTATISTICSREADWRITE_H

#ifdef USE_THREADS
#include <pthread.h>
#endif

#include "common.h"
#include "Statistics.h"
#include "TimeHack.h"
#include "JobStatistics.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////  Class JobStatisticsReadWrite  ////////////////////
///////////////////////////////////////////////////////////////////////////////
class JobStatisticsReadWrite: public JobStatistics
{
public:
   JobStatisticsReadWrite();
   JobStatisticsReadWrite(const JobStatisticsReadWrite &rhs);

   virtual void init();
   
   virtual TimeHack getJobStartTime() const;
   virtual void setJobStartTime();
   virtual TimeHack getJobEndTime() const;
   virtual void setJobEndTime();

	virtual capacity_t getBytesInJob() const;
	virtual void setBytesInJob(capacity_t bytes);

   virtual capacity_t getJobBytesTransferred() const;
   virtual void setJobBytesTransferred(capacity_t bytes);
   virtual void addToJobBytesTransferred(capacity_t bytes);

   virtual TimeHack getIntervalStartTime() const;
   virtual void setIntervalStartTime();
   virtual TimeHack getIntervalEndTime() const;
   virtual void setIntervalEndTime();
   virtual capacity_t getIntervalBytesTransferred() const;
   virtual void setIntervalBytesTransferred(capacity_t bytes);
   virtual void addToIntervalBytesTransferred(capacity_t bytes);

   virtual TimeHack getTransferStartTime() const;
   virtual void setTransferStartTime();
   virtual TimeHack getTransferEndTime() const;
   virtual void setTransferEndTime();
   virtual capacity_t getTransferBytesTransferred() const;
   virtual void setTransferBytesTransferred(capacity_t bytes);
   virtual void addToTransferBytesTransferred(capacity_t bytes);

   virtual capacity_t getNumTransfersWithDataIntegrityErrors() const;
   virtual void setNumTransfersWithDataIntegrityErrors(capacity_t errors);
   virtual void incNumTransfersWithDataIntegrityErrors();

   virtual ~JobStatisticsReadWrite();

private:
   JobStatisticsReadWrite& operator=(const JobStatisticsReadWrite &rhs);

protected:
#ifdef USE_THREADS
   void lock() const;
   void unlock() const;
#else
   void lock() const {};
   void unlock() const {};
#endif

protected:
#ifdef USE_THREADS
   mutable pthread_mutex_t mMutex;
#endif
};

#endif // JOBSTATISTICSREADWRITE_H
