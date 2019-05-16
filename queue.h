// Queue module interface
// Guilherme Dantas

typedef struct queue_head_s * qhead;
typedef struct queue_node_s * qnode;

///////////////////////////////////////////
// QUEUE NODE FUNCTIONS
///////////////////////////////////////////

// Create node
// id is the node's identification
// Returns pointer to node or NULL (mem)
qnode qnode_create ( int id );

// Get node id
// node is pointer to node
// Returns id or -1 if null
int qnode_getid ( qnode node );

// Destroy node
// node is pointer to node
void qnode_destroy ( qnode node );

////////////////////////////////////////////////
// QUEUE HEAD FUNCTIONS
////////////////////////////////////////////////

// Create head
// id is the head's identification
// Returns pointer to head or NULL (mem)
qhead qhead_create ( int id );

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
// head is pointer to head
void qhead_destroy ( qhead head );

