//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewTuiWindow.cpp
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

#include <stdio.h>
#include <stdarg.h>
#include <ncurses.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common.h"
#include "SpewTui.h"
#include "SpewTuiWindow.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Local Constants()  ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/////////  SpewTuiWindow::SpewTuiWindow()  ////////////////////////////////////
SpewTuiWindow::SpewTuiWindow(const SpewTui *spewTui,
                             int height,
                             int width,
                             int startx,
                             int starty) :
   mSpewTui(spewTui),
   mWindowHeight(height),
   mWindowWidth(width),
   mWindowStartX(startx),
   mWindowStartY(starty)
{
   mWindow = 0;
   mIsShowing = false;
}


/////////////////  SpewTuiWindow::create()  ///////////////////////////////////
int SpewTuiWindow::create()
{
   mWindow = newwin(mWindowHeight,
                    mWindowWidth,
                    mWindowStartY,
                    mWindowStartX);

   keypad(mWindow, TRUE);
   nodelay(mWindow, TRUE);

   return 0;
}


/////////////////  SpewTuiWindow::destroy()  //////////////////////////////////
int SpewTuiWindow::destroy()
{
   mIsShowing = false;
   delwin(mWindow);
   return 0;
}


/////////////////  SpewTuiWindow::show()  //////////////////////////////////
int SpewTuiWindow::show()
{
   mIsShowing = true;
   wclear(mWindow);

   return 0;
}


/////////////////  SpewTuiWindow::refresh()  //////////////////////////////////
int SpewTuiWindow::refresh()
{
   wrefresh(mWindow);
   return 0;
}


/////////////////  SpewTuiWindow::hide()  ////////////////////////////////////
int SpewTuiWindow::hide()
{
   mIsShowing = false;
   touchwin(mWindow);

   return 0;
}


/////////////////  SpewTuiWindow::clear()  ////////////////////////////////////
int SpewTuiWindow::clear()
{
   wclear(mWindow);
   wrefresh(mWindow);

   return 0;
}


/////////////////  SpewTuiWindow::resize()  //////////////////////////////////
int SpewTuiWindow::resize(int height, int width, int startx, int starty)
{
   mWindowHeight = height;
   mWindowWidth = width;
   mWindowStartX = startx;
   mWindowStartY = starty;
   wresize(mWindow, height, width);
   mvwin(mWindow, mWindowStartY, mWindowStartX);
   
   
   return 0;
}


/////////////////  SpewTuiWindow::getKey()  ///////////////////////////////////
int SpewTuiWindow::getKey(bool block)
{
   char ch = ERR;

   if (block)
      nodelay(mWindow, FALSE);
   ch = wgetch(mWindow);
   if (block)
      nodelay(mWindow, TRUE);

   return ch;
}


////////////////  SpewTuiWindow::~SpewTuiWindow()  ////////////////
SpewTuiWindow::~SpewTuiWindow()
{
}

