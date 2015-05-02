//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// TransferInfo.cpp
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
#include "TransferInfo.h"

//////////////////////////  TransferInfo::TransferInfo()  /////////////////////
TransferInfo::TransferInfo(capacity_t offset, capacity_t size):
   mOffset(offset),
   mSize(size)
{
}


//////////////////////////  TransferInfo::TransferInfo()  /////////////////////
TransferInfo::TransferInfo(const TransferInfo &info)
{
   mOffset = info.mOffset;
   mSize = info.mSize;
}


//////////////////////////  TransferInfo::operator=()  ////////////////////////
TransferInfo& TransferInfo::operator=(const TransferInfo &info)
{
   mOffset = info.mOffset;
   mSize = info.mSize;

   return *this;
}


//////////////////////////  TransferInfo::~TransferInfo()  ////////////////////
TransferInfo::~TransferInfo()
{
}







