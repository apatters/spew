//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewTuiHelpWindow.h
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

#ifndef SPEWTUIHELPWINDOW_H
#define SPEWTUIHELPWINDOW_H

#include <string>
#include <ncurses.h>

#include "SpewTui.h"
#include "SpewTuiWindow.h"

class SpewTuiHelpWindow: public SpewTuiWindow
{

public:
   SpewTuiHelpWindow(const SpewTui *spewTui,
                     int height,
                     int width,
                     int startx,
                     int starty);
   
   virtual int show();
   virtual int resize(int height, int width, int startx, int starty);

   void pause();

   virtual ~SpewTuiHelpWindow();

private:
   SpewTuiHelpWindow();  // Hide default constructor.
};

#endif // SPEWTUIHELPWINDOW_H
