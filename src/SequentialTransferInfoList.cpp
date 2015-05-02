//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SequentialTransferInfoList.cpp
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

#include "common.h"
#include "SequentialTransferInfoList.h"


//////  SequentialTransferInfoList::SequentialTransferInfoList()  /////////////
SequentialTransferInfoList::SequentialTransferInfoList(
   capacity_t minBufferSize, 
   capacity_t maxBufferSize,
   buffer_size_method_t bufferSizeMethod,
   fill_method_t fillMethod,
   capacity_t startingOffset,
   capacity_t transferSize):
   TransferInfoList(minBufferSize,
                    maxBufferSize,
                    bufferSizeMethod,
                    fillMethod,
                    startingOffset,
                    transferSize)
{
   mNextOffset = startingOffset;
}


//////  SequentialTransferInfoList::SequentialTransferInfoList()  /////////////
int SequentialTransferInfoList::init()
{
   mNumOfElems = mTransferSize/mMaxBufferSize;
   return EXIT_OK;
}


/////////////////  SequentialTransferInfoList::next()  ////////////////////////
const TransferInfo* SequentialTransferInfoList::next()
{
   static TransferInfo tranInfo(0, mMaxBufferSize);

   tranInfo.setSequenceNumber(mCurrentElem);
   tranInfo.setOffset(mNextOffset);
   mNextOffset += mMaxBufferSize;
   mCurrentElem++;

   return &tranInfo;
}


//////  SequentialTransferInfoList::~SequentialTransferInfoList()  ////////////
SequentialTransferInfoList::~SequentialTransferInfoList()
{
}





