//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// ReadJob.cpp
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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>

#include "ReadJob.h"
#include "Transfer.h"
#include "TransferFactory.h"


///////////////////  ReadJob::ReadJob()  //////////////////////////////////////
ReadJob::ReadJob(Log &logger,
                 const string &fileName,
                 capacity_t offset,
                 capacity_t transferSize,
                 capacity_t minBufferSize,
                 capacity_t maxBufferSize,
                 TransferInfoList::buffer_size_method_t bufferSizeMethod,
                 pattern_t pattern,
                 TransferInfoList::fill_method_t fillMethod,
                 io_method_t ioMethod,
                 u32_t seed,
                 capacity_t jobId)
   : Job(logger,
         fileName,
         offset,
         transferSize,
         minBufferSize,
         maxBufferSize,
         bufferSizeMethod,
         pattern,
         fillMethod,
         ioMethod,
         seed,
         jobId)
{
}


///////////////////////////  ReadJob::startJob()  /////////////////////////////
int ReadJob::startJob()
{
   Job::startJob();
   int flags;
#ifdef O_LARGEFILE
   flags = O_RDONLY|O_LARGEFILE;
#else
   flags = O_RDONLY;
#endif
   switch (mIOMethod)
   {
   case ASYNCH_IO:
      break;
   case SYNCH_IO:
      flags |= O_SYNC;
      break;
#ifdef HAVE_DIRECT_IO
   case DIRECT_IO:
      flags |= O_DIRECT;
      break;
#endif
   }
   mFd = open(mFileName.c_str(), flags);
   if (mFd < 0)
   {
      mLastErrorMsg = strPrintf("Cannot open file \"%s\" for reading.\n", 
                                mFileName.c_str());
      return EXIT_ERROR_SYSTEM;
   }

   mTransfer = TransferFactory::createInstance(mLogger,
                                               mPattern,
                                               mFd,
                                               mBuffer, 
                                               mMaxBufferSize, 
                                               mJobId, 
                                               mSeed,
															  this->getIoDirection());
   if (!mTransfer)
   {
      mLastErrorMsg = strPrintf("Could not allocate memory for transfer.\n"); 
      return EXIT_ERROR_MEMORY_ALLOC;
   }

   this->setJobStartTime();

   return EXIT_OK;
}


///////////////////////////  ReadJob::finishJob()  ////////////////////////////
int ReadJob::finishJob()
{
   Job::finishJob();

   close(mFd);  // Flush I/O before closing.
   setJobEndTime();

   return EXIT_OK;
}


///////////////////////////  ReadJob::~ReadJob()  /////////////////////////////
ReadJob::~ReadJob()
{
}
