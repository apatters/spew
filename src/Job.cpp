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

namespace std {} using namespace std;

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
#include "BytePatternTransfer.h"
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
			unsigned char userPattern,
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
	mUserPattern(userPattern),
   mFillMethod(fillMethod),
   mIOMethod(ioMethod),
   mSeed(seed),
   mJobId(jobId)
{
   mTransfer = (Transfer *)NULL;
   mRealBuffer = (unsigned char *)NULL;
   mBuffer = (unsigned char *)NULL;
   mLastErrorMsg = "";
   mJobBytesTransferred = 0;
   mHackBytesTransferred = 0;
   mRunningHack = false;
   mNumTransfersWithDataIntegrityErrors = 0;
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
   mJobBytesTransferred = 0;
   return EXIT_OK;
}


/////////////////////////  Job::finishJob()  //////////////////////////////////
int Job::finishJob()
{
   if (mRealBuffer != (unsigned char*)NULL)
   {
      free(mRealBuffer);
      mRealBuffer = (unsigned char*)NULL;
      mBuffer = (unsigned char*)NULL;
   }

   return EXIT_OK;
}


/////////////////////////  Job::setJobStartTime()  ////////////////////////////
void Job::setJobStartTime()
{
   mJobStartTime.setTimeNow();}



/////////////////////////  Job::getJobStartTime()  ////////////////////////////
TimeHack::timehack_t Job::getJobStartTime() const
{
   return mJobStartTime.getTime();
}


/////////////////////////  Job::setJobEndTime()  //////////////////////////////
void Job::setJobEndTime()
{
   mJobEndTime.setTimeNow();
}


/////////////////////////  Job::getJobEndTime()  //////////////////////////////
TimeHack::timehack_t Job::getJobEndTime() const
{
   return mJobEndTime.getTime();
}


/////////////////////////  Job::getJobElapsedTime()  //////////////////////////
TimeHack::timehack_t Job::getJobElapsedTime() const
{
   return TimeHack::getCurrentTime() - this->getJobStartTime();
}


/////////////////////////  Job::getTotalJobTime()  ////////////////////////////
TimeHack::timehack_t Job::getTotalJobTime() const
{
   return mJobEndTime.getTime() - mJobStartTime.getTime();
}


/////////////////////////  Job::setHackStartTime()  ///////////////////////////
void Job::setHackStartTime()
{
   mHackStartTime.setTimeNow();
}


////////////////////////  Job::getHackStartTime()  ////////////////////////////
TimeHack::timehack_t Job::getHackStartTime() const
{
   return mHackStartTime.getTime();
}


/////////////////////////  Job::setHackEndTime()  /////////////////////////////
void Job::setHackEndTime()
{
   mHackEndTime.setTimeNow();
}


/////////////////////////  Job::getHackEndTime()  /////////////////////////////
TimeHack::timehack_t Job::getHackEndTime() const
{
   return mHackEndTime.getTime();
}


/////////////////////////  Job::getHackElapsedTime()  /////////////////////////
TimeHack::timehack_t Job::getHackElapsedTime() const
{

   return TimeHack::getCurrentTime() - this->getHackStartTime();
}


/////////////////////////  Job::getTotalHackTime()  ///////////////////////////
TimeHack::timehack_t Job::getTotalHackTime() const
{
   return mHackEndTime.getTime() - mHackStartTime.getTime();
}


/////////////////////////  Job::startHack()  //////////////////////////////////
int Job::startHack()
{
   mRunningHack = true;
   mHackBytesTransferred = 0LLU;
   this->setHackStartTime();

   return EXIT_OK;
}


/////////////////////////  Job::endHack()  ////////////////////////////////////
int Job::endHack()
{
   this->setHackEndTime();
   mRunningHack = false;

   return EXIT_OK;
}


/////////////////////////  Job::setTransferStartTime()  ///////////////////////
void Job::setTransferStartTime()
{
   mTransferStartTime.setTimeNow();
}


////////////////////////  Job::getTransferStartTime()  ////////////////////////
TimeHack::timehack_t Job::getTransferStartTime() const
{
   return mTransferStartTime.getTime();
}


/////////////////////////  Job::setTransferEndTime()  /////////////////////////
void Job::setTransferEndTime()
{
   mTransferEndTime.setTimeNow();
}


/////////////////////////  Job::getTransferEndTime()  /////////////////////////
TimeHack::timehack_t Job::getTransferEndTime() const
{
   return mTransferEndTime.getTime();
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
   delete mTransferInfoList;
}
