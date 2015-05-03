//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// Transfer.cpp
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
#include <sys/types.h>
#include <errno.h>

#include "common.h"
#include "Transfer.h"

//////////////////////////  Transfer::Transfer()  /////////////////////////////
Transfer::Transfer(Log &logger,
                   int fd, 
                   unsigned char *buffer, 
                   capacity_t maxBufferSize,
                   capacity_t id,
						 IoDirection_t direction) : 
   mLogger(logger),
   mFd(fd),
   mBuffer(buffer), 
   mMaxBufferSize(maxBufferSize),
   mId(id),
	mIoDirection(direction)
      
{
   mCurrentOffset = 0;
}


//////////////////////////  Transfer::read()  /////////////////////////////////
int Transfer::read(const TransferInfo &transInfo,  string &errorMsg)
{
   capacity_t bufferSize = transInfo.getSize();
   capacity_t remaining = bufferSize;
   capacity_t transferNumber = transInfo.getSequenceNumber();
   capacity_t offset = transInfo.getOffset();

   while (remaining > 0)
   {
      ssize_t count = ::read(mFd, 
                             mBuffer + bufferSize - remaining, 
                             remaining); 
      if (count == 0)
         break;

      if (count < 0)
      {
         errorMsg = strPrintf("Read I/O error at offset %llu of %llu bytes during transfer %llu - %s.\n",  offset + bufferSize - remaining, remaining, transferNumber, strError(errno).c_str()); 
         return count;
      }

      if ((capacity_t)count < remaining)
      {
         mLogger.logNote("Read underrun -- read only %llu bytes of %llu bytes at offset %llu during transfer %llu.\n", (capacity_t)count, bufferSize,  offset + bufferSize - remaining, transferNumber); 
      }
      remaining -= count;
   }

   return bufferSize - remaining;
}


//////////////////////////  Transfer::write()  ///////////////////////////////
int Transfer::write(const TransferInfo &transInfo, string &errorMsg)
{
   capacity_t bufferSize = transInfo.getSize();
   capacity_t remaining = bufferSize;
   capacity_t transferNumber = transInfo.getSequenceNumber();
   capacity_t offset = transInfo.getOffset();

   while (remaining > 0)
   {
      ssize_t count = ::write(mFd, 
                              mBuffer + bufferSize - remaining, 
                              remaining); 
      if (count == 0)
         break;

      if (count < 0)
      {
         errorMsg = strPrintf("Write I/O error -- attempted write of %llu bytest at offset %llu during transfer %llu - %s.\n",  remaining, offset + bufferSize - remaining, transferNumber, strError(errno).c_str()); 
         return count;
      }

      if ((capacity_t)count < remaining)
      {
         mLogger.logNote("Write underrun -- wrote only %llu bytes of %llu bytes at offset %llu during transfer %llu.\n", (capacity_t)count, bufferSize,  offset + bufferSize - remaining, transferNumber); 
      }
      remaining -= count;
   }

   return bufferSize - remaining;
}


//////////////////////////  Transfer::io()  //////////////////////////////////
int Transfer::io(const TransferInfo &transInfo, string &errorMsg)
{
	int rtn;
	switch (mIoDirection)
	{
	case READING:
		rtn = this->read(transInfo, errorMsg);
		break;
	case WRITING:
		rtn = this->write(transInfo, errorMsg);
	}
	return rtn;
}


//////////////////////////  Transfer::seek()  /////////////////////////////////
int Transfer::seek(const TransferInfo &tranInfo, string &errorMsg)
{
   capacity_t newOffset = tranInfo.getOffset();
   if (newOffset != mCurrentOffset)
   {
      off_t ret = lseek(mFd, (off_t)newOffset, SEEK_SET);
      if (ret < 0)
      {
         errorMsg = strError(errno);
         return EXIT_ERROR_IO;
      }
   }

   mCurrentOffset = newOffset;
   return EXIT_OK;
}



