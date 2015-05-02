//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// RandomTransfer.h
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

#ifndef RANDOMTRANSFER_H
#define RANDOMTRANSFER_H

#include <string>

#include "common.h"
#include "Job.h"
#include "Log.h"
#include "Transfer.h"
#include "Random.h"


class RandomTransfer: public Transfer
{
public:
   RandomTransfer(Log &logger,
                  int fd, 
                  unsigned char *buffer, 
                  capacity_t bufferSize,
                  capacity_t id,
                  u32_t seed);
             
   virtual int read(const TransferInfo &tranInfo, string &errorMsg);
   virtual int write(const TransferInfo &tranInfo, string &errorMsg);

   virtual ~RandomTransfer() {};

private:
   RandomTransfer();                      // Hide default constructor.
   RandomTransfer(const RandomTransfer&); // Hide copy constructor.

   Random mRnd;                           // Hold random number generator.
};

#endif  // RANDOMTRANSFER_H
