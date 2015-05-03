//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// TransferInfoList.h
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

#ifndef TRANSFERINFOLIST_H
#define TRANSFERINFOLIST_H

#include "common.h"
#include "TransferInfo.h"

class TransferInfoList
{
public:
   typedef enum {GEOMETRIC_PROGRESSION} buffer_size_method_t;
   typedef enum {SEQUENTIAL_FILL, 
                 RANDOM_FILL} fill_method_t;

public:
   TransferInfoList(capacity_t minBufferSize, 
                    capacity_t maxBufferSize,
                    buffer_size_method_t bufferSizeMethod,
                    fill_method_t fillMethod,
                    capacity_t startingOffset,
                    capacity_t transferSize);

   virtual int init() = 0;

   virtual const TransferInfo* next() = 0;
   capacity_t getSize() const { return mNumOfElems; };
   
   virtual ~TransferInfoList();

private:
   // Hide default methods.
   TransferInfoList();  
   TransferInfoList(const TransferInfoList&); 
   TransferInfoList& operator=(const TransferInfoList &list);

protected:
   capacity_t mMinBufferSize;
   capacity_t mMaxBufferSize;
   buffer_size_method_t mBufferSizeMethod;
   fill_method_t mFillMethod;
   capacity_t mStartingOffset;
   capacity_t mTransferSize;

   capacity_t mCurrentElem;
   capacity_t mNumOfElems;
};

#endif  // TRANSFERINFOLIST_H
