#ifndef _OPERATOR_H_
#define _OPERATOR_H_

#include <stdarg.h>
#include <stdio.h>

typedef enum
{
  operator_math_add,
  operator_math_sub,
  operator_math_mul,
  operator_math_div,
  operator_math_asig,
} operator_math_t;

typedef enum
{
  operator_cmp_lt,
  operator_cmp_le,
  operator_cmp_gt,
  operator_cmp_ge,
  operator_cmp_eq,
  operator_cmp_ne,
  operator_cmp_bi,
  operator_cmp_et,
  operator_cmp_last
} operator_cmp_t;

operator_cmp_t operator_cmp_revert( operator_cmp_t cmp );
char           operator_math_to_char( operator_math_t math_op );
const char*    operator_math_to_cstr( operator_math_t math_op );


#endif