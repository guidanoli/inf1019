// Queue module interface
// Guilherme Dantas

#ifndef _H_QUEUE
#define _H_QUEUE

typedef struct queue_head_s * qhead;
typedef struct queue_node_s * qnode;

///////////////////////////////////////////
// QUEUE NODE FUNCTIONS
///////////////////////////////////////////

// Create node
// info is the node's information
// Returns pointer to node or NULL (mem) in *pnode
// Returns 0 if successful or -1 otherwise
int qnode_create ( qnode * pnode , void * info );

// Get node info
// node is pointer to node
// Returns info or null if node doesn't exist
void * qnode_getinfo ( qnode node );

// Destroy node and its info
// node is pointer to pointer to node
void qnode_destroy ( qnode * node );

////////////////////////////////////////////////
// QUEUE HEAD FUNCTIONS
////////////////////////////////////////////////

// Create head
// info is the head's information
// Returns pointer to head or NULL (mem) in *phead
// Returns 0 if successful or -1 otherwise
int qhead_create ( qhead * phead , void * info );

// Get head info
// head is pointer to head
// Returns info or NULL if head doesn't exist
void * qhead_getinfo ( qhead head );

// Insert node (at the end)
// head is pointer to head
// node is pointer do node to be inserted
void qhead_ins ( qhead head , qnode node );

// Remove node (from the beggining)
// head is pointer to head
// Returns  node if successful or
//          NULL if head is NULL or queue is empty
qnode qhead_rm ( qhead head );

// Destroy head, its info and its nodes (and its infos)
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


