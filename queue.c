// queue - Queue module
// Guilherme Dantas

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

typedef struct queue_node_s {
  int id;       // id
  qnode next;   // next
} queue_node;

typedef struct queue_head_s {
  int id;      // id
  qnode ini;   // initial  
  qnode end;   // ending
} queue_head;

///////////////////////////////////////////
// QUEUE NODE FUNCTIONS
///////////////////////////////////////////

// Create node
// id is the node's identification
// Returns pointer to node or NULL (mem)
qnode qnode_create ( int id )
{
  qnode node = ( qnode ) malloc(sizeof(queue_node));
  if( node == NULL ) return NULL;
  node->id = id;
  node->next = NULL;
  return node;
}

// Get node id
// node is pointer to node
// Returns id or -1 if null
int qnode_getid ( qnode node )
{
  if( node == NULL ) return -1;
  return node->id;
}

// Destroy node
// node is pointer to node
void qnode_destroy ( qnode node )
{
  if( node ) free(node);
}

////////////////////////////////////////////////
// QUEUE HEAD FUNCTIONS
////////////////////////////////////////////////

// Create head
// id is the head's identification
// Returns pointer to head or NULL (mem)
qhead qhead_create ( int id )
{
  qhead head = ( qhead ) malloc(sizeof(queue_head));
  if( head == NULL ) return NULL;
  head->id = id;
  head->ini = head->end = NULL;
  return head;
}

// Get head id
// head is pointer to head
// Returns id or -1 if null
int qhead_getid ( qhead head )
{
  if( head == NULL ) return -1;
  return head->id;
}

// Insert node (at the end)
// head is pointer to head
// node is pointer do node to be inserted
void qhead_ins ( qhead head , qnode node )
{
  if( head == NULL || node == NULL ) return;
  if( head->ini == NULL )
  {
    head->ini = node;
    head->end = node;
  }
  else
  {
    head->end->next = node;
    head->end = node;
  }
}

// Remove node (from the beggining)
// head is pointer to head
// Returns  node if successful or
//          NULL if head is NULL or queue is empty
qnode qhead_rm ( qhead head )
{
  qnode node;
  if( head == NULL ) return NULL;
  node = head->ini;
  if( node == NULL ) return NULL;
  if( node == head->end ) head->end = NULL;
  head->ini = node->next;
  return node;
}

// Destroy head and its nodes
// head is pointer to head
void qhead_destroy ( qhead head )
{
  if ( head == NULL ) return;
  for( qnode q = head->ini; q != NULL ; q = q->next ) qnode_destroy(q);
  free(head);
}

