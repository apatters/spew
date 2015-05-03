//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// TransferInfoListFactory.h
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

#ifndef TRANSFERINFOLISTFACTORY_H
#define TRANSFERINFOLISTFACTORY_H

#include "common.h"
#include "TransferInfo.h"
#include "TransferInfoList.h"

class TransferInfoListFactory
{
public:
   static TransferInfoList *createInstance(
      capacity_t minBufferSize,
      capacity_t maxBufferSize,
      TransferInfoList::buffer_size_method_t bufferSizeMethod,
      TransferInfoList::fill_method_t fillMethod,
      capacity_t offset,
      capacity_t transferSize,
      u64_t seed);

private:
   // Hide default methods.
   TransferInfoListFactory();
   TransferInfoListFactory(const TransferInfoListFactory&); 
   TransferInfoListFactory& operator=(const TransferInfoListFactory &fact);
};

#endif  // TRANSFERINFOLISTFACTORY_H
