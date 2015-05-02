//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// NumbersTransfer.cpp
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

#include <unistd.h>
#include <errno.h>

#include "common.h"
#include "Transfer.h"
#include "NumbersTransfer.h"

//////////////////////////  NumbersTransfer::NumbersTransfer()  ///////////////
NumbersTransfer::NumbersTransfer(int fd, 
                                 unsigned char *buffer, 
                                 capacity_t bufferSize,
                                 capacity_t id) : 
   Transfer(fd, buffer, bufferSize, id)
{
}


//////////////////////////  NumbersTransfer::read()  //////////////////////////
int NumbersTransfer::read(const TransferInfo &transInfo, string &errorMsg)
{  
   int ret;
   ret = Transfer::seek(transInfo, errorMsg);
   if (ret != EXIT_OK)
      return ret;

   capacity_t bufferSize = transInfo.getSize();
   capacity_t transferNumber = transInfo.getSequenceNumber();
   capacity_t fileOffset = transInfo.getOffset();

   ssize_t count = Transfer::read(transInfo, errorMsg); 
   if (count < 0)
      return EXIT_ERROR_IO;
   mCurrentOffset += count;
   if ((capacity_t)count < bufferSize)
   {
      errorMsg = strPrintf("Read underrun -- read only %llu bytes of %llu bytes at offset %llu during transfer %llu.\n", (capacity_t)count, transInfo.getSize(), transInfo.getOffset(), transInfo.getSequenceNumber()); 
      return EXIT_ERROR_UNDERRUN;
   }

   // Check contents of buffer.
   struct datum chkData;
   chkData.d_id = mId;
   chkData.d_sequence_number = transferNumber;
   chkData.d_offset = fileOffset;
   chkData.d_buffer_size = bufferSize;

   string errors;
   capacity_t errorsFound = 0;
   bool inErrorRange = false;
   capacity_t startingErrorRange = fileOffset;
   capacity_t endingErrorRange = fileOffset;
   capacity_t iterations = bufferSize/sizeof(struct datum);
   capacity_t bufferOffset = 0;
   unsigned int datum_size = sizeof(struct datum);
   unsigned int payload_size = sizeof(chkData.d_payload)/sizeof(capacity_t);
   for (capacity_t i = 0; i < iterations; i++)
   {
      for (unsigned int j = 0; j < payload_size; j++)
      {
         chkData.d_payload[j] = i;
      }
      if (memcmp(&chkData, 
                 (const void *)(mBuffer + bufferOffset), 
                 datum_size) == 0)
      {
         if (inErrorRange)
         {
            endingErrorRange = fileOffset + (i * datum_size) - 1;
            errors += strPrintf("\t%lld - %lld\n", 
                                startingErrorRange, endingErrorRange);
            inErrorRange = false;
         }
      }
      else
      {
         errorsFound++;
         if (!inErrorRange)
         {
            startingErrorRange = fileOffset + (i * datum_size);
            inErrorRange = true;
         }
      }
      bufferOffset += datum_size;
   }

   // Report last error range (if it exists).
   if (inErrorRange)
   {
      endingErrorRange = fileOffset + bufferSize - 1;
      errors += strPrintf("\t%lld - %lld\n", 
                          startingErrorRange,
                          endingErrorRange);

   }
   if (errorsFound > 0)
   {
      errorMsg = strPrintf("Data integrity errors in ~%llu byte(s) found during\ntransfer %llu at file offset(s):\n", errorsFound * datum_size, transferNumber);
      errorMsg += errors;
      return EXIT_ERROR_DATA_INTEGRITY;
   }

   return EXIT_OK;
}


//////////////////////////  NumbersTransfer::write()  /////////////////////////
int NumbersTransfer::write(const TransferInfo &transInfo, string &errorMsg)
{  
   int ret;
   ret = Transfer::seek(transInfo, errorMsg);
   if (ret != EXIT_OK)
      return ret;

   capacity_t bufferSize = transInfo.getSize();
   capacity_t transferNumber = transInfo.getSequenceNumber();
   capacity_t fileOffset = transInfo.getOffset();

   struct datum data;
   data.d_id = mId;
   data.d_sequence_number = transferNumber;
   data.d_offset = transInfo.getOffset();
   data.d_buffer_size = bufferSize;

   // Fill buffer with transfer number pattern (This needs to be optimized
   // as much as possible.
   capacity_t iterations = bufferSize/sizeof(struct datum);
   capacity_t bufferOffset = 0;
   unsigned int datum_size = sizeof(struct datum);
   unsigned int payload_size = sizeof(data.d_payload)/sizeof(capacity_t);
   for (capacity_t i = 0; i < iterations; i++)
   {
      for (capacity_t j = 0;  j < payload_size; j++)
      {
         data.d_payload[j] = i;
      }
      memcpy(mBuffer + bufferOffset, &data, datum_size);  // Expensive.
      bufferOffset += datum_size;
   }

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
