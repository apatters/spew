//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// spewer.cpp
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

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <errno.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <iterator>
#include <string>
#include <popt.h>

#include "common.h"
#include "Log.h"
#include "Job.h"
#include "Tui.h"
#include "SpewerTui.h"


//////////////////////////////////////////////////////////////////////////////
///////////////////////////  Typedefs  ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
enum output_level_enum {NONE, SHORT, LONG};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////  Constants  ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
static const char PROG_VERSION[] = VERSION;
static const int DEFAULT_NUM_JOBS = 1;
static const Job::pattern_t DEFAULT_PATTERN = Job::PATTERN_ZEROS;
static const char *PATTERN_LOOKUP[] = 
{
   "zeros",
   "number",
   "random",
   (char *)NULL,
};


///////////////////////////////////////////////////////////////////////////////
//////////////////////////  Global Variables  /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
char *gPrgName = NULL;
int gGetVersion = 0;


//////////////////////////////////////////////////////////////////////////////
////////////////////  Function Prototypes  ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void error_msg(char *fmt, ...);
void note(char *fmt, ...);
void usage();
bool parse_options(int argc, const char **argv);
bool validate_options();


//////////////////////////////////////////////////////////////////////////////
//////////////////////  Function Definitions  ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


////////////////////////  error_msg()  ///////////////////////////////////////
void error_msg(char *fmt, ...)
{
   fprintf(stderr, "%s: ", gPrgName);
   va_list ap;
   va_start(ap, fmt);
   vfprintf(stderr, fmt, ap);
   va_end(ap);
   fflush(stderr);
}


/////////////////////////  note()  ///////////////////////////////////////////
void note(char *fmt, ...)
{
   va_list ap;
   va_start(ap, fmt);
   vfprintf(stdout, fmt, ap);
   va_end(ap);
   fflush(stdout);
}

void usage()
{
   char fmtStr[] =
"Usage: %s [OPTIONS] [-j JOBS] [-b BUFFER_SIZE[kKmMgG] [-p PATTERN] TRANFER_SIZE[kKmMgG] DIR...\n"
"       %s -V|--help\n"
"Multiple serial-write data transfer speed test.\n"
"\n"
"   -?, --help                      Print this message.\n"
"   -q, --quiet                     Don't output statistics.\n"
"   -v, --verbose                   Output detailed statistics.\n"
"   -V, --version                   Output version info and exit.\n"
"   -b, --buffer-size=BUFFER_SIZE   Each write(2) call uses a buffer of size\n"
"                                   BUFFER_SIZE.\n"
"   -p, --pattern=PATTERN           Use data pattern PATTERN when\n"
"                                   transferring data.\n"
"   -q, --quiet                     Don't output statistics.\n"
"   -v, --verbose                   Output detailed statistics.\n"
"   -s, --sync                      Use synchronous I/O.\n"
"   -d, --direct                    Use direct I/O. Should only be used on\n"
"                                   block device files..\n"
"   -i, --iterations=COUNT          Write data COUNT times. If count is 0, \n"
"                                   repeats forever.\n"
"   -r, --random                    Write blocks to random offsets.\n" 
"   -R, --read-after-write          Read back data after writing.\n"    
"   -c, --continue-after-error      Continue after data integrity errors.\n"   
"   -P, --progress                  Show progess.\n"
"   -S, --seed=SEED                 Random number seed.\n"
"   -g, --generate-load             Equivalent to: -P -p random -i 0 -R.\n"
"   -j, --jobs=JOBS                 Number of jobs per directory.\n"
"   JOBS                            Number of read/write procesess per\n"
"                                   directory (default %d).\n"
"   BUFFER_SIZE                     Buffer size used in each write(2) call\n"
"                                   (default is %llu bytes). BUFFER_SIZE\n"
"                                   must be an even multiple of %llu bytes\n"
"                                   and can be specified in bytes,\n"
"                                   kilobytes, megabytes, or gigabytes.\n"
"   PATTERN                         Data pattern used when writing data.\n"
"                                   Available patterns are:\n"
"                                      %s - Fill buffer with 0's.\n"
"                                      %s - Fill buffer with 1's.\n"
"                                      %s - Fill buffer with alternating 0's and 1's.\n"
"                                      %s - Fill buffer with random data.\n"
"                                      %s - Fill buffer with the transfer\n"
"                                      %s - Fill buffer with the seed pattern\n"
"                                               number.\n"
"                                   The default pattern is %s.\n"
"   SEED                            Used to seed the random number generator or\n"
"                                   as the byte pattern for the user defined fill\n"
"                                   depending on the pattern type selected.\n"
"                                   Must be >= 1 and <= 2^32 for random pattern.\n"
"                                   Must be >= 0 and <= 255 for user pattern.\n"
"   TRANSFER_SIZE                   Total number of bytes to transfer per\n" 
"                                   job (must be an even multiple of \n"
"                                   BUFFER_SIZE). TRANSER_SIZE can be\n"
"                                   specified in bytes, kilobytes,\n"
"                                   megabytes, or gigabytes.\n"
"   DIR                             Directories to write data to.\n"
   "";

      char outStr[3096];
      snprintf(outStr, sizeof(outStr) - 1, fmtStr,
               gPrgName, gPrgName,
               DEFAULT_NUM_JOBS,
               Transfer::BUFFER_SIZE_INCREMENT,
               Transfer::BUFFER_SIZE_INCREMENT,
               PATTERN_LOOKUP[Job::PATTERN_ZEROS],
               PATTERN_LOOKUP[Job::PATTERN_ONES],
               PATTERN_LOOKUP[Job::PATTERN_ALTERNATING],
               PATTERN_LOOKUP[Job::PATTERN_RANDOM],
               PATTERN_LOOKUP[Job::PATTERN_TRANSFER_NUMBERS],
               PATTERN_LOOKUP[Job::PATTERN_USER_DEFINED],
               PATTERN_LOOKUP[DEFAULT_PATTERN]);
      cout << outStr << endl;
}


//////////////////////////  get_size()  //////////////////////////////////////
capacity_t get_size(const char *arg)
{
   char units;
   capacity_t size;

   if (sscanf(arg, "%llu%c", &size, &units) == 2)
   {
      switch (units)
      {
      case 'k':
      case 'K':
         size *= 1024LL;
         break;
      case 'm':
      case 'M':
         size *= 1024LL * 1024LL;
         break;
      case 'g':
      case 'G':
         size *= 1024LL * 1024LL * 1024LL;
         break;
      default:
         error_msg("Invalid unit\n");
         exit(EXIT_ERROR_USAGE);
         break;
      }
   }
   else if (sscanf(arg, "%llu", &size) != 1)
   {
      error_msg("Could not parse size.\n");
      exit(EXIT_ERROR_USAGE);
   }
   return size;
}


//////////////////////////  parse_options()  /////////////////////////////////
bool parse_options(int argc, const char **argv)
{
   int helpArg = 0;
   int quietArg = 0;
   int verboseArg = 0;

   struct poptOption optionsTable[] =  {
      {"help", '?', POPT_ARG_NONE, &helpArg, 0, 
       NULL, NULL},
      {"version", 'V', POPT_ARG_NONE, &gGetVersion, 0, 
       NULL, NULL},
      {"quiet", 'q', POPT_ARG_NONE, &quietArg, 0, 
       NULL, NULL},
      {"verbose", 'v', POPT_ARG_NONE, &verboseArg, 0,
       NULL, NULL},
      { NULL, 0, 0, NULL, 0 }
   };

   poptContext context = poptGetContext(gPrgName, argc, argv, optionsTable, 0);
   int rc = poptGetNextOpt(context);
   if (rc < -1)
   {
      error_msg( "%s\n", poptStrerror(rc));
      poptFreeContext(context);
      return false;
   }

   if (helpArg)
      return false;

   if (gGetVersion)
      return true;

   poptFreeContext(context);
   return true;
}


/////////////////////////  validate_options()  ////////////////////////////////
bool validate_options()
{
   if (gGetVersion)
      return true;

   return true;
}


//////////////////////////  main()  ///////////////////////////////////////////
int main(int argc, char *argv[])
{
   // Process command-line options.
   gPrgName = basename(argv[0]);
   if (!parse_options(argc, (const char **)argv))
   {
      usage();
      exit(EXIT_ERROR_USAGE);
   }
   if (!validate_options())
      exit(EXIT_ERROR_USAGE);

   if (gGetVersion)
   {
      note("%s\n", PROG_VERSION);
      exit(0);
   }

   Log logger;
   try 
   {

   } catch(...)
   {
      error_msg("Caught exception\n");
      exit(EXIT_ERROR_CAUGHT_EXCEPTION);
   }

   SpewerTui tui;
   tui.init();
   tui.test();
   tui.close();

   return 0;
}
