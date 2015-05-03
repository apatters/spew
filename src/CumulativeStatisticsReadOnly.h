//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// CumulativeStatisticsReadOnly.h
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

#ifndef CUMULATIVESTATISTICSREADONLY_H
#define CUMULATIVESTATISTICSREADONLY_H

#include "common.h"
#include "Statistics.h"
#include "TimeHack.h"
#include "CumulativeStatistics.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////  Class CumulativeStatisticsReadOnly  //////////////////////
///////////////////////////////////////////////////////////////////////////////
class CumulativeStatisticsReadOnly: public CumulativeStatistics
{
public:
   CumulativeStatisticsReadOnly();

   virtual unsigned int getIterations() const { return mIterations; };
   virtual TimeHack getTotalReadTransferTime() const { return mTotalReadTransferTime; };
   virtual TimeHack getTotalWriteTransferTime() const { return mTotalWriteTransferTime; };
   virtual capacity_t getTotalBytesRead() const { return mTotalBytesRead; };
   virtual capacity_t getTotalBytesWritten() const { return mTotalBytesWritten; };
   virtual capacity_t getTotalReadOps() const { return mTotalReadOps; };
   virtual  capacity_t getTotalWriteOps() const { return mTotalWriteOps; };

   virtual ~CumulativeStatisticsReadOnly();

private:
   CumulativeStatisticsReadOnly(const CumulativeStatisticsReadOnly& stats);
   CumulativeStatisticsReadOnly& operator=(const CumulativeStatisticsReadOnly &rhs);
};

#endif // CUMULATIVESTATISTICSREADONLY_H
