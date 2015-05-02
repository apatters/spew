//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// TransferInfoListFactory.cpp
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

#include "common.h"
#include "TransferInfoList.h"
#include "RandomTransferInfoList.h"
#include "SequentialTransferInfoList.h"
#include "TransferInfoListFactory.h"


////////////  TransferInfoListFactory::createInstance()  //////////////////////
TransferInfoList *TransferInfoListFactory::createInstance(
      capacity_t minBufferSize,
      capacity_t maxBufferSize,
      TransferInfoList::buffer_size_method_t bufferSizeMethod,
      TransferInfoList::fill_method_t fillMethod,
      capacity_t offset,
      capacity_t transferSize,
      u64_t seed)
{
   if (fillMethod == TransferInfoList::RANDOM_FILL ||
       minBufferSize != maxBufferSize)
   {
      return new RandomTransferInfoList(minBufferSize,
                                        maxBufferSize,
                                        bufferSizeMethod,
                                        fillMethod,
                                        offset,
                                        transferSize,
                                        seed);
   }
   else
   {
      return new SequentialTransferInfoList(minBufferSize,
                                            maxBufferSize,
                                            bufferSizeMethod,
                                            fillMethod,
                                            offset,
                                            transferSize);
   }
}





