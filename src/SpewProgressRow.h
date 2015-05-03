//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewProgressRow.h
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

#ifndef SPEWPROGRESSROW_H
#define SPEWPROGRESSROW_H

#include "common.h"
#include "TimeHack.h"


class SpewProgressRow
{
public:
   SpewProgressRow(capacity_t totalTransfersInRow, 
                   capacity_t totalHacksInRow);
   SpewProgressRow(const SpewProgressRow& rhs);
   SpewProgressRow& operator=(const SpewProgressRow& rhs);

   capacity_t getTransfersCompletedInRow() const { return mTransfersCompletedInRow; }; 
   void setTranfersCompletedInRow(capacity_t numTransfers) { mTotalTransfersInRow = numTransfers; };
   void addToTransfersCompletedInRow(capacity_t numTransfers) { mTransfersCompletedInRow += numTransfers; };

   capacity_t getTotalTransfersInRow() const { return mTotalTransfersInRow; };
   void setTotalTransfersInRow(capacity_t numTransfers) { mTotalTransfersInRow = numTransfers; };

   capacity_t getHacksCompletedInRow() const;
   capacity_t getTotalHacksInRow() const { return mTotalHacksInRow; };

   TimeHack getTransferTimeForRow() const { return mTransferTimeForRow; };
   TimeHack setTransferTimeForRow(TimeHack time) { mTransferTimeForRow = time; };
   capacity_t getTransfersInNextHack() const;

   ~SpewProgressRow();

private:
   SpewProgressRow();  // Hide default constructor.


protected:
   capacity_t mTotalTransfersInRow;
   capacity_t mTotalHacksInRow;
   capacity_t mTransfersCompletedInRow;
   TimeHack mTransferTimeForRow;
};

#endif // SPEWPROGRESSROW_H
