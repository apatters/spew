//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// CumulativeStatisticsReadWrite.cpp
//
// Spew
//
// Copyright (C) 2006 Hewlett-Packard Corp.
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
#include "CumulativeStatisticsReadWrite.h"


///////  CumulativeStatisticsReadWrite::CumulativeStatisticsReadWrite()  //////
CumulativeStatisticsReadWrite::CumulativeStatisticsReadWrite()
{
#ifdef USE_THREADS
   pthread_mutex_init(&mMutex, NULL);
#endif
   this->init();
}


//////////////  CumulativeStatisticsReadWrite::init()  ////////////////////////
void CumulativeStatisticsReadWrite::init()
{
   this->lock();

   CumulativeStatistics::init();
   mIterations = 0;
   mTotalReadTransferTime = 0.0;
   mTotalWriteTransferTime = 0.0;
   mTotalBytesRead = 0;
   mTotalBytesWritten = 0;
   mTotalReadOps = 0;
   mTotalWriteOps = 0;

   this->unlock();
}


//////////////  CumulativeStatisticsReadWrite::getIterations()  ///////////////
unsigned int CumulativeStatisticsReadWrite::getIterations() const 
{
   unsigned int iterations;

   this->lock();
   iterations = mIterations; 
   this->unlock();

   return iterations;
}


/////////  CumulativeStatisticsReadWrite::setIterations()  ////////////////////
void CumulativeStatisticsReadWrite::setIterations(unsigned int iter)
{ 
   this->lock();
   mIterations = iter; 
   this->unlock();
}


/////////  CumulativeStatisticsReadWrite::incIterations()()  //////////////////
void CumulativeStatisticsReadWrite::incIterations() 
{
   this->lock();
   mIterations++; 
   this->unlock();
}
 
  
/////////  CumulativeStatisticsReadWrite::incIterations()()  //////////////////
TimeHack CumulativeStatisticsReadWrite::getTotalReadTransferTime() const
{
   TimeHack time;

   this->lock();
   time =  mTotalReadTransferTime;
   this->unlock();

   return time;
}


/////////  CumulativeStatisticsReadWrite::setTotalReadTransferTime()  /////////
void CumulativeStatisticsReadWrite::setTotalReadTransferTime(const TimeHack &hack)
{
   this->lock();
   mTotalReadTransferTime = hack;
   this->unlock();
}


/////////  CumulativeStatisticsReadWrite::addToTotalReadTransferTime()  ///////
void CumulativeStatisticsReadWrite::addToTotalReadTransferTime(const TimeHack &hack)
{
   this->lock();
   mTotalReadTransferTime += hack;
   this->unlock();
}


/////////  CumulativeStatisticsReadWrite::getTotalWriteTransferTime()  ////////
TimeHack CumulativeStatisticsReadWrite::getTotalWriteTransferTime() const
{
   TimeHack time;

   this->lock();
   time = mTotalWriteTransferTime; 
   this->unlock();

   return time;
}


/////////  CumulativeStatisticsReadWrite::setTotalWriteTransferTime()  ////////
void CumulativeStatisticsReadWrite::setTotalWriteTransferTime(const TimeHack &hack)
{
   this->lock();
   mTotalWriteTransferTime = hack; 
   this->unlock();
}


/////////  CumulativeStatisticsReadWrite::addToTotalWriteTransferTime()  //////
void CumulativeStatisticsReadWrite::addToTotalWriteTransferTime(const TimeHack &hack)
{ 
   this->lock();
   mTotalWriteTransferTime += hack; 
   this->unlock();
}


/////////  CumulativeStatisticsReadWrite::getTotalBytesRead()()  //////////////
capacity_t CumulativeStatisticsReadWrite::getTotalBytesRead() const 
{
   capacity_t bytes;

   this->lock();
   bytes = mTotalBytesRead;
   this->unlock();

   return bytes;
}


/////////  CumulativeStatisticsReadWrite::setTotalBytesRead()  ////////////////
void CumulativeStatisticsReadWrite::setTotalBytesRead(capacity_t bytes)
{
   this->lock();
   mTotalBytesRead = bytes;
   this->unlock();
}


/////////  CumulativeStatisticsReadWrite::addToTotalBytesRead()  //////////////
void CumulativeStatisticsReadWrite::addToTotalBytesRead(capacity_t bytes)
{
   this->lock();
   mTotalBytesRead += bytes;
   this->unlock();
}


/////////  CumulativeStatisticsReadWrite::getTotalBytesWritten()()  ///////////
capacity_t CumulativeStatisticsReadWrite::getTotalBytesWritten() const
{
   capacity_t bytes;

   this->lock();
   bytes = mTotalBytesWritten;
   this->unlock();

   return bytes;
}


/////////  CumulativeStatisticsReadWrite::setTotalBytesWritten()  /////////////
void CumulativeStatisticsReadWrite::setTotalBytesWritten(capacity_t bytes) 
{ 
   this->lock();
   mTotalBytesWritten = bytes;
   this->unlock();
}


/////////  CumulativeStatisticsReadWrite::addToTotalBytesWritten()  ///////////
void CumulativeStatisticsReadWrite::addToTotalBytesWritten(capacity_t bytes)
{
   this->lock();
   mTotalBytesWritten += bytes; 
   this->unlock();
}


/////////  CumulativeStatisticsReadWrite::getTotalReadOps()()  ////////////////
capacity_t CumulativeStatisticsReadWrite::getTotalReadOps() const
{ 
   capacity_t ops;
   
   this->lock();
   ops = mTotalReadOps;
   this->unlock();

   return ops;
}


/////////  CumulativeStatisticsReadWrite::setTotalReadOps()  //////////////////
void CumulativeStatisticsReadWrite::setTotalReadOps(capacity_t ops)
{
   this->lock();
   mTotalReadOps = ops;
   this->unlock();
}


/////////  CumulativeStatisticsReadWrite::addToTotalReadOps()  ////////////////
void CumulativeStatisticsReadWrite::addToTotalReadOps(capacity_t ops) 
{
   this->lock();
   mTotalReadOps += ops;
   this->unlock();
}

/////////  CumulativeStatisticsReadWrite::getTotalWriteOps()  /////////////////
capacity_t CumulativeStatisticsReadWrite::getTotalWriteOps() const
{
   capacity_t ops;

   this->lock();
   ops = mTotalWriteOps;
   this->unlock();

   return ops;
}


/////////  CumulativeStatisticsReadWrite::setTotalWriteOps()  /////////////////
void CumulativeStatisticsReadWrite::setTotalWriteOps(capacity_t ops) 
{ 
   this->lock();
   mTotalWriteOps = ops;
   this->unlock();
}


/////////  CumulativeStatisticsReadWrite::addToTotalWriteOps()  ///////////////
void CumulativeStatisticsReadWrite::addToTotalWriteOps(capacity_t ops)
{
   this->lock();
   mTotalWriteOps += ops;
   this->unlock();
};


#ifdef USE_THREADS
/////////  CumulativeStatisticsReadWrite::lock()  /////////////////////////////
void CumulativeStatisticsReadWrite::lock() const
{
   pthread_mutex_lock(&mMutex);
}


/////////  CumulativeStatisticsReadWrite::unlock()  ///////////////////////////
void CumulativeStatisticsReadWrite::unlock() const
{
   pthread_mutex_unlock(&mMutex);
}
#endif // USE_THREADS


/////  CumulativeStatisticsReadWrite::~CumulativeStatisticsReadWrite()  ///////
CumulativeStatisticsReadWrite::~CumulativeStatisticsReadWrite()
{
#ifdef USE_THREADS
   pthread_mutex_destroy(&mMutex);
#endif
}


