//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// Statistics.h
//
// Spew
//
// Copyright (C) 2005 Hewlett-Packard Corp.
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

#ifndef STATISTICS_H
#define STATISTICS_H


///////////////////////////////////////////////////////////////////////////////
///////////////////////////  Class Statistics  ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Statistics
{
public:
   Statistics();

   virtual ~Statistics();

private:
   Statistics(const Statistics& stats);
   Statistics& operator=(const Statistics &rhs);
};

#endif // STATISTICS_H
