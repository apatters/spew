//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewTuiStatusWindow.cpp
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
#include <unistd.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common.h"
#include "SpewTui.h"
#include "SpewTuiWindow.h"
#include "SpewTuiStatusWindow.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Local Constants()  ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



/////////  SpewTuiStatusWindow::SpewTuiStatusWindow()  ////////////////////////
SpewTuiStatusWindow::SpewTuiStatusWindow(const SpewTui *spewTui,
                                         int height,
                                         int width,
                                         int startx,
                                         int starty) :
   SpewTuiWindow(spewTui, height, width, startx, starty)
{
   mMessageFieldStartY = mWindowHeight - 1;
   mMessageFieldStartX = 0;
}


/////////////////  SpewTuiStatusWindow::show()  ///////////////////////////////
int SpewTuiStatusWindow::show()
{
   SpewTuiWindow::show();

   this->helpPrompt();
   this->refresh();
   return 0;
}


/////////////////  SpewTuiStatusWindow::resize()  /////////////////////////////
int SpewTuiStatusWindow::resize(int height, int width, int startx, int starty)
{
   SpewTuiWindow::resize(height, width, startx, starty);
   mMessageFieldStartY = mWindowHeight - 1;

   if (mIsShowing)
   {
      this->show();
   }

   return EXIT_OK;
}

/////////////////  SpewTuiStatusWindow::message()  ////////////////////////////
void SpewTuiStatusWindow::message(const char *fmt, ...)
{
   char msg[256];
   int len;
   va_list ap;
   va_start(ap, fmt);
   len = vsnprintf(msg, 256, fmt, ap), 
   va_end(ap);

   mvwprintw(mWindow,
             mMessageFieldStartY,
             mMessageFieldStartX,
             "%-*s",
             mWindowWidth,
             "");

   // Center the message in the field.
   mvwprintw(mWindow,
             mMessageFieldStartY,
             mMessageFieldStartX + (mWindowWidth - len)/2,
             "%s",
             msg);
}


/////////////////  SpewTuiStatusWindow::pause()  ////////////////////////////
void SpewTuiStatusWindow::pause(unsigned int timeout)
{
   if (timeout)
   {
      for (unsigned int i = timeout; i > 0; i--)
      {
         this->message("<Pausing for %d seconds, press any key to continue>",
                       i);
         wrefresh(mWindow);
         if (this->getKey() != ERR)
            break;
         sleep(1);
         if (this->getKey() != ERR)
            break;
      }
   }
   else
   {
      this->message("<Pausing, press any key to continue>");
      wrefresh(mWindow);
      this->getKey(TRUE);
   }
}


/////////////////  SpewTuiStatusWindow::helpPrompt()  /////////////////////////
void SpewTuiStatusWindow::helpPrompt()
{
   this->message("<Press 'h' or '?' for help>");
}


/////////////////  SpewTuiStatusWindow::nextJob()  ////////////////////////////
void SpewTuiStatusWindow::nextJob()
{
   this->clear();
   this->show();
}


////////////////  SpewTuiStatusWindow::~SpewTuiStatusWindow()  ////////////////
SpewTuiStatusWindow::~SpewTuiStatusWindow()
{
}

