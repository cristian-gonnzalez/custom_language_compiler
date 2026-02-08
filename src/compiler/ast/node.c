#include "node.h"

#include <stdlib.h>

static int label = 0;

node_t* node_new( void* data )
{
    node_t* new = ( node_t* ) malloc( sizeof( node_t ) );
    new->data = data;
    new->left = new->right = NULL;
    new->label = ++label;
    new->jump_label = -1;

    return new;
}

void node_delete( node_t* n )
{
  if(n)
    free( n );
}

bool node_is_leaf( node_t* node )
{
  return node->left == NULL && node->right == NULL;
}

bool node_is_node( node_t* node )
{
  return node->left != NULL || node->right != NULL ; 
}

bool node_is_node_with_leafs( node_t* node )
{
  if( node_is_leaf( node ))
    return false;

  return ( node->left && node_is_leaf(node->left) )  &&
          ( node->right && node_is_leaf(node->right) );
}