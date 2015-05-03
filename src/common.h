//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// common.h
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

#ifndef COMMON_H
#define COMMON_H

#include <time.h>
#include <sys/time.h>
#include <string>
#include <stdlib.h>


//////////////////////////////////////////////////////////////////////////////
///////////////////////////  Macros  /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#define ROUND_UP_OFFSET(X, M) ((M) - 1 - (((X) + (M) - 1) % (M)))
#define PTR_ALIGN(Ptr, M) ((Ptr) \
                           + ROUND_UP_OFFSET ((char *)(Ptr) - (char *)0, (M)))

#ifndef max
#define max(X,Y) ((X) >= (Y) ? (X) : (Y))
#endif

#define STRINGIFY(s) #s
#define QUOTE(s) STRINGIFY(s)

//////////////////////////////////////////////////////////////////////////////
///////////////////////////  Typedefs  ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

typedef enum 
{
   VERBOSITY_NONE, 
   VERBOSITY_SHORT, 
   VERBOSITY_LONG,
} Verbosity_t;

typedef enum
{
   EXIT_OK = 0,
   EXIT_ERROR_USAGE,
   EXIT_ERROR_INVALID_USAGE,
   EXIT_ERROR_PERM,
   EXIT_ERROR_SYSTEM,
   EXIT_ERROR_ILLEGAL_OPERATION,
   EXIT_ERROR_CAUGHT_EXCEPTION,
   EXIT_ERROR_DATA_INTEGRITY,
   EXIT_ERROR_MEMORY_ALLOC,
   EXIT_ERROR_UNDERRUN,
   EXIT_ERROR_IO,
   EXIT_ERROR_SCREEN_SIZE,
} ExitCode_t;

typedef enum 
{
   KIBIBYTES,
   KILOBYTES,
   MEBIBYTES,
   MEGABYTES,
   GIBIBYTES,
   GIGABYTES,
   TEBIBYTES,
   TERABYTES,
} Units_t;

typedef enum
{
   READING,
   WRITING,
} IoDirection_t;

typedef UINT32_TYPE u32_t;
typedef UINT64_TYPE u64_t;
typedef unsigned long long capacity_t;


//////////////////////////////////////////////////////////////////////////////
///////////////////////////  Constants  //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifdef __CYGWIN__
static const unsigned int DEFAULT_SCREEN_ROWS = 25;
static const unsigned int DEFAULT_SCREEN_COLUMNS = 79;
#else
static const unsigned int DEFAULT_SCREEN_ROWS = 24;
static const unsigned int DEFAULT_SCREEN_COLUMNS = 80;
#endif


//////////////////////////////////////////////////////////////////////////////
///////////////////////////  Functions  //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
const char *basename(char *path);
string strPrintf(const char *fmt, ...);
string strSignal(int sig);
string strError(int error_number);
bool is_mounted(const char *path);
long double convertCapacity(long double bytes, Units_t units);
const char *getTransferRateUnitsStr(Units_t units);
void localTime(const time_t *timep, struct tm *result);

#endif // COMMON_H
