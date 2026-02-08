/* file: symtab.c
 * Symbol Table module - Interface.
 */
#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <stdlib.h>
#include <stdbool.h>
#include "datatype.h"

/** The maximun number of elements. */
#define SYMTAB_MAX_ELEMS 1000
#define SYMBOL_BUFFER_SIZE 1024


/** Element symbol. */
typedef struct
{
  /* Indicates the type. */
  token_type_t token_type;

  /* Indicates the type. */
  id_type_t id_type;

  /* Token's lexema value. */
  char name[SYMBOL_BUFFER_SIZE];

  /* Value of the variable. */
  char value[SYMBOL_BUFFER_SIZE];

  /** Length of string (Just for string). */
  size_t length;

} symbol_t;


/** Symbol table. */
typedef struct
{
  /** Table. */
  symbol_t table[SYMTAB_MAX_ELEMS];

  /** Number of elements in the table. */
  size_t num_elems;

} symtab_t;


extern symtab_t _symtab;

void symtab_init( symtab_t* table );
void symtab_uninit( symtab_t* table );

bool symtab_add( symtab_t* table, symbol_t* smb );
bool symtab_update( symtab_t* table, symbol_t* smb, int pos );
bool symtab_get( symtab_t* table, symbol_t* smb, int pos );
int  symtab_find_by_name( symtab_t* st, const char* name );
int  symtab_find_by_value( symtab_t* st, const char* value );
void symtab_print( symtab_t* st );
void symtab_generate_file_ts( symtab_t* st );
symbol_t* symtab_symbol_by_id( const char* id_name );

int symtab_add_cte( symtab_t* st, int cte );
int symtab_new_id_aux( symtab_t* st, id_type_t id_type );

int symtab_add_id( symtab_t* st, const char* name, id_type_t id_type );
int symtab_add_cte_s( symtab_t* st, const char* cte );

#endif
