//////////////////////////  -*-C++-*- /////////////////////////////////////////
//
// spew.cpp
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

#include <string.h>
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
#include <signal.h>

#include "common.h"
#include "Log.h"
#include "Job.h"
#include "WriteJob.h"
#include "ReadJob.h"
#include "Random.h"
#include "SpewDisplay.h"
#include "SpewConsole.h"
#include "SpewTui.h"
#include "TransferInfoList.h"
#include "CumulativeStatisticsReadWrite.h"


//////////////////////////////////////////////////////////////////////////////
///////////////////////////  Typedefs  ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
enum operation_enum {OPERATION_NONE,
                     OPERATION_WRITE,
                     OPERATION_READ, 
                     OPERATION_READ_AFTER_WRITE};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////  Constants  ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
static const char CANONICAL_PROG_NAME[] = "spew";
static const char *PROG_NAME_LOOKUP[] =
{
   (char *)NULL,
   "spew",
   "gorge",
   "regorge",
   (char *)NULL,

};
static const char SPEWRC_ENV[] = "SPEWRC";
static const char DEFAULT_SPEWRC_FILENAME[] = ".spewrc";
static const char SYSTEM_SPEWRC_FILENAME[] = "spew.conf";
static const char PROG_VERSION[] = VERSION;
static const int MAX_TMP_STR_LEN = 1024;
static const unsigned int INDENT_SIZE = 4;
static const char INDENT_CHAR = ' ';

static const Units_t DEFAULT_UNITS = KIBIBYTES;
static const capacity_t DEFAULT_MIN_BUFFER_SIZE = Transfer::BUFFER_SIZE_INCREMENT;
static const capacity_t DEFAULT_MAX_BUFFER_SIZE = DEFAULT_MIN_BUFFER_SIZE;
static unsigned int DEFAULT_ITERATIONS = 1;

static const u32_t DEFAULT_SEED = 0;
static const TransferInfoList::fill_method_t DEFAULT_FILL_METHOD = TransferInfoList::SEQUENTIAL_FILL;
static const Job::io_method_t DEFAULT_IO_METHOD = Job::ASYNCH_IO;
static const Job::pattern_t DEFAULT_PATTERN = Job::PATTERN_NONE;
static const char *PATTERN_LOOKUP[] = 
{
   "none",
   "zeros",
   "numbers",
   "random",
   (char *)NULL,
};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////  Global Variables  ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
char *gPrgName = NULL;
operation_enum gOperation;
int gGetVersion = 0;
Verbosity_t gVerbosity = VERBOSITY_SHORT;
Job::io_method_t gIOMethod = DEFAULT_IO_METHOD;
TransferInfoList::fill_method_t gFillMethod = DEFAULT_FILL_METHOD;
Units_t gUnits = DEFAULT_UNITS;
bool gProgress = false;
unsigned int gIterationsToDo = 1;
int gContinueAfterError = 0;
u32_t gSeed = 0;
capacity_t gMinBufferSize = DEFAULT_MIN_BUFFER_SIZE;
capacity_t gMaxBufferSize = DEFAULT_MAX_BUFFER_SIZE;
capacity_t gOffset = 0;
Job::pattern_t gPattern = DEFAULT_PATTERN;
capacity_t gTransferSize = 0;
string gFile = "";
bool gUseTui = false;
string gLogfilePath = "";
bool gUseStdRcFiles = true;
Log *gLogger = (Log *)NULL;
capacity_t gJobId = 0;
SpewDisplay *gDisplay = (SpewDisplay *)NULL; 
CumulativeStatisticsReadWrite gCumStats;
TimeHack gProgramStartTime;
unsigned int gFoundTransferErrors = 0;

//////////////////////////////////////////////////////////////////////////////
////////////////////  Function Prototypes  ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void error_msg(const char *fmt, ...);
void note(const char *fmt, ...);
void usage();
bool parse_options(int argc, const char **argv, string& cmdArgs);
bool validate_options();
void end_program(int exitCode);
void end_program(int exitCode, const char *fmt, ...);
void cumulative_statistics(const Job *job, 
                           const CumulativeStatistics *cumStats);
void end_statistics(const CumulativeStatistics *cumStats);
int run_transfers(Job &job, 
                  capacity_t numTransfers, 
                  bool continueAfterError,
                  CumulativeStatisticsReadWrite *cumStats);
void run_job(Job *job,
             CumulativeStatistics *cumStats);
void run_jobs(operation_enum operation, CumulativeStatistics *cumStats);


//////////////////////////////////////////////////////////////////////////////
//////////////////////  Function Definitions  ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

////////////////////////  error_msg()  ///////////////////////////////////////
void error_msg(const char *fmt, ...)
{
   fprintf(stderr, "%s: ", gPrgName);
   va_list ap;
   va_start(ap, fmt);
   vfprintf(stderr, fmt, ap);
   va_end(ap);
   fflush(stderr);
}


/////////////////////////  note()  ///////////////////////////////////////////
void note(const char *fmt, ...)
{
   va_list ap;
   va_start(ap, fmt);
   vfprintf(stdout, fmt, ap);
   va_end(ap);
   fflush(stdout);
}


//////////////////////////  version()  ///////////////////////////////////////
void version()
{
   note("%s %s\n", CANONICAL_PROG_NAME, PROG_VERSION);
   note("Copyright 2004 Hewlett-Packard Corp.\n");
   note("Written by Andrew Patterson <andrew.patterson@hp.com>\n");
}


//////////////////////////  help()  //////////////////////////////////////////
void help(poptContext &context)
{
   poptSetOtherOptionHelp(context, "--write|--read|--read-after-write [OPTION]... TRANSFER_SIZE[kKmMgG] FILE\n"
"An I/O performance measurement and load generation tool. Writes and/or \n"
"reads generated data to or from a character device, block device, or \n"
"regular file.\n");

   poptPrintHelp(context, stdout, 0);

   char fmtStr[] = 
"\n"
"  FILE                              Regular or device file to write data to.\n"
"  LOGFILE                           Path to a file used for logging.\n"
"  MAX_BUFFER_SIZE                   Mimimum buffer size used in each \n"
"                                    read(2)/write(2) call (default is\n"
"                                    MIN_BUFFER_SIZE bytes).\n"
"                                    MAX_BUFFER_SIZE. Must be an even\n"
"                                    multiple of %llu bytes and can be\n"
"                                    specified in bytes,\n"
"                                    kibibytes(k), kilobytes(K),\n"
"                                    mebibytes(m), megabytes(M),\n"
"                                    gibibytes(g), gigabytes(G).\n"
"                                    tebibytes(t), or terabytes(T).\n"
"                                    MAX_BUFFER_SIZE defaults to \n"
"                                    MIN_BUFFER_SIZE.  If MAX_BUFFER_SIZE >\n"
"                                    MIN_BUFFER_SIZE, random buffers sizes\n"
"                                    between the two limits are used.\n"
"                                    MAX_BUFFER_SIZE must be an even\n"
"                                    multiple of MIN_BUFFER_SIZE.\n"
"  MIN_BUFFER_SIZE                   Mimimum buffer size used in each \n"
"                                    read(2)/write(2) call (default is\n"
"                                    %llu bytes).\n"
"                                    MIN_BUFFER_SIZE. Must be an even\n"
"                                    multiple of %llu bytes and can be\n"
"                                    specified in bytes,\n"
"                                    kibibytes(k), kilobytes(K),\n"
"                                    mebibytes(m), megabytes(M),\n"
"                                    gibibytes(g), gigabytes(G).\n"
"                                    tebibytes(t), or terabytes(T).\n"
"  OFFSET                            Position to seek to in the file before\n"
"                                    starting I/O (default is 0). OFFSET\n"
"                                    must be an even multiple of %llu bytes\n"
"                                    and can be specified in bytes,\n"
"                                    kibibytes(k), kilobytes(K),\n"
"                                    mebibytes(m), megabytes(M),\n"
"                                    gibibytes(g), gigabytes(G).\n"
"                                    tebibytes(t), or terabytes(T).\n"
"  PATTERN                           Data pattern used when writing/reading\n"
"                                    data. Available patterns are: none, \n"
"                                    zeros, random, and numbers. The default\n"
"                                    pattern is %s.\n"
"  RCFILE                            Read additional command-line options\n"
"                                    from RCFILE.  Other options on the\n"
"                                    command-line will override options in\n"
"                                    RCFILE.\n"
"  SEED                              Used to seed the random number generator\n"
"                                    Must be >= 1 and <= 2^32.\n"
"  TRANSFER_SIZE                     Total number of bytes to transfer (must\n"
"                                    be an even multiple of both\n"
"                                    MIN_BUFFER_SIZE and MAX_BUFFER)SIZE).\n"
"                                    TRANSER_SIZE can be specified in bytes,\n"
"                                    kilobytes, megabytes, or gigabytes.\n"
"  UNITS                             Kibibytes(k), kilobytes(K), \n"
"                                    mebibytes(m), megabytes(M),\n" 
"                                    gibibytes(g), gigabytes(G).\n"
"                                    tebibytes(t), or terabytes(T).\n"
      "";

   char outStr[4096];
   snprintf(outStr, sizeof(outStr) - 1, fmtStr,
            Transfer::BUFFER_SIZE_INCREMENT,
            Transfer::BUFFER_SIZE_INCREMENT,
            Transfer::BUFFER_SIZE_INCREMENT,
            Transfer::OFFSET_INCREMENT,
            PATTERN_LOOKUP[Job::PATTERN_NONE],
            PATTERN_LOOKUP[Job::PATTERN_ZEROS],
            PATTERN_LOOKUP[Job::PATTERN_RANDOM],
            PATTERN_LOOKUP[Job::PATTERN_TRANSFER_NUMBERS],
            PATTERN_LOOKUP[DEFAULT_PATTERN]);
   fprintf(stdout, "%s", outStr);

   fprintf(stdout, "\nReport bugs to Andrew Patterson <andrew.patterson@hp.com>.\n");
}


//////////////////////////  usage()  /////////////////////////////////////////
void usage(poptContext &context)
{
   poptSetOtherOptionHelp(context, "TRANSFER_SIZE[kKmMgG] FILE");
   poptPrintUsage(context, stdout, 0);
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
      case 't':
      case 'T':
         size *= 1024LL * 1024LL * 1024LL * 102;
         break;
      default:
         error_msg("Invalid unit in argument \"%s\"\n", arg);
         exit(EXIT_ERROR_USAGE);
         break;
      }
   }
   else if (sscanf(arg, "%llu", &size) != 1)
   {
      error_msg("Could not parse size for argument \"%s\".\n", arg);
      exit(EXIT_ERROR_USAGE);
   }
   return size;
}


//////////////////////////  get_units()  //////////////////////////////////////
Units_t get_units(const char *arg)
{
   char unitsCh;
   Units_t units;

   if (sscanf(arg, "%c", &unitsCh) == 1)
   {
      switch (unitsCh)
      {
      case 'k':
         units = KIBIBYTES;
         break;
      case 'K':
         units = KILOBYTES;
         break;
      case 'm':
         units = MEBIBYTES;
         break;
      case 'M':
         units = MEGABYTES;
         break;
      case 'g':
         units = GIBIBYTES;
         break;
      case 'G':
         units = GIGABYTES;
         break;
      case 't':
         units = TEBIBYTES;
         break;
      case 'T':
         units = TERABYTES;
         break;
      default:
         error_msg("Invalid unit in argument \"%s\" - use kKmMgG.\n", arg);
         exit(EXIT_ERROR_USAGE);
         break;
      }
   }
   else
   {
      error_msg("Could not parse units in argument \"%s\"\n", arg);
      exit(EXIT_ERROR_USAGE);
   }
   return units;
}


//////////////////////////  parse_rcfile()  ///////////////////////////////////
bool parse_rcfile(FILE *rcfile,
                  string& cmdArgs)
{
   char rcArgs[MAX_TMP_STR_LEN];
   int len;
   while (fgets(rcArgs, MAX_TMP_STR_LEN, rcfile) != (char *)NULL)
   {
      // Chop off string after comment character.
      char *commentStart = strchr(rcArgs, '#');
      if (commentStart)
         *commentStart = '\0';
      len = strlen(rcArgs);
      if (rcArgs[len - 1] == '\n')
         rcArgs[len - 1] = '\0';
      if (rcArgs[0] != '\0')
      {
         cmdArgs += rcArgs;
         cmdArgs += " ";
      }
   }
   return true;
}


//////////////////////////  read_rcfiles()  ///////////////////////////////////
bool read_rcfiles(int argc, const char **argv, string& cmdArgs)
{
   struct stat statbuf;
   string rcFilePath;
   vector<string> rcFilePaths;


   // Check command-line for --no-rcfiles option.
   for (int i = 0; i < argc; i++)
   {
      if (strncmp(argv[i], "--no-rcfiles", strlen("--no-rcfiles")) == 0)
      {
         gUseStdRcFiles = false;
         break;
      }
   }

   // Read standard config locations.
   if (gUseStdRcFiles)
   {
      // Read system-wide rcfile if it exists.
      rcFilePath = QUOTE(SYSCONFDIR);
      rcFilePath += "/";
      rcFilePath += SYSTEM_SPEWRC_FILENAME;
      if (stat(rcFilePath.c_str(), &statbuf) >= 0)
      {
         rcFilePaths.push_back(rcFilePath);
      }
      else
      {
         if (errno != ENOENT)
         {
            error_msg("Cannot access rc file \"%s\" -- %s.\n", 
                      rcFilePath.c_str(), strError(errno).c_str());
         }
      }

      rcFilePath = "";
      if (getenv(SPEWRC_ENV))
      {
         rcFilePath = getenv(SPEWRC_ENV);
      }
      else
      {
         char *home = getenv("HOME");
         if (home)
         {
            rcFilePath = home;
            rcFilePath += "/";
            rcFilePath += DEFAULT_SPEWRC_FILENAME;
         }
      }
      if (stat(rcFilePath.c_str(), &statbuf) >= 0)
      {
         rcFilePaths.push_back(rcFilePath);
      }
      else
      {
         if (errno != ENOENT)
         {
            error_msg("Cannot access rc file \"%s\" -- %s.\n", 
                      rcFilePath.c_str(), strError(errno).c_str());
         }
      }
   }

   // Check the command-line for --rcfile.
   for (int i = 1; i < argc; i++)
   {
      rcFilePath = "";
      if (strncmp(argv[i], "--rcfile", 8) == 0)
      {
         const char *eqPos = strrchr(argv[i], '=');
         if (eqPos == (char *)NULL)
         {
            if (i + 1 < argc)
            {
               rcFilePath = argv[i+1];
            }
            else
            {
               error_msg("Missing RCFILE argument.\n");
               return false;
            }
         }
         else
         {
            rcFilePath = eqPos + 1;
         }
         if (stat(rcFilePath.c_str(), &statbuf) < 0)
         {
            error_msg("Cannot access RCFILE \"%s\" -- %s.\n", 
                      rcFilePath.c_str(), strError(errno).c_str());
            return false;
         }
         else
            rcFilePaths.push_back(rcFilePath);
         break;
      }
   }

   vector<string>::iterator pathIter;
   for (pathIter = rcFilePaths.begin();
        pathIter != rcFilePaths.end();
        pathIter++)
   {
      FILE *rcfile = fopen(pathIter->c_str(), "r");
      if (rcfile == (FILE *)NULL)
      {
         error_msg("Cannot open rc file \"%s\" -- %s.\n", 
                   pathIter->c_str(), strError(errno).c_str());
      }
      else
      {
         parse_rcfile(rcfile, cmdArgs);
         fclose(rcfile);
      }
   }

   return true;
}


//////////////////////////  parse_options()  /////////////////////////////////
bool parse_options(int argc, const char **argv, string& cmdArgs)
{
   char *minBufferSizeArgStr = (char *)NULL;
   char *maxBufferSizeArgStr = (char *)NULL;
   char *offsetArgStr = (char *)NULL;
   char *patternArgStr = (char *)NULL;
   char *unitsArgStr = (char *)NULL;
   char *logfilePathArgStr = (char *)NULL;
   char *dummyArgStr = (char *)NULL;
   int writeArg = 0;
   int readArg = 0;
   int readAfterWriteArg = 0;
   int helpArg = 0;
   int usageArg = 0;
   int iterationsArg = -1;
   int noStatisticsArg = 0;
   int statisticsArg = 0;
   int detailedStatisticsArg = 0;
   int noProgressArg = 0;
   int progressArg = 0;
   int noTuiArg = 0;
   int tuiArg = 0;
   int syncArg = 0;
   int directArg = 0;
   int randomArg = 0;
   int generateLoadArg = 0;
   int useStdRcFilesArg = 1;

   struct poptOption optionsTable[] =  {
      {"max-buffer-size", 'B', POPT_ARG_STRING, &maxBufferSizeArgStr, 0, "Each read(2)/write(2) call uses a maximum buffer of size BUFFER_SIZE.", "BUFFER_SIZE"},
      {"min-buffer-size", 'b', POPT_ARG_STRING, &minBufferSizeArgStr, 0, "Each read(2)/write(2) call uses a minimum buffer of size BUFFER_SIZE.", "BUFFER_SIZE"},
      {"continue-after-error", 'c', POPT_ARG_NONE, &gContinueAfterError, 0, "Continue after data integrity errors.", NULL},
#ifdef HAVE_DIRECT_IO
      {"direct", 'd', POPT_ARG_NONE, &directArg, 0, "Use direct I/O. Should only be used on block device files. Not all operating systems support direct I/O", NULL},
#endif
      {"generate-load", 'g', POPT_ARG_NONE, &generateLoadArg, 0, "Equivalent to: -v -t -P -p random -i 0.",  NULL},
      {"iterations", 'i', POPT_ARG_INT, &iterationsArg, 0, "Write/read data COUNT times. If count is 0, repeats forever.", "COUNT"},
      {"logfile", 'l', POPT_ARG_STRING, &logfilePathArgStr, 0, "Send log messages to LOGFILE.", "LOGFILE"},
      {"no-progress", 0, POPT_ARG_NONE, &noProgressArg, 0, "Don't show progess (default).", NULL},
      {"no-rcfiles", 0, POPT_ARG_NONE, NULL, 0, "Don't use standard rcfiles.", NULL},
      {"no-statistics", 'q', POPT_ARG_NONE, &noStatisticsArg, 0, "Don't output statistics.", NULL},
      {"no-tui", 0, POPT_ARG_NONE, &noTuiArg, 0, "Don't use TUI interface.", NULL},
      {"offset", 'o', POPT_ARG_STRING, &offsetArgStr, 0, "Seek to OFFSET before starting I/O.", "OFFSET"},
      {"progress", 'P', POPT_ARG_NONE, &progressArg, 0, "Show progess.", NULL},
      {"pattern", 'p', POPT_ARG_STRING, &patternArgStr, 0, "Use data pattern PATTERN when reading or writing data.", "PATTERN"},
      {"random", 'r', POPT_ARG_NONE, &randomArg, 0, "Read/Write buffers to random offsets.", NULL},
      {"raw", 0, POPT_ARG_NONE, &readAfterWriteArg, 0, "An alias for --read-after-write.", NULL},
      {"rcfile", 0, POPT_ARG_STRING, &dummyArgStr, 0, "Read command-line options from RCFILE.", "RCFILE"},
      {"read", 0, POPT_ARG_NONE, &readArg, 0, "Read date from FILE.", NULL},
      {"read-after-write", 0, POPT_ARG_NONE, &readAfterWriteArg, 0, "Read back data after writing to FILE.", NULL},
      {"seed", 'S', POPT_ARG_LONG, &gSeed, 0, "Use SEED for random number seed.","SEED"},
      {"sync", 's', POPT_ARG_NONE, &syncArg, 0, "Use synchronous I/O.", NULL},
      {"statistics", 0, POPT_ARG_NONE, &statisticsArg, 0, "Output statistics (default).", NULL},
      {"tui", 't', POPT_ARG_NONE, &tuiArg, 0, "Use curses-based, terminal user interface.", NULL},
      {"units", 'u', POPT_ARG_STRING, &unitsArgStr, 0, "Show tranfer rate in UNITS units.", "UNITS"},
      {"usage", 0, POPT_ARG_NONE, &usageArg, 0, "Show brief usage message and exit.", NULL},
      {"version", 'V', POPT_ARG_NONE, &gGetVersion, 0, "Output version information and exit.", NULL},
      {"detailed-statistics", 'v', POPT_ARG_NONE, &detailedStatisticsArg, 0, "Output detailed statistics.", NULL},
      {"write", 0, POPT_ARG_NONE, &writeArg, 0, "Write data to FILE.", NULL},
      {"help", '?', POPT_ARG_NONE, &helpArg, 0, "Show this help and exit.", NULL},
      POPT_TABLEEND
   };

   cmdArgs = "";
   if (!read_rcfiles(argc, argv, cmdArgs))
       return false;
   for (int i = 1; i < argc; i++)
   {
      cmdArgs += argv[i];
      cmdArgs += " ";
   }
   cmdArgs = " " + cmdArgs;
   cmdArgs = argv[0] + cmdArgs;
   int newArgc;
   const char **newArgv;
   poptParseArgvString((char *)cmdArgs.c_str(), &newArgc, &newArgv);
   poptContext context = poptGetContext(gPrgName,
                                        newArgc, 
                                        newArgv, 
                                        optionsTable, 
                                        POPT_CONTEXT_POSIXMEHARDER);

   int rc = poptGetNextOpt(context);
   if (rc < -1)
   {
      switch (rc)
      {
      case POPT_ERROR_BADOPT:
         error_msg("bad or unknown option \"%s\"\n.", 
                   poptBadOption(context, 0));
         break;
      default:
         error_msg("%s.\n", poptStrerror(rc));
         break;
      }
      usage(context);
      poptFreeContext(context);
      return false;
   }

   // Help.
   if (helpArg)
   {
      help(context);
      poptFreeContext(context);
      return false;
   }

   // Usage.
   if (usageArg)
   {
      usage(context);
      poptFreeContext(context);
      return false;
   }

   // Version.
   if (gGetVersion)
   {
      poptFreeContext(context);
      return true;
   }

   // Operation.
   if (writeArg + readArg + readAfterWriteArg > 1)
   {
      error_msg("Only one operation (--write|--read|--read-after-write) allowed.\n");
      usage(context);
      poptFreeContext(context);
      return false;
   }
   if (writeArg)
      gOperation = OPERATION_WRITE;
   else if (readArg)
      gOperation = OPERATION_READ;
   else if (readAfterWriteArg)
      gOperation = OPERATION_READ_AFTER_WRITE;

   // Min Buffer size.
   if (minBufferSizeArgStr)
   {
      gMinBufferSize = get_size(minBufferSizeArgStr);
   }
   // Max Buffer size.
   if (maxBufferSizeArgStr)
   {
      gMaxBufferSize = get_size(maxBufferSizeArgStr);
   }
   else
   {
      gMaxBufferSize = gMinBufferSize;
   }

   // Offset.
   if (offsetArgStr)
   {
      gOffset = get_size(offsetArgStr);
   }

   // Pattern.
   if (patternArgStr)
   {
      bool found = false;
      for (int pattern = (int)Job::PATTERN_NONE;
           PATTERN_LOOKUP[pattern] != (char *)NULL;
           pattern++)
      {
         if (strcmp(patternArgStr, PATTERN_LOOKUP[pattern]) == 0)
         {
            gPattern = (Job::pattern_t)pattern;
            found = true;
            break;
         }
      }
      if (!found)
      {
         error_msg("\"%s\" is not a valid pattern. Use none, zeros, numbers, or random.\n", patternArgStr);
         usage(context);
         poptFreeContext(context);
         return false;
      }
   }  

   // Units.
   if (unitsArgStr)
   {
      gUnits = get_units(unitsArgStr);
   }

   // Logfile.
   if (logfilePathArgStr)
   {
      gLogfilePath = logfilePathArgStr;
   }
   
   // Verbosity.
   if (noStatisticsArg)
      gVerbosity = VERBOSITY_NONE;
   if (statisticsArg)
      gVerbosity = VERBOSITY_SHORT;
   if (detailedStatisticsArg)
      gVerbosity = VERBOSITY_LONG;
   
   // Sync, direct I/O, and random seek.
   if (syncArg)
      gIOMethod = Job::SYNCH_IO;
   if (directArg)
      gIOMethod = Job::DIRECT_IO;
   if (randomArg)
      gFillMethod = TransferInfoList::RANDOM_FILL;

   // Progress.
   if (noProgressArg)
      gProgress = false;
   if (progressArg)
      gProgress = true;

   // Iterations.
   if (iterationsArg >= 0)
      gIterationsToDo = (unsigned int)iterationsArg;

   // TUI.
   if (noTuiArg)
      gUseTui = false;
   if (tuiArg)
      gUseTui = true;

   // Generate load -v -t -P -p random -i 0, above options will override 
   // these.
   if (generateLoadArg)
   {
      if (!noProgressArg)
         gProgress = true;
      if (!noStatisticsArg && !statisticsArg)
         gVerbosity = VERBOSITY_LONG;
      if (!noTuiArg)
         gUseTui = true;
      if (!patternArgStr)
         gPattern = Job::PATTERN_RANDOM;
      if (iterationsArg < 0)
         gIterationsToDo = 0;
   }
   if (gIterationsToDo < 0)
      gIterationsToDo = DEFAULT_ITERATIONS;

   // Count the rest of the arguments.
   const char **argsLeft = poptGetArgs(context);
   int argsCount = 0;
   if (argsLeft)
   {
      while (argsLeft[argsCount] != NULL)
         argsCount++;
   }
   if (argsCount < 2)
   {
      error_msg("Need TRANSFER_SIZE and FILE.\n");
      usage(context);
      poptFreeContext(context);
      return false;
   }

   gTransferSize = get_size(poptGetArg(context));
   gFile = poptGetArg(context);

   if (gSeed == DEFAULT_SEED)
   {
      gSeed = Random().getRandom32();
      gJobId = 0;
   }
   else
   {
      gJobId = gSeed;
   }

   poptFreeContext(context);
   return true;
}


/////////////////////////  validate_options()  ////////////////////////////////
bool validate_options()
{
   if (gGetVersion)
      return true;

   if (gOperation == OPERATION_NONE)
   {
      error_msg("Must specify an operation (--write|--read|--read-after-write).\n");
      return false;
   }

   if (gMinBufferSize > gMaxBufferSize)
   {
      error_msg("Minimum buffer size must be <= maximum buffer size.\n");
      return false;
   }
   if (gMaxBufferSize % gMinBufferSize != 0)
   {
      error_msg("Maximum buffer size must be an even multiple of minimum buffer size.\n");
      return false;
   }
   if (gMinBufferSize % Transfer::BUFFER_SIZE_INCREMENT != 0)
   {
      error_msg("Minimum buffer size must be an even multiple of %llu bytes.\n",  Transfer::BUFFER_SIZE_INCREMENT);
      return false;
   }
   if (gMaxBufferSize % Transfer::BUFFER_SIZE_INCREMENT != 0)
   {
      error_msg("Maximum buffer size must be an even multiple of %llu bytes.\n", Transfer::BUFFER_SIZE_INCREMENT);
      return false;
   }
   if (gTransferSize < gMinBufferSize)
   {
      error_msg("TRANSFER_SIZE must be >= MIN_BUFFER_SIZE.\n");
      return false;
   }
   if (gTransferSize < gMaxBufferSize)
   {
      error_msg("TRANSFER_SIZE must be >= MAX_BUFFER_SIZE.\n");
      return false;
   }
   if (gTransferSize % gMinBufferSize != 0)
   {
      error_msg("TRANSFER_SIZE must be an even multiple of MIN_BUFFER_SIZE.\n");
      return false;
   }
   if (gTransferSize % gMaxBufferSize != 0)
   {
      error_msg("TRANSFER_SIZE must be an even multiple of MAX_BUFFER_SIZE.\n");
      return false;
   }


#ifdef HAVE_DIRECT_IO
   if (gIOMethod == Job::DIRECT_IO)
   {
      if (gMinBufferSize % Transfer::DIRECTIO_BUFFER_SIZE_INCREMENT != 0)
      {
         error_msg("MIN_BUFFER_SIZE must be a multiple of %llu bytes when using direct I/O. Use -b|--min-buffer-size to set MIN_BUFFER_SIZE.\n", Transfer::DIRECTIO_BUFFER_SIZE_INCREMENT);
         return false;
      }
      if (gMaxBufferSize % Transfer::DIRECTIO_BUFFER_SIZE_INCREMENT != 0)
      {
         error_msg("MAX_BUFFER_SIZE must be a multiple of %llu bytes when using direct I/O. Use -B|--max-buffer-size to set MAX_BUFFER_SIZE.\n", Transfer::DIRECTIO_BUFFER_SIZE_INCREMENT);
         return false;
      }
   }
#endif

#ifdef HAVE_MOUNT_CHECK
   if (is_mounted(gFile.c_str()))
   {
      error_msg("Cannot use a mounted device for FILE.\n");
      return false;
   }
#endif // HAVE_MOUNT_CHECK

   return true;
}


//////////////////////////  end_program()  ////////////////////////////////////
void end_program(int exitCode)
{
   end_program(exitCode, "");
}


//////////////////////////  end_program()  ////////////////////////////////////
void end_program(int exitCode, const char *fmt, ...)
{
   int rtn;
   char msg[MAX_TMP_STR_LEN];

   if (gDisplay)
      gDisplay->close();
   
   va_list ap;
   va_start(ap, fmt);
   vsnprintf(msg, MAX_TMP_STR_LEN, fmt, ap);
   va_end(ap);

   if (strlen(msg) > 0)
   {
      if (gLogger)
         gLogger->logError(msg);
      error_msg(msg);
   }

   if (gLogger)
   {
      if (exitCode == EXIT_OK)
         end_statistics(&gCumStats);
      if (gFoundTransferErrors > 0)
      {
         if (gLogfilePath.length() > 0)
         {
            gLogger->showError("One or more data integrity or I/O errors were encountered during data\ntransfers.  Descriptions of these errors can be found in the logfile\n%s.\n", gLogfilePath.c_str());
         }
         else
         {
            gLogger->showError("One or more data integrity or I/O errors were encountered during data\ntransfers.  Descriptions of these types of errors are output to a log file if\nthe -l|--logfile option is used.\n");
         }
      }
      gLogger->logFinish();
      if ((rtn = gLogger->close()) != 0)
      {
         error_msg("Could not close logfile \"%s\" -- %s\n",
                   gLogfilePath.c_str(), strError(rtn).c_str());
         if (exitCode == EXIT_OK)
            exitCode = EXIT_ERROR_SYSTEM;
      }
   }

   exit(exitCode);
}


//////////////////////////  cleanup()  ///////////////////////////////////////
void cleanup(int signal)
{
   if (signal == SIGQUIT || signal == SIGINT)
      end_program(EXIT_OK);
   else
      end_program(EXIT_ERROR_CAUGHT_EXCEPTION, 
                  "Caught signal \"%s\", exiting.\n", strSignal(signal).c_str());
}


//////////////////////////  resize()  /////////////////////////////////////////
void resize(int sig)
{
   static int working = 0;  // Latch for repeated interrupts.

   if (!working++)
   {
      if (gDisplay)
         gDisplay->resize();
   }
   working--;
}


#ifdef USE_THREADS
//////////////////////////  threads_signal_handler()  /////////////////////////
void *threads_signal_handler(void *arg)
{
   sigset_t sigsToCatch;
   int caught;

   sigemptyset(&sigsToCatch);
   sigaddset(&sigsToCatch, SIGQUIT);
   sigaddset(&sigsToCatch, SIGTERM);
   sigaddset(&sigsToCatch, SIGINT);

   for (;;)
   {
      sigwait(&sigsToCatch, &caught);
      switch(caught)
      {
      case SIGQUIT:
      case SIGINT:
         end_program(EXIT_OK);
         break;
      default:
         end_program(EXIT_ERROR_CAUGHT_EXCEPTION, 
                     "Caught signal \"%s\", exiting.\n", 
                     strSignal(caught).c_str());
         break;
      }
   }
}
#endif // USE_THREADS


//////////////////////////  cumulative_statistics()  //////////////////////////
void cumulative_statistics(const Job *job, 
                           const CumulativeStatistics *cumStats)
{
   TimeHack now(TimeHack::getCurrentTime());
   TimeHack jobTransferTime(job->getJobEndTime() - job->getJobStartTime());
   gDisplay->cumulativeStatistics(job->getStatistics(),
                                  cumStats,
                                  now - gProgramStartTime);

   long double transferRate = convertCapacity((long double)job->getJobBytesTransferred(), gUnits)/jobTransferTime.getTime();
   long double iops = (long double)job->getTotalNumberOfTransfers()/(long double)jobTransferTime.getTime();

   switch (job->getIoDirection())
   {
   case READING:
      gLogger->logNote("Iter: %5d   RTR: %11.2Lf %-5s   TT: %s   IOPS: %11.2Lf\n",
                       cumStats->getIterations(),
                       transferRate, 
                       getTransferRateUnitsStr(gUnits), 
                       jobTransferTime.getElapsedTimeStr().c_str(),
                       iops);
      break;
   case WRITING:
      gLogger->logNote("Iter: %5d   WTR: %11.2Lf %-5s   TT: %s   IOPS: %11.2Lf\n",
                       cumStats->getIterations(),
                       transferRate, 
                       getTransferRateUnitsStr(gUnits), 
                       jobTransferTime.getElapsedTimeStr().c_str(),
                       iops);
      break;
   }  
}


//////////////////////////  end_statistics()  ////////////////////////////////
void end_statistics(const CumulativeStatistics *cumStats)
{
   unsigned int device;
   device = Log::OUTPUT_LOG_STDOUT;
   if (gVerbosity == VERBOSITY_LONG)
      device |= Log::OUTPUT_DISPLAY_STDOUT;

   TimeHack now(TimeHack::getCurrentTime());

   if (!gUseTui)
      gLogger->note(device, "\n");
   gLogger->note(device, "Total iterations:                %17u\n",
                 cumStats->getIterations());
   gLogger->note(device, "Total runtime:                   %17s\n",
                 gProgramStartTime.getTimeDiffStr(now).c_str());

   if (cumStats->getTotalBytesWritten() > 0)
   {
      long double writeTransferRate = convertCapacity((long double)(cumStats->getTotalBytesWritten()), gUnits)/(long double)(cumStats->getTotalWriteTransferTime().getTime());
      long double writeIops = (long double)cumStats->getTotalWriteOps()/(long double)(cumStats->getTotalWriteTransferTime().getTime());
      gLogger->note(device, "Total write transfer time (WTT): %17s\n", 
                    cumStats->getTotalWriteTransferTime().getElapsedTimeStr().c_str());
      gLogger->note(device, "Total write transfer rate (WTR): %11.2Lf %-5s\n",
                    writeTransferRate, getTransferRateUnitsStr(gUnits));
      gLogger->note(device, "Total write IOPS:                %11.2Lf IOPS\n", writeIops);
                    
   }
   if (cumStats->getTotalBytesRead() > 0)
      
   {
      long double readTransferRate = convertCapacity((long double)cumStats->getTotalBytesRead(), gUnits)/(long double)(cumStats->getTotalReadTransferTime().getTime());
      long double readIops = (long double)cumStats->getTotalReadOps()/(long double)(cumStats->getTotalReadTransferTime().getTime());
      gLogger->note(device, "Total read transfer time (RTT):  %17s\n", 
                    cumStats->getTotalReadTransferTime().getElapsedTimeStr().c_str());
      gLogger->note(device, "Total read transfer rate (RTR):  %11.2Lf %-5s\n",
                    readTransferRate, getTransferRateUnitsStr(gUnits));
      gLogger->note(device, "Total read IOPS:                 %11.2Lf IOPS\n", readIops);
   }
}


//////////////////////////  run_transfers()  //////////////////////////////////
int run_transfers(Job *job, 
                  capacity_t numTransfers, 
                  bool continueAfterError,
                  CumulativeStatisticsReadWrite *cumStats)
{
   int ret;

   ret = job->runTransfers(numTransfers, continueAfterError);

   switch (job->getIoDirection())
   {
   case READING:
      cumStats->addToTotalBytesRead(job->getBytesTransferred());
      cumStats->addToTotalReadTransferTime(job->getTransferEndTime() - job->getTransferStartTime()); 
      break;
   case WRITING:
      cumStats->addToTotalBytesWritten(job->getBytesTransferred());
      cumStats->addToTotalWriteTransferTime(job->getTransferEndTime() - job->getTransferStartTime()); 
      break;
   }

   return ret;
}


//////////////////////////  run_job()  ////////////////////////////////////////
void run_job(Job *job, CumulativeStatisticsReadWrite *cumStats)
{
   int ret;
   capacity_t startingVerticalHacks = gDisplay->getCurrentProgressRows();
   capacity_t startingHorizontalHacks = gDisplay->getCurrentProgressColumns();
   capacity_t totalTransfers = job->getTotalNumberOfTransfers();
   capacity_t transferSize = job->getTransferSize();
   TimeHack savedTotalWriteTransferTime(cumStats->getTotalWriteTransferTime());
   TimeHack savedTotalReadTransferTime(cumStats->getTotalReadTransferTime());
   gDisplay->setTotalTransfers(totalTransfers);

   if (gProgress || gUseTui)
   {
      capacity_t bufferSize = job->getBufferSize();
      gDisplay->startJob(cumStats->getIterations(), job->getIoDirection());
      ret = job->startJob();
      if (ret != EXIT_OK)
      {
         end_program(ret, "%s\n", job->getLastErrorMessage().c_str());
      }
      capacity_t numTrans;
      while ((numTrans = gDisplay->getTransfersInNextHack()) > 0)
      {
         if (gDisplay->isStartOfProgressRow())
            job->startInterval();
         ret = run_transfers(job, 
                             numTrans, 
                             gContinueAfterError,
                             cumStats);
         if (gDisplay->isEndOfProgressRow(numTrans))
            job->endInterval();
         if (ret != EXIT_OK)
         {
            gFoundTransferErrors++;
            if (gContinueAfterError)
            {
               gLogger->logError("%s\n", 
                                 job->getLastErrorMessage().c_str());
               gDisplay->addToProgress(numTrans, 
                                       true, 
                                       job->getStatistics(),
                                       cumStats);
            }
            else
            {
               end_program(ret, "%s\n", job->getLastErrorMessage().c_str());
            }
         }
         else
         {
            gDisplay->addToProgress(numTrans, 
                                    false,
                                    job->getStatistics(),
                                    cumStats);
         }
      }            
      ret = job->finishJob();
      if (ret != EXIT_OK)
      {
         gLogger->showError("%s\n", job->getLastErrorMessage().c_str());
         exit(ret);
      }
   }
   else  // no progress or TUI.
   {
      gDisplay->startJob(cumStats->getIterations(), job->getIoDirection());
      ret = job->startJob();
      if (ret != EXIT_OK)
      {
         end_program(ret, "%s\n", job->getLastErrorMessage().c_str());
         exit(ret);
      }
      ret = run_transfers(job, 
                          totalTransfers,
                          gContinueAfterError,
                          cumStats);
      if (ret != EXIT_OK)
      {
         gFoundTransferErrors++;
         if (gContinueAfterError)
         {
            gLogger->logError("%s\n", job->getLastErrorMessage().c_str());
         }
         else
         {
            end_program(ret, "%s\n", job->getLastErrorMessage().c_str());
         }
      }
      ret = job->finishJob();
      if (ret != EXIT_OK)
      {
         end_program(ret, "%s\n", job->getLastErrorMessage().c_str());
      }
   }
   // May get incremental time errors during transfers, so reset transfer 
   // totals to old value plus total job time.
   switch (job->getIoDirection())
   {
   case READING:
      cumStats->setTotalReadTransferTime(savedTotalReadTransferTime + job->getJobEndTime() - job->getJobStartTime());
      break;
   case WRITING:
      cumStats->setTotalWriteTransferTime(savedTotalWriteTransferTime + job->getJobEndTime() - job->getJobStartTime());
      break;
   }

   // Update IOPS.
   switch (job->getIoDirection())
   {
   case READING:
      cumStats->addToTotalReadOps(job->getTotalNumberOfTransfers()); 
      break;
   case WRITING:
      cumStats->addToTotalWriteOps(job->getTotalNumberOfTransfers()); 
      break;
   }

   cumulative_statistics(job, cumStats);
   gDisplay->endJob();
}


//////////////////////////  run_jobs()  ///////////////////////////////////////
   void run_jobs(operation_enum operation, 
                 CumulativeStatisticsReadWrite *cumStats)
   {
      int ret;

      cumStats->setIterations(0);
      do
      {
         cumStats->incIterations();
         if (operation == OPERATION_WRITE || 
             operation == OPERATION_READ_AFTER_WRITE)
         {
            Job *job = new WriteJob(*gLogger,
                                    gFile,
                                    gOffset,
                                    gTransferSize,
                                    gMinBufferSize,
                                    gMaxBufferSize,
                                    TransferInfoList::GEOMETRIC_PROGRESSION,
                                    gPattern,
                                    gFillMethod,
                                    gIOMethod,
                                    gSeed,
                                    gJobId);
            if (job == (Job *)NULL)
            {
               gLogger->showError("Could not allocate memory.");
               exit(EXIT_ERROR_MEMORY_ALLOC);
            }
            if ((ret = job->init()) != EXIT_OK)
            {
               gLogger->showError("Could not initialize job.\n");
               exit(ret);
            }
         
            run_job(job, cumStats);
            delete job;
         
            if (gIterationsToDo == 0 || 
                cumStats->getIterations() < gIterationsToDo ||
                cumStats->getIterations() == gIterationsToDo && 
                operation == OPERATION_READ_AFTER_WRITE)
            {
               gDisplay->nextJob();
            }
         }

         if (operation == OPERATION_READ || 
             operation == OPERATION_READ_AFTER_WRITE)
         {
            Job *job = new ReadJob(*gLogger,
                                   gFile,
                                   gOffset,
                                   gTransferSize,
                                   gMinBufferSize,
                                   gMaxBufferSize,
                                   TransferInfoList::GEOMETRIC_PROGRESSION,
                                   gPattern,
                                   gFillMethod,
                                   gIOMethod,
                                   gSeed,
                                   gJobId);
            if (job == (Job *)NULL)
            {
               gLogger->showError("Could not allocate memory.");
               exit(EXIT_ERROR_MEMORY_ALLOC);
            }
            if ((ret = job->init()) != EXIT_OK)
            {
               gLogger->showError("Could not initialize job.\n");
               exit(ret);
            }

            run_job(job, cumStats);
            delete job;

            if (gIterationsToDo == 0 || cumStats->getIterations() < gIterationsToDo)
            {
               gDisplay->nextJob();
            }
         }

      } while (gIterationsToDo == 0LLU || cumStats->getIterations() < gIterationsToDo);
   }


//////////////////////////  main()  ///////////////////////////////////////////
   int main(int argc, char *argv[])
   {
      int rtn;

      // Process command-line options.
      gPrgName = argv[0];
      const char *prgBasename = basename(gPrgName);
      string cmdArgs;
   
      for (int i = 1; PROG_NAME_LOOKUP[i] != (char *)NULL; i++)
      {
         if (strcmp(prgBasename, PROG_NAME_LOOKUP[i]) == 0)
         {
            gOperation = (operation_enum)i;
            break;

         }
      }

      if (!parse_options(argc, (const char **)argv, cmdArgs))
      {
         exit(EXIT_ERROR_USAGE);
      }
      if (!validate_options())
         exit(EXIT_ERROR_USAGE);

      if (gGetVersion)
      {
         version();
         exit(0);
      }

      struct sigaction action;
#ifdef USE_THREADS
      sigset_t sigsToBlock;
      sigemptyset(&sigsToBlock);
      sigaddset(&sigsToBlock, SIGQUIT);
      sigaddset(&sigsToBlock, SIGTERM);
      sigaddset(&sigsToBlock, SIGINT);
      pthread_sigmask(SIG_BLOCK, &sigsToBlock, NULL);

      action.sa_handler = resize;
      sigemptyset(&action.sa_mask);
      action.sa_flags = SA_RESTART;
      sigaction(SIGWINCH, &action, NULL);

      pthread_t signal_handler_thread;
      if (pthread_create(&signal_handler_thread, 
                         NULL,
                         &threads_signal_handler,
                         NULL))
      {
         error_msg("Could not create thread.\n");
         exit(EXIT_ERROR_SYSTEM);
      }
#else
      action.sa_handler = cleanup;
      sigemptyset(&action.sa_mask);
      action.sa_flags = 0;
      sigaction(SIGQUIT, &action, NULL);
      sigaction(SIGTERM, &action, NULL);
      sigaction(SIGINT, &action, NULL);

      action.sa_handler = resize;
      sigemptyset(&action.sa_mask);
      action.sa_flags = SA_RESTART;
      sigaction(SIGWINCH, &action, NULL);
#endif // USE_THREADS

      gLogger = new Log(gLogfilePath);
      if (gLogger == (Log *)NULL)
      {
         end_program(EXIT_ERROR_MEMORY_ALLOC, "Out of memory.\n");
      }
      if ((rtn = gLogger->open()) != EXIT_OK)
      {
         end_program(rtn, "Could not open logfile \"%s\" -- %s\n",
                     gLogfilePath.c_str(), strError(rtn).c_str());
      }

      if (gUseTui)
         gDisplay = new SpewTui(gIterationsToDo, gUnits, gProgress, gVerbosity);
      else
         gDisplay = new SpewConsole(gIterationsToDo, gUnits, gProgress, gVerbosity);

      if (gDisplay == (SpewDisplay *)NULL)
         end_program(EXIT_ERROR_MEMORY_ALLOC, "Out of memory.\n");
      if ((rtn = gDisplay->init()) != EXIT_OK)
         end_program(rtn, "Could not initialize display.\n");

      gDisplay->setCurrentUnits(gUnits);
      gProgramStartTime = TimeHack::getCurrentTime();
      gLogger->logStart();
      gLogger->logCmdLine(cmdArgs.c_str());
      gLogger->logNote("\n");
      gDisplay->startRun(gProgramStartTime);

      try 
      {
         run_jobs(gOperation, &gCumStats);
      } catch(...)
      {
         gLogger->showError("Caught exception\n");
         exit(EXIT_ERROR_CAUGHT_EXCEPTION);
      }
      gDisplay->endRun();
      end_program(EXIT_OK);

      return 0;
   }



