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
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>

#ifdef USE_THREADS
#include <pthread.h>
#endif

#include "common.h"
#include "Log.h"

///////////////////////////////////////////////////////////////////////////////
////////////////////////  Local constants  ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const int TIMESTAMP_STR_LEN = 19;  // Does not includes space for \0.
static const char LOGFILE_SEPARATOR_CHAR = '#';
static const int LOGFILE_WIDTH = 80;

#ifdef USE_THREADS
//////////////////////////  Log::threadEntryPoint()  //////////////////////////
void *Log::doWork(void *pthis)
{
   Log& log = *((Log *)pthis);

   sigset_t sigsToBlock;
   sigfillset(&sigsToBlock);
   pthread_sigmask(SIG_BLOCK, &sigsToBlock, NULL);

   unsigned int queueLen;
   for (;;)
   {
      pthread_mutex_lock(&log.mMsgQueueMutex);
      if (!log.mShutdown)
         pthread_cond_wait(&log.mMsgQueueNotEmpty, &log.mMsgQueueMutex);

      // Flush DisplayStdout
      queueLen = log.mDisplayStdoutQueue.size();
      for (unsigned int i = 0; i < queueLen; i++)
      {
         fprintf(log.mDisplayStdoutFile, 
                 "%s", 
                 log.mDisplayStdoutQueue.front().c_str());
         log.mDisplayStdoutQueue.pop_front();
      }
      if (queueLen > 0)
         fflush(log.mDisplayStdoutFile);

      // Flush DisplayStderr
      queueLen = log.mDisplayStderrQueue.size();
      for (unsigned int i = 0; i < queueLen; i++)
      {
         fprintf(log.mDisplayStderrFile, 
                 "%s", 
                 log.mDisplayStderrQueue.front().c_str());
         log.mDisplayStderrQueue.pop_front();
      }
      if (queueLen > 0)
         fflush(log.mDisplayStderrFile);

      // Flush LogStdout
      queueLen = log.mLogStdoutQueue.size();
      for (unsigned int i = 0; i < queueLen; i++)
      {
         fprintf(log.mLogStdoutFile,
                 "%s", 
                 log.mLogStdoutQueue.front().c_str());
         log.mLogStdoutQueue.pop_front();
      }
      if (queueLen > 0)
         fflush(log.mLogStdoutFile);

      // Flush LogStderr
      queueLen = log.mLogStderrQueue.size();
      for (unsigned int i = 0; i < queueLen; i++)
      {
         fprintf(log.mLogStderrFile, 
                 "%s", 
                 log.mLogStderrQueue.front().c_str());
         log.mLogStderrQueue.pop_front();
      }
      if (queueLen > 0)
         fflush(log.mLogStderrFile);

      pthread_mutex_unlock(&log.mMsgQueueMutex);
      if (log.mShutdown)
         pthread_exit(NULL);
   }
}


//////////////////////////  Log::startThread() ////////////////////////////////
int Log::startThread()
{
   mShutdown = false;
   pthread_mutex_init(&mMsgQueueMutex, NULL);
   pthread_cond_init(&mMsgQueueNotEmpty, NULL);
   mThread = new pthread_t;
   if (!mThread)
      return ENOMEM;
   if (pthread_create(mThread, NULL, Log::doWork, this) != 0)
      return errno;
   return EXIT_OK;
}


//////////////////////////  Log::stopThread()  ////////////////////////////////
int Log::stopThread()
{
   int ret;

   pthread_mutex_lock(&mMsgQueueMutex);
   mShutdown = true;
   pthread_cond_signal(&mMsgQueueNotEmpty);
   pthread_mutex_unlock(&mMsgQueueMutex);
	ret = pthread_join(*mThread, (void **)NULL);
   if (!ret)
       return ret;
   pthread_mutex_destroy(&mMsgQueueMutex);
   pthread_cond_destroy(&mMsgQueueNotEmpty);
   if (mThread)
   {
      delete mThread;
      mThread = 0;
   }
   return EXIT_OK;
}
#endif  // USE_THREADS


//////////////////////////  Log::Log()  ///////////////////////////////////////
Log::Log()
{
   mDisplayStdoutFile = stdout;
   mDisplayStderrFile = stderr;
   mLogStdoutFile = (FILE *)NULL;
   mLogStderrFile = (FILE *)NULL;

#ifdef USE_THREADS
   mThread = (pthread_t *)NULL;
   mShutdown = false;
#endif

}


//////////////////////////  Log::Log()  ///////////////////////////////////////
Log::Log(const string &logfilePath): mLogfilePath(logfilePath)
{
   mDisplayStdoutFile = stdout;
   mDisplayStderrFile = stderr;
   mLogStdoutFile = (FILE *)NULL;
   mLogStderrFile = (FILE *)NULL;

#ifdef USE_THREADS
   mThread = 0;
   mShutdown = false;
#endif
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

#ifdef USE_THREADS
	int rtn = this->startThread();
   if (rtn)
      return rtn;
#endif
   return EXIT_OK;
}


//////////////////////////  Log::close()  /////////////////////////////////////
int Log::close()
{
   int rtn = EXIT_OK;
#ifdef USE_THREADS
   rtn = this->stopThread();
#endif
   if (mLogStdoutFile)
      fclose(mLogStdoutFile);
   if (mLogStderrFile && (mLogStderrFile != mLogStdoutFile))
      fclose(mLogStderrFile);

   return rtn;
}


//////////////////////////  Log::note()  //////////////////////////////////////
void Log::note(unsigned int outputDevice, const char *fmt, ...) const
{
   va_list ap;
   int len;

	// Find out how long it is.
   va_start(ap, fmt);
   len = vsnprintf((char *)NULL, 0, fmt, ap);
   va_end(ap);

	char *s = new char[len + 1];
	if (!s)
      return;

	// Fill the string.
   va_start(ap, fmt);
   vsnprintf(s, len + 1, fmt, ap);
   va_end(ap);

   this->submitWork(outputDevice, s);
   delete s;
}


//////////////////////////  Log::error()  /////////////////////////////////////
void Log::error(unsigned int outputDevice, const char *fmt, ...) const
{
   va_list ap;
   int len;
   string str;

	// Find out how long it is.
   va_start(ap, fmt);
   len = vsnprintf((char *)NULL, 0, fmt, ap);
   va_end(ap);

	char *s = new char[len + 1];
	if (!s)
      return;

	// Fill the string.
   va_start(ap, fmt);
   vsnprintf(s, len + 1, fmt, ap);
   va_end(ap);

   str = "error: ";
   str += s;

   this->submitWork(outputDevice, str);
   delete s;
}


//////////////////////////  Log::showNote()  //////////////////////////////////
void Log::showNote(const char *fmt, ...) const
{
   va_list ap;
   int len;

	// Find out how long it is.
   va_start(ap, fmt);
   len = vsnprintf((char *)NULL, 0, fmt, ap);
   va_end(ap);

	char *s = new char[len + 1];
	if (!s)
      return;

	// Fill the string.
   va_start(ap, fmt);
   vsnprintf(s, len + 1, fmt, ap);
   va_end(ap);

   this->submitWork(OUTPUT_DISPLAY_STDOUT, s);
   delete s;
}


//////////////////////////  Log::showError()  /////////////////////////////////
void Log::showError(const char *fmt, ...) const
{
   va_list ap;
   int len;
   string str;

   // Find out how long it is.
   va_start(ap, fmt);
   len = vsnprintf((char *)NULL, 0, fmt, ap);
   va_end(ap);
   
	char *s =  new char[len + 1];
	if (!s)
      return;

	// Fill the string.	
   va_start(ap, fmt);
   vsnprintf(s, len + 1, fmt, ap);
   va_end(ap);

   str = "error: ";
   str += s;

   this->submitWork(OUTPUT_DISPLAY_STDERR, str);
   delete s;
}


//////////////////////////  Log::logTimestamp()  /////////////////////////////
void Log::logTimestamp(const char *fmt, ...) const
{
   if (!mLogStdoutFile)
      return;

   va_list ap;
   int len;

   // Find out how long it is.
   va_start(ap, fmt);
   len = vsnprintf((char *)NULL, 0, fmt, ap);
   va_end(ap);

	char *s = new char[len + 1];
	if (!s)
      return;

	// Fill the string.
   va_start(ap, fmt);
   vsnprintf(s, len + 1, fmt, ap);
   va_end(ap);

   // Put timestamp followed by message in the log.
   string str = this->timestamp() + s;

   this->submitWork(OUTPUT_LOG_STDOUT, str);
   delete s;
}


//////////////////////////  Log::logNote()  //////////////////////////////////
void Log::logNote(const char *fmt, ...) const
{

   if (!mLogStdoutFile)
      return;

   va_list ap;
   int len;

   // Find out how long it is.
   va_start(ap, fmt);
   len = vsnprintf((char *)NULL, 0, fmt, ap);
   va_end(ap);

	char *s = new char[len + 1];
	if (!s)
      return;

	// Fill the string.
   va_start(ap, fmt);
   vsnprintf(s, len + 1, fmt, ap);
   va_end(ap);

   this->submitWork(OUTPUT_LOG_STDOUT, s);
   delete s;
}


//////////////////////////  Log::logError()  /////////////////////////////////
void Log::logError(const char *fmt, ...) const
{
   if (!mLogStderrFile)
      return;

   va_list ap;
   int len;
   string str;


   // Find out how long it is.
   va_start(ap, fmt);
   len = vsnprintf((char *)NULL, 0, fmt, ap);
   va_end(ap);

	char *s = new char[len + 1];
	if (!s)
      return;

	// Fill the string.
   va_start(ap, fmt);
   vsnprintf(s, len + 1, fmt, ap);
   va_end(ap);

   this->submitWork(OUTPUT_LOG_STDERR, s);
   delete s;
}


//////////////////////////  Log::logStart()  /////////////////////////////////
void Log::logStart() const
{
   if (!mLogStdoutFile)
      return;

   string str;

   str = string(LOGFILE_WIDTH, LOGFILE_SEPARATOR_CHAR);
   str += "\n";
   str += this->logSeparatorNote(this->timestamp().c_str(), "START");
   str += "\n";

   this->submitWork(OUTPUT_LOG_STDOUT, str);
}


//////////////////////////  Log::logFinish()  ///////////////////////////////
void Log::logFinish() const
{
   if (!mLogStderrFile)
      return;

   string str;

   str = "\n";
   str += this->logSeparatorNote(this->timestamp().c_str(), "FINISH");
   str += string(LOGFILE_WIDTH, LOGFILE_SEPARATOR_CHAR);
   str += "\n\n";

   this->submitWork(OUTPUT_LOG_STDOUT, str);
}


//////////////////////////  Log::logCmdLine()  ///////////////////////////////
void Log::logCmdLine(const char *args) const
{
   if (!mLogStderrFile)
      return;
   const char *leader = "Command-line: ";
   const char *indent = "              ";
   const char *follower = "\\";

   string str = args;
   this->justify(str, leader, indent, follower);

   this->submitWork(OUTPUT_LOG_STDOUT, str);
}


//////////////////////////  Log::logSeparator()  //////////////////////////////
string Log::logSeparatorNote(const char *timestamp, const char *note) const
{
   int noteLen = strlen(timestamp) + strlen(note) + 6;  
   int leaderLen = (LOGFILE_WIDTH - noteLen)/2;
   int followerLen = leaderLen;
   if (((LOGFILE_WIDTH - noteLen) % 2) == 1)
      followerLen += 1;

   string str;
   strPrintf(str, "%s  %s  %s  %s\n", 
             string(leaderLen, LOGFILE_SEPARATOR_CHAR).c_str(),
             timestamp, 
             note,
             string(followerLen, LOGFILE_SEPARATOR_CHAR).c_str());

   return str;
}


//////////////////////////  Log::timestamp()  ////////////////////////////////
string Log::timestamp() const
{
   time_t currentTime;
   struct tm currentBrokenDownTime;
   
   time(&currentTime);
   localTime(&currentTime, &currentBrokenDownTime);

	char timestamp[TIMESTAMP_STR_LEN + 1];
   strftime(timestamp, 
            sizeof(timestamp),
            "%Y/%m/%d %T", 
            &currentBrokenDownTime);

   return string(timestamp);
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


//////////////////////////  Log::submitWork()  ////////////////////////////////
void Log::submitWork(unsigned int outputDevice, const string& msg) const
{
   if (!outputDevice || msg.empty())
      return;

#ifdef USE_THREADS
	if (mShutdown)
		return;
   pthread_mutex_lock(&mMsgQueueMutex);
   if (outputDevice & OUTPUT_DISPLAY_STDOUT)
      mDisplayStdoutQueue.push_back(string(msg));
   if (outputDevice & OUTPUT_DISPLAY_STDERR)
      mDisplayStderrQueue.push_back(string(msg));
   if ((outputDevice & OUTPUT_LOG_STDOUT) && mLogStdoutFile)
      mLogStdoutQueue.push_back(string(msg));
   if ((outputDevice & OUTPUT_LOG_STDERR) && mLogStderrFile)
      mLogStderrQueue.push_back(string(msg));

   pthread_cond_signal(&mMsgQueueNotEmpty);
   pthread_mutex_unlock(&mMsgQueueMutex);
#else
   if (outputDevice & OUTPUT_DISPLAY_STDOUT)
   {
      fprintf(mDisplayStdoutFile, msg);
      fflush(mDisplayStdoutFile);
   }
   if (outputDevice & OUTPUT_DISPLAY_STDERR)
   {
      fflush(mDisplayStderrFile);
      fprintf(mDisplayStderrFile, msg);
   }
   if ((outputDevice & OUTPUT_LOG_STDOUT) && mLogStdoutFile)
   {
      fprintf(mLogStdoutFile, msg);
      fflush(mLogStdoutFile);
   }
   if ((outputDevice & OUTPUT_LOG_STDERR) && mLogStderrFile)
   {
      fprintf(mLogStderrFile, msg);
      fflush(mLogStderrFile);
   }
#endif  // USE_THREADS
}


//////////////////////////  Log::~Log()  //////////////////////////////////////
Log::~Log()
{
#ifdef USE_THREADS
      delete mThread;
#endif
}
