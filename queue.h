  
  // Queue module interface
  // Guilherme Dantas

  #ifndef _H_QUEUE
  #define _H_QUEUE

  typedef struct queue_head_s * qhead;
  typedef struct queue_node_s * qnode;

  ///////////////////////////////////////////
  // QUEUE MODULE CONSTANTS
  ///////////////////////////////////////////

  typedef enum {
    QUEUE_OK ,
    QUEUE_FALSE ,
    QUEUE_NULL ,
    QUEUE_MEM
  } queue_ret;

  ///////////////////////////////////////////
  // QUEUE NODE FUNCTIONS
  ///////////////////////////////////////////

  // Create node
  // id is the node's identification
  // Returns pointer to node or NULL (mem) in *pnode
  // Returns OK, NULL or MEM
  queue_ret qnode_create ( qnode * pnode , int id );

  // Get node id
  // node is pointer to node
  // Returns id or -1 if null
  int qnode_getid ( qnode node );

  // Destroy node
  // node is pointer to pointer to node
  void qnode_destroy ( qnode * node );

  ////////////////////////////////////////////////
  // QUEUE HEAD FUNCTIONS
  ////////////////////////////////////////////////

  // Create head
  // id is the head's identification
  // Returns pointer to head or NULL (mem) in *phead
  // Returns OK, NULL or MEM
  queue_ret qhead_create ( qhead * phead , int id );

  // Check if head is empty (no nodes)
  // head is pointer to head
  // Returns OK, FALSE or NULL
  queue_ret qhead_empty ( qhead head );

  // Get head id
  // head is pointer to head
  // Returns id or -1 if null
  int qhead_getid ( qhead head );

  // Insert node (at the end)
  // head is pointer to head
  // node is pointer do node to be inserted
  void qhead_ins ( qhead head , qnode node );

  // Remove node (from the beggining)
  // head is pointer to head
  // Returns  node if successful or
  //          NULL if head is NULL or queue is empty
  qnode qhead_rm ( qhead head );

  // Destroy head and its nodes
  // head is pointer to pointer to head
  // [!] this will lead any node pointers
  // to nodes of the queue to point to trash
  // [!] To avoid dealing with crazy pointers
  // consider removing and destroying nodes
  // until the queue is empty, as follows:
  //
  // while((node=qhead_rm(head))!=NULL)
  //   qnode_destroy(&node);
  // qhead_destroy(&head);
  //
  void qhead_destroy ( qhead * head );

  #endif

