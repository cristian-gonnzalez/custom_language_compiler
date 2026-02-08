/* file: symtab.c
 * Symbol Table module - Implementation.
 */
#include <string.h>
#include <stdio.h>

#include "symtab.h"


/*--------------------------------------------------*/
/* Initialization                                   */
/*--------------------------------------------------*/

void symtab_init( symtab_t* table )
{
    if( !table ) 
      return;
    memset(table, 0, sizeof(symtab_t));
}

void symtab_uninit( symtab_t* table )
{
    if( !table ) 
       return;
    memset(table, 0, sizeof(symtab_t));
}

/*--------------------------------------------------*/
/* Core operations                                  */
/*--------------------------------------------------*/


/** Adds an element in the table.
 *
 * param  table  Table.
 * param  smb    Element.
 *
 * return True if the element was added. False if the element existed.
 */
bool symtab_add( symtab_t* table, symbol_t* smb )
{
    if (!table || !smb)
        return false;

    if (table->num_elems >= SYMTAB_MAX_ELEMS)
        return false;

    if (symtab_find_by_name(table, smb->name) != -1)
        return false;

    table->table[table->num_elems++] = *smb;
    return true;
}


/** Find an element in the table searching by name.
 *
 * param  table  Table.
 * param  name   Element name.
 *
 * return The position of the element in the table if found. Otherwise, -1 is returned.
 */
int symtab_find_by_name( symtab_t *table, const char *name )
{
    if (!table || !name)
        return -1;

    for (size_t i = 0; i < table->num_elems; i++)
    {
        if (strcmp(table->table[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}


/** Find an element in the table searching by value.
 *
 * param  table  Table.
 * param  name   Element value.
 *
 * return The position of the first element in the table if found. Otherwise, -1 is returned.
 */
int symtab_find_by_value( symtab_t* table, const char* value )
{
    if (!table || !value)
        return -1;

    for (size_t i = 0; i < table->num_elems; i++)
    {
        if (strcmp(table->table[i].value, value) == 0)
            return (int)i;
    }
    return -1;
}

symbol_t* symtab_symbol_by_id( const char* id_name )
{
    if (!id_name)
        return NULL;

    int pos = symtab_find_by_name(&_symtab, id_name);
    return (pos >= 0) ? &_symtab.table[pos] : NULL;
}


/** Prints the symbol table to standard output.
 *
 * param  table  Table.
 *
 * return void.
 */
void symtab_print( symtab_t* table )
{
    if (!table) return;

    printf("\nNumber of elements: %d\n", (int)table->num_elems);
    printf("|------------------------------------------------ Symbol table ------------------------------------------------|\n");
    printf("|      Name       |        Type        |              Value              |  Length                             |\n");
    printf("|--------------------------------------------------------------------------------------------------------------|\n");

    for (size_t i = 0; i < table->num_elems; i++)
    {
        symbol_t* sym = &table->table[i];
        printf(" %-17s | %-18s | %-31s | %11d |\n",
               sym->name,
               token_type_to_cstr(sym->token_type),
               sym->value,
               (int)sym->length);
    }
    printf("\n");
}

void symtab_generate_file_ts( symtab_t* table )
{
    if (!table) 
      return;

    FILE* fp = fopen("symbol_table.txt", "w");
    if (!fp)
    {
        perror("Failed to create symbol_table.txt");
        return;
    }

    fprintf(fp, "------------------------------------------------ Symbol table ------------------------------------------------|\n");
    fprintf(fp, "|      Name      |        Type        |              Value              |  Lenght |\n");
    fprintf(fp, "---------------------------------------------------------------------------------------------------------------|\n");

    for (size_t i = 0; i < table->num_elems; i++)
    {
        symbol_t* sym = &table->table[i];
        fprintf(fp, " %-17s | %-18s | %-31s | %11d |\n",
                sym->name,
                token_type_to_cstr(sym->token_type),
                sym->value,
                (int)sym->length);
    }

    fclose(fp);
}

void replace_spaces(char* dst, size_t dst_size, const char* src)
{
    size_t i = 0;
    while (*src && i + 1 < dst_size)
    {
        dst[i++] = (*src == ' ' || *src == ':') ? '_' : *src;
        src++;
    }
    dst[i] = '\0';
}

int symtab_add_cte_s( symtab_t* st, const char* cte )
{
    if (!st || !cte)
        return -1;

    char name[SYMBOL_BUFFER_SIZE-2];
    replace_spaces(name, sizeof(name), cte);

    char full_name[SYMBOL_BUFFER_SIZE];
    snprintf(full_name, sizeof(full_name), "_%s", name);

    int pos = symtab_find_by_name(st, full_name);
    if (pos != -1)
        return pos;

    symbol_t s = {0};
    s.token_type = token_type_cte;
    s.length = strlen(cte);

    snprintf(s.name, sizeof(s.name), "%s", full_name);
    snprintf(s.value, sizeof(s.value), "%s", cte);

    if (!symtab_add(st, &s))
        return -1;

    return symtab_find_by_name(st, full_name);
}


int symtab_add_cte( symtab_t* st, int cte )
{
    if (!st)
        return -1;

    char name[SYMBOL_BUFFER_SIZE];
    char value[SYMBOL_BUFFER_SIZE];

    snprintf(name, sizeof(name), "_%d", cte);
    snprintf(value, sizeof(value), "%d.0", cte);

    int pos = symtab_find_by_name(st, name);
    if (pos != -1)
        return pos;

    symbol_t s = {0};
    s.token_type = token_type_cte;
    s.length = strlen(value);

    snprintf(s.name, sizeof(s.name), "%s", name);
    snprintf(s.value, sizeof(s.value), "%s", value);

    if (!symtab_add(st, &s))
        return -1;

    return symtab_find_by_name(st, name);
}


int symtab_add_id(symtab_t* st, const char* name, id_type_t id_type)
{
    if (!st || !name)
        return -1;

    int pos = symtab_find_by_name(st, name);
    if (pos != -1)
        return pos;

    symbol_t s = {0};
    s.token_type = token_type_id;
    s.id_type = id_type;

    snprintf(s.name, sizeof(s.name), "%s", name);

    if (id_type == id_type_numeric)
        snprintf(s.value, sizeof(s.value), "0.0");

    if (!symtab_add(st, &s))
        return -1;

    return symtab_find_by_name(st, name);
}

int symtab_new_id_aux(symtab_t* st, id_type_t id_type)
{
    static int counter = 0;

    if (!st)
        return -1;

    char name[SYMBOL_BUFFER_SIZE];

    do
    {
        snprintf(name, sizeof(name), "_id_aux_%d", counter++);
    } while (symtab_find_by_name(st, name) != -1);

    symbol_t s = {0};
    s.token_type = token_type_id;
    s.id_type = id_type;

    snprintf(s.name, sizeof(s.name), "%s", name);

    if (id_type == id_type_numeric)
        snprintf(s.value, sizeof(s.value), "0.0");

    if (!symtab_add(st, &s))
        return -1;

    return symtab_find_by_name(st, name);
}