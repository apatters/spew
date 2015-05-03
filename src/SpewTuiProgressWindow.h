//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewTuiProgressWindow.h
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

#ifndef SPEWTUIPROGRESSWINDOW_H
#define SPEWTUIPROGRESSWINDOW_H

#include <string>
#include <ncurses.h>

#include "SpewTui.h"
#include "SpewTuiWindow.h"

class SpewTuiProgressWindow: public SpewTuiWindow
{
public:
   SpewTuiProgressWindow(const SpewTui *spewTui,
                         int height,
                         int width,
                         int startx,
                         int starty);
   
   int show();
   int clear();
   virtual int resize(int height, int width,int startx, int starty);

   unsigned int getCurrentNumHackRows() const;
   unsigned int getCurrentNumHackColumns() const;

   void hack();
   void endHack();
   void errorHack();
   void errorEndHack();
   void noHack();
   void noEndHack();
   void nextHackRow();
   void currentTransferPercentage(long double percentage);
   void currentTransferRate(capacity_t bytesTransferred,
                            const TimeHack& transferTime);
   void statistics(unsigned int iteration,
                   capacity_t bytesTransferred,
                   const TimeHack& transferTime,
                   const TimeHack& totalTransferTime);

   void startRun() {}; 
   void endRun() {};
   void startJob();
   void endJob();
   void nextJob();

   virtual ~SpewTuiProgressWindow();

private:
   SpewTuiProgressWindow();  // Hide default constructor.

private:
   int mNextHackX;
   int mNextHackY;

   int mPercCompFieldStartX;
   int mTransRateFieldStartX;

};

#endif // SPEWTUIPROGRESSWINDOW_H
