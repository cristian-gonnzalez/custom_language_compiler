#include "assembler.h"
#include <stdio.h>

#include "tree_dump.h"
#include "symtab.h"
#include "operator.h"

extern symtab_t _symtab;

char* tab_1_column = "\t\t\t\t\t";

static void _asm_free( FILE* fp );
static void asm_string_rutines( FILE* final );
static void asm_generate_code( FILE* final );
static void asm_tree_code_generate( node_t* root);

static void asm_start( FILE* final )
{
  // Declare the hearders 
  fprintf( final, "include  macros2.asm  ; Include macros\n" );
  fprintf( final, "include  number.asm   ; Include the asm to print numbers\n\n" );

  fprintf( final, ".model large        ; Memory model.\n" );
  fprintf( final, ".stack 200h         ; Bytes in the stack.\n" );
  fprintf( final, ".386                ; Proccessor type.\n\n\n" );
}
//-----------------------------------------------------------------------------

static void save_ts_data( FILE* final )
{
  int i;
  fprintf( final, ".data \t\t; Data section.\n\n" );
  fprintf( final, "%s;======> Program variables <======\n", tab_1_column );
  fprintf( final, "%sMAXTEXTSIZE \t\t\tequ \t\t\t%i\n", tab_1_column, CHAR_MAXIMUM );

  for( i = 0; i < _symtab.num_elems; i++ )
  {
    if( _symtab.table[i].token_type == token_type_id  )
    {
      fprintf( final, "%s%s\t\t\t\t\tdd\t\t\t?\n", tab_1_column, _symtab.table[i].name );
    }

    if( _symtab.table[i].token_type == token_type_cte_s )
    {
      fprintf( final,
               "%s%s\t\t\t\t\tdb\t\t\tMAXTEXTSIZE dup (?),'$'\n",
               tab_1_column,
               _symtab.table[i].name );
    }

    if( _symtab.table[i].token_type == token_type_cte )
    {
      fprintf( final,
               "%s%s\t\t\t\t\tdd\t\t\t%s\n",
               tab_1_column,
               _symtab.table[i].name,
               _symtab.table[i].value );
    }
  }

  fprintf( final, "%s__ENTER\t\t\t\tdb\t\t\t0Dh,0Ah,'$'\n", tab_1_column );

  fprintf( final, "%s_AUXPrintReal db 14 dup(?), '$'\n", tab_1_column );
  fprintf( final, "\n" );

}

static void generate_code_header( FILE* final )
{
  // Inicializar el segmento de datos
  fprintf( final, "\n%s;=====> Code segment start <====\n", tab_1_column );
  fprintf( final, ".code\n" );

  fprintf( final, "begin:\t\t\t.startup\n" );
  // Inicializar el co-procesador
  // Guardar en el registro de segmento DS y ES, la posición del segmento de datos.
  fprintf( final, "%smov AX,@DATA    ; Code segment\n", tab_1_column );
  fprintf( final, "%smov DS,AX       ; Code segment\n", tab_1_column );
  fprintf( final, "%smov ES,AX       ; Code segment\n", tab_1_column );
  // Inicializar el co-procesador
  fprintf( final, "%sfinit           ; Initialize the co-processor\n", tab_1_column );
}

//-----------------------------------------------------------------------------
void asm_end( FILE* fp )
{

  fprintf( fp, "LabelEndTreeCode:\n" );
  fprintf( fp, "\n%smov ah, 0\n", tab_1_column );
  fprintf( fp, "%sint 16h\n", tab_1_column );
  fprintf( fp, "%s;=======> End the execution <=======\n", tab_1_column );
  fprintf( fp, "%smov AX, 4C00h   ; Return the execution to DOS.\n", tab_1_column );
  fprintf( fp, "%sint 21h\n", tab_1_column );
  asm_string_rutines( fp );
  fprintf( fp, "END   begin    ; End of the file\n" );
}


static void _copy_code_section( FILE* target )
{
  FILE* src = fopen( "code.asm", "r" );
  if( src == NULL )
  {
    printf( "Cannot open file %s \n", "code.asm" );
    exit( 0 );
  }

  // Read contents from file
  char c = fgetc( src );
  while( c != EOF )
  {
    fputc( c, target );
    c = fgetc( src );
  }
  fclose( src );
}

void asm_generate(node_t* node )
{
  asm_tree_code_generate(node);

  FILE* fp = fopen( "final.asm", "wt" );
  if( fp == NULL )
  {
    printf( "Failed to open final.asm\n" );
    system( "PAUSE" );
    exit( 1 );
  }

  printf( "Generating asm...\n" );

  asm_start( fp );
  save_ts_data( fp );
  generate_code_header( fp );
  _copy_code_section( fp );
  asm_end( fp );

  printf( "asm is completed.\n" );

  fclose( fp );
}


static void asm_add( node_t* n, FILE* fp )
{

  int result_pos = symtab_add_id( &_symtab, "_asm_aux_result", id_type_numeric );
              
  token_t* left_ptr = (token_t*)n->left->data;
  token_t* right_ptr = (token_t*)n->right->data;


  fprintf( fp,
           "\n%s;==============> %s <================\n",
           tab_1_column,
           operator_math_to_cstr( operator_math_add ) );
  fprintf( fp, "Label%d:\n", n->label );

    // loads operands
    fprintf(
      fp, "%sfld\t\t%s\n", tab_1_column, _symtab.table[ left_ptr->symtab_id ].name );
    fprintf(
      fp, "%sfstp %s\n", tab_1_column, _symtab.table[ right_ptr->symtab_id ].name );
  

  fprintf( fp, "%sfadd\n", tab_1_column );
  fprintf( fp, "%sfstp\t%s\n", tab_1_column, _symtab.table[ result_pos ].name );
  fprintf( fp, "\n" );

  _asm_free( fp );

  fprintf( fp, "\n" );

  
  token_t* node_data = (token_t*)n->data;

  node_data->type = token_type_id;
  node_data->symtab_id = result_pos;
}


static void asm_assing( node_t* n, FILE* fp )
{
  token_t* op_ptr = (token_t*)n->data;
  token_t* left_ptr = (token_t*)n->left->data;
  token_t* right_ptr = (token_t*)n->right->data;
  

  fprintf( fp,
           "\n%s;==============> %s <================\n",
           tab_1_column,
           operator_math_to_cstr( operator_math_asig ) );
  fprintf( fp, "Label%d:\n", n->label );

    // loads operands
    fprintf(
      fp, "%sfld\t\t%s\n", tab_1_column, _symtab.table[ left_ptr->symtab_id ].name );
    fprintf(
      fp, "%sfstp %s\n", tab_1_column, _symtab.table[ right_ptr->symtab_id ].name );
  
  fprintf( fp, "\n" );

  _asm_free( fp );
}


void node_to_asm( node_t* n ,FILE* fp )
{
  token_t* token_ptr = (token_t*)n->data;

  switch(  token_ptr->type)
  { 
    case token_type_write: 
    case token_type_read: 
    {
      node_t* left = n->left;

      token_t* op_ptr = (token_t*)left->data;

      fprintf( fp,
               "\n%s;===============> %s <================\n",
               tab_1_column,
               ( token_ptr->type == token_type_read ? "read" : "write" ) );
      fprintf( fp, "Label%d:\n", n->label );

      switch( op_ptr->type )
      {
        case token_type_id : 
        case token_type_cte : 
        {
          if( token_ptr->type == token_type_read )
            fprintf( fp, 
                     "%sgetFloat \t\t%s\n", 
                     tab_1_column, 
                     _symtab.table[ op_ptr->symtab_id ].name );
          else
            fprintf( fp,
                     "%sdisplayFloat \t\t%s\n",
                     tab_1_column,
                     _symtab.table[ op_ptr->symtab_id ].name );
          break;
        }
        case token_type_cte_s : 
        {

          if( token_ptr->type == token_type_read )
            fprintf( fp, 
                     "%sgetString \t\t%s\n", 
                     tab_1_column, 
                     _symtab.table[ op_ptr->symtab_id ].name );
          else
            fprintf( fp,
                     "%sdisplayString \t\t%s\n",
                     tab_1_column,
                     _symtab.table[ op_ptr->symtab_id ].name );
          break;
        }
        default:
        {  
          printf( "Un-handled token '%s'", token_type_to_cstr( op_ptr->type ) );
          exit(-1);
        }
      }
      break;
    }
    case token_type_take:
    {
      fprintf( fp,
                    "\n%s;===============> take <================\n",
                    tab_1_column );
      fprintf( fp, "Label%d:", n->label );
      fprintf( fp, "%sjump\t\t", tab_1_column );
      fprintf( fp, "Label%d\n", n->jump_label );   
      break;
    }
    case token_type_id: 
    case token_type_cte: 
    case token_type_cte_s:
    case token_type_semi_colon : 
    {
      break;
    }
    case token_type_assig :
    {
      asm_assing( n, fp);
      break;
    }
    case token_type_add :
    {
      asm_add( n, fp);
      
      break;
    }
    case token_type_if :
    {
      break;
    }
    case token_type_if_then :
    {
      fprintf( fp,
                    "\n%s;===============> end if then <================\n",
                    tab_1_column );
      fprintf( fp, "Label%d:", n->label );
      fprintf( fp, "%sjump\t\t", tab_1_column );
      fprintf( fp, "Label%d\n", n->jump_label );   
      
   
        token_t* left_ptr = (token_t*)n->right->data;
        
        if( left_ptr->type == token_type_err )
        {
           fprintf( fp,
                    "\n%s;===============> error <================\n",
                    tab_1_column );
           fprintf( fp, "Label%d:\n", n->label );

           // adding error message as cte_s
           int err_pos = symtab_add_cte_s( &_symtab, "Not enough cte numbers in the list to apply take" );


           fprintf( fp,
                     "%sdisplayString \t\t%s\n",
                     tab_1_column,
                     _symtab.table[ err_pos ].name );
           fprintf( fp, "%sjump\t\t", tab_1_column );
           fprintf( fp, "LabelEndTreeCode\n" );
        }
      break;
    }
    case token_type_cmp_le :
    {
        token_t* left_ptr = (token_t*)n->left->data;
        token_t* right_ptr = (token_t*)n->right->data;
        
        fprintf( fp, "Label%d:", n->label );
        // loads operands
        fprintf(
          fp, "%sfld\t\t%s\n", tab_1_column, _symtab.table[ left_ptr->symtab_id ].name );
        fprintf(
          fp, "%sfld\t\t%s\n", tab_1_column, _symtab.table[ right_ptr->symtab_id  ].name );

        // Comparamos y enviamos el vector de estado al EFLAG del x86
        fprintf(
          fp, "%sfcompp              ; Compare ST(0) with ST(1), -> STATUS\n", tab_1_column );
        fprintf(
          fp, "%sfstsw AX            ; Save status word in register AX.\n", tab_1_column );
        fprintf( fp, "%sfwait\n", tab_1_column );
        fprintf( fp, "%ssahf                ; Save AX in EFLAGS\n", tab_1_column );

        fprintf( fp, "%sjnbe\t\t", tab_1_column );
        fprintf( fp, "Label%d\n", n->jump_label );
        
        break;
    }
    case token_type_err :
    default:
    {  
      printf( "Un-handled token '%s' on node", token_type_to_cstr( token_ptr->type ) );
      exit(-1);
    }
  }
}


void tree_to_asm(node_t* n, FILE* fp ) 
{
   if( !node_is_node_with_leafs( n ) )
   {
     if(n->left) 
       tree_to_asm( n->left, fp );

     if(n->right) 
       tree_to_asm( n->right, fp );
   }

   if( !node_is_leaf(n) )
   {
      printf("node: [");
      token_print(n->data, stdout);
      printf(" ");
      if(n->left ) {
        printf("l->");
        token_print(n->left->data, stdout);
        printf(" ");
      }
      if(n->right ) {
      
       printf("r->");
       token_print(n->right->data, stdout);
       printf(" ");
      
      }
      printf("]\n");
      
      node_to_asm( n, fp );
   } 
}

// Iterate the tercetos list and generate the code
static void asm_tree_code_generate( node_t* n )
{
  FILE* fp = fopen( "code.asm", "wt" );
  if( fp == NULL )
  {
    printf( "Failed to open code.asm\n" );
    system( "PAUSE" );
    exit( 1 );
  }

  tree_to_asm( n, fp);

  fclose( fp );
}


static void _asm_free( FILE* fp )
{
  fprintf( fp, "\n%s;=> clean stack of co-processor <=\n", tab_1_column );
  fprintf( fp, "%sffree st(0)\n", tab_1_column );
  fprintf( fp, "%sffree st(1)\n", tab_1_column );
  fprintf( fp, "%sffree st(2)\n", tab_1_column );
  fprintf( fp, "%sffree st(3)\n", tab_1_column );
  fprintf( fp, "%sffree st(4)\n", tab_1_column );
  fprintf( fp, "%sffree st(5)\n", tab_1_column );
  fprintf( fp, "%sffree st(6)\n", tab_1_column );
  fprintf( fp, "%sffree st(7)\n", tab_1_column );
  fprintf( fp, "\n" );
}

static void asm_string_rutines( FILE* final )
{
  // Routine to diplay 
  fprintf( final, "\n%s;==========> String routine <==========\n", tab_1_column );
  fprintf( final, "STRPRINT PROC\n" );
  fprintf( final, "%smov AH, 9\n", tab_1_column );
  fprintf( final, "%sint 21h\n", tab_1_column );
  fprintf( final, "%smov DX, OFFSET __ENTER\n", tab_1_column );
  fprintf( final, "%smov AH, 9\n", tab_1_column );
  fprintf( final, "%sint 21h\n", tab_1_column );
  fprintf( final, "%sret\n", tab_1_column );
  fprintf( final, "STRPRINT ENDP\n\n" );

  // Routine to count the number of characters in a string
  fprintf( final, "STRLEN PROC\n" );
  fprintf( final, "%smov bx,0\n", tab_1_column );
  fprintf( final, "STRL01:\n" );
  fprintf( final, "%scmp BYTE PTR [SI+BX],'$'\n", tab_1_column );
  fprintf( final, "%sje STREND\n", tab_1_column );
  fprintf( final, "%sinc BX\n", tab_1_column );
  fprintf( final, "%scmp BX, MAXTEXTSIZE\n", tab_1_column );
  fprintf( final, "%sjl STRL01\n", tab_1_column );
  fprintf( final, "STREND:\n" );
  fprintf( final, "%sret\n", tab_1_column );
  fprintf( final, "STRLEN ENDP\n\n" );

  // Routine to copy a string
  fprintf( final, "STRCOPY PROC\n" );
  fprintf(
    final, "%scall STRLEN                 ; Find the number of characters\n", tab_1_column );
  fprintf( final, "%scmp bx,MAXTEXTSIZE\n", tab_1_column );
  fprintf( final, "%sjle COPYSIZEOK\n", tab_1_column );
  fprintf( final, "%smov bx,MAXTEXTSIZE\n", tab_1_column );
  fprintf( final, "COPYSIZEOK:\n" );
  fprintf(
    final, "%smov cx,bx                   ; la copia se hace de 'CX' caracteres\n", tab_1_column );
  fprintf( final,
           "%scld                         ; cld es para que la copia se realice hacia adelante\n",
           tab_1_column );
  fprintf( final, "%srep movsb                   ; copia la cadea\n", tab_1_column );
  fprintf( final, "%smov al,'$'                  ; carácter terminador\n", tab_1_column );
  fprintf( final,
           "%smov BYTE PTR [DI],al        ; el registro DI quedo apuntando al final\n",
           tab_1_column );
  fprintf( final, "%sret\n", tab_1_column );
  fprintf( final, "STRCOPY ENDP\n\n" );

  fprintf( final, "CONCAT PROC\n" );
  fprintf( final, "%spush ds\n", tab_1_column );
  fprintf( final, "%spush si\n", tab_1_column );
  fprintf( final,
           "%scall STRLEN                 ; busco la cantidad de caracteres del 2do string\n",
           tab_1_column );
  fprintf(
    final, "%smov dx,bx ; guardo en DX la cantidad de caracteres en el origen.\n", tab_1_column );
  fprintf( final, "%smov si,di\n", tab_1_column );
  fprintf( final, "%spush es\n", tab_1_column );
  fprintf( final, "%spop ds\n", tab_1_column );
  fprintf( final, "%scall STRLEN ; tama#o del 1er string\n", tab_1_column );
  fprintf( final, "%sadd di,bx ; DI ya queda apuntando al final del primer string\n", tab_1_column );
  fprintf( final, "%sadd bx,dx ; tama#o total\n", tab_1_column );
  fprintf( final, "%scmp bx,MAXTEXTSIZE ; excede el tama#o maximo?\n", tab_1_column );
  fprintf( final, "%sjg CONCATSIZEMAL\n", tab_1_column );
  fprintf( final, "CONCATSIZEOK: ; La suma no excede el maximo, copio todos\n" );
  fprintf( final, "%smov cx,dx ; los caracteres del segundo string.\n", tab_1_column );
  fprintf( final, "%sjmp CONCATSIGO\n", tab_1_column );
  fprintf( final, "CONCATSIZEMAL: ; La suma de caracteres de los 2 strings exceden el maximo\n" );
  fprintf( final, "%ssub bx,MAXTEXTSIZE\n", tab_1_column );
  fprintf( final, "%ssub dx,bx\n", tab_1_column );
  fprintf( final, "%smov cx,dx ; copio lo maximo permitido el el resto se pierde.\n", tab_1_column );
  fprintf( final, "CONCATSIGO:\n" );
  fprintf( final, "%spush ds\n", tab_1_column );
  fprintf( final, "%spop es\n", tab_1_column );
  fprintf( final, "%spop si\n", tab_1_column );
  fprintf( final, "%spop ds\n", tab_1_column );
  fprintf( final, "%scld ; cld es para que la copia se realice hacia adelante\n", tab_1_column );
  fprintf( final, "%srep movsb ; copia la cadea\n", tab_1_column );
  fprintf( final, "%smov al,'$' ; carácter terminador\n", tab_1_column );
  fprintf(
    final, "%smov BYTE PTR [DI],al ; el registro DI quedo apuntando al final\n", tab_1_column );
  fprintf( final, "%sret\n", tab_1_column );
  fprintf( final, "CONCAT ENDP\n\n" );

  fprintf( final, "STRCMP PROC\n" );
  fprintf( final, "%scall STRLEN\n", tab_1_column );
  fprintf( final, "%smov ax, bx\n", tab_1_column );
  fprintf( final, "%smov cx, si\n", tab_1_column );
  fprintf( final, "%smov si, di\n", tab_1_column );
  fprintf( final, "%scall STRLEN\n", tab_1_column );
  fprintf( final, "%scmp ax, bx\n", tab_1_column );
  fprintf( final, "%sjne CMPEND\n", tab_1_column );
  fprintf( final, "%smov si, cx\n", tab_1_column );
  fprintf( final, "%smov bx, 0\n", tab_1_column );
  fprintf( final, "CMPL01:\n" );
  fprintf( final, "%smov al, [di + bx]\n", tab_1_column );
  fprintf( final, "%scmp BYTE PTR [si+bx], al\n", tab_1_column );
  fprintf( final, "%sjne CMPEND\n", tab_1_column );
  fprintf( final, "%sinc bx\n", tab_1_column );
  fprintf( final, "%scmp BYTE PTR [si+bx], '$'\n", tab_1_column );
  fprintf( final, "%sje CMPEND\n", tab_1_column );
  fprintf( final, "%sjmp CMPL01\n", tab_1_column );
  fprintf( final, "CMPEND:\n" );
  fprintf( final, "%sret\n", tab_1_column );
  fprintf( final, "STRCMP ENDP\n" );

  fprintf( final, "FRBUFFLEN equ 13\n" );
  fprintf( final, "FRBUFFPTO equ 9\n" );
  fprintf( final, "formatReal PROC\n" );
  fprintf( final, "%spush di\n", tab_1_column );
  fprintf( final, "%smov cx,FRBUFFLEN\n", tab_1_column );
  fprintf( final, "%smov al,byte ptr ' '\n", tab_1_column );
  fprintf( final, "%srep stosb\n", tab_1_column );
  fprintf( final, "%smov [di],byte ptr '$'\n", tab_1_column );
  fprintf( final, "%scall getSigno\n", tab_1_column );
  fprintf( final, "%spop di\n", tab_1_column );
  fprintf( final, "%spush di\n", tab_1_column );
  fprintf( final, "%smov byte ptr [di],al\n", tab_1_column );
  fprintf( final, "%smov byte ptr [di+FRBUFFPTO],byte ptr '.'\n", tab_1_column );
  fprintf( final, "%scall getExponente\n", tab_1_column );
  fprintf( final, "%smov bx,ax\n", tab_1_column );
  fprintf( final, "%scall getMantisa\n", tab_1_column );
  fprintf( final, "%spush eax\n", tab_1_column );
  fprintf( final, "%smov cl,10d\n", tab_1_column );
  fprintf( final, "%sshr eax,cl\n", tab_1_column );
  fprintf( final, "%sadd di,FRBUFFPTO-1\n", tab_1_column );
  fprintf( final, "%scall format9\n", tab_1_column );
  fprintf( final, "%spop eax\n", tab_1_column );
  fprintf( final, "%sand eax,003FFFFh\n", tab_1_column );
  fprintf( final, "%scall convDecimal\n", tab_1_column );
  fprintf( final, "%spop di\n", tab_1_column );
  fprintf( final, "%sadd di,FRBUFFPTO+3\n", tab_1_column );
  fprintf( final, "%smov cx,3\n", tab_1_column );
  fprintf( final, "%scall formatX\n", tab_1_column );
  fprintf( final, "%sret\n", tab_1_column );
  fprintf( final, "formatReal ENDP\n" );

  fprintf( final, "getSigno:\n" );
  fprintf( final, "%smov word ptr ax,[si+2]\n", tab_1_column );
  fprintf( final, "%stest ax,08000h\n", tab_1_column );
  fprintf( final, "%sje csPos\n", tab_1_column );
  fprintf( final, "%smov al,'-'\n", tab_1_column );
  fprintf( final, "%sjmp csEnd\n", tab_1_column );
  fprintf( final, "cSPos:\n" );
  fprintf( final, "%smov al,'+'\n", tab_1_column );
  fprintf( final, "csEnd:\n" );
  fprintf( final, "%sret\n", tab_1_column );

  fprintf( final, "getExponente:\n" );
  fprintf( final, "%smov ax,[si+2]\n", tab_1_column );
  fprintf( final, "%sshl ax,1\n", tab_1_column );
  fprintf( final, "%sxchg al,ah\n", tab_1_column );
  fprintf( final, "%sand ax,000FFh\n", tab_1_column );
  fprintf( final, "%sadd eax,0FF81h ; bug del TASM, no genera sub ax,07Fh!!!\n", tab_1_column );
  fprintf( final, "%sret\n", tab_1_column );

  fprintf( final, "getMantisa:\n" );
  fprintf( final, "%smov eax,[si]\n", tab_1_column );
  fprintf( final, "%sand eax,007FFFFFh\n", tab_1_column );
  fprintf( final, "%sor eax,00800000h\n", tab_1_column );
  fprintf( final, "%smov cx,13d\n", tab_1_column );
  fprintf( final, "%ssub cx,bx\n", tab_1_column );
  fprintf( final, "%sjns gM01\n", tab_1_column );
  fprintf( final, "%sneg cx\n", tab_1_column );
  fprintf( final, "%sshl eax,cl\n", tab_1_column );
  fprintf( final, "%sjmp gMend\n", tab_1_column );
  fprintf( final, "gM01:\n" );
  fprintf( final, "%sclc\n", tab_1_column );
  fprintf( final, "%sshr eax,cl\n", tab_1_column );
  fprintf( final, "gMend:\n" );
  fprintf( final, "%sret\n", tab_1_column );

  fprintf( final, "formatear:\n" );
  fprintf( final, "format9:\n" );
  fprintf( final, "%smov edx,eax\n", tab_1_column );
  fprintf( final, "%smov cl,16\n", tab_1_column );
  fprintf( final, "%sshr edx,cl ; genero DX:AX\n", tab_1_column );
  fprintf( final, "%smov bx,10000\n", tab_1_column );
  fprintf( final, "%sdiv bx ; ax resultado, dx, reminder\n", tab_1_column );
  fprintf( final, "%spush ax\n", tab_1_column );
  fprintf( final, "%smov bx,10\n", tab_1_column );
  fprintf( final, "%smov ax,dx\n", tab_1_column );
  fprintf( final, "%smov cl,4\n", tab_1_column );
  fprintf( final, "%sfCiclo01:\n", tab_1_column );
  fprintf( final, "%sxor dx,dx\n", tab_1_column );
  fprintf( final, "%sidiv bx\n", tab_1_column );
  fprintf( final, "%sadd dl,'0'\n", tab_1_column );
  fprintf( final, "%smov es:[di],dl\n", tab_1_column );
  fprintf( final, "%ssub di,1\n", tab_1_column );
  fprintf( final, "%ssub cl,1\n", tab_1_column );
  fprintf( final, "%sjne fCiclo01\n", tab_1_column );
  fprintf( final, "%spop ax\n", tab_1_column );
  fprintf( final, "%smov cl,3\n", tab_1_column );
  fprintf( final, "formatX:\n" );
  fprintf( final, "%smov bx,10\n", tab_1_column );
  fprintf( final, "fCiclo02:\n" );
  fprintf( final, "%sxor dx,dx\n", tab_1_column );
  fprintf( final, "%sidiv bx\n", tab_1_column );
  fprintf( final, "%sadd dl,'0'\n", tab_1_column );
  fprintf( final, "%smov es:[di],dl\n", tab_1_column );
  fprintf( final, "%ssub di,1\n", tab_1_column );
  fprintf( final, "%ssub cl,1\n", tab_1_column );
  fprintf( final, "%sjnz fCiclo02\n", tab_1_column );
  fprintf( final, "%sret\n", tab_1_column );
  fprintf( final, "%snumeros dw 500,250,125,62,31,15,8,4,2,1\n", tab_1_column );

  fprintf( final, "convDecimal:\n" );
  fprintf( final, "%smov cx,09\n", tab_1_column );
  fprintf( final, "%sxor bx,bx\n", tab_1_column );
  fprintf( final, "%sxor dx,dx\n", tab_1_column );
  fprintf( final, "%sclc\n", tab_1_column );
  fprintf( final, "cvCiclo:\n" );
  fprintf( final, "%sshr ax,1\n", tab_1_column );
  fprintf( final, "%sjnc cvCiclo01\n", tab_1_column );
  fprintf( final, "%smov bx,cx\n", tab_1_column );
  fprintf( final, "%sshl bx,1\n", tab_1_column );
  fprintf( final, "%sadd bx,offset numeros\n", tab_1_column );
  fprintf( final, "%smov bx,cs:[bx]\n", tab_1_column );
  fprintf( final, "%sadd bx,dx\n", tab_1_column );
  fprintf( final, "%smov dx,bx\n", tab_1_column );
  fprintf( final, "cvCiclo01:\n" );
  fprintf( final, "%ssub cx,1\n", tab_1_column );
  fprintf( final, "%sjns cvCiclo\n", tab_1_column );
  fprintf( final, "%smov ax,dx\n", tab_1_column );
  fprintf( final, "%sadd ax,1\n", tab_1_column );
  fprintf( final, "%sret\n", tab_1_column );
}