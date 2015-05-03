//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// CumulativeStatistics.cpp
//
// Spew
//
// Copyright (C) 2005 Hewlett-Packard Corp.
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
#include "CumulativeStatistics.h"


///////////////  CumulativeStatistics::CumulativeStatistics()  ////////////////
CumulativeStatistics::CumulativeStatistics()
{
   this->init();
}

///////////////  CumulativeStatistics::init()  ////////////////////////////////
void CumulativeStatistics::init()
{
   mIterations = 0;
   mTotalReadTransferTime = 0.0;
   mTotalWriteTransferTime = 0.0;
   mTotalBytesRead = 0;
   mTotalBytesWritten = 0;
   mTotalReadOps = 0;
   mTotalWriteOps = 0;
}


////////////////  CumulativeStatistics::~CumulativeStatistics()  //////////////
CumulativeStatistics::~CumulativeStatistics()
{
}


