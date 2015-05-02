//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// Log.cpp
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
#include <errno.h>

#include "common.h"
#include "Log.h"

///////////////////////////////////////////////////////////////////////////////
////////////////////////  Local constants  ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TIMESTAMP_STR_LEN = 19;  // Does not includes space for \0.



//////////////////////////  Log::Log()  ///////////////////////////////////////
Log::Log()
{
   mDisplayStdoutFile = stdout;
   mDisplayStderrFile = stderr;
   mLogStdoutFile = (FILE *)NULL;
   mLogStderrFile = (FILE *)NULL;
}


//////////////////////////  Log::Log()  ///////////////////////////////////////
Log::Log(const string &logfilePath): mLogfilePath(logfilePath)
{
   mDisplayStdoutFile = stdout;
   mDisplayStderrFile = stderr;
   mLogStdoutFile = (FILE *)NULL;
   mLogStderrFile = (FILE *)NULL;
}


//////////////////////////  Log::open()  //////////////////////////////////////
int Log::open()
{
   FILE *logStdoutFile;
   if (mLogfilePath.size() > 0)
   {
      logStdoutFile = fopen(mLogfilePath.c_str(), "a+");
      if (logStdoutFile == (FILE *)NULL)
         return errno;
      mLogStdoutFile = logStdoutFile;
      mLogStderrFile = logStdoutFile;
   }
   return EXIT_OK;
}


//////////////////////////  Log::close()  /////////////////////////////////////
int Log::close()
{
   if (mLogStdoutFile)
   {
      fclose(mLogStdoutFile);
   }
   return EXIT_OK;
}


//////////////////////////  Log::note()  //////////////////////////////////////
void Log::note(unsigned int outputDevice, const char *fmt, ...) const
{
   va_list ap;
   va_start(ap, fmt);

   if (outputDevice & OUTPUT_DISPLAY_STDOUT)
      vfprintf(mDisplayStdoutFile, fmt, ap);
   if (outputDevice & OUTPUT_DISPLAY_STDERR)
      vfprintf(mDisplayStderrFile, fmt, ap);
   if ((outputDevice & OUTPUT_LOG_STDOUT) && mLogStdoutFile)
      vfprintf(mLogStdoutFile, fmt, ap);
   if ((outputDevice & OUTPUT_LOG_STDERR) && mLogStderrFile)
      vfprintf(mLogStderrFile, fmt, ap);

   va_end(ap);

   if (outputDevice & OUTPUT_DISPLAY_STDOUT)
      fflush(mDisplayStdoutFile);
   if (outputDevice & OUTPUT_DISPLAY_STDERR)
      fflush(mDisplayStderrFile);
   if ((outputDevice & OUTPUT_LOG_STDOUT) && mLogStdoutFile)
      fflush(mLogStdoutFile);
   if ((outputDevice & OUTPUT_LOG_STDERR) && mLogStderrFile)
      fflush(mLogStderrFile);

}


//////////////////////////  Log::error()  /////////////////////////////////////
void Log::error(unsigned int outputDevice, const char *fmt, ...) const
{
   if (outputDevice & OUTPUT_DISPLAY_STDOUT)
      fprintf(mDisplayStdoutFile, "error: ");
   if (outputDevice & OUTPUT_DISPLAY_STDERR)
      fprintf(mDisplayStderrFile, "error: ");
   if ((outputDevice & OUTPUT_LOG_STDOUT) && mLogStdoutFile)
      fprintf(mLogStdoutFile, "error: ");
   if ((outputDevice & OUTPUT_LOG_STDERR) && mLogStderrFile)
      fprintf(mLogStderrFile, "error: ");

   va_list ap;
   va_start(ap, fmt);

   if (outputDevice & OUTPUT_DISPLAY_STDOUT)
      vfprintf(mDisplayStdoutFile, fmt, ap);
   if (outputDevice & OUTPUT_DISPLAY_STDERR)
      vfprintf(mDisplayStderrFile, fmt, ap);
   if ((outputDevice & OUTPUT_LOG_STDOUT) && mLogStdoutFile)
      vfprintf(mLogStdoutFile, fmt, ap);
   if ((outputDevice & OUTPUT_LOG_STDERR) && mLogStderrFile)
      vfprintf(mLogStderrFile, fmt, ap);

   va_end(ap);

   if (outputDevice & OUTPUT_DISPLAY_STDOUT)
      fflush(mDisplayStdoutFile);
   if (outputDevice & OUTPUT_DISPLAY_STDERR)
      fflush(mDisplayStderrFile);
   if ((outputDevice & OUTPUT_LOG_STDOUT) && mLogStdoutFile)
      fflush(mLogStdoutFile);
   if ((outputDevice & OUTPUT_LOG_STDERR) && mLogStderrFile)
      fflush(mLogStderrFile);
}


//////////////////////////  Log::showNote()  //////////////////////////////////
void Log::showNote(const char *fmt, ...) const
{
   va_list ap;
   va_start(ap, fmt);
   vfprintf(mDisplayStdoutFile, fmt, ap);
   va_end(ap);
   fflush(stdout);
}


//////////////////////////  Log::showError()  /////////////////////////////////
void Log::showError(const char *fmt, ...) const
{
   fprintf(stderr, "error: ");
   va_list ap;
   va_start(ap, fmt);
   vfprintf(mDisplayStderrFile, fmt, ap);
   va_end(ap);
   fflush(stderr);
}


//////////////////////////  Log::logTimestamp()  /////////////////////////////
void Log::logTimestamp(const char *fmt, ...) const
{
   if (!mLogStdoutFile)
      return;

   // Put timestamp followed by message in the log.

   char timestamp[TIMESTAMP_STR_LEN + 1];
   
   fprintf(mLogStdoutFile, "%s ", this->timestamp(timestamp));
   va_list ap;
   va_start(ap, fmt);
   vfprintf(mLogStdoutFile, fmt, ap);
   va_end(ap);
   fflush(mLogStdoutFile);
}


//////////////////////////  Log::logNote()  //////////////////////////////////
void Log::logNote(const char *fmt, ...) const
{

   if (!mLogStdoutFile)
      return;

   va_list ap;
   va_start(ap, fmt);
   vfprintf(mLogStdoutFile, fmt, ap);
   va_end(ap);
   fflush(mLogStdoutFile);
}


//////////////////////////  Log::logError()  /////////////////////////////////
void Log::logError(const char *fmt, ...) const
{
   if (!mLogStderrFile)
      return;

   va_list ap;
   va_start(ap, fmt);
   vfprintf(mLogStderrFile, fmt, ap);
   va_end(ap);
   fflush(mLogStderrFile);
}


//////////////////////////  Log::logStart()  /////////////////////////////////
void Log::logStart() const
{
   if (!mLogStdoutFile)
      return;

   char timestamp[TIMESTAMP_STR_LEN + 1];
   fprintf(mLogStdoutFile, "###############################################################################\n");
   fprintf(mLogStdoutFile, "#######################  %s  START  ##########################\n", this->timestamp(timestamp));
   fprintf(mLogStdoutFile, "\n");

}


//////////////////////////  Log::logFinish()  ///////////////////////////////
void Log::logFinish() const
{
   if (!mLogStderrFile)
      return;

   char timestamp[TIMESTAMP_STR_LEN + 1];
   fprintf(mLogStdoutFile, "\n");
   fprintf(mLogStdoutFile, "#######################  %s  FINISH  #########################\n", this->timestamp(timestamp));
   fprintf(mLogStdoutFile, "###############################################################################\n");
   fprintf(mLogStdoutFile, "\n");

}


//////////////////////////  Log::logCmdLine()  ///////////////////////////////
void Log::logCmdLine(int argc, char **argv) const
{
   if (!mLogStderrFile)
      return;

   fprintf(mLogStdoutFile, "Command-line: ");
   if (argc > 0)
      fprintf(mLogStdoutFile, "%s ", basename(argv[0]));
   for (int i = 1; i < argc; i++)
      fprintf(mLogStdoutFile, "%s ", argv[i]);
   fprintf(mLogStdoutFile, "\n");
}


//////////////////////////  Log::timestamp()  ////////////////////////////////
char *Log::timestamp(char *timestamp) const
{
   time_t currentTime;
   struct tm currentBrokenDownTime;
   
   time(&currentTime);
   localTime(&currentTime, &currentBrokenDownTime);
   strftime(timestamp, 
            TIMESTAMP_STR_LEN + 1,
            "%Y/%m/%d %T", 
            &currentBrokenDownTime);

   return timestamp;
}


//////////////////////////  Log::~Log()  //////////////////////////////////////
Log::~Log()
{
}
