
  // queue - Queue module implementation
  // Guilherme Dantas

  #include <stdlib.h>
  #include "queue.h"

  typedef struct queue_node_s {
    unsigned long info;  // information
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

  queue_ret qnode_create  ( qnode * pnode , unsigned long info )
  {
    qnode node;
    if( pnode == NULL ) return QUEUE_NULL;
    node = ( qnode ) malloc(sizeof(queue_node));
    if( node == NULL ) return QUEUE_MEM;
    node->info = info;
    node->next = NULL;
    *pnode = node;
    return QUEUE_OK;
  }

  queue_ret qnode_getinfo ( qnode node , unsigned long * pinfo )
  {
    if( node == NULL || pinfo == NULL ) return QUEUE_NULL;
    *pinfo = node->info;
    return QUEUE_OK;
  }

  void qnode_destroy ( qnode * node )
  {
    if( node != NULL && *node != NULL )
    {
      free(*node);
      *node = NULL;
    }
  }

  ////////////////////////////////////////////////
  // QUEUE HEAD FUNCTIONS
  ////////////////////////////////////////////////

  queue_ret qhead_create ( qhead * phead , int id )
  {
    qhead head;
    if( phead == NULL ) return QUEUE_NULL;
    head = ( qhead ) malloc(sizeof(queue_head));
    if( head == NULL ) return QUEUE_MEM;
    head->id = id;
    head->ini = head->end = NULL;
    *phead = head;
    return QUEUE_OK;
  }

  queue_ret qhead_empty ( qhead head )
  {
    if( head == NULL ) return QUEUE_NULL;
    return head->ini == NULL ? QUEUE_OK : QUEUE_FALSE;
  }

  int qhead_getid ( qhead head )
  {
    if( head == NULL ) return -1;
    return head->id;
  }

  queue_ret qhead_ins ( qhead head , qnode node )
  {
    if( head == NULL || node == NULL ) return QUEUE_NULL;
    if( qhead_has_node(head,node) ) return QUEUE_DUPLICATE;
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
    return QUEUE_OK;
  }

  qnode qhead_rm ( qhead head )
  {
    qnode node;
    if( head == NULL ) return NULL;
    node = head->ini;
    if( node == NULL ) return NULL;
    if( node == head->end ) head->end = NULL;
    head->ini = node->next;
    node->next = NULL;
    return node;
  }

  queue_ret qhead_transfer( qhead src , qhead dest , int qtd )
  {
    if( src == NULL || dest == NULL ) return QUEUE_NULL;
    if( qtd <= 0 && qtd != QFLAG_TRANSFER_ALL ) return QUEUE_PARAM;
    for(; qtd != 0 && qhead_empty(src)==QUEUE_FALSE;qtd--)
      qhead_ins(dest,qhead_rm(src));
    return QUEUE_OK;
  }

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

  ///////////////////////////////////////////
  // PRIVATE FUNCTIONS' IMPLEMENTATION
  ///////////////////////////////////////////

  // Checks if queue has node or not
  // head - queue head
  // node - queue node searched
  // >  0 - no
  //    1 - yes
  static int qhead_has_node ( qhead head, qnode node )
  {
    for( qnode q = head->ini; q != NULL ; q = q->next ) if( q == node ) return 1;
    return 0;
  }
