//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// RandomTransferInfoList.cpp
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

#include <cstring>

#include "common.h"
#include "Random.h"
#include "RandomTransferInfoList.h"


//////////////  RandomTransferInfoList::count_bits()  /////////////////////////
unsigned int RandomTransferInfoList::count_bits(capacity_t value)
{
   unsigned int bitCount = 0;
   while (value > 0)
   {
      bitCount++;
      value >>= 1;
   }
   return bitCount;
}


//////////////  RandomTransferInfoList::RandomTransferInfoList()  /////////////
RandomTransferInfoList::RandomTransferInfoList(
   capacity_t minBufferSize, 
   capacity_t maxBufferSize,
   buffer_size_method_t bufferSizeMethod,
   fill_method_t fillMethod,
   capacity_t startingOffset,
   capacity_t transferSize,
   u64_t seed):
   TransferInfoList(minBufferSize,
                    maxBufferSize,
                    bufferSizeMethod,
                    fillMethod,
                    startingOffset,
                    transferSize),
   mSeed(seed)
{
   mInternalTransferInfoList = 0;
}


//////////////  RandomTransferInfoList::RandomTransferInfoList()  /////////////
int RandomTransferInfoList::init()
{
   Random rnd(mSeed);

   capacity_t minBufferSizeBits = count_bits(mMinBufferSize);
   capacity_t maxBufferSizeBits = count_bits(mMaxBufferSize);
   capacity_t bitDifference = maxBufferSizeBits - minBufferSizeBits;
   capacity_t chunkSize  = (bitDifference + 1) * mMaxBufferSize;
   capacity_t transfersPerChunk = (1 << (bitDifference + 1)) - 1; 
   capacity_t numChunks = mTransferSize/chunkSize;
   capacity_t numChunkTransfers = numChunks * transfersPerChunk;
   capacity_t numNonChunkTransfers = (mTransferSize - (numChunks * chunkSize))/mMinBufferSize;

   mNumOfElems = numChunkTransfers + numNonChunkTransfers;

   // Allocate a rand_transferinfo for each transfer.
   mInternalTransferInfoList = (rand_transferinfo *)calloc(sizeof(struct rand_transferinfo), mNumOfElems);
   if (!mInternalTransferInfoList)
   {
      return EXIT_ERROR_MEMORY_ALLOC;
   }

   // Populate the transfer info array.
   if (mMinBufferSize == mMaxBufferSize)
   {
      capacity_t currentOffset = mStartingOffset;
      for (capacity_t i = 0; i < mNumOfElems; i++)
      {
         mInternalTransferInfoList[i].rti_offset = currentOffset;
         mInternalTransferInfoList[i].rti_size = mMaxBufferSize;
         currentOffset += mMaxBufferSize;
      }
   }
   else
   {
      // 1. Generate buffer sizes in a geometric progression within one chunck.
      // 2. Shuffle to randomize.
      // 3. Fix up offsets.
      capacity_t arrayPos = 0;
      for (capacity_t i = 0; i < numChunks; i++)
      {
         for (capacity_t numBits = maxBufferSizeBits, 
                 numSameSize = 1, 
                 currBufferSize = 1 << (numBits - 1);
              numBits >= minBufferSizeBits;
              numBits--,
                 numSameSize <<= 1,
                 currBufferSize >>= 1)
         {
            for (capacity_t sameSize = 1;
                 sameSize <= numSameSize;
                 sameSize++, arrayPos++)
            {
               mInternalTransferInfoList[arrayPos].rti_size = currBufferSize;
            }
            
         }
      }
      for (capacity_t i = 0; i < numNonChunkTransfers; i++, arrayPos++) 
      {
         mInternalTransferInfoList[arrayPos].rti_size = mMinBufferSize;
      }
      this->shuffle(rnd);
      for (capacity_t i = 0, currOffset = 0; i < mNumOfElems;  i++)
      {
         mInternalTransferInfoList[i].rti_offset = currOffset;
         currOffset += mInternalTransferInfoList[i].rti_size;
      }
   }

   switch (mFillMethod)
   {
   case SEQUENTIAL_FILL:
      break;
   case RANDOM_FILL:
      this->shuffle(rnd);
      break;
   }

   mCurrentElem = 0;
   return EXIT_OK;
}


/////////////////  RandomTransferInfoList::next()  ////////////////////////////
const TransferInfo* RandomTransferInfoList::next()
{
   static TransferInfo tranInfo(0, 0);

   tranInfo.setSequenceNumber(mCurrentElem);
   tranInfo.setOffset(mInternalTransferInfoList[mCurrentElem].rti_offset);
   tranInfo.setSize(mInternalTransferInfoList[mCurrentElem].rti_size);
   mCurrentElem++;

   return &tranInfo;
}


/////////////////  RandomTransferInfoList::shuffle()  /////////////////////////
void RandomTransferInfoList::shuffle(Random &rnd)
{
   // Randomly swap pairs of transfer numbers.
   struct rand_transferinfo tmpTranInfo;
   capacity_t posA, posB;
   for (capacity_t i = 0; i < mNumOfElems/2; i++)
   {
      posA = (capacity_t)(rnd.getRandom64(mNumOfElems));
      posB = (capacity_t)(rnd.getRandom64(mNumOfElems));
      if (posA == posB)
         continue;
      memcpy(&tmpTranInfo, 
             &mInternalTransferInfoList[posA], 
             sizeof(struct rand_transferinfo));
      memcpy(&mInternalTransferInfoList[posA], 
             &mInternalTransferInfoList[posB], 
             sizeof(struct rand_transferinfo));
      memcpy(&mInternalTransferInfoList[posB], 
             &tmpTranInfo,
             sizeof(struct rand_transferinfo));
   }
}

//////////////  RandomTransferInfoList::~RandomTransferInfoList()  ////////////
RandomTransferInfoList::~RandomTransferInfoList()
{
   free(mInternalTransferInfoList);
}





