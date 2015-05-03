//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// CumulativeStatistics.h
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

#ifndef CUMULATIVESTATISTICS_H
#define CUMULATIVESTATISTICS_H

#include "common.h"
#include "Statistics.h"
#include "TimeHack.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////  Class CumulativeStatistics  //////////////////////
///////////////////////////////////////////////////////////////////////////////
class CumulativeStatistics: public Statistics
{
public:
   CumulativeStatistics();

   void init();
   
   virtual unsigned int getIterations() const = 0;
   virtual TimeHack getTotalReadTransferTime() const = 0;
   virtual TimeHack getTotalWriteTransferTime() const = 0;
   virtual capacity_t getTotalBytesRead() const = 0;
   virtual capacity_t getTotalBytesWritten() const = 0;
   virtual capacity_t getTotalReadOps() const = 0;
   virtual capacity_t getTotalWriteOps() const = 0;
 
   virtual ~CumulativeStatistics();

private:
   CumulativeStatistics(const CumulativeStatistics& stats);
   CumulativeStatistics& operator=(const CumulativeStatistics &rhs);

protected:
   unsigned int mIterations;
   TimeHack mTotalReadTransferTime;
   TimeHack mTotalWriteTransferTime;
   capacity_t mTotalBytesRead;
   capacity_t mTotalBytesWritten;
   capacity_t mTotalReadOps;
   capacity_t mTotalWriteOps;

};

#endif // CUMULATIVESTATISTICS_H
