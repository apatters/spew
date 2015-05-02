//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewerTui.cpp
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

#include <stdio.h>
#include <stdarg.h>
#include <ncurses.h>

#include "common.h"
#include "SpewerTui.h"
#include "SpewerTuiStatisticsWindow.h"

//////////////////////////  SpewerTui::SpewerTui()  ///////////////////////////
SpewerTui::SpewerTui()
{
}


//////////////////////////  SpewerTui::test()  ////////////////////////////////
int SpewerTui::test()
{
   refresh();
   SpewerTuiStatisticsWindow statsWin;
   statsWin.show();
   getch();
   statsWin.setIterations(5);
   statsWin.setNumJobs(20);
   statsWin.setRunTime(7256.0);
   statsWin.setUnits(MEGABYTES);
   statsWin.setCumulativeWriteTransferRate(345.0);
   statsWin.setCumulativeReadTransferRate(120.0);
   statsWin.show();
   getch();
   statsWin.clear();
   getch();
   statsWin.setIterations(0);
   statsWin.show();
   getch();

   return 0;
}


//////////////////////////  SpewerTui::~SpewerTui()  //////////////////////////
SpewerTui::~SpewerTui()
{
}

