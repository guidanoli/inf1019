
  // List module implementation
  // Guilherme Dantas

  #include <stdlib.h>
  #include <time.h>
  #include "list.h"

  static unsigned char SRAND_CALLED = 0;

  struct node_s {
    lnode next;
    void * info;
    void (* del_func)(void *);
  };

  struct head_s {
    lnode first;
    unsigned int count;
  };

  static void call_srand();

  list_ret list_create(plist * phead) {
    if( !phead ) return LIST_PARAM;
    plist head = (plist) malloc(sizeof(struct head_s));
    if( !head ) return LIST_MEM;
    head->first = NULL;
    head->count = 0;
    *phead = head;
    return LIST_OK;
  }

  list_ret list_destroy(plist * phead) {
    if( !phead ) return LIST_PARAM;
    plist head = *phead;
    if( !head ) return LIST_OK;
    lnode node = head->first;
    while( node ) {
      lnode temp = node->next;
      if( node->del_func ) node->del_func(node->info);
      free(node);
      node = temp;
    }
    free(head);
    *phead = NULL;
    return LIST_OK;
  }

  list_ret list_count(plist head, unsigned int * count)
  {
    if( !head || !count ) return LIST_PARAM;
    *count = head->count;
    return LIST_OK;
  }

  list_ret list_empty(plist head)
  {
    int count;
    list_ret ret = list_count(head,&count);
    if( ret != LIST_OK ) return ret;
    return count == 0 ? LIST_EMPTY : LIST_NOT_EMPTY;
  }

  list_ret list_ins(plist head, void * info, void * delete_info(void *))
  {
    if( !head || !info ) return LIST_PARAM;
    lnode node = (lnode) malloc(sizeof(struct node_s));
    if( !node ) return LIST_MEM;
    node->info = info;
    node->del_func = delete_info;
    node->next = head->first;
    head->count = head->count + 1;
    head->first = node;
    return LIST_OK;
  }

  list_ret list_rand(plist head, void ** info)
  {
    if( !head || !info ) return LIST_PARAM;
    call_srand();
    int r = rand() % head->count;
    lnode temp = head->first;
    while( r )
    {
      if( !temp ) return LIST_ERR; // error in structure
      temp = temp->next;
      r--;
    }
    *info = temp->info;
    return LIST_OK;
  }

  static void call_srand()
  {
    if( !SRAND_CALLED ) {
      srand((unsigned int)time(NULL));
      SRAND_CALLED = 1;
    }
  }
