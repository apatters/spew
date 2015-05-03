//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewDisplay.cpp
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


#include "common.h"
#include "SpewDisplay.h"

//////////////////////////  SpewDisplay::SpewDisplay()  ///////////////////////
SpewDisplay::SpewDisplay(unsigned int iterationsToDo,
                         Units_t units,
                         bool showProgress,
                         Verbosity_t verbosity): 
   mIterationsToDo(iterationsToDo),
   mCurrentUnits(units),
   mShowProgress(showProgress),
   mVerbosity(verbosity)
{
   mCurrentIteration = 0;
   mCurrentIoDirection = WRITING;
}


//////////////////////////  SpewDisplay::setCurrentUnits()  ///////////////////
void SpewDisplay::setCurrentUnits(Units_t units)
{
   mCurrentUnits = units;
}


//////////////////////////  SpewDisplay::startJob()  //////////////////////////
void SpewDisplay::startJob(unsigned int iteration, IoDirection_t direction)
{
   mCurrentIteration = iteration;
   mCurrentIoDirection = direction;
}

//////////////////////////  SpewDisplay~SpewDisplay()  ////////////////////////
SpewDisplay::~SpewDisplay()
{
}
