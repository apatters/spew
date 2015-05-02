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

namespace std {} using namespace std;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <cstring>

#include "common.h"
#include "Log.h"

///////////////////////////////////////////////////////////////////////////////
////////////////////////  Local constants  ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const int TIMESTAMP_STR_LEN = 19;  // Does not includes space for \0.
const char LOGFILE_SEPARATOR_CHAR = '#';
const int LOGFILE_WIDTH = 80;


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
   this->timestamp(timestamp);

   fprintf(mLogStdoutFile, "%s\n",
           string(LOGFILE_WIDTH, LOGFILE_SEPARATOR_CHAR).c_str());
   this->logSeparatorNote(timestamp, "START");
   fprintf(mLogStdoutFile, "\n");
}


//////////////////////////  Log::logFinish()  ///////////////////////////////
void Log::logFinish() const
{
   if (!mLogStderrFile)
      return;

   char timestamp[TIMESTAMP_STR_LEN + 1];
   this->timestamp(timestamp);

   fprintf(mLogStdoutFile, "\n");
   this->logSeparatorNote(timestamp, "FINISH");
   fprintf(mLogStdoutFile, "%s\n",
           string(LOGFILE_WIDTH, LOGFILE_SEPARATOR_CHAR).c_str());
   fprintf(mLogStdoutFile, "\n");
}


//////////////////////////  Log::logCmdLine()  ///////////////////////////////
void Log::logCmdLine(const char *args) const
{
   if (!mLogStderrFile)
      return;
   const char *leader = "Command-line: ";
   const char *indent = "              ";
   const char *follower = "\\";

   string msg = args;
   this->justify(msg, leader, indent, follower);

   fprintf(mLogStdoutFile, "%s\n", msg.c_str());
}


//////////////////////////  Log::logSeparator()  //////////////////////////////
void Log::logSeparatorNote(const char *timestamp, const char *note) const
{
   if (!mLogStdoutFile)
      return;

   int noteLen = strlen(timestamp) + strlen(note) + 6;  
   int leaderLen = (LOGFILE_WIDTH - noteLen)/2;
   int followerLen = leaderLen;
   if (((LOGFILE_WIDTH - noteLen) % 2) == 1)
      followerLen += 1;

   fprintf(mLogStdoutFile, "%s  %s  %s  %s\n", 
           string(leaderLen, LOGFILE_SEPARATOR_CHAR).c_str(),
           timestamp, 
           note,
           string(followerLen, LOGFILE_SEPARATOR_CHAR).c_str());
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


//////////////////////////  Log::justify()  ///////////////////////////////////
string& Log::justify(string& str, 
                     const string& leader, 
                     const string& hangingIndent,
                     const string& follower) const
{
   string origStr = str;
   string::size_type leaderLen = leader.length();
   string::size_type indentLen = hangingIndent.length();
   string::size_type followerLen = follower.length();
   string::size_type origLen = origStr.length();
   string::size_type curOrigPos = 0;
   string::size_type spacePos = string::npos;
   string::size_type curLinePos = 0;

   str = leader;
   curOrigPos = 0;
   curLinePos = leaderLen;
   while (curOrigPos < origLen)
   {
      spacePos = origStr.find(' ', curOrigPos);
      if (spacePos != string::npos)
      {
         // Skip over successive or leading spaces.
         if (curOrigPos == spacePos) 
         {
            curOrigPos++;
            continue;
         }
         string word = origStr.substr(curOrigPos, spacePos - curOrigPos);
         string::size_type wordLen = word.length();
         if (wordLen + followerLen + curLinePos > LOGFILE_WIDTH)
         {
            str += follower;
            str += "\n";
            str += hangingIndent;
            curLinePos = indentLen + wordLen + 1;
         }
         else
            curLinePos += wordLen + 1;
         str += word;
         str += " "; 
      curOrigPos = spacePos + 1;
      }
      else
      {
         str += origStr.substr(curOrigPos, origLen - curOrigPos);
         str += "\n";
         break;
      }
   }

   return str;
}


//////////////////////////  Log::~Log()  //////////////////////////////////////
Log::~Log()
{
}
