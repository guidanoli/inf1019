
  // List module interface
  // Guilherme Dantas

  //
  //    The list module works with a very rudimentar data structure
  // composed of two core strucutures: head and nodes.
  //
  //    The head is the access point to the list data. It stores and
  // manages abstract information about the nodes.
  //
  //    The node points to a generic information and to a function
  // that deals with it when the list is tore down. Sort of a
  // deconstructor, in OOP terms.
  //
  //    Once created, the list can only store new nodes, be destroyed
  // or emptied, and - of course - have its state checked in between
  // these actions. Thus, you cannot remove a single node from the
  // list.
  //
  //    The motivation behind this module was to store basic data and
  // when done, pick a random item from the list. This was not possible
  // using arrays because it is not a dynamic data structure.
  //
  //    This implementation allows for multiple items to be inserted
  // with the same information, making it more likely to be picked on
  // the list_rand routine.
  //

  typedef enum {

    LIST_ERR,
    // Fatal error

    LIST_OK,
    // OK

    LIST_MEM,
    // Memory Fault

    LIST_PARAM,
    // Invalid or null parameter

    LIST_EMPTY,
    // List is empty

    LIST_NOT_EMPTY,
    // List is not empty

  } list_ret;

  typedef struct head_s * plist; // List head

  // Creates a list and stores it in phead
  // phead - pointer to list head
  // > OK, MEM, PARAM
  list_ret list_create (plist * phead);

  // Destroys list and nodes according to each individual delete function
  // phead - pointer to list head
  // > OK, PARAM, every parameter of list_empty but OK
  // [!] list head will be set to NULL
  list_ret list_destroy (plist * phead);

  // Counts the number of nodes on list and stores in count
  // head - list head
  // count - pointer to counter
  // > OK, PARAM
  list_ret list_count (plist head, unsigned int * count);

  // Checks if list is empty
  // head - list head
  // > EMPTY, NOT_EMPTY, every parameter of list_count but OK
  list_ret list_isempty (plist head);

  // Inserts node in list with information and a pointer to delete function
  // that will be called to free (or not) the information on list_destroy
  // head - list head
  // info - information
  // delete_info - takes info as argument when the list is being tore down
  // > OK, MEM, PARAM
  // [!] delete_info can be NULL, thus, not being ever called
  list_ret list_ins (plist head, void * info, void * delete_info(void *));

  // Selects a random node from the list and stores its information in info
  // head - list head
  // info - pointer to information
  // > ERR, OK, PARAM, EMPTY
  // [!] in case of an error, ERR will be returned and info will not have
  // a trustworthy content.
  // [!] in case of the list being empty, EMPTY will be returned and info
  // will point to NULL.
  list_ret list_rand (plist head, void ** info);

  // Empties list but does not destroy it
  // head - list head
  // > OK, PARAM
  list_ret list_empty (plist head);
