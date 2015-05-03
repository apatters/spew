//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewTuiProgressWindow.cpp
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

#include <cstring>

#include "common.h"
#include "SpewTui.h"
#include "SpewTuiProgressWindow.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Local Constants  //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const char WRITE_HACK_STR[] = "w-";
static const char END_WRITE_HACK_STR[] = "w ";
static const char READ_HACK_STR[] = "r-";
static const char END_READ_HACK_STR[] = "r ";
static const char ERROR_HACK_STR[] = "E-";
static const char END_ERROR_HACK_STR[] = "E ";
static const char NO_HACK_STR[] = "  ";
static const char END_NO_HACK_STR[] = "  ";


static const int PERCENTCOMP_FIELD_WIDTH = 5;
static const int TRANSRATE_FIELD_MAX_DIGITS = 12; 
static const int TRANSRATE_FIELD_PRECISION = 2;
static const int TRANSRATE_FIELD_UNITS_WIDTH = 5;
static const int TRANSRATE_FIELD_WIDTH = TRANSRATE_FIELD_MAX_DIGITS + 1 +
                                         TRANSRATE_FIELD_UNITS_WIDTH;


/////////  SpewTuiProgressWindow::SpewTuiProgressWindow()  ////////////////////
SpewTuiProgressWindow::SpewTuiProgressWindow(const SpewTui *spewTui,
                                             int height,
                                             int width,
                                             int startx,
                                             int starty) :
   SpewTuiWindow(spewTui, height, width, startx, starty)
{
   mNextHackX = 0;
   mNextHackY = 0;

   mPercCompFieldStartX = this->getCurrentNumHackColumns() * 2;
   mTransRateFieldStartX = mPercCompFieldStartX + PERCENTCOMP_FIELD_WIDTH;
}


/////////////////  SpewTuiProgressWindow::show()  ////////////////////////////
int SpewTuiProgressWindow::show()
{
   SpewTuiWindow::show();

   this->refresh();
   return 0;
}


/////////////////  SpewTuiProgressWindow::clear()  ////////////////////////////
int SpewTuiProgressWindow::clear()
{
   SpewTuiWindow::clear();

   mNextHackX = 0;
   mNextHackY = 0;

   return 0;
}


/////////////////  SpewTuiProgressWindow::resize()  ///////////////////////////
int SpewTuiProgressWindow::resize(int height, 
                                  int width, 
                                  int startx,
                                  int starty)
{
   SpewTuiWindow::resize(height, width, startx, starty);

   mPercCompFieldStartX = this->getCurrentNumHackColumns() * 2;
   mTransRateFieldStartX = mPercCompFieldStartX + PERCENTCOMP_FIELD_WIDTH;

   if (mIsShowing)
   {
      this->show();
   }
   
   return EXIT_OK;
}


/////  SpewTuiProgressWindow::getCurrentNumHackRows()  ////////////////////////
unsigned int SpewTuiProgressWindow::getCurrentNumHackRows() const
{
   return mWindowHeight;
}


/////  SpewTuiProgressWindow::getCurrentNumHackColumns()  /////////////////////
unsigned int SpewTuiProgressWindow::getCurrentNumHackColumns() const
{
   return (mWindowWidth - PERCENTCOMP_FIELD_WIDTH - TRANSRATE_FIELD_WIDTH)/2;
}


//////////////////  SpewTuiProgressWindow::hack()  ////////////////////////////
void SpewTuiProgressWindow::hack()
{
   const char *hackStr;
   switch (mSpewTui->getCurrentIoDirection())
   {
   case READING:
      hackStr = READ_HACK_STR;
      break;
   case WRITING:
      hackStr = WRITE_HACK_STR;
      break;
   }

   mvwprintw(mWindow, mNextHackY, mNextHackX, (char *)hackStr);
   mNextHackX += strlen(WRITE_HACK_STR);
}


/////////////////  SpewTuiProgressWindow::endHack()  //////////////////////////
void SpewTuiProgressWindow::endHack()
{
   const char *noHackStr;
   switch (mSpewTui->getCurrentIoDirection())
   {
   case READING:
      noHackStr = END_READ_HACK_STR;
      break;
   case WRITING:
      noHackStr = END_WRITE_HACK_STR;
      break;
   }

   mvwprintw(mWindow, mNextHackY, mNextHackX, (char *)noHackStr);
   mNextHackX += strlen(WRITE_HACK_STR);;
}


////////////////  SpewTuiProgressWindow::errorHack()  /////////////////////////
void SpewTuiProgressWindow::errorHack()
{
   mvwprintw(mWindow, mNextHackY, mNextHackX, (char *)ERROR_HACK_STR);
   mNextHackX += strlen(ERROR_HACK_STR);
}


////////////////  SpewTuiProgressWindow::errorEndHack()  //////////////////////
void SpewTuiProgressWindow::errorEndHack()
{
   mvwprintw(mWindow, mNextHackY, mNextHackX, (char *)END_ERROR_HACK_STR);
   mNextHackX += strlen(END_ERROR_HACK_STR);
}


////////////////  SpewTuiProgressWindow::noHack()  ////////////////////////////
void SpewTuiProgressWindow::noHack() 
{
   mvwprintw(mWindow, mNextHackY, mNextHackX, (char *)NO_HACK_STR);
   mNextHackX += strlen(NO_HACK_STR);
}


//////////////////  SpewTuiProgressWindow::noEndHack()  ///////////////////////
void SpewTuiProgressWindow::noEndHack()
{

   mvwprintw(mWindow, mNextHackY, mNextHackX, (char *)NO_HACK_STR);
   mNextHackX += strlen(NO_HACK_STR);
}


/////////////////  SpewTuiProgressWindow::nextHackRow()  //////////////////////
void SpewTuiProgressWindow::nextHackRow()
{
   mNextHackX = 0;
   mNextHackY++;
}


/////////////////  SpewTuiProgressWindow::currentTransferPercentage()  ////////
void SpewTuiProgressWindow::currentTransferPercentage(long double percentage)
{
   mvwprintw(mWindow, 
             mNextHackY,
             mPercCompFieldStartX,
             "%*s",
             PERCENTCOMP_FIELD_WIDTH,
             "");
             
   mvwprintw(mWindow, 
             mNextHackY,
             mPercCompFieldStartX,
             "%3.Lf%%", percentage);
}


/////////////////  SpewTuiProgressWindow::currentTransferRate()  //////////////
void SpewTuiProgressWindow::currentTransferRate(capacity_t bytesTransferred,
                                                const TimeHack& transferTime) 
{
   long double transferRate = convertCapacity(bytesTransferred, mSpewTui->getCurrentUnits())/(long double)transferTime.getTime();

   mvwprintw(mWindow, 
             mNextHackY,
             mTransRateFieldStartX,
             "%*s",
             TRANSRATE_FIELD_WIDTH,
             "");

   mvwprintw(mWindow, 
             mNextHackY, 
             mTransRateFieldStartX,
             "%*.*Lf %-*s",
             TRANSRATE_FIELD_MAX_DIGITS,
             TRANSRATE_FIELD_PRECISION,
             transferRate,
             TRANSRATE_FIELD_UNITS_WIDTH,
             getTransferRateUnitsStr(mSpewTui->getCurrentUnits()));
}


/////////////////  SpewTuiProgressWindow::startJob()  /////////////////////////
void SpewTuiProgressWindow::startJob()
{
}


/////////////////  SpewTuiProgressWindow::endJob()  ///////////////////////////
void SpewTuiProgressWindow::endJob()
{
}


/////////////////  SpewTuiProgressWindow::nextJob()  //////////////////////////
void SpewTuiProgressWindow::nextJob()
{
   this->clear();
}


////////////////  SpewTuiProgressWindow::~SpewTuiProgressWindow()  ////////////
SpewTuiProgressWindow::~SpewTuiProgressWindow()
{
}

