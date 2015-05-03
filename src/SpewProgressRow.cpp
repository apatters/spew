//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewProgressRow.cpp
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
#include "SpewProgressRow.h"

//////////////////  SpewProgressRow::SpewProgressRow()  ///////////////////////
SpewProgressRow::SpewProgressRow(capacity_t totalTransfersInRow, 
                                 capacity_t totalHacksInRow) :
   mTotalTransfersInRow(totalTransfersInRow),
   mTotalHacksInRow(totalHacksInRow)
{
   mTransfersCompletedInRow = 0;
}


//////////////////  SpewProgressRow::SpewProgressRow()  ///////////////////////
SpewProgressRow::SpewProgressRow(const SpewProgressRow& rhs)
{
   mTotalTransfersInRow = rhs.mTotalTransfersInRow;
   mTotalHacksInRow = rhs.mTotalHacksInRow;
   mTransfersCompletedInRow = rhs.mTransfersCompletedInRow;
   mTransferTimeForRow = rhs.mTransferTimeForRow;
}


//////////////////  SpewProgressRow::operator=()  /////////////////////////////
SpewProgressRow& SpewProgressRow::operator=(const SpewProgressRow& rhs)
{
   mTotalTransfersInRow = rhs.mTotalTransfersInRow;
   mTotalHacksInRow = rhs.mTotalHacksInRow;
   mTransfersCompletedInRow = rhs.mTransfersCompletedInRow;
   mTransferTimeForRow = rhs.mTransferTimeForRow;
}


//////////////  SpewProgressRow::getHacksCompletedInRow()  ////////////////////
capacity_t SpewProgressRow::getHacksCompletedInRow() const
{
   // Assumes we are at the end of the row.
   capacity_t transPerHack = mTotalTransfersInRow/mTotalHacksInRow;
   if (transPerHack == 0)
      transPerHack = 1;

   return mTransfersCompletedInRow/transPerHack;
}


//////////////  SpewProgressRow::getTransfersInNextHack()  ////////////////////
capacity_t SpewProgressRow::getTransfersInNextHack() const
{
   capacity_t transPerHack = mTotalTransfersInRow/mTotalHacksInRow;
   capacity_t transLeft = mTotalTransfersInRow - mTransfersCompletedInRow;
   capacity_t hacksLeft;

   if (transLeft == 0)
   {
      return 0;
   }
   else if (transPerHack == 0)
   {
      return 1;
   }
   else
   {
      hacksLeft = mTotalHacksInRow - mTransfersCompletedInRow/transPerHack;
      if (hacksLeft <= 1)
         return transLeft;
   }

   return transPerHack;
}


//////////////////  SpewProgressRow~SpewProgressRow()  ////////////////////////
SpewProgressRow::~SpewProgressRow()
{
}
