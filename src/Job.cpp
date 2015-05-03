//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// Job.cpp
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

#include "common.h"
#include "Random.h"
#include "Job.h"
#include "Transfer.h"
#include "GarbageTransfer.h"
#include "NumbersTransfer.h"
#include "RandomTransfer.h"
#include "ZerosTransfer.h"
#include "TransferInfo.h"
#include "TransferInfoList.h"
#include "TransferInfoListFactory.h"


//////////////////////////  Job::Job()  ///////////////////////////////////////
Job::Job(Log &logger,
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
   :
   mLogger(logger),
   mFileName(fileName),
   mOffset(offset),
   mTransferSize(transferSize),
   mMinBufferSize(minBufferSize),
   mMaxBufferSize(maxBufferSize),
   mBufferSizeMethod(bufferSizeMethod),
   mPattern(pattern),
   mFillMethod(fillMethod),
   mIOMethod(ioMethod),
   mSeed(seed),
   mJobId(jobId)
{
   mTransfer = (Transfer *)NULL;
   mRealBuffer = (unsigned char *)NULL;
   mBuffer = (unsigned char *)NULL;
   mLastErrorMsg = "";
   mRunningInterval = false;
}


/////////////////////////  Job::init()  ///////////////////////////////////////
int Job::init()
{
   int ret;

   // Get a page-aligned buffer big enough to fit the biggest block size.
   size_t pageSize = getpagesize();
   mRealBuffer = (unsigned char *)malloc(mMaxBufferSize + (2 * pageSize));
   if (mRealBuffer == (unsigned char *)NULL)
   {
      mLastErrorMsg = "Cannot allocate memory for buffer.\n";
      return EXIT_ERROR_MEMORY_ALLOC;
   }
   mBuffer = PTR_ALIGN(mRealBuffer, pageSize);

   // Initialize statistics.
   mStats = new JobStatisticsReadWrite();
   if (!mStats)
   {
      mLastErrorMsg = "Cannot allocate memory.\n";
      return EXIT_ERROR_MEMORY_ALLOC;
   }

   // Initialize the list of transfers.
   mTransferInfoList = TransferInfoListFactory::createInstance(
      mMinBufferSize,
      mMaxBufferSize,
      mBufferSizeMethod,
      mFillMethod,
      mOffset,
      mTransferSize,
      mSeed
      );
   if (!mTransferInfoList)
   {
      mLastErrorMsg = "Cannot allocate memory.\n";
      return EXIT_ERROR_MEMORY_ALLOC;
   }
   if ((ret = mTransferInfoList->init()) != EXIT_OK)
   {
      mLastErrorMsg = "Fatal internal error.\n";
      return ret;
   }

   return EXIT_OK;
}


/////////////////////////  Job::getTotalNumberOfTransfers()  //////////////////
capacity_t Job::getTotalNumberOfTransfers() const
{
   return mTransferInfoList->getSize();
}


/////////////////////////  Job::startJob()  ///////////////////////////////////
int Job::startJob()
{
   int rtn;

   mStats->init();
   mStats->setBytesInJob(mTransferSize);

   return EXIT_OK;
}


/////////////////////////  Job::finishJob()  //////////////////////////////////
int Job::finishJob()
{
   int rtn = EXIT_OK;

   if (mRealBuffer != (unsigned char*)NULL)
   {
      free(mRealBuffer);
      mRealBuffer = (unsigned char*)NULL;
      mBuffer = (unsigned char*)NULL;
   }

   return rtn;
}


/////////////////////////  Job::setJobStartTime()  ////////////////////////////
void Job::setJobStartTime()
{
   mStats->setJobStartTime();
}


/////////////////////////  Job::getJobStartTime()  ////////////////////////////
TimeHack::timehack_t Job::getJobStartTime() const
{
   return mStats->getJobStartTime().getTime();
}


/////////////////////////  Job::setJobEndTime()  //////////////////////////////
void Job::setJobEndTime()
{
   mStats->setJobEndTime();
}


/////////////////////////  Job::getJobEndTime()  //////////////////////////////
TimeHack::timehack_t Job::getJobEndTime() const
{
   return mStats->getJobEndTime().getTime();
}


/////////////////////////  Job::getJobElapsedTime()  //////////////////////////
TimeHack::timehack_t Job::getJobElapsedTime() const
{
   return TimeHack::getCurrentTime() - this->getJobStartTime();
}


/////////////////////////  Job::getTotalJobTime()  ////////////////////////////
TimeHack::timehack_t Job::getTotalJobTime() const
{
   return mStats->getJobEndTime().getTime() - mStats->getJobStartTime().getTime();
}


/////////////////////////  Job::setIntervalStartTime()  ///////////////////////
void Job::setIntervalStartTime()
{
   mStats->setIntervalStartTime();
}


////////////////////////  Job::getIntervalStartTime()  ////////////////////////
TimeHack::timehack_t Job::getIntervalStartTime() const
{
   return mStats->getIntervalStartTime().getTime();
}


/////////////////////////  Job::setIntervalEndTime()  /////////////////////////
void Job::setIntervalEndTime()
{
   mStats->setIntervalEndTime();
}


/////////////////////////  Job::getIntervalEndTime()  /////////////////////////
TimeHack::timehack_t Job::getIntervalEndTime() const
{
   return mStats->getIntervalEndTime().getTime();
}


/////////////////////////  Job::getIntervalElapsedTime()  /////////////////////
TimeHack::timehack_t Job::getIntervalElapsedTime() const
{

   return TimeHack::getCurrentTime() - this->getIntervalStartTime();
}


/////////////////////////  Job::getTotalIntervalTime()  ///////////////////////
TimeHack::timehack_t Job::getTotalIntervalTime() const
{
   return mStats->getIntervalEndTime().getTime() - mStats->getIntervalStartTime().getTime();
}


/////////////////////////  Job::startInterval()  //////////////////////////////
int Job::startInterval()
{
   mRunningInterval = true;
   mStats->setIntervalBytesTransferred(0);
   this->setIntervalStartTime();

   return EXIT_OK;
}


/////////////////////////  Job::endInterval()  ////////////////////////////////
int Job::endInterval()
{
   this->setIntervalEndTime();
   mRunningInterval = false;

   return EXIT_OK;
}


/////////////////////////  Job::setTransferStartTime()  ///////////////////////
void Job::setTransferStartTime()
{
   mStats->setTransferStartTime();
}


////////////////////////  Job::getTransferStartTime()  ////////////////////////
TimeHack::timehack_t Job::getTransferStartTime() const
{
   return mStats->getTransferStartTime().getTime();
}


/////////////////////////  Job::setTransferEndTime()  /////////////////////////
void Job::setTransferEndTime()
{
   mStats->setTransferEndTime();
}


/////////////////////////  Job::getTransferEndTime()  /////////////////////////
TimeHack::timehack_t Job::getTransferEndTime() const
{
   return mStats->getTransferEndTime().getTime();
}


/////////////////////////  Job::runTransfers()  ///////////////////////////////
int Job::runTransfers(capacity_t numTransfers, bool continueAfterError)
{
   this->setTransferStartTime();
   mStats->setTransferBytesTransferred(0);
   int exitCode = EXIT_OK;
   for (capacity_t i = 0LLU; i < numTransfers; i++)
   {
      const TransferInfo *nextTransfer = mTransferInfoList->next();
      if (!nextTransfer)
      {
         mLastErrorMsg += "Fatal internal error - no transfers left to process.";
         return EXIT_ERROR_ILLEGAL_OPERATION;
      }
      int ret = mTransfer->io(*nextTransfer, mLastErrorMsg);
      capacity_t transferSize = nextTransfer->getSize();
      switch (ret)
      {
      case EXIT_OK:
         mStats->addToJobBytesTransferred(transferSize);
         mStats->addToTransferBytesTransferred(transferSize);
         if (mRunningInterval)
            mStats->addToIntervalBytesTransferred(transferSize);
         break;
      case EXIT_ERROR_DATA_INTEGRITY:
         exitCode = EXIT_ERROR_DATA_INTEGRITY;
         if (continueAfterError)
         {
            mStats->incNumTransfersWithDataIntegrityErrors();
            mLogger.logError(mLastErrorMsg.c_str());
         }
         else
         {
            mLastErrorMsg += "More data integrity errors may exist in other parts of the file.";
            return EXIT_ERROR_DATA_INTEGRITY;
         }
         break;
      default:
         exitCode = ret;
         return exitCode;
         break;
      }
   }
   this->setTransferEndTime();
   if (mStats->getNumTransfersWithDataIntegrityErrors())
      return EXIT_ERROR_DATA_INTEGRITY;
   else
      return exitCode;
}


//////////////////////////  Job::~Job()  //////////////////////////////////////
Job::~Job()
{
   if (mRealBuffer != (unsigned char*)NULL)
   {
      free(mRealBuffer);
      mRealBuffer = (unsigned char*)NULL;
      mBuffer = (unsigned char*)NULL;
   }
   delete mTransfer;
   delete mStats;
   delete mTransferInfoList;
}
