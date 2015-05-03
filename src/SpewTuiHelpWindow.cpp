//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewTuiHelpWindow.cpp
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

#include <stdio.h>
#include <stdarg.h>
#include <ncurses.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common.h"
#include "SpewTuiWindow.h"
#include "SpewTuiHelpWindow.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Local Constants()  ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const int HEADER_START_X = 0;
const int HEADER_START_Y = 0;

const int TEXT_START_X = 0;
const int TEXT_START_Y = 2;

const int PROMPT_START_X = 0; 

const char HEADER_STR[] = "HELP";
const char PROMPT_STR[] = "<Press any key to continue>";
const char TEXT_STR[] =
"Abbreviations\n"
"\n"
"  WTR  write transfer rate              KiB/KB  kibibytes/kilobytes\n"
"  RTR  read transfer rate               MiB/MB  mebibytes/megabytes\n"
"  WTT  write transfer time              GiB/GB  gibibytes/gigabytes\n"
"  RTT  read transfer time               TiB/TB  tebibytes/terabytes\n"
"  s    seconds\n"
"\n"
"Keys\n"
"\n"
"  h,?        Put up this help screen\n"
"  <space>    Pause\n"
"  q          Quit\n"
"  p          Toggle display of progress\n"
"  k/K        Use Kibibytes (2^10)/Kilobytes (10^3)\n"
"  m/M        Use Mebibytes (2^20)/Megabytes (10^3)\n"        
"  g/G        Use Gibibytes (2^30)/Gigabytes (10^9)\n"
"  t/T        Use Tebibytes (2^40)/Terabytes (10^12)\n"
   "";


///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Class variable()  /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/////////  SpewTuiHelpWindow::SpewTuiHelpWindow()  ////////////////////////////
SpewTuiHelpWindow::SpewTuiHelpWindow(const SpewTui *spewTui,
                                     int height,
                                     int width,
                                     int startx,
                                     int starty) :
   SpewTuiWindow(spewTui, height, width, startx, starty)
{
}


/////////////////  SpewTuiHelpWindow::show()  /////////////////////////////////
int SpewTuiHelpWindow::show()
{
   SpewTuiWindow::show();

   mvwaddstr(mWindow, 
             HEADER_START_Y, 
             HEADER_START_X + (mWindowWidth - strlen(HEADER_STR))/2, 
             HEADER_STR);

   mvwaddstr(mWindow, TEXT_START_Y, TEXT_START_X, TEXT_STR);

   mvwaddstr(mWindow,
             mWindowHeight - 1, 
             PROMPT_START_X + (mWindowWidth - strlen(PROMPT_STR))/2,
             PROMPT_STR);

   this->refresh();
   return 0;
}


/////////////////  SpewTuiHelpWindow::resize()  ///////////////////////////////
int SpewTuiHelpWindow::resize(int height, int width, int startx, int starty)
{
   SpewTuiWindow::resize(height, width, startx, starty);
   if (mIsShowing)
   {
      this->show();
   }

   return EXIT_OK;
}


////////////////  SpewTuiHelpWindow::~SpewTuiHelpWindow()  ////////////////
SpewTuiHelpWindow::~SpewTuiHelpWindow()
{
}

