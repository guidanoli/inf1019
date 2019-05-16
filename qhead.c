// qhead - Queue Head
// Guilherme Dantas

#include <stdio.h>
#include <stdlib.h>

typedef struct queue_head {
  int id;       // id
  void * ini;   // initial  
  void * end;   // ending
} qhead;

// Create head
// id is the head's identification
// Returns pointer to head or NULL (mem)
qhead * qhead_create ( int id )
{
  qhead * head = ( qhead * ) malloc(sizeof(qhead));
  if( head == NULL ) return NULL;
  head->id = id;
  head->ini = head->end = NULL;
  return head;
}

// Insert node (at the end)
// head is pointer to head
// node is pointer do node to be inserted
// Returns 0 if successful, 1 otherwise
int qhead_ins ( qhead * head , void * node )
{
  if( head == NULL || node == NULL ) return 1;
  //TODO: make node point to head
  if( head->ini == NULL )
  {
    head->ini = node;
    head->end = node;
  }
  else
  {
    //TODO: make head->end <next> point to node
    head->end = node;
  }
  return 0;
}

// Remove node (from the beggining)
// head is pointer to head
// Returns  node if successful or
//          NULL if head is NULL or queue is empty
void * qhead_rm ( qhead * head )
{
  void * node;
  if( head == NULL ) return NULL;
  node = head->ini;
  if( node == NULL ) return NULL;
  //TODO: if node == head->end make head->end NULL
  //TODO: make head->ini point to node.<next>
  //TODO: make node not point to head anymore
  return node;
}

