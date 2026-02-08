#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <stdio.h>

typedef enum s_token_type
{
  token_type_id,
  token_type_cte,
  token_type_cte_s,
  token_type_take,
  token_type_write,
  token_type_read,
  token_type_assig,
  token_type_add,
  token_type_if,
  token_type_if_then,
  token_type_cmp_le,
  token_type_err,
  token_type_semi_colon
}token_type_t;

  
/** Element symbol. */
typedef struct s_token_t
{
  token_type_t type;

  int symtab_id;

} token_t;


token_t* token_new( token_type_t token_type, int symtab_id);
void     token_delete( token_t* t);
void     token_print( token_t* t,  FILE* fp );

const char* token_type_to_cstr( token_type_t t );

#endif 
