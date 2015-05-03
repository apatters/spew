//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// Tui.h
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

#ifndef TUI_H
#define TUI_H

#include <string>

class Tui
{
public:
   Tui();  
   
   int init();
   int close();
   int clearScreen();
   int print(const char *msg);
   int setScreenSize();

   int getCurrentScreenRows() const { return mCurrentScreenRows; };
   int getCurrentScreenColumns() const { return mCurrentScreenColumns; };
   
   ~Tui();

private:
   bool mClosed;
   int mCurrentScreenRows;
   int mCurrentScreenColumns;

};

#endif // TUI_H
