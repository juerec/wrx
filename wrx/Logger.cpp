/****************************************************************************
**
**  This file is a part of the program "PC NAVTEX".
**
**  Copyright © 2020 Jürgen Reche
**
** PC NAVTEX is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** PC NAVTEX is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see <https://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "Logger.h"

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

Logger logger;

Logger::Logger(const char * LogFile, Level lev) {

  level = lev;
  f = nullptr;
  if (strcmp(LogFile, "stderr") == 0)
    f = stderr;
  if (strcmp(LogFile, "stdout") == 0)
    f = stdout;
  if (f == nullptr) {
    f = fopen(LogFile, "a");
    if (f == nullptr) {
      f = stderr;
      error("Logger::Logger()", "Cannot open LogFile \"%s\"     \"%s\"\n", LogFile, strerror(errno));
    }
  }
}

Logger::~Logger() {
  if (f != stderr && f != stdout)
    fclose(f);
}

void Logger::setLevel(const char * cLevel) {

  if (cLevel != nullptr) {
    if (!strcmp(cLevel, "DEBUG"))
      level = DEBUG;
    if (!strcmp(cLevel, "INFO"))
      level = INFO;
    if (!strcmp(cLevel, "WARNING"))
      level = WARNING;
    if (!strcmp(cLevel, "ERROR"))
      level = ERROR;
    if (!strcmp(cLevel, "NONE"))
      level = NONE;
  }
}

void Logger::log(const char * Level, const char * Location, const char * fmt, va_list * ap) {
//time_t t = time(nullptr);
struct tm tt; // = *localtime(&t);
struct timeval tv;
const char  * nl;

  gettimeofday(&tv, nullptr);
  tt = *localtime(&tv.tv_sec);

#ifndef NO_THREADS
  mutex.lock();
#endif

  fprintf(f, "%04i-%02i-%02i %02i:%02i:%02i.%03i %s : %s ", tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, (int)(tv.tv_usec / 1000), Level == nullptr ? "LOG:   " : Level, Location == nullptr ? "---" : Location);

  if (fmt != nullptr) {
    vfprintf(f, fmt, *ap);
    nl = strrchr(fmt, '\n');
    if (nl != nullptr && nl[1] != 0)
      fprintf(f, "\n");
  } else {
    fprintf(f, "\n");
  }
#ifndef NO_THREADS
  mutex.unlock();
#endif
}

void Logger::log(const char * Level, const char * Location, int Line, const char * fmt, va_list * ap) {
struct tm tt;
struct timeval tv;
const char * pSpace;

#ifndef NO_THREADS
  mutex.lock();
#endif

  if (Location == nullptr) {
    pSpace = "---";
  } else {
    pSpace = strrchr(Location, ' ');
    if (pSpace != nullptr)
      pSpace++;
    else
      pSpace = Location;
  }
  gettimeofday(&tv, nullptr);
  tt = *localtime(&tv.tv_sec);

  // "2010-09-06 18:07:12.000  DEBUG : Logger::log() "
  fprintf(f, "%04i-%02i-%02i %02i:%02i:%02i.%03i %s : %s[%i] ", tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, (int)(tv.tv_usec / 1000), Level == nullptr ? "LOG:   " : Level, pSpace, Line);

  if (fmt != nullptr) {
    vfprintf(f, fmt, *ap);
    if (strchr(fmt, '\n') == nullptr)
      fprintf(f, "\n");
  } else {
    fprintf(f, "\n");
  }

#ifndef NO_THREADS
  mutex.unlock();
#endif
}


char * Logger::log_str(const char * Level, const char * Location, int Line, const char * fmt, va_list * ap, int varlength) {
struct tm tt; // = *localtime(&t);
struct timeval tv;
const char * pSpace;
int          length;
char       * ret;

  if (Location == nullptr) {
    pSpace = "---";
  } else {
    pSpace = strrchr(Location, ' ');
    if (pSpace != nullptr)
      pSpace++;
    else
      pSpace = Location;
  }
  gettimeofday(&tv, nullptr);
  tt = *localtime(&tv.tv_sec);

  length = snprintf(nullptr, 0, "%04i-%02i-%02i %02i:%02i:%02i.%03i %s : %s[%i] ", tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, (int)(tv.tv_usec / 1000), Level == nullptr ? "LOG:   " : Level, pSpace, Line);
  ret = (char *)malloc(length + varlength + 2);
  sprintf(ret, "%04i-%02i-%02i %02i:%02i:%02i.%03i %s : %s[%i] ", tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, (int)(tv.tv_usec / 1000), Level == nullptr ? "LOG:   " : Level, pSpace, Line);
  if (fmt != nullptr) {
    vsprintf(ret + length, fmt, *ap);
  }
  return ret;
}


void  Logger::error(const char * Location, const char * fmt, ...) {
  if (level <= ERROR) {
    va_list ap;
    va_start(ap, fmt);
    log("ERROR", Location, fmt, &ap);
    va_end(ap);
  }
}

void  Logger::error(const char * Location, int Line, const char * fmt, ...) {
  if (level <= ERROR) {
    va_list ap;
    va_start(ap, fmt);
    log("ERROR", Location, Line, fmt, &ap);
    va_end(ap);
  }
}

char * Logger::error_str(const char * Location, int Line, const char * fmt, ...) {
char * ret = nullptr;
int varlength = 0;

  if (level <= ERROR) {
    va_list ap;
    if (fmt != nullptr) {
      va_start(ap, fmt);
      varlength = vsnprintf(nullptr, 0, fmt, ap);
      va_end(ap);
    }
    va_start(ap, fmt);
    ret = log_str("ERROR", Location, Line, fmt, &ap, varlength);
    va_end(ap);
  }
  return ret;
}

void  Logger::warning(const char * Location, const char * fmt, ...) {
  if (level <= WARNING) {
    va_list ap;
    va_start(ap, fmt);
    log("WARNING", Location, fmt, &ap);
    va_end(ap);
  }
}

void  Logger::warning(const char * Location, int Line, const char * fmt, ...) {
  if (level <= WARNING) {
    va_list ap;
    va_start(ap, fmt);
    log("WARNING", Location, Line, fmt, &ap);
    va_end(ap);
  }
}

char * Logger::warning_str(const char * Location, int Line, const char * fmt, ...) {
char * ret = nullptr;
int varlength = 0;

  if (level <= WARNING) {
    va_list ap;
    if (fmt != nullptr) {
      va_start(ap, fmt);
      varlength = vsnprintf(nullptr, 0, fmt, ap);
      va_end(ap);
    }
    va_start(ap, fmt);
    ret = log_str("WARNING", Location, Line, fmt, &ap, varlength);
    va_end(ap);
  }
  return ret;
}

void  Logger::info(const char * Location, const char * fmt, ...) {
  if (level <= INFO) {
    va_list ap;
    va_start(ap, fmt);
    log("INFO ", Location, fmt, &ap);
    va_end(ap);
  }
}

void  Logger::info(const char * Location, int Line, const char * fmt, ...) {
  if (level <= INFO) {
    va_list ap;
    va_start(ap, fmt);
    log("INFO ", Location, Line, fmt, &ap);
    va_end(ap);
  }
}

char * Logger::info_str(const char * Location, int Line, const char * fmt, ...) {
char * ret = nullptr;
int varlength = 0;

  if (level <= INFO) {
    va_list ap;
    if (fmt != nullptr) {
      va_start(ap, fmt);
      varlength = vsnprintf(nullptr, 0, fmt, ap);
      va_end(ap);
    }
    va_start(ap, fmt);
    ret = log_str("INFO ", Location, Line, fmt, &ap, varlength);
    va_end(ap);
  }
  return ret;
}

void  Logger::debug(const char * Location, const char * fmt, ...) {
  if (level <= DEBUG) {
    va_list ap;
    va_start(ap, fmt);
    log("DEBUG", Location, fmt, &ap);
    va_end(ap);
  }
}

void  Logger::debug(const char * Location, int Line,const char * fmt, ...) {
  if (level <= DEBUG) {
    va_list ap;
    va_start(ap, fmt);
    log("DEBUG", Location, Line, fmt, &ap);
    va_end(ap);
  }
}

char * Logger::debug_str(const char * Location, int Line,const char * fmt, ...) {
char * ret = nullptr;
int varlength = 0;

  if (level <= DEBUG) {
    va_list ap;
    if (fmt != nullptr) {
      va_start(ap, fmt);
      varlength = vsnprintf(nullptr, 0, fmt, ap);
      va_end(ap);
    }
    va_start(ap, fmt);
    ret = log_str("DEBUG", Location, Line, fmt, &ap, varlength);
    va_end(ap);
  }
  return ret;
}
