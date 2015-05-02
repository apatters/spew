//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// TransferInfo.h
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

#ifndef TRANSFERINFO_H
#define TRANSFERINFO_H

#include "common.h"

class TransferInfo
{
   friend class SequentialTransferInfoList;
   friend class RandomTransferInfoList;

public:
   TransferInfo(capacity_t offset, capacity_t size);
   TransferInfo(const TransferInfo &info);
   TransferInfo& operator=(const TransferInfo &info);

   capacity_t getSequenceNumber() const { return mSeqNumber; };
   capacity_t getOffset() const { return mOffset; };
   capacity_t getSize() const { return mSize; };
   
   ~TransferInfo();

private:
   TransferInfo();  // Hide default constructor.

   void setSequenceNumber(capacity_t seqNumber) { mSeqNumber = seqNumber; };
   void setOffset(capacity_t offset) { mOffset = offset; };
   void setSize(capacity_t size) {mSize = size; };

private:
   capacity_t mSeqNumber; 
   capacity_t mOffset;
   capacity_t mSize;
};

#endif  // TRANSFERINFO_H
