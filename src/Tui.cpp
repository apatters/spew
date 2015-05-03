//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// Tui.cpp
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

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/ioctl.h>

#include "common.h"
#include "Tui.h"

//////////////////////////  Tui::Tui()  ///////////////////////////////////////
Tui::Tui()
{
   mClosed = false;
   mCurrentScreenRows = 0;
   mCurrentScreenColumns = 0;
}


//////////////////////////  Tui::init()  //////////////////////////////////////
int Tui::init()
{
   initscr();
   cbreak();
   noecho();
   curs_set(0);

   this->setScreenSize();

   return 0;
}


//////////////////////////  Tui::close()  /////////////////////////////////////
int Tui::close()
{
   endwin();
   mClosed = true;

   return 0;
}


//////////////////////////  Tui::clearScreen()  ///////////////////////////////
int Tui::clearScreen()
{
   clear();
   return 0;
}


//////////////////////////  Tui::print()  /////////////////////////////////////
int Tui::print(const char *msg)
{
   printw((char *)msg);
   refresh();
   return 0;
}


//////////////////////////  Tui::setScreenSize()  ////////////////////////////
int Tui::setScreenSize()
{
#ifdef HAVE_WINSIZE_IOCTL
   int newScreenRows, newScreenColumns;
   getmaxyx(stdscr, newScreenRows, newScreenColumns);

#  ifdef TIOCGWINSZ 
   struct winsize w;
   if (ioctl(2, TIOCGWINSZ, &w) < 0)
   {
      return EXIT_ERROR_SYSTEM;
   }
   else
   {
      if (w.ws_row > 0)
          newScreenRows = w.ws_row;
      if (w.ws_col > 0)
         newScreenColumns = w.ws_col;
   }
#  else
#    ifdef WIOCGETD
   struct uwdata w;
   if (ioctl(2, WIOCGETD, &w) < 0)
   {
      return EXIT_ERROR_SYSTEM
   }
   else
   {
      if (w.uw_height > 0)
          newScreenRows = w.uw_height / w.uw_vs;
      if (w.uw_width > 0)
          newScreenColumns = w.uw_width / w.uw_hs;
   }
#    endif
#  endif

   if (newScreenRows != mCurrentScreenRows || 
       newScreenColumns != mCurrentScreenColumns)
   {
      mCurrentScreenRows = newScreenRows;
      mCurrentScreenColumns = newScreenColumns;
      resizeterm(mCurrentScreenRows, mCurrentScreenColumns);
   }
#else // HAVE_WINSIZE_IOCTL
   char *errPtr;
   long int tmpval;

   mCurrentScreenColumns = DEFAULT_SCREEN_COLUMNS;
   const char *colsEnv = getenv("COLUMNS");
   if (colsEnv)
   {
      tmpval = strtol(colsEnv, &errPtr, 10);
      if (errPtr == '\0')
          mCurrentScreenColumns = tmpval;
   }
   
   mCurrentScreenRows = DEFAULT_SCREEN_ROWS;
   const char *rowsEnv = getenv("LINES");
   if (rowsEnv)
   {
      tmpval = strtol(rowsEnv, &errPtr, 10);
      if (errPtr == '\0')
          mCurrentScreenRows = tmpval;
   }
#endif // HAVE_WINSIZE_IOCTL

   return EXIT_OK;
}



//////////////////////////  Tui::~Tui()  //////////////////////////////////////
Tui::~Tui()
{
   if (!mClosed)
      this->close();
}
