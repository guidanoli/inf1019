
  // List module interface
  // Guilherme Dantas

  typedef enum {
    LIST_ERR,
    LIST_OK,
    LIST_MEM,
    LIST_PARAM,
    LIST_EMPTY,
    LIST_NOT_EMPTY,
  } list_ret;

  typedef struct head_s * plist; // List head

  // Creates a list and stores it in phead
  // phead - pointer to list head
  // > OK, MEM, PARAM
  list_ret list_create(plist * phead);

  // Destroys list and nodes according to each individual delete function
  // phead - pointer to list head
  // > OK, PARAM
  // [!] list head will be set to NULL
  list_ret list_destroy(plist * phead);

  // Counts the number of nodes on list and stores in count
  // head - list head
  // count - pointer to counter
  // > OK, PARAM
  list_ret list_count(plist head, unsigned int * count);

  // Checks if list is empty
  // head - list head
  // > EMPTY, NOT_EMPTY, every parameter of list_count but OK
  list_ret list_empty(plist head);

  // Inserts node in list with information and a pointer to delete function
  // that will be called to free (or not) the information on list_destroy
  // head - list head
  // info - information
  // delete_info - takes info as argument when the list is being tore down
  // > OK, MEM, PARAM
  // [!] delete_info can be NULL, thus, not being ever called
  list_ret list_ins(plist head, void * info, void * delete_info(void *));

  // Selects a random node from the list and stores its information in info
  // head - list head
  // info - pointer to information
  // > ERR, OK, PARAM, EMPTY
  // [!] in case of an error, ERR will be returned and info will not have
  // a trustworthy content.
  // [!] in case of the list being empty, EMPTY will be returned and info
  // will point to NULL.
  list_ret list_rand(plist head, void ** info);
