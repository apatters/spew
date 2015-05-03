//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// GarbageTransfer.cpp
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

#include <unistd.h>
#include <errno.h>

#include "common.h"
#include "Transfer.h"
#include "GarbageTransfer.h"

//////////////////////////  GarbageTransfer::GarbageTransfer()  ///////////////
GarbageTransfer::GarbageTransfer(Log &logger,
                                 int fd, 
                                 unsigned char *buffer, 
                                 capacity_t bufferSize,
                                 capacity_t id) : 
   Transfer(logger, fd, buffer, bufferSize, id)
{
}


//////////////////////////  GarbageTransfer::read()  //////////////////////////
int GarbageTransfer::read(const TransferInfo &transInfo, string &errorMsg)
{  
   int ret;
   ret = this->seek(transInfo, errorMsg);
   if (ret != EXIT_OK)
      return ret;

   capacity_t bufferSize = transInfo.getSize();
   capacity_t transferNumber = transInfo.getSequenceNumber();

   ssize_t count = Transfer::read(transInfo, errorMsg); 
   if (count < 0)
      return EXIT_ERROR_IO;
   mCurrentOffset += count;
   if ((capacity_t)count < bufferSize)
   {
      errorMsg = strPrintf("Read underrun -- read only %llu bytes of %llu bytes at offset %llu during transfer %llu.\n", (capacity_t)count, transInfo.getSize(), transInfo.getOffset(), transInfo.getSequenceNumber()); 
      return EXIT_ERROR_UNDERRUN;
   }

   return EXIT_OK;
}


//////////////////////////  GarbageTransfer::write()  /////////////////////////
int GarbageTransfer::write(const TransferInfo &transInfo, string &errorMsg)
{  
   int ret;
   ret = Transfer::seek(transInfo, errorMsg);
   if (ret != EXIT_OK)
      return ret;

   capacity_t bufferSize = transInfo.getSize();
   ssize_t count = Transfer::write(transInfo, errorMsg); 
   if (count < 0)
      return EXIT_ERROR_IO;
   mCurrentOffset += count;
   if ((capacity_t)count < bufferSize)
   {
      errorMsg = strPrintf("Write underrun -- wrote only %llu bytes of %llu bytes at offset %llu during transfer %llu.\n", (capacity_t)count, transInfo.getSize(), transInfo.getOffset(), transInfo.getSequenceNumber()); 
      return EXIT_ERROR_UNDERRUN;
   }

   return EXIT_OK;
}
