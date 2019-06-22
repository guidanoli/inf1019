
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
  typedef struct node_s * lnode; // List node

  list_ret list_create(plist * phead);
  list_ret list_destroy(plist * phead);
  list_ret list_count(plist head, unsigned int * count);
  list_ret list_empty(plist head);
  list_ret list_ins(plist head, void * info, void * delete_info(void *));
  list_ret list_rand(plist head, void ** info);
