//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewTuiWindow.h
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

#ifndef SPEWTUIWINDOW_H
#define SPEWTUIWINDOW_H

#include <string>
#include <ncurses.h>

#include "SpewTui.h"

class SpewTuiWindow
{

public:
   SpewTuiWindow(const SpewTui *spewTui,
                 int height,
                 int width,
                 int startx,
                 int starty);
   
   virtual int create();
   virtual int destroy();
   virtual int show() = 0;
   virtual int refresh();
   virtual int hide();
   virtual int clear();
   virtual int resize(int height, int width, int startx, int starty);

   virtual int getKey(bool block=false);

   virtual ~SpewTuiWindow();

private:
   SpewTuiWindow();  // Hide default constructor.

protected:
   const SpewTui *mSpewTui;
   int mWindowHeight;
   int mWindowWidth;
   int mWindowStartX;
   int mWindowStartY;

   WINDOW *mWindow;
   bool mIsShowing;
};

#endif // SPEWTUIWINDOW_H
