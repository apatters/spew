//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// WriteJob.cpp
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

namespace std {} using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>

#include "WriteJob.h"
#include "Transfer.h"
#include "TransferFactory.h"


///////////////////  WriteJob::WriteJob()  ////////////////////////////////////
WriteJob::WriteJob(Log &logger,
                   const string &fileName,
                   capacity_t offset,
                   capacity_t transferSize,
                   capacity_t minBufferSize,
                   capacity_t maxBufferSize,
                   TransferInfoList::buffer_size_method_t bufferSizeMethod,
                   pattern_t pattern,
						 unsigned char userPattern,
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
			userPattern,
         fillMethod,
         ioMethod,
         seed,
         jobId)
{
}


///////////////////////////  WriteJob::startJob()  ////////////////////////////
int WriteJob::startJob()
{
   Job::startJob();
   int flags;

#ifdef O_LARGEFILE
   flags = O_WRONLY|O_CREAT|O_LARGEFILE;
#else
   flags = O_WRONLY|O_CREAT;
#endif
	if (mOffset == 0)
		flags |= O_TRUNC;
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
   mFd = open(mFileName.c_str(), flags, S_IRUSR|S_IWUSR);
   if (mFd < 0)
   {
      mLastErrorMsg = strPrintf("Cannot open file \"%s\" for writing. -- %s\n", mFileName.c_str(), strError(errno).c_str());
      return EXIT_ERROR_SYSTEM;
   }

   mTransfer = TransferFactory::createInstance(mPattern,
															  mUserPattern,
                                               mFd,
                                               mBuffer, 
                                               mMaxBufferSize, 
                                               mJobId, 
                                               mSeed);
   if (mTransfer == (Transfer *)NULL)
   {
      mLastErrorMsg = strPrintf("Could not allocate memory for transfer.\n"); 
      return EXIT_ERROR_MEMORY_ALLOC;
   }

   mJobBytesTransferred = 0;
   this->setJobStartTime();

   return EXIT_OK;
}


///////////////////////////  WriteJob::finishJob()  ///////////////////////////
int WriteJob::finishJob()
{
   Job::finishJob();

   close(mFd);  // Flush I/O before closing.
   setJobEndTime();

   return EXIT_OK;
}


///////////////////////////  WriteJob::runTransfers()  ////////////////////////
int WriteJob::runTransfers(capacity_t numTransfers, bool continueAfterError)
{
   // continueAfterError (data integrity errors) is meaningless for
   // writes, so it is ignored.

   this->setTransferStartTime();
   mBytesTransferred = 0;
	mLastErrorMsg = "";
   int exitCode = EXIT_OK;
   for (capacity_t i = 0; i < numTransfers; i++)
   {
      const TransferInfo *nextTransfer = mTransferInfoList->next();
      if (!nextTransfer)
      {
         mLastErrorMsg += "Fatal internal error - no transfers left to process.";
         return EXIT_ERROR_ILLEGAL_OPERATION;
      }
      int ret = mTransfer->write(*nextTransfer, mLastErrorMsg);
      capacity_t transferSize = nextTransfer->getSize();
      if (ret == EXIT_OK)
      {
         mBytesTransferred += transferSize;
         mJobBytesTransferred += transferSize;
         if (mRunningHack)
            mHackBytesTransferred += transferSize;
      }
      else
      {
         exitCode = ret;
         break;
      }
   }
   this->setTransferEndTime();
   return exitCode;
}
