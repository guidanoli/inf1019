
  // queue - Queue module implementation
  // Guilherme Dantas

  #include <stdio.h>
  #include <stdlib.h>
  #include "queue.h"

  typedef struct queue_node_s {
    void * info;  // information
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
  // Returns pointer to node in *pnode
  // Returns OK, NULL or MEM
  queue_ret qnode_create ( qnode * pnode , void * info )
  {
    qnode node;
    if( pnode == NULL ) return QUEUE_NULL;
    if( info == NULL ) return QUEUE_NULL;
    node = ( qnode ) malloc(sizeof(queue_node));
    if( node == NULL ) return QUEUE_MEM;
    node->info = info;
    node->next = NULL;
    *pnode = node;
    return QUEUE_OK;
  }

  // Get node information
  // node is pointer to node
  // Returns information or null
  void * qnode_getinfo ( qnode node )
  {
    if( node == NULL ) return NULL;
    return node->info;
  }

  // Destroy node
  // node is pointer to pointer to node
  // Does not free information pointed by node
  void qnode_destroy ( qnode * node )
  {
    if( node && *node )
    {
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
  // Returns OK, NULL or MEM
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

  // Check if head is empty (no nodes)
  // head is pointer to head
  // [!] Don't use 1 or 0, use the ret values!!!
  // Returns QUEUE_OK, QUEUE_FALSE or NULL
  queue_ret qhead_empty ( qhead head )
  {
    if( head == NULL ) return QUEUE_NULL;
    return head->ini == NULL ? QUEUE_OK : QUEUE_FALSE;
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
    node->next = NULL;
    return node;
  }

  // Transfers qtd nodes from src to dest
  // src is pointer to source queue
  // dest is pointer to destintation queue
  // qtd is the quantity of nodes to be transfered
  // Returns QUEUE_OK, QUEUE_NULL
  // if qtd == QFLAG_TRANSFER_ALL (-1), src will be emptied and
  // all its nodes will be inserted in dest
  // if qtd < -1, nothing will be done, and QUEUE_PARAM will be returned
  queue_ret qhead_transfer( qhead src , qhead dest , int qtd )
  {
    if( src == NULL || dest == NULL ) return QUEUE_NULL;
    if( qtd <= 0 && qtd != QFLAG_TRANSFER_ALL ) return QUEUE_PARAM;
    for(; qtd != 0 && qhead_empty(src)==QUEUE_FALSE;qtd--)
      qhead_ins(dest,qhead_rm(src));
    return QUEUE_OK;
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
