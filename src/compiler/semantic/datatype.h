#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include <stdarg.h>
#include <stdio.h>
#include "token.h"

/** Maximum number of character allowed. */
#define CHAR_MAXIMUM 30

/** Maximum number of character allowed. */
#define STRING_MAX_LENGTH 1250

#define LOG( ... ) log_bla( __FILE__, __LINE__, __VA_ARGS__ )

typedef enum
{
  id_type_unknown,
  id_type_numeric,
  id_type_string
} id_type_t;

void log_bla( const char* file,
          int line,
          const char* fmt,
          ... );
void log_nothing( const char* file,
          int line,
          const char* fmt,
          ... );

#endif