//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// TransferFactory.cpp
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
#include "Job.h"
#include "TransferFactory.h"
#include "GarbageTransfer.h"
#include "NumbersTransfer.h"
#include "RandomTransfer.h"
#include "ZerosTransfer.h"

///////////////////  TransferFactory::createInstance()  ///////////////////////
Transfer *TransferFactory::createInstance(Log &logger,
                                          Job::pattern_t pattern,
                                          int fd, 
                                          unsigned char *buffer, 
                                          capacity_t maxBufferSize,
                                          capacity_t id,
                                          u64_t seed)
{
   Transfer *transferPtr;
   switch (pattern)
   {
   case Job::PATTERN_NONE:
      transferPtr = new GarbageTransfer(logger,
                                        fd, 
                                        buffer, 
                                        maxBufferSize, 
                                        id);
      break;
   case Job::PATTERN_ZEROS:
      transferPtr = new ZerosTransfer(logger,
                                      fd, 
                                      buffer, 
                                      maxBufferSize, 
                                      id);
      break;
   case Job::PATTERN_TRANSFER_NUMBERS:
      transferPtr = new NumbersTransfer(logger,
                                        fd, 
                                        buffer, 
                                        maxBufferSize, 
                                        id);
      break;
   case Job::PATTERN_RANDOM:
      transferPtr = new RandomTransfer(logger,
                                       fd, 
                                       buffer, 
                                       maxBufferSize, 
                                       id, 
                                       seed);
      break;
   default:
      transferPtr = 0;
      break;
   }

   return transferPtr;
}





