//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// Job.h
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

#ifndef JOB_H
#define JOB_H

#include <sys/time.h>
#include <string>

#include "common.h"
#include "Log.h"
#include "TimeHack.h"
#include "Transfer.h"
#include "TransferInfo.h"
#include "TransferInfoList.h"

class Job
{
public:
   typedef enum {PATTERN_NONE,
                 PATTERN_ZEROS, 
                 PATTERN_ONES, 
                 PATTERN_ALTERNATING, 
                 PATTERN_TRANSFER_NUMBERS, 
                 PATTERN_RANDOM,
                 PATTERN_USER_DEFINED} pattern_t;

   typedef enum {ASYNCH_IO,
                 SYNCH_IO,
                 DIRECT_IO} io_method_t;

public:
   Job(Log &logger,
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
       capacity_t jobId);

   int init();

   const string &getFileName() const { return mFileName; };
   capacity_t getTransferSize() const { return mTransferSize; };
   capacity_t getMinBufferSize() const { return mMinBufferSize; };
   capacity_t getMaxBufferSize() const { return mMaxBufferSize; };
   capacity_t getBufferSize() const { return mMaxBufferSize; };
   pattern_t getPattern() const { return mPattern; };
   TransferInfoList::fill_method_t getFillMethod() const { return mFillMethod; };
   io_method_t getIOMethod() const { return mIOMethod; };
   capacity_t getNumDataIntegrityErrors() { return mNumTransfersWithDataIntegrityErrors; };

   void setSeed(u32_t seed) { mSeed = seed; };
   u32_t getSeed() const { return mSeed; };
   
   capacity_t getJobId() const { return mJobId; };
   capacity_t getTotalNumberOfTransfers() const;

   void setJobStartTime();
   TimeHack::timehack_t getJobStartTime() const;
   void setJobEndTime();
   TimeHack::timehack_t getJobEndTime() const;
   TimeHack::timehack_t getJobElapsedTime() const;
   TimeHack::timehack_t getTotalJobTime() const;
   capacity_t getJobBytesTransferred() const { return mJobBytesTransferred; };
   virtual int startJob() = 0;
   virtual int finishJob() = 0;

   void setHackStartTime();
   TimeHack::timehack_t getHackStartTime() const;
   void setHackEndTime();
   TimeHack::timehack_t getHackEndTime() const;
   TimeHack::timehack_t getHackElapsedTime() const;
   TimeHack::timehack_t getTotalHackTime() const;
   capacity_t getHackBytesTransferred() const { return mHackBytesTransferred;};
   virtual int startHack();
   virtual int endHack();

   void setTransferStartTime();
   TimeHack::timehack_t getTransferStartTime() const;
   void setTransferEndTime();
   TimeHack::timehack_t getTransferEndTime() const;
   capacity_t getBytesTransferred() const { return mBytesTransferred; };

   string getLastErrorMessage() const { return mLastErrorMsg; };
   virtual int runTransfers(capacity_t numTransfers, 
                            bool continueAfterError) = 0;

   virtual ~Job();

private:
   Job();           // Hide default constuctor.
   Job(const Job&); // Hide copy constructor.

protected:
   Log &mLogger;
   string mFileName;
   capacity_t mOffset;
   capacity_t mTransferSize;
   capacity_t mMinBufferSize;
   capacity_t mMaxBufferSize;
   TransferInfoList::buffer_size_method_t mBufferSizeMethod;
   pattern_t mPattern;
	unsigned char mUserPattern;
   TransferInfoList::fill_method_t mFillMethod;     // Random or sequential.
   io_method_t mIOMethod;         // Synchronous or asynchronous.

   Transfer *mTransfer;           // Encapulates the transfer operations. 

   u32_t mSeed;                   // Random number seed.

   int mFd;                       // File descriptor for transfer.
   unsigned char *mRealBuffer;    // Memory block to hold buffer.
   unsigned char *mBuffer;        // Page-aligned pointer to mRealBuffer.

   TimeHack mJobStartTime;
   TimeHack mJobEndTime;
   capacity_t mJobBytesTransferred;

   bool mRunningHack;
   TimeHack mHackStartTime;
   TimeHack mHackEndTime;
   capacity_t mHackBytesTransferred;

   TimeHack mTransferStartTime;
   TimeHack mTransferEndTime;
   capacity_t mBytesTransferred;  // Bytes in last transfer.

   string mLastErrorMsg;          // Holds last reported error message.
   capacity_t mNumTransfersWithDataIntegrityErrors; // Number of transfers
                                                    // with any number of data 
                                                    // integrity errors.
   capacity_t mJobId;  

protected:
   TransferInfoList *mTransferInfoList;  

};

#endif  // JOB_H
