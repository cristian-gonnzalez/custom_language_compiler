%{

/* include area */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "datatype.h"
#include "symtab.h"
#include "node.h"
#include "tree_dump.h"
#include "assembler.h"

/*==================================================================================================
 * Configuration
 *=================================================================================================*/

/** Enables the logging in the current module. */
#define SIN_LOGGER 1

#if SIN_LOGGER
  #define SIN_LOG(...) LOG( __VA_ARGS__ )
#else
  #define SIN_LOG(...) log_nothing( __FILE__, __LINE__, __VA_ARGS__ )
#endif

/** File pointer to the source file. */
extern FILE* yyin;

/** External variable used to know the line number. */
extern int yylineno;

/** External variable used to know the last lexem. */
extern char* yytext;

int yystopparser=0;


/*==================================================================================================
 * Internal data
 *=================================================================================================*/

int yylex();
void yyerror(const char* fmt, ... );

symtab_t _symtab;

int take_num;
int take_cte_list[100];
int take_cte_num = 0;

node_t* program_ptr, 
      * sentences_ptr, 
      * assig_ptr, 
      * read_ptr, 
      * take_ptr, 
      * lista_ptr, 
      * write_ptr;

/*==================================================================================================
 * Internal function declaration
 *=================================================================================================*/

static node_t* _make_node( token_type_t token_type, 
                           int symtab_id )
{
  token_t* t = token_new( token_type, symtab_id );
  return node_new( t );             
}

static void _tree_destroy(node_t* root)
{
    if (!root)
        return;

    _tree_destroy(root->left);
    _tree_destroy(root->right);

    if (root->data)
        token_delete(root->data);

    node_delete(root);
}

%}
%union 
{
  int   num;
  char* str;
}

%token <str> ID 
 
%token <str> CTE 
%token <str> CTE_S

%token <num> TAKE

%token <num> WRITE
%token <num> READ 

%token <num> ASSIG_OP 
%token <num> ADD_OP 

%token <str> COMA
%token <str> COLON
%token <str> SEMI_COLON


/* Left and right parenthesis () */
%token <str> LP
%token <str> RP

/* Left and right brackets [] */
%token <str> LB
%token <str> RB

/* Se utiliza para el intermedio */

%start program
%%

program:    sentences 
            {
                program_ptr = sentences_ptr;
                SIN_LOG("Success\n");  
            }
            | program sentences 
            { 
              node_t* semicolon_ptr = _make_node( token_type_semi_colon, -1 );

              //                ;
              //               / \
              //            prog  sentences 
              semicolon_ptr->left = program_ptr;
              semicolon_ptr->right = sentences_ptr;

              program_ptr = semicolon_ptr;
            
              SIN_LOG("program sentences"); 
            }
		        ;

sentences:  assig
            {  
              sentences_ptr = assig_ptr;
              SIN_LOG("assig"); 
            }
            | read
            {
              sentences_ptr = read_ptr;
              SIN_LOG("read");   
            }
            | write
            {  
              sentences_ptr = write_ptr;
              SIN_LOG("write");  
            }
            ;

assig:      ID ASSIG_OP take
            {
              int pos = symtab_find_by_name( &_symtab, $1 );

              if ( _symtab.table[ pos ].id_type == id_type_string )
                yyerror("'%s' is string", $1);
              
              assig_ptr = _make_node( token_type_assig, -1 );              
              node_t* id_ptr = _make_node( token_type_id, pos );
              
              take_ptr->jump_label = assig_ptr->label;
            
              //                =
              //               / \
              //             id  take 
              assig_ptr->left = id_ptr;
              assig_ptr->right = take_ptr;

              free( $1 );
            }
            ;

read:       READ ID 
            {              
              int pos = symtab_find_by_name( &_symtab, $2 );

              read_ptr = _make_node( token_type_read, -1 );              
              node_t* id_ptr = _make_node( token_type_id, pos );
              
              //              read
              //               / 
              //             id   
              read_ptr->left = id_ptr;
            
              free( $2 );
            }
            ;

take:       TAKE { take_cte_num = 0;}  LP ADD_OP SEMI_COLON ID  SEMI_COLON LB  lista RB RP
            { 
              int n_pos = symtab_find_by_name( &_symtab, $6 );

              // number of cte in the list
              int cte_num_pos = symtab_add_cte( &_symtab, take_cte_num );

              // variable to save the result
              int result_pos = symtab_add_id( &_symtab, "_take_result", id_type_numeric );
              //node_t* aux_ptr = _make_node( token_type_id, result_pos );
              
              /* take( +, n, [cte1, cte2, ..] )*/

              //         take
              //     /         \
              //  take_root   NULL
              node_t* new_take = _make_node( token_type_take, -1 );
              node_t* take_root = _make_node( token_type_semi_colon, -1 );
              
              new_take->left = take_root;

              take_ptr = new_take;
 
              
              //      if
              //     /  \
              //    <=   if_then
              node_t* if_ptr = _make_node( token_type_if, -1 );
              node_t* if_then_ptr = _make_node( token_type_if_then, -1 );
              node_t* cmp_ptr = _make_node( token_type_cmp_le, -1 );
              

              cmp_ptr->jump_label = if_then_ptr->label;
              
              if_ptr->left = cmp_ptr;
              if_ptr->right = if_then_ptr;
              

              //      take_root
              //     /  \
              //    =   if
              //       /  \
              //      <=   if_then
              
              node_t* assig_zero = _make_node( token_type_assig, -1 );              
              
              take_root->left = assig_zero;
              take_root->right = if_ptr;
              
              //      take_root
              //      /    \
              //     =       if
              //  /    \     /  \
              //result  0   <=   if_then
              int zero_pos = symtab_add_cte( &_symtab, 0 );
              node_t* cte_zero = _make_node( token_type_cte, zero_pos );  

              assig_zero->left = _make_node( token_type_id, result_pos );
              assig_zero->right = cte_zero;
                          
              
              node_t* err_ptr = _make_node( token_type_err, -1 );
              
              //      <=
              //     /  \
              //    id   N
              node_t* id_n = _make_node( token_type_id, n_pos );
              node_t* cte_num = _make_node( token_type_cte, cte_num_pos );
              
              cmp_ptr->left = id_n;
              cmp_ptr->right = cte_num;

              node_t* root_if = NULL; 
              node_t* prev_if = NULL;

              for( size_t i=0; i<take_cte_num; i++ )
              {
                int i_pos = symtab_add_cte( &_symtab, (i + 1) );

                node_t* new_if = _make_node( token_type_if, -1 );
 
                if( prev_if )
                {
                  node_t* semicolon_ptr = _make_node( token_type_semi_colon, -1 );
                  node_t* aux = prev_if->right;
                  prev_if->right = semicolon_ptr;

                  semicolon_ptr->left = aux;
                  semicolon_ptr->right = new_if;
                }

                if( root_if == NULL )
                  root_if = new_if;

                //      ( if )
                //       /  \
                //      <=   NULL                
                node_t* new_cmp = _make_node( token_type_cmp_le, -1 );
                new_cmp->jump_label = new_take->label;

                new_if->left = new_cmp;
                new_if->right = NULL;

                //        if
                //       /  \
                //    ( <= )   NULL
                //     /  \
                //    id   i
                node_t* new_n = _make_node( token_type_id, n_pos );
                node_t* new_cte_count = _make_node( token_type_cte, i_pos );
                
                new_cmp->left = new_n;
                new_cmp->right = new_cte_count;

                node_t* new_assig = _make_node( token_type_assig, -1 );
                node_t* new_add = _make_node( token_type_add, -1 );
                node_t* new_cte = _make_node( token_type_cte, take_cte_list[ i ] );
                
                //            ( = ) 
                //            /   \
                //           aux   ( + )
                //                /   \
                //              aux   cte
                
                new_assig->left = _make_node( token_type_id, result_pos );
                new_assig->right = new_add;

                new_add->left = _make_node( token_type_id, result_pos );
                new_add->right = new_cte;


                //          if
                //       /     \
                //    ( <= )     =
                //     /  \     /   \
                //    id   i   aux   ( + )
                //                    /   \
                //                  aux   cte
                new_if->right = new_assig;

                prev_if = new_if;
              }

              if_then_ptr->left = root_if;
              if_then_ptr->right = err_ptr;


              if_then_ptr->jump_label = new_take->label;
            
            }
            ;

lista:      CTE
            {
              SIN_LOG("take:  cte [%s]", $1); 
             
              int cte = atoi( $1 );

              take_cte_list[ take_cte_num ] = symtab_add_cte( &_symtab, cte );
              take_cte_num++;      

              free($1);
            }
            | lista COMA CTE 
            {              
              SIN_LOG("take: lista, cte [%s]", $3);  

              int cte = atoi( $3 );
              
              take_cte_list[ take_cte_num ] = symtab_add_cte( &_symtab, cte );
              take_cte_num++;   
              free($3);            
            };

write:      WRITE CTE_S 
            {
              int pos = symtab_add_cte_s( &_symtab, $2 );
              // NOTE: This value was passed with strdup so we must free it
              free($2);

              node_t* cte_s_ptr = _make_node( token_type_cte_s, pos );
              write_ptr = _make_node( token_type_write, -1 );              

              write_ptr->left = cte_s_ptr;

              SIN_LOG("write cte_s [%d-%s]", pos, _symtab.table[ pos ].name );  
            }
            | WRITE ID 
            {
              int pos = symtab_find_by_name( &_symtab, $2 );

              write_ptr = _make_node( token_type_write, -1 );              
              node_t* id_ptr = _make_node( token_type_id, pos );
   
              write_ptr->left = id_ptr;
              write_ptr->right = NULL;
           
              SIN_LOG("write id"); 
              
              free( $2 );
            };


%%
/*==================================== MAIN ====================================*/
int main( int argc ,char* argv[] ) 
{
  if(argc < 2)
  {
    fprintf(stderr, "Usage: %s src_file\n", argv[0] );
    return -1;
  }

  const char* file= argv[1];

  SIN_LOG("Opening '%s'", file);

  yyin = fopen(file,"rt+");
  if( yyin == NULL)
  {
    fprintf(stderr, "Fail to open src file '%s'", argv[1] );
    return -1;
  }

  symtab_init( &_symtab );

  SIN_LOG("Start compilation");

  yyparse();

  fclose(yyin);

  symtab_print( &_symtab );
  symtab_generate_file_ts( &_symtab );
  
  tree_dump_file( program_ptr);
  asm_generate( program_ptr );

  _tree_destroy(program_ptr);
  symtab_uninit( &_symtab );

  SIN_LOG("Compilation end");
  
  return 0;
}


int yywhere()
{   
  int colon=0;
  int i;

  if(yytext) 
  { 
      for( i = 0; i < 50; i++ ) 
       if( !yytext[i] || yytext[i] == '\n' ) 
          break;
      
      if( i )
      {  
          if( !colon )  
          { 
              printf( "error near '%s'. ", yytext );
              colon = 1; 
          }
      }  
  }  
}   


static void _print_err( const char* fmt, va_list args )
{
  printf("\nline n#%d: ", yylineno );
  yywhere(); 
  vprintf( fmt, args );
  printf("\n" );
}


void yyerror( const char* fmt, ... )
{ 
  va_list args;
  va_start(args, fmt);
  
  _print_err( fmt, args );

  va_end(args);

  exit(-1);
}
