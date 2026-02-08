#ifndef _NODE_H_
#define _NODE_H_

#include <stdbool.h>

typedef struct s_node
{
  struct s_node* left;
  struct s_node* right;
  
  void* data;
  int label;
  int jump_label;
  
} node_t;

node_t* node_new( void* data );
void node_delete( node_t* n );

bool node_is_leaf( node_t* node );
bool node_is_node( node_t* node );
bool node_is_node_with_leafs( node_t* node );

#endif