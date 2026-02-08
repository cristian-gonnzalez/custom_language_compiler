#include "token.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>


extern symtab_t _symtab;

const char* token_type_to_cstr( token_type_t t )
{
  switch( t )
  {
    case token_type_id : 
    {
      return "id";
    }
    case token_type_cte : 
    {
      return "cte";
    }
    case token_type_cte_s : 
    {
      return "cte_s";
    }
    case token_type_take : 
    {
      return "take";
    }
    case token_type_write : 
    {
      return "write";
    }
    case token_type_read : 
    {
      return "read";
    }
    case token_type_assig : 
    {
      return "=";
    }
    case token_type_add : 
    {
      return "+";
    }
    case token_type_if : 
    {
      return "if";
    }
    case token_type_if_then : 
    {
      return "then";
    }
    case token_type_cmp_le : 
    {
      return "<=";
    }
    case token_type_err : 
    {
      return "msg_err";
    }
    case token_type_semi_colon : 
    {
      return ";";
    }
    default:
      perror( "Unkown type" );
  }

  return NULL;
}

void token_print( token_t* t,  FILE* fp )
{
  switch( t->type )
  {
    case token_type_id: 
    case token_type_cte: 
    case token_type_cte_s: 
    {
      fprintf(fp, "%s" , _symtab.table[ t->symtab_id ].name );
      break;
    }
    case token_type_take: 
    case token_type_write: 
    case token_type_read: 
    case token_type_assig: 
    case token_type_add: 
    case token_type_if: 
    case token_type_if_then: 
    case token_type_cmp_le: 
    case token_type_err: 
    case token_type_semi_colon:
    {
      fprintf(fp, "%s", token_type_to_cstr(t->type) );
      break;
    }
    default:
    {
      fprintf(stderr, "Unknown token type: %d\n", t->type);
      break;
    }
  }
}



token_t* token_new( token_type_t token_type, int symtab_id)
{
    token_t* new = (token_t*) malloc( sizeof( token_t ) );
    new->type = token_type;
    new->symtab_id = symtab_id;
    
   return new;
}


void token_delete( token_t* t)
{
  if( t )
    free(t);
}