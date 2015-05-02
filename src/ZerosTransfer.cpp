//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// ZerosTransfer.cpp
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
#include "ZerosTransfer.h"

//////////////////////////  ZerosTransfer::ZerosTransfer()  ///////////////////
ZerosTransfer::ZerosTransfer(Log &logger,
                             int fd, 
                             unsigned char *buffer, 
                             capacity_t bufferSize,
                             pid_t pid) : 
   Transfer(logger, fd, buffer, bufferSize, pid)
{
   // Zero out buffer.  This buffer will not change during writes. 
   memset(mBuffer, 0, mMaxBufferSize);
}


//////////////////////////  ZerosTransfer::read()  ////////////////////////////
int ZerosTransfer::read(const TransferInfo &transInfo, string &errorMsg)
{  
   int ret;
   ret = this->seek(transInfo, errorMsg);
   if (ret != EXIT_OK)
      return ret;

   capacity_t bufferSize = transInfo.getSize();
   capacity_t transferNumber = transInfo.getSequenceNumber();
   capacity_t fileOffset = transInfo.getOffset();

   ssize_t count = ::read(mFd, mBuffer, bufferSize); 
   if (count < 0)
   {
      errorMsg = strError(errno);
      return EXIT_ERROR_IO;
   }

   mCurrentOffset += count;
   if ((capacity_t)count != bufferSize)
   {
      errorMsg = strPrintf("Read only %llu bytes of %llu during transfer %llu at offset %llu.", (capacity_t)count, bufferSize, transferNumber, fileOffset); 
      return EXIT_ERROR_UNDERRUN;
   }

   // Check contents of buffer.
   string errors;
   capacity_t errorsFound = 0LLU;
   bool inErrorRange = false;
   capacity_t startingErrorRange = fileOffset;
   capacity_t endingErrorRange = fileOffset;
   for (capacity_t i = 0; i < bufferSize; i++)
   {
      if (*(mBuffer + i) == 0)
      {
         if (inErrorRange)
         {
            endingErrorRange = fileOffset + i - 1;
            if (startingErrorRange == endingErrorRange)
               errors += strPrintf("\t%lld\n", startingErrorRange);
            else
               errors += strPrintf("\t%lld - %lld\n", 
                                   startingErrorRange, 
                                   endingErrorRange);
            inErrorRange = false;
         }
      }
      else
      {
         errorsFound++;
         if (!inErrorRange)
         {
            startingErrorRange = fileOffset + i;
            inErrorRange = true;
         }
      }
   }

   // Report last error range (if it exists).
   if (inErrorRange)
   {
      endingErrorRange = bufferSize;
      if (startingErrorRange == endingErrorRange)
         errors += strPrintf("\t%lld\n", startingErrorRange);
      else
         errors += strPrintf("\t%lld - %lld\n", 
                             startingErrorRange, 
                             endingErrorRange);
   }
   if (errorsFound > 0)
   {
      errorMsg = strPrintf("Data integrity errors for a total of %lld byte(s) found during\ntransfer %llu at offset(s):\n", errorsFound, transferNumber);
      errorMsg += errors;
      return EXIT_ERROR_DATA_INTEGRITY;
   }

   return EXIT_OK;
}


//////////////////////////  ZerosTransfer::write()  ///////////////////////////
int ZerosTransfer::write(const TransferInfo &transInfo, string &errorMsg)
{  
   int ret;
   ret = this->seek(transInfo, errorMsg);
   if (ret != EXIT_OK)
      return ret;

   capacity_t bufferSize = transInfo.getSize();
   capacity_t transferNumber = transInfo.getSequenceNumber();
   capacity_t fileOffset = transInfo.getOffset();

   ssize_t count = ::write(mFd, mBuffer, bufferSize); 
   if (count < 0)
   {
      errorMsg = strError(errno);
      return EXIT_ERROR_IO;
   }

   mCurrentOffset += count;
   if ((capacity_t)count != bufferSize)
   {
      errorMsg = strPrintf("Wrote only %llu bytes of %llu during transfer %llu at offset %llu.", (capacity_t)count, bufferSize, transferNumber, fileOffset); 
      return EXIT_ERROR_UNDERRUN;
   }

   return EXIT_OK;
}
