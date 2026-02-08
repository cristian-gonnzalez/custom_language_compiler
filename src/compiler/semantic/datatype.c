#include "datatype.h"
#include <libgen.h>
#include <stdlib.h>


void log_bla( const char* file,
          int line,
          const char* fmt,
          ... )
{
  /* gets the varargs */
  va_list args;
  va_start( args, fmt );

  fflush(NULL);
  printf("\n%s:%d - ",  basename( (char*)file ), line );
  vprintf(fmt, args);
  fflush(NULL);

  va_end( args );
}


void log_nothing( const char* file,
          int line,
          const char* fmt,
          ... )
{
}


