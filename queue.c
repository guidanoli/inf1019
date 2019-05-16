// queue - Queue module
// Guilherme Dantas

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

typedef struct queue_node_s {
  void * info;  // info
  qnode next;   // next
} queue_node;

typedef struct queue_head_s {
  int id;      // id
  qnode ini;   // initial  
  qnode end;   // ending
} queue_head;

///////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////

static int qhead_has_node ( qhead head, qnode node );

///////////////////////////////////////////
// QUEUE NODE FUNCTIONS
///////////////////////////////////////////

// Create node
// info is the node's information
// Returns pointer to node or NULL (mem) in *pnode
// Returns 0 if successful or -1 otherwise
int qnode_create ( qnode * pnode , void * info )
{
  qnode node;
  if( pnode == NULL || info == NULL ) return -1;
  node = ( qnode ) malloc(sizeof(queue_node));
  if( node == NULL ) return -1;
  node->info = info;
  node->next = NULL;
  *pnode = node;
  return 0;
}

// Get node info
// node is pointer to node
// Returns info or null if node doesn't exist
void * qnode_getinfo ( qnode node )
{
  if( node == NULL ) return NULL;
  return node->info;
}

// Destroy node
// node is pointer to pointer to node
void qnode_destroy ( qnode * node )
{
  if( node && *node )
  {
  	free((*node)->info);
    free(*node);
    *node = NULL;
  }
}

////////////////////////////////////////////////
// QUEUE HEAD FUNCTIONS
////////////////////////////////////////////////

// Create head
// id is the head's identification
// Returns pointer to head or NULL (mem) in *phead
// Returns 0 if successful or -1 otherwise
int qhead_create ( qhead * phead , int id )
{
  qhead head;
  if( phead == NULL ) return -1;
  head = ( qhead ) malloc(sizeof(queue_head));
  if( head == NULL ) return -1;
  head->id = id;
  head->ini = head->end = NULL;
  *phead = head;
  return 0;
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
  if( qhead_has_node(head,node) ) return;
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
// head is pointer to pointer to head
void qhead_destroy ( qhead * head )
{
  if ( head == NULL || *head == NULL ) return;
  qnode q = (*head)->ini, qnext;
  while( q != NULL )
  {
    qnext = q->next;
    qnode_destroy(&q);
    q = qnext;
  }
  free(*head);
  *head=NULL;
}

// assumes both not null
static int qhead_has_node ( qhead head, qnode node )
{
  for( qnode q = head->ini; q != NULL ; q = q->next ) if( q == node ) return 1;
  return 0;
}

