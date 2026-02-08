%{

/* include area */

#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <stdio.h>

#include "datatype.h"
#include "symtab.h"
#include "compiler.tab.h"

 
/*==================================================================================================
 * Configuration
 *==================================================================================================*/

/** Enables the loggin in current module. */
#define LEX_LOGGER 1

/*==================================================================================================
 * Useful macros
 *==================================================================================================*/

#if LEX_LOGGER
  #define LEX_LOG(...) LOG( __VA_ARGS__ )
#else
  #define LEX_LOG(...) log_nothing( __FILE__, __LINE__, __VA_ARGS__ )
#endif

/*==================================================================================================
 * Data
 *==================================================================================================*/


extern symtab_t _symtab;

/*==================================================================================================
 * Function declaration
 *==================================================================================================*/

int yylex();
void yyerror( const char* fmt, ... );

void int_check_bound( int integer );
void string_check_bound( char* string );

%}

%option noyywrap
%option yylineno

%x STR_CONST

DIGIT                     [0-9]
CHAR                      [a-zA-Z]        

CTE                       [1-9]{DIGIT}{0,4}|0
CTE_S                     ^"[[:alnum:]]+"$

ID                        {CHAR}[A-Za-z_0-9]*


ignora                    " "|\t|\n
NEW_LINE                  [\r\n]
WHITE_SPACE               [ \t]
SPECIAL_CHAR              [^\t\^\¿\\\r\n\"\~\?\¥\®\@\∞\¨]+

%%                         

{ignora}                  {;}

"write"                   { LEX_LOG("token WRITE"); return WRITE; }
"read"                    { LEX_LOG("token READ"); return READ; }
"take"                    { LEX_LOG("token TAKE"); return TAKE; }

";"                       { LEX_LOG("token SEMI_COLON"); return SEMI_COLON; }
","                       { LEX_LOG("token COMA"); return COMA; }

"="                       { LEX_LOG("token ASSIG_OP"); return ASSIG_OP; }
"+"                       { LEX_LOG("token ADD_OP"); return ADD_OP; }
                           
"("                       { LEX_LOG("token LP"); return LP; }
")"                       { LEX_LOG("token RP"); return RP; }
"["                       { LEX_LOG("token LB"); return LB; }
"]"                       { LEX_LOG("token RB"); return RB; }

{NEW_LINE}                { /*ignoring new lines*/ }
{WHITE_SPACE}             { /*ignoring white spaces*/ }
                              
{ID}                      { 
                            if (strlen(yytext) + 1 > SYMBOL_BUFFER_SIZE) {
                                yyerror("Identifier too long: %s", yytext);
                                exit(-1);
                            }

                            /* checks if the tokens exists in the symbol table. If not, adds it. */
                            if( symtab_find_by_name( &_symtab, yytext ) == -1 )
                            {
                                symbol_t s = {
                                  .token_type = token_type_id,
                                  .id_type = id_type_unknown
                                };

                                snprintf(s.name, sizeof(s.name), "%s", yytext);
                                s.value[0] = '\0';
                                
                                if( !symtab_add( &_symtab, &s ) )
                                {
                                  yyerror("Failed to add token ID:[%s]", yytext );  
                                  exit( -1 );
                                }
                            }
                            
                            LEX_LOG("token ID:[%s]", yytext );  
                            
                            
                            yylval.str = yytext;

                            return ID;
                          }

{CTE}                     { 

                            int value = atoi(yytext);
                            int_check_bound(value);

                            char name[SYMBOL_BUFFER_SIZE];
                            char val[SYMBOL_BUFFER_SIZE];

                            snprintf(name, sizeof(name), "_%s", yytext);
                            snprintf(val, sizeof(val), "%s.0", yytext);
              
                            /* checks if the tokens exists in the symbol table. If not, adds it. */
                            if( symtab_find_by_name( &_symtab, name ) == -1)
                            {
                              symbol_t s = {
                                .token_type = token_type_cte,
                                .length = strlen(val)
                              };
                              
                              snprintf(s.name, sizeof(s.name), "%s", name);
                              snprintf(s.value, sizeof(s.value), "%s", val);

                              if( !symtab_add( &_symtab, &s ) )
                              {
                                yyerror("Failed to add token cte:[%s]", yytext );  
                                exit( -1 );
                              }
                            }
                            
                            LEX_LOG("token CTE:[%s] -> {'name': %s, 'value': '%s'}", yytext, name, val );  
                            
                            // pass the pointer to the parser
                            yylval.str = yytext;
                            return CTE; 
                          }


\"                        {  BEGIN(STR_CONST); }  

<STR_CONST>\"             {                        
                            LEX_LOG("CTE_S"); 
                            BEGIN(INITIAL);
                            return CTE_S; 
                          }

<STR_CONST>{SPECIAL_CHAR} {
                            // NOTE: We are allocating heap memory with strdup but that memory belongs to the parser now.
                            // The parser action that consumes the value $1, $2, etc. must free it
                            yylval.str = strdup(yytext);
                            string_check_bound(yytext); 
                            
                            /* checks if the tokens exists in the symbol table. If not, adds it. */
                            if( symtab_add_cte_s( &_symtab, yytext ) < 0 )
                              yyerror("Error when adding token CTE_S:[%s]\n", yytext );  
                            
                            LEX_LOG("Token CTE_S:[%s]\n", yytext );
                          }

<<EOF>>                   { return 0; }
%%


void int_check_bound( int integer ) 
{
   char strError[200];
  
  /* Check for the maximum signed integer number 32767 for 16bits; Check for the minimum signed integer number -32768 for 16bits*/
  if( integer < SHRT_MIN || integer > SHRT_MAX || integer == -1 )  {
    memset( strError, 0,  sizeof(strError));
    if (integer != -1)
      sprintf(strError,"\nInteger '%d' out of bounds. Values is between %d and %d \n", integer, SHRT_MIN, SHRT_MAX );
    else
      sprintf(strError,"\nInteger out of bounds. Values is between %d and %d \n", SHRT_MIN, SHRT_MAX );
    yyerror(strError);
  }

}

void string_check_bound( char* string ) 
{
  int count = 0;
  int i = 0;
  for( ; string[i] != 0 ; i++ ) 
  {
    if ( string[i] != '"' ) 
      count++;
    
    if ( count > STRING_MAX_LENGTH ) 
    {
      yyerror( "\nString '%s' out of bounds", string );
    }
  }
}
