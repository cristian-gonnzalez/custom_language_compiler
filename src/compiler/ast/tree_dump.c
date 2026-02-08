#include "tree_dump.h"

#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define TRUNK_BUFFER_SIZE 1024

typedef struct trunk
{
    const struct trunk* prev;

    char str[TRUNK_BUFFER_SIZE];

} trunk_t;

// Helper function to print branches of the binary tree
static void _trunk_show(const trunk_t* p,  FILE* fp)
{
	if( !p)
		return;

	_trunk_show(p->prev, fp);
	fprintf(fp, "%s", p->str);
}

// Recursive function to print binary tree
// It uses inorder traversal
static void _tree_dump(node_t* root, trunk_t* prev, bool is_left, FILE* fp)
{
	if( !root )
		return;

  trunk_t trunk = { .prev = prev };
  
  char prev_str[TRUNK_BUFFER_SIZE];
  snprintf(prev_str, TRUNK_BUFFER_SIZE, "  ");
  snprintf(trunk.str, TRUNK_BUFFER_SIZE, "%s", prev_str);
  
  _tree_dump(root->right, &trunk, false, fp);

	if( !prev )
  {
      snprintf(trunk.str, TRUNK_BUFFER_SIZE, "--");
  }
  else if( is_left )
	{
      snprintf(trunk.str, TRUNK_BUFFER_SIZE, "`--");
      snprintf(prev->str, TRUNK_BUFFER_SIZE, "%s", prev_str);
  }
  else
  {
      snprintf(trunk.str, TRUNK_BUFFER_SIZE, ".--");
      snprintf(prev_str, TRUNK_BUFFER_SIZE, "  |");
	}

	_trunk_show(&trunk, fp);
	token_print( root->data, fp );
	fprintf(fp, "\n");
    
	if (prev)
		snprintf(prev->str, TRUNK_BUFFER_SIZE, "%s", prev_str);

  snprintf(trunk.str, TRUNK_BUFFER_SIZE, "  |");
  _tree_dump(root->left, &trunk, true, fp);
}



void tree_dump_file(node_t* root)
{
   FILE* fp = fopen( "intermediate_code.txt", "wt" );
   if( !fp )
   {
     perror( "Failed to open intermediate_code.txt\n" );
     exit( 1 );
   }

  _tree_dump(root, NULL, false, fp);
  fclose( fp );
}

void tree_dump(node_t* root)
{
  _tree_dump(root, NULL, false, stdout);
}
