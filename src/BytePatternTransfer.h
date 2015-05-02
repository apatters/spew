//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// BytePatternTransfer.h
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

#ifndef BYTEPATTERNTRANSFER_H
#define BYTEPATTERNTRANSFER_H

#include <string>

#include "common.h"
#include "Job.h"
#include "Log.h"
#include "Transfer.h"
#include "TransferInfo.h"


class BytePatternTransfer: public Transfer
{
public:
   BytePatternTransfer(int fd, 
							  unsigned char *buffer, 
							  capacity_t bufferSize,
							  pid_t pid,
							  unsigned char pattern);
             
   virtual int read(const TransferInfo &transInfo, string &errorMsg);
   virtual int write(const TransferInfo &transInfo, string &errorMsg);

   virtual ~BytePatternTransfer() {};

private:
   BytePatternTransfer();                     // Hide default constructor.
   BytePatternTransfer(const BytePatternTransfer&); // Hide copy constructor.

protected:
   int mPattern;

};

#endif  // BYTEPATTERNTRANSFER_H
