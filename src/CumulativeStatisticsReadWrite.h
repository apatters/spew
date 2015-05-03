//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// CumulativeStatisticsReadWrite.h
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

#ifndef CUMULATIVESTATISTICSREADWRITE_H
#define CUMULATIVESTATISTICSREADWRITE_H

#ifdef USE_THREADS
#include <pthread.h>
#endif

#include "common.h"
#include "Statistics.h"
#include "TimeHack.h"
#include "CumulativeStatistics.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////  Class CumulativeStatisticsReadWrite  //////////////////////
///////////////////////////////////////////////////////////////////////////////
class CumulativeStatisticsReadWrite: public CumulativeStatistics
{
public:
   CumulativeStatisticsReadWrite();

   virtual void init();
   
   virtual unsigned int getIterations() const;
   virtual void setIterations(unsigned int iter);
   virtual void incIterations();
   
   virtual TimeHack getTotalReadTransferTime() const;
   virtual void setTotalReadTransferTime(const TimeHack &hack);
   virtual void addToTotalReadTransferTime(const TimeHack &hack);

   virtual TimeHack getTotalWriteTransferTime() const;
   virtual void setTotalWriteTransferTime(const TimeHack &hack);
   virtual void addToTotalWriteTransferTime(const TimeHack &hack);

   virtual capacity_t getTotalBytesRead() const;
   virtual void setTotalBytesRead(capacity_t bytes);
   virtual void addToTotalBytesRead(capacity_t bytes);

   virtual capacity_t getTotalBytesWritten() const;
   virtual void setTotalBytesWritten(capacity_t bytes);
   virtual void addToTotalBytesWritten(capacity_t bytes);

   virtual capacity_t getTotalReadOps() const;
   virtual void setTotalReadOps(capacity_t ops);
   virtual void addToTotalReadOps(capacity_t ops);

   virtual capacity_t getTotalWriteOps() const;
   virtual void setTotalWriteOps(capacity_t ops);
   virtual void addToTotalWriteOps(capacity_t ops);

   virtual ~CumulativeStatisticsReadWrite();

private:
   CumulativeStatisticsReadWrite(const CumulativeStatisticsReadWrite& stats);
   CumulativeStatisticsReadWrite& operator=(const CumulativeStatisticsReadWrite &rhs);


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


#endif // CUMULATIVESTATISTICSREADWRITE_H
