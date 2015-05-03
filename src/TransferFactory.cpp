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

namespace std {} using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common.h"
#include "Job.h"
#include "TransferFactory.h"
#include "GarbageTransfer.h"
#include "NumbersTransfer.h"
#include "RandomTransfer.h"
#include "BytePatternTransfer.h"

///////////////////  TransferFactory::createInstance()  ///////////////////////
Transfer *TransferFactory::createInstance(Job::pattern_t pattern,
														unsigned char userPattern,
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
      transferPtr = new GarbageTransfer(fd, 
                                        buffer, 
                                        maxBufferSize, 
                                        id);
      break;
   case Job::PATTERN_ZEROS:
      transferPtr = new BytePatternTransfer(fd, 
														  buffer, 
														  maxBufferSize, 
														  id,
			                                   0x00);
      break;
   case Job::PATTERN_ONES:
      transferPtr = new BytePatternTransfer(fd, 
														  buffer, 
														  maxBufferSize, 
														  id,
			                                   0xff);
		break;
	case Job::PATTERN_ALTERNATING:
		transferPtr = new BytePatternTransfer(fd, 
                                            buffer, 
                                            maxBufferSize, 
                                            id,
                                            0xaa);
      break;
   case Job::PATTERN_TRANSFER_NUMBERS:
      transferPtr = new NumbersTransfer(fd, 
                                        buffer, 
                                        maxBufferSize, 
                                        id);
      break;
   case Job::PATTERN_RANDOM:
      transferPtr = new RandomTransfer(fd, 
                                       buffer, 
                                       maxBufferSize, 
                                       id, 
                                       seed);
      break;
   case Job::PATTERN_USER_DEFINED:
      transferPtr = new BytePatternTransfer(fd, 
                                            buffer, 
                                            maxBufferSize, 
                                            id,
                                            userPattern);
      break;
   default:
      transferPtr = 0;
      break;
   }

   return transferPtr;
}





