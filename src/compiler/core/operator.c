#include "operator.h"

#include "datatype.h"
#include <libgen.h>
#include <stdlib.h>


const char* operator_math_to_cstr( operator_math_t math_op )
{
  switch( math_op )
  {
    case operator_math_add:
    {
      return "add";
    }
    case operator_math_sub:
    {
      return "sub";
    }
    case operator_math_mul:
    {
      return "mult";
    }
    case operator_math_div:
    {
      return "div";
    }
    case operator_math_asig:
    {
      return "assign";
    }
  }
  return "???";
}


char operator_math_to_char( operator_math_t math_op )
{
  switch( math_op )
  {
    case operator_math_add:
    {
      return '+';
    }
    case operator_math_sub:
    {
      return '-';
    }
    case operator_math_mul:
    {
      return '*';
    }
    case operator_math_div:
    {
      return '/';
    }
    case operator_math_asig:
    {
      return '=';
    }
  }
  return '?';
}

operator_cmp_t operator_cmp_revert( operator_cmp_t cmp )
{
  switch( cmp )
  {
    case operator_cmp_lt:
    {
      return operator_cmp_ge;
    };
    case operator_cmp_le:
    {
      return operator_cmp_gt;
    };
    case operator_cmp_gt:
    {
      return operator_cmp_le;
    };
    case operator_cmp_ge:
    {
      return operator_cmp_lt;
    };
    case operator_cmp_eq:
    {
      return operator_cmp_ne;
    };
    case operator_cmp_ne:
    {
      return operator_cmp_eq;
    };
    case operator_cmp_bi:
    {
      break;
    };
    case operator_cmp_last:
    {
      break;
    };
  }

  return operator_cmp_last;
}