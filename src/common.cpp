//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// common.cpp
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
#include <unistd.h>
#include <math.h>
#include <mntent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#include "common.h"

static const char *TRANSFER_RATE_UNITS_STR_LOOKUP[] = 
{
   "KiB/s",
   "KB/s",
   "MiB/s",
   "MB/s",
   "GiB/s",
   "GB/s",
   "TiB/s",
   "TB/s",
   (char *)NULL,
};


static capacity_t TRANSFER_RATE_UNITS_CONV_LOOKUP[] = 
{
   1024ULL,
   1000ULL,
   1024ULL * 1024ULL,
   1000ULL * 1000ULL,
   1024ULL * 1024ULL * 1024ULL,
   1000ULL * 1000ULL * 1000ULL,
   1024ULL * 1024ULL * 1024ULL * 1024ULL,
   1000ULL * 1000ULL * 1000ULL * 1000ULL,
};


static const size_t MAX_SPRINTF_CHARS = 2048;
static const size_t MAX_STR_CHARS = 1024;


/////////////////////////  basename()  ///////////////////////////////////////
const char *basename(char *path)
{
   char *name = strrchr(path, '/');
   if (name == NULL)
      return path;
   else
      return name + 1;
}


#ifdef HAVE_MOUNT_CHECK
/////////////////////////  is_mounted()  /////////////////////////////////////
bool is_mounted(const char *path)
{
   struct stat statbuf;
   dev_t path_rdev;

   if (stat(path, &statbuf) < 0)
      return false;

   if (!S_ISBLK(statbuf.st_mode))
      return false;
   path_rdev = statbuf.st_rdev;

   FILE *mtabfile;
   struct mntent *ent;

   mtabfile = setmntent(MNTTAB_PATH, "r");
   while ((ent = getmntent(mtabfile)) != (struct mntent *)NULL)
   {
      if (stat(ent->mnt_fsname, &statbuf) < 0)
         continue;
      if (path_rdev == statbuf.st_rdev)
      {
         return true;
      }
   }
   endmntent(mtabfile);
   
   return false;
}
#endif // HAVE_MOUNT_CHECK


///////////////////////  strPrintf()  /////////////////////////////////////////
string strPrintf(const char *fmt, ...)
{
   char *tmpStr = new char[MAX_SPRINTF_CHARS + 1];

   va_list ap;
   va_start(ap, fmt);
   vsnprintf(tmpStr, MAX_SPRINTF_CHARS, fmt, ap);
   va_end(ap);

   string returnString = tmpStr;
   delete[] tmpStr;
   return returnString;
}


///////////////////////  strSignal()  /////////////////////////////////////////
string strSignal(int sig)
{
#ifdef HAVE_STRSIGNAL
   return string(strsignal(sig));
#else
   return strPrintf("signal %d", sig); 
#endif
}


///////////////////////  strError()  //////////////////////////////////////////
string strError(int error_number)
{
#ifdef HAVE_STRERROR
   return string(strerror(error_number));
#else
   return strPrintf("errno %d", error_number);  
#endif
}


//////////////////////////  ConvertUnits()  ////////////////////////////////
long double convertCapacity(long double bytes, Units_t units)
{
   return bytes/(long double)TRANSFER_RATE_UNITS_CONV_LOOKUP[units];
}


////////////////  getTransferRateUnitStr()  ///////////////////////////////////
const char *getTransferRateUnitsStr(Units_t units)
{
   return TRANSFER_RATE_UNITS_STR_LOOKUP[units];
}


////////////////////////  localTime()  ////////////////////////////////////////
//
// Encapsulate thread-safe/non-thread safe versions of localtime.  Note that
// this function is not thread-safe unless localtime_r is actually available 
// (which means using pthreads on some platforms, e.g., HP-UX).
//
void localTime(const time_t *timep, struct tm *result)
{
   time_t currentTime;
   struct tm currentBrokenDownTime;
   struct tm *currentBrokenDownTimePtr;
   
   time(&currentTime);
#ifdef HAVE_LOCALTIME_R
   currentBrokenDownTimePtr = localtime_r(&currentTime, 
                                          &currentBrokenDownTime);
#else
   currentBrokenDownTimePtr = localtime_r(&currentTime);
#endif
   memcpy(result, currentBrokenDownTimePtr, sizeof(struct tm));
}
