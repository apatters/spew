//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// Transfer.h
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

#ifndef TRANSFER_H
#define TRANSFER_H

#include <string>

#include "common.h"
#include "TransferInfo.h"


class Transfer
{
public:
   static const capacity_t BUFFER_SIZE_INCREMENT = 512LLU;
   static const capacity_t OFFSET_INCREMENT = BUFFER_SIZE_INCREMENT;
#ifdef HAVE_DIRECT_IO
   static const capacity_t DIRECTIO_BUFFER_SIZE_INCREMENT = 1024LLU;
#endif

protected:
   static const unsigned int PAYLOAD_SIZE = 4;  // sizeof(struct datum) must
                                                // and even multiple of 512.
   struct datum 
   {
      capacity_t d_id;
      capacity_t d_sequence_number;
      capacity_t d_offset;
      capacity_t d_buffer_size;
      capacity_t d_payload[PAYLOAD_SIZE];
   };

public:
   Transfer(int fd, 
            unsigned char *buffer, 
            capacity_t maxBufferSize,
            capacity_t id);
             
   virtual int read(const TransferInfo &tranInfo,  string &errorMsg) = 0;
   virtual int write(const TransferInfo &tranInfo, string &errorMsg) = 0;
   

   virtual ~Transfer() {};

protected:
   int seek(const TransferInfo &tranInfo, string &errorMsg);

private:
   Transfer();                // Hide default constructor.
   Transfer(const Transfer&); // Hide copy constructor.

protected:
   int mFd;
   unsigned char *mBuffer;
   capacity_t mMaxBufferSize;
   capacity_t mCurrentOffset;
   capacity_t mId;

};

#endif  // TRANSFER_H
