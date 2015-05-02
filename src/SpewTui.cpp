//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// SpewTui.cpp
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <ncurses.h>
#include <signal.h>

#include "common.h"
#include "Tui.h"
#include "SpewTui.h"
#include "SpewTuiWindow.h"
#include "SpewTuiHelpWindow.h"
#include "SpewTuiProgressWindow.h"
#include "SpewTuiStatisticsWindow.h"
#include "SpewTuiStatusWindow.h"


///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Local Constants()  ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const int DEFAULT_PAUSE_LENGTH = 5;

static const int MIN_SCREEN_HEIGHT = 9;
static const int MIN_SCREEN_WIDTH = DEFAULT_SCREEN_COLUMNS;

static const int MIN_STATISTICS_WINDOW_HEIGHT = 6;
static const int MIN_STATISTICS_WINDOW_WIDTH = MIN_SCREEN_WIDTH;

static const int MIN_PROGRESS_WINDOW_HEIGHT = 1;
static const int MIN_PROGRESS_WINDOW_WIDTH = MIN_SCREEN_WIDTH;

static const int MIN_STATUS_WINDOW_HEIGHT = 2;
static const int MIN_STATUS_WINDOW_WIDTH = MIN_SCREEN_WIDTH;

static const int MIN_HELP_WINDOW_HEIGHT = 24;
static const int MIN_HELP_WINDOW_WIDTH = MIN_SCREEN_WIDTH;


///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Class Variables  //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int SpewTui::sHelpWindowHeight;
int SpewTui::sHelpWindowWidth;
int SpewTui::sHelpWindowStartY;
int SpewTui::sHelpWindowStartX;
   
int SpewTui::sStatisticsWindowHeight;
int SpewTui::sStatisticsWindowWidth;
int SpewTui::sStatisticsWindowStartY;
int SpewTui::sStatisticsWindowStartX;

int SpewTui::sProgressWindowHeight;
int SpewTui::sProgressWindowWidth;
int SpewTui::sProgressWindowStartY;
int SpewTui::sProgressWindowStartX;

int SpewTui::sStatusWindowHeight;
int SpewTui::sStatusWindowWidth;
int SpewTui::sStatusWindowStartY;
int SpewTui::sStatusWindowStartX;


//////////////////////////  SpewTui::SpewTui()  ///////////////////////////////
SpewTui::SpewTui(unsigned int iterationsToDo,
                 Units_t units,
                 bool showProgress,
                 Verbosity_t verbosity): 
   SpewDisplay(iterationsToDo, 
               units,
               showProgress,
               verbosity)
{
   mTui = 0;
   mHelpWindow = 0;
   mStatsWindow = 0;
   mProgressWindow = 0;
   mStatusWindow = 0;
}


//////////////////////////  SpewTui::init()  /////////////////////////////////
int SpewTui::init()
{
   mTui = new Tui();
   mTui->init();

   if (this->checkScreenDimensions())
   {
      mTui->close();
      delete mTui;
      mTui = 0;
      fprintf(stderr, "Screen too small.\n");
      return EXIT_ERROR_SCREEN_SIZE;
   }

   this->setWindowsDimensions(mTui->getCurrentScreenRows(),
                              mTui->getCurrentScreenColumns());

   mHelpWindow = new SpewTuiHelpWindow(this,
                                       sHelpWindowHeight,
                                       sHelpWindowWidth,
                                       sHelpWindowStartX,
                                       sHelpWindowStartY);
   mHelpWindow->create();

   mStatsWindow = new SpewTuiStatisticsWindow(this,
                                              sStatisticsWindowHeight,
                                              sStatisticsWindowWidth,
                                              sStatisticsWindowStartX,
                                              sStatisticsWindowStartY,
                                              mCurrentUnits);
   if (mStatsWindow == (SpewTuiStatisticsWindow *)NULL)
   {
      fprintf(stderr, "Cannot create statistics window\n");
      return 1;
   }
   
   mStatsWindow->create();
   mStatsWindow->show();

   mProgressWindow = new SpewTuiProgressWindow(this,
                                               sProgressWindowHeight,
                                               sProgressWindowWidth,
                                               sProgressWindowStartX,
                                               sProgressWindowStartY);
   mProgressWindow->create();
   mProgressWindow->show();

   mStatusWindow = new SpewTuiStatusWindow(this,
                                           sStatusWindowHeight,
                                           sStatusWindowWidth,
                                           sStatusWindowStartX,
                                           sStatusWindowStartY);
   mStatusWindow->create();
   mStatusWindow->show();

   return 0;
}


//////////////////////////  SpewTui::close() //////////////////////////////////
int SpewTui::close()
{
   if (mStatsWindow)
      mStatsWindow->destroy();
   if (mProgressWindow)
      mProgressWindow->destroy();
   if (mStatusWindow)
      mStatusWindow->destroy();
   if (mTui)
      mTui->close();

   return 0;
}


//////////////////////////  SpewTui::resize()  ////////////////////////////////
int SpewTui::resize()
{
#ifdef HAVE_WINSIZE_IOCTL
   mTui->setScreenSize();
   if (this->checkScreenDimensions())
   {
      fprintf(stderr, "Screen to small\n");
   }
      
   this->setWindowsDimensions(mTui->getCurrentScreenRows(),
                              mTui->getCurrentScreenColumns());

   mHelpWindow->resize(sHelpWindowHeight,
                       sHelpWindowWidth,
                       sHelpWindowStartX,
                       sHelpWindowStartY);
   mStatsWindow->resize(sStatisticsWindowHeight,
                        sStatisticsWindowWidth,
                        sStatisticsWindowStartX,
                        sStatisticsWindowStartY);
   mProgressWindow->resize(sProgressWindowHeight,
                           sProgressWindowWidth,
                           sProgressWindowStartX,
                           sProgressWindowStartY);
   mStatusWindow->resize(sStatusWindowHeight, 
                         sStatusWindowWidth,
                         sStatusWindowStartX,
                         sStatusWindowStartY);
#endif

   return 0;
}


//////////////////////////  SpewTui::getCurrentNumHackRows()  /////////////////
unsigned int SpewTui::getCurrentNumHackRows() const
{
   if (mShowProgress)
      return mProgressWindow->getCurrentNumHackRows();
   else
      return 20;  // Update every 5%.
}


////////////////  SpewTui::getCurrentNumVerticalHacks()  //////////////////////
unsigned int SpewTui::getCurrentNumVerticalHacks() const 
{
   return this->getCurrentNumHackRows();
} 


//////////////////////////  SpewTui::getCurrentNumHackColumns()  //////////////
unsigned int SpewTui::getCurrentNumHackColumns() const
{
   if (mShowProgress)
      return mProgressWindow->getCurrentNumHackColumns();
   return 1;
}


////////////////  SpewTui::getCurrentNumHorizontalHacks()  ////////////////////
unsigned int SpewTui::getCurrentNumHorizontalHacks() const 
{
   return this->getCurrentNumHackColumns();
}


//////////////////////////  SpewTui::getCurrentPauseLength()  //////////////
unsigned int SpewTui::getCurrentPauseLength() const
{
   return DEFAULT_PAUSE_LENGTH;
}


//////////////////////////  SpewTui::hack()  //////////////////////////////////
void SpewTui::hack()
{
   if (!mShowProgress)
      return;

   mProgressWindow->hack();
   mProgressWindow->refresh();
   this->checkKeyboard();
}


//////////////////////////  SpewTui::endHack()  ///////////////////////////////
void SpewTui::endHack()
{
   if (!mShowProgress)
      return;

   mProgressWindow->endHack();
   mProgressWindow->refresh();
   this->checkKeyboard();
}


//////////////////////////  SpewTui::errorHack()  /////////////////////////////
void SpewTui::errorHack()
{
   if (!mShowProgress)
      return;

   mProgressWindow->errorHack();
   mProgressWindow->refresh();
   this->checkKeyboard();
}


//////////////////////////  SpewTui::errorEndHack()  //////////////////////////
void SpewTui::errorEndHack()
{
   if (!mShowProgress)
      return;

   mProgressWindow->errorEndHack();
   mProgressWindow->refresh();
   this->checkKeyboard();
}

//////////////////////////  SpewTui::noHack()  ////////////////////////////////
void SpewTui::noHack() 
{
   if (!mShowProgress)
      return;

   mProgressWindow->noHack();
   mProgressWindow->refresh();
   this->checkKeyboard();
}


//////////////////////////  SpewTui::noEndHack()  /////////////////////////////
void SpewTui::noEndHack()
{
   if (!mShowProgress)
      return;

   mProgressWindow->noEndHack();
   mProgressWindow->refresh();
   this->checkKeyboard();
}


//////////////////////////  SpewTui::nextHackRow()  ///////////////////////////
void SpewTui::nextHackRow()
{
   if (!mShowProgress)
      return;

   mProgressWindow->nextHackRow();
   mProgressWindow->refresh();
   this->checkKeyboard();
}


////////////////////  SpewTui::intermediateStatistics()  //////////////////////
void SpewTui::intermediateStatistics(capacity_t hackRowBytesTransferred,
                                     const TimeHack& hackRowTransferTime,
                                     capacity_t jobBytesTransferred,
                                     const TimeHack& jobTransferTime,
                                     capacity_t bytesInJob,
                                     capacity_t totalBytesRead,
                                     const TimeHack& totalReadTransferTime,
                                     capacity_t totalBytesWritten,
                                     const TimeHack& totalWriteTransferTime,
                                     const TimeHack& totalRunTime)
{
   long double percentage = (long double)jobBytesTransferred/(long double)bytesInJob*100.0;

   if (mShowProgress)
   {
      mProgressWindow->currentTransferPercentage(percentage);
      mProgressWindow->currentTransferRate(hackRowBytesTransferred, 
                                           hackRowTransferTime);
      mProgressWindow->refresh();
   }

   mStatsWindow->setJobBytesTransferred(jobBytesTransferred);
   mStatsWindow->setJobTransferTime(jobTransferTime);
   mStatsWindow->setBytesInJob(bytesInJob);
   mStatsWindow->setTotalBytesRead(totalBytesRead);
   mStatsWindow->setTotalReadTransferTime(totalReadTransferTime);
   mStatsWindow->setTotalBytesWritten(totalBytesWritten);
   mStatsWindow->setTotalWriteTransferTime(totalWriteTransferTime);
   mStatsWindow->setTotalRunTime(totalRunTime);
   mStatsWindow->refresh();

   this->checkKeyboard();
}

/////////////////////  SpewTui::cumulativeStatistics() ////////////////////////
void SpewTui::cumulativeStatistics(capacity_t jobBytesTransferred,
                                   const TimeHack& jobTransferTime,
                                   capacity_t totalBytesRead,
                                   const TimeHack& totalReadTransferTime,
                                   capacity_t totalBytesWritten,
                                   const TimeHack& totalWriteTransferTime,
                                   const TimeHack& totalRunTime)
{
   mStatsWindow->setJobBytesTransferred(jobBytesTransferred);
   mStatsWindow->setJobTransferTime(jobTransferTime);
   mStatsWindow->setTotalBytesRead(totalBytesRead);
   mStatsWindow->setTotalReadTransferTime(totalReadTransferTime);
   mStatsWindow->setTotalBytesWritten(totalBytesWritten);
   mStatsWindow->setTotalWriteTransferTime(totalWriteTransferTime);
   mStatsWindow->setTotalRunTime(totalRunTime);
   mStatsWindow->refresh();

   this->checkKeyboard();
}


//////////////////////////  SpewTui::startRun()  //////////////////////////////
void SpewTui::startRun()
{
   mStatusWindow->startRun();
   mStatsWindow->startRun();
   if (mShowProgress)
      mProgressWindow->startRun();
}


//////////////////////////  SpewTui::endRun()  ////////////////////////////////
void SpewTui::endRun()
{
   mStatusWindow->endRun();
   mStatsWindow->endRun();
   if (mShowProgress)
      mProgressWindow->endRun();
}


//////////////////////////  SpewTui::startJob()  //////////////////////////////
void SpewTui::startJob(unsigned int iteration, IoDirection_t direction)
{
   SpewDisplay::startJob(iteration, direction);
   mStatusWindow->startJob();
   mStatsWindow->startJob();
   if (mShowProgress)
      mProgressWindow->startJob();

   this->checkKeyboard();
}


//////////////////////////  SpewTui::endJob()  ////////////////////////////////
void SpewTui::endJob()
{
   mStatusWindow->endJob();
   mStatsWindow->endJob();
   if (mShowProgress)
      mProgressWindow->endJob();

   this->checkKeyboard();
}


//////////////////////////  SpewTui::nextJob()  ///////////////////////////////
void SpewTui::nextJob()
{
   this->pause(this->getCurrentPauseLength());

   mStatusWindow->nextJob();
   mStatsWindow->nextJob();
   if (mShowProgress)
      mProgressWindow->nextJob();

   this->checkKeyboard();
}


//////////////  SpewTui::setWindowsDimensions()  //////////////////////////////
void SpewTui::setWindowsDimensions(int scrRows, int scrColumns)
{
   sHelpWindowHeight = max(scrRows, MIN_HELP_WINDOW_HEIGHT);
   sHelpWindowWidth = max(scrColumns, MIN_HELP_WINDOW_WIDTH);
   sHelpWindowStartY = 0;
   sHelpWindowStartX = 0;

   sStatisticsWindowHeight = MIN_STATISTICS_WINDOW_HEIGHT;
   sStatusWindowHeight = MIN_STATUS_WINDOW_HEIGHT;
   sProgressWindowHeight = max(scrRows - sStatisticsWindowHeight - sStatusWindowHeight, MIN_PROGRESS_WINDOW_HEIGHT);
   
   sStatisticsWindowWidth = max(scrColumns, MIN_STATISTICS_WINDOW_WIDTH);
   sStatisticsWindowStartY = 0;
   sStatisticsWindowStartX = 0;

   sProgressWindowWidth = max(scrColumns, MIN_PROGRESS_WINDOW_WIDTH);
   sProgressWindowStartY = sStatisticsWindowStartY + sStatisticsWindowHeight;
   sProgressWindowStartX = 0;

   sStatusWindowWidth = max(scrColumns, MIN_STATUS_WINDOW_WIDTH);
   sStatusWindowStartY = scrRows - sStatusWindowHeight;
   sStatusWindowStartX = 0;
}


//////////////  SpewTui::checkScreenDimensions()  ////////////////////////////
int SpewTui::checkScreenDimensions()
{
   if (mTui->getCurrentScreenRows() < MIN_SCREEN_HEIGHT ||
       mTui->getCurrentScreenColumns() < MIN_SCREEN_WIDTH)
   {
      return EXIT_ERROR_SCREEN_SIZE;
   }
   else
      return EXIT_OK;
}


//////////////////////////  SpewTui::checkKeyboard()  /////////////////////////
void SpewTui::checkKeyboard()
{
   int ch = this->getKey();
   switch (ch)
   {
   case ERR:
      return;
   case 'q':
      raise(SIGQUIT);
      break;
   case 'p':
      if (mShowProgress)
      {
         mShowProgress = false;
         mProgressWindow->show();
      }
      else
      {
         mShowProgress = true;
         mProgressWindow->clear();
      }
      break;
   case 'k':
      mCurrentUnits = KIBIBYTES;
      break;
   case 'K':
      mCurrentUnits = KILOBYTES;
      break;
   case 'm':
      mCurrentUnits = MEBIBYTES;
      break;
   case 'M':
      mCurrentUnits = MEGABYTES;
      break;
   case 'g':
      mCurrentUnits = GIBIBYTES;
      break;
   case 'G':
      mCurrentUnits = GIGABYTES;
      break;
   case 't':
      mCurrentUnits = TEBIBYTES;
      break;
   case 'T':
      mCurrentUnits = TERABYTES;
      break;
   case ' ':
      this->pause(0);
      break;
   case 'h':
   case '?':
      this->help();
      break;
   default:
      break;
   }
}


//////////////////////////  SpewTui::checkKeyboard()  /////////////////////////
int SpewTui::getKey(bool block)
{
   int ch = ERR;
   ch = mProgressWindow->getKey();
   if (ch == ERR)
   {
      ch = mStatsWindow->getKey();
      if (ch == ERR)
      {
         ch = mStatusWindow->getKey();
      }
   }
   return ch;
}


//////////////////////////  SpewTui::help()  //////////////////////////////////
void SpewTui::help()
{
   // Save the state of the current windows.
   mStatsWindow->hide();
   mProgressWindow->hide();
   mStatusWindow->hide();

   // Switch to help window and pause for a key-stroke.
   mHelpWindow->clear();
   mHelpWindow->show();
   mHelpWindow->getKey(TRUE);
   
   // Switch back.  
   mHelpWindow->hide();
   mStatsWindow->show();
   mProgressWindow->show();
   mStatusWindow->show();
   
}


//////////////////////////  SpewTui::pause()  /////////////////////////////////
void SpewTui::pause(unsigned int timeout)
{
   mStatusWindow->pause(timeout);
   mStatusWindow->helpPrompt();
   mStatusWindow->refresh();
}


//////////////////////////  SpewTui::~SpewTui()  /////////////////////////////
SpewTui::~SpewTui()
{
   delete mHelpWindow;
   delete mProgressWindow;
   delete mStatsWindow;
   delete mStatusWindow;
   delete mTui;
}

