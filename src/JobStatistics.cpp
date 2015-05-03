//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// JobStatistics.cpp
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

using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common.h"
#include "JobStatistics.h"


//////////////////////////  JobStatistics::JobStatistics()  ///////////////////
JobStatistics::JobStatistics()
{
   this->init();
}


//////////////////////////  JobStatistics::JobStatistics()  ///////////////////
JobStatistics::JobStatistics(const JobStatistics &rhs)
{
   mJobStartTime = rhs.mJobStartTime;
   mJobEndTime = rhs.mJobEndTime;
	mBytesInJob = rhs.mBytesInJob;
   mJobBytesTransferred = rhs.mJobBytesTransferred;

   mIntervalStartTime = rhs.mIntervalStartTime;
   mIntervalEndTime = rhs.mIntervalEndTime;
   mIntervalBytesTransferred = rhs.mIntervalBytesTransferred;

   mTransferStartTime = rhs.mTransferStartTime;
   mTransferEndTime = rhs.mTransferEndTime;
   mTransferBytesTransferred = rhs.mTransferBytesTransferred;

   mNumTransfersWithDataIntegrityErrors = rhs.mNumTransfersWithDataIntegrityErrors;
}


//////////////////////////  JobStatistics::init()  ////////////////////////////
void JobStatistics::init()
{
	mBytesInJob = 0;
   mJobBytesTransferred = 0;
   mIntervalBytesTransferred = 0;
   mTransferBytesTransferred = 0;
   mNumTransfersWithDataIntegrityErrors = 0;
}


//////////////////////////  JobStatistics::~JobStatistics()  //////////////////
JobStatistics::~JobStatistics()
{
}


