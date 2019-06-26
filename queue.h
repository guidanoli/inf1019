
  // Queue module interface
  // Guilherme Dantas

  #ifndef _H_QUEUE
  #define _H_QUEUE

  #define QFLAG_TRANSFER_ALL -1

  ///////////////////////////////////////////
  // QUEUE MODULE CONSTANTS
  ///////////////////////////////////////////

  typedef struct queue_head_s * qhead;
  typedef struct queue_node_s * qnode;

  typedef enum {
    QUEUE_FALSE ,
    QUEUE_OK ,
    QUEUE_NULL ,
    QUEUE_MEM ,
    QUEUE_PARAM ,
    QUEUE_DUPLICATE ,
  } queue_ret;

  ///////////////////////////////////////////
  // QUEUE NODE FUNCTIONS
  ///////////////////////////////////////////

  // Create queue node with information
  // info - reference to information
  // pnode - reference to queue node
  // > QUEUE_OK, QUEUE_NULL, QUEUE_MEM
  // if QINFO_T == QINFO_T_VOID, info can't be NULL
  queue_ret qnode_create  ( qnode * pnode , unsigned long info );

  // Get information from queue node
  // node - queue node
  // > information through info pointer
  // > QUEUE_OK, QUEUE_NULL
  queue_ret qnode_getinfo ( qnode node , unsigned long * pinfo );

  // Destroy queue node
  // node - reference to queue node
  // > void
  // [!] Does not free queue node information
  void qnode_destroy ( qnode * node );

  ////////////////////////////////////////////////
  // QUEUE HEAD FUNCTIONS
  ////////////////////////////////////////////////

  // Create queue head with identification
  // id - identification
  // phead - reference to queue head
  // > QUEUE_OK, QUEUE_NULL, QUEUE_MEM
  queue_ret qhead_create ( qhead * phead , int id );

  // Check if queue is empty (no queue nodes)
  // head - queue head
  // > QUEUE_OK, QUEUE_FALSE or QUEUE_NULL
  queue_ret qhead_empty ( qhead head );

  // Get queue identification
  // head - queue head
  // > identification, -1 (err)
  int qhead_getid ( qhead head );

  // Insert queue node at the end of the queue
  // head - queue head
  // node - queue node to be inserted
  // > QUEUE_OK, QUEUE_DUPLICATE, QUEUE_NULL
  queue_ret qhead_ins ( qhead head , qnode node );

  // Remove queue node from the beggining of the queue
  // head - queue head
  // > removed node, NULL
  qnode qhead_rm ( qhead head );

  // Transfers a certain amount of queue nodes between queues
  // src - source queue head
  // dest - destintation queue head
  // qtd - quantity of nodes to be transfered or QFLAG_TRANSFER_ALL
  // > QUEUE_OK, QUEUE_NULL, QUEUE_PARAM
  queue_ret qhead_transfer( qhead src , qhead dest , int qtd );

  // Destroy head and its nodes
  // head - pointer to pointer to head
  // > void
  //
  // [!] To avoid dealing with rogue pointers,
  // consider removing and destroying nodes
  // until the queue is empty before destroying
  // the queue, as follows:
  //
  // while((node=qhead_rm(head))!=NULL)
  //   qnode_destroy(&node);
  // qhead_destroy(&head);
  //
  void qhead_destroy ( qhead * head );

  #endif
