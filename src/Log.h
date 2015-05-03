//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// Log.h
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

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string>

class Log
{
public:
   // Output devices.  Or these together to send note() and error() output
   // to various log devices.
   static const unsigned int OUTPUT_NONE           = 0x0;
   static const unsigned int OUTPUT_DISPLAY_STDOUT = 0x1;
   static const unsigned int OUTPUT_DISPLAY_STDERR = 0x2;
   static const unsigned int OUTPUT_LOG_STDOUT     = 0x4;
   static const unsigned int OUTPUT_LOG_STDERR     = 0x8;

public:
   Log();  
   Log(const string &logfilePath);

   const string &getLogfilePath() { return mLogfilePath; };

   int open();
   int close();
   void note(unsigned int outputDevices, const char *fmt, ...) const;
   void error(unsigned int outputDevices, const char *fmt, ...) const;
   void showNote(const char *fmt, ...) const;
   void showError(const char *fmt, ...) const;
   void logTimestamp(const char *fmt, ...) const;
   void logNote(const char *fmt, ...) const;
   void logError(const char *fmt, ...) const;
   void logStart() const;
   void logFinish() const;
   void logCmdLine(int argc, char **argv) const;
   
   ~Log();

private:
   Log(const Log&); // Hide copy constructor.

   char *timestamp(char *timestamp) const;

protected:
   string mLogfilePath;
   FILE *mDisplayStdoutFile;
   FILE *mDisplayStderrFile;
   FILE *mLogStdoutFile;
   FILE *mLogStderrFile;
};

#endif // LOG_H
