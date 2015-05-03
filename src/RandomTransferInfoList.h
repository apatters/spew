//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// RandomTransferInfoList.h
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

#ifndef RANDOMTRANSFERINFOLIST_H
#define RANDOMTRANSFERINFOLIST_H

#include "common.h"
#include "Random.h"
#include "TransferInfo.h"
#include "TransferInfoList.h"

class RandomTransferInfoList: public TransferInfoList
{
private:
   static unsigned int count_bits(capacity_t value);

private:
   struct rand_transferinfo 
   {
      capacity_t rti_offset;
      capacity_t rti_size;
   };

public:
   RandomTransferInfoList(capacity_t minBufferSize, 
                          capacity_t maxBufferSize,
                          buffer_size_method_t bufferSizeMethod,
                          fill_method_t fillMethod,
                          capacity_t startingOffset,
                          capacity_t transferSize,
                          u64_t seed);

   virtual int init();
   virtual const TransferInfo* next();
   
   virtual ~RandomTransferInfoList();

private:
   // Hide default methods.
   RandomTransferInfoList(); 
   RandomTransferInfoList(const TransferInfoList&);
   RandomTransferInfoList& operator=(const RandomTransferInfoList &list); 

   void shuffle(Random &rnd);

private:
   rand_transferinfo *mInternalTransferInfoList;
   u64_t mSeed;
};

#endif  // RANDOMTRANSFERINFOLIST_H
