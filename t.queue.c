
  // Queue test
  // Guilherme Dantas

  #include "queue.h"
  #include "test.h"
  #define NULL (void *) 0 // why import stdlib just for this, right?

  int main( void )
  {
    qhead head = NULL, other = NULL;
    qnode node = NULL;
    int aux, i;
    unsigned long pinfo = 10;

    /*********************/
    /* NULL as parameter */
    /*********************/

    // qnode
    assert(qnode_create(NULL,123) == QUEUE_NULL);
    assert(qnode_getinfo(NULL,&pinfo) == QUEUE_NULL);
    assert(pinfo == 10);
    assert(qnode_getinfo(NULL,NULL) == QUEUE_NULL);
    assert(qnode_getinfo(&node,NULL) == QUEUE_NULL);
    qnode_destroy(NULL);

    // qhead
    assert(qhead_create(NULL,456) == QUEUE_NULL);
    assert(qhead_getid(NULL) == -1);
    assert(qhead_ins(NULL,NULL) == QUEUE_NULL);
    assert(qhead_rm(NULL) == NULL);
    qhead_destroy(NULL);
    assert(qhead_empty(NULL) == QUEUE_NULL);

    // empty queue
    assert(qhead_create(&head,2019) == QUEUE_OK);
    assert(head != NULL);
    assert(qhead_getid(head) == 2019);
    assert(qhead_rm(head) == NULL);
    assert(qhead_ins(head,NULL) == QUEUE_NULL);
    assert(qhead_empty(head) == QUEUE_OK);

    // one node
    assert(qnode_create(&node,1019) == QUEUE_OK);
    assert(node != NULL);
    assert(qnode_getinfo(node,&pinfo) == QUEUE_OK);
    assert(pinfo == 1019);

    // test if won't add twice
    // queue will contain only one
    assert(qhead_ins(head,node) == QUEUE_OK);
    assert(qhead_ins(head,node) == QUEUE_DUPLICATE);
    assert(qhead_empty(head) == QUEUE_FALSE);
    assert(node == qhead_rm(head));
    assert(qhead_empty(head) == QUEUE_OK);
    assert(NULL == qhead_rm(head));
    assert(qhead_empty(head) == QUEUE_OK);

    // check if destroying head destroys nodes
    // [!] wrong way of doing it
    assert(qhead_ins(head,node) == QUEUE_OK);
    qhead_destroy(&head);
    assert(head == NULL);

    assert(node != NULL); // [!] contains trash (bad)
    node = NULL; // safety first

    // correct way of destroying queue
    assert(qhead_create(&head,10) == QUEUE_OK);
    assert(qnode_create(&node,64) == QUEUE_OK);
    qhead_ins(head,node);
    assert(qhead_rm(head) == node);
    qnode_destroy(&node);
    qhead_destroy(&head);
    assert(head == NULL);
    assert(node == NULL); // both clean!

    // for many items...
    qhead_create(&head,32);
    aux = 0;
    for( int i = 0 ; i < 10 ; i++ )
    {
      // [!] dangerous. loss of data about nodes
      // just from test conciseness sake
      qnode_create(&node,i+1);
      assert(qhead_ins(head,node) == QUEUE_OK);
      aux++;
    }

    // destroy until empty
    while( 1 )
    {
      if( (node = qhead_rm(head)) == NULL ) break;
      aux--;
      qnode_destroy(&node);
      assert(node == NULL);
    }
    qhead_destroy(&head);
    assert(head == NULL);
    assert(aux == 0); // #inserted = #removed

    qhead_create(&head,123);
    qhead_create(&other,456);
    for( int i = 0 ; i < 10 ; i++ )
    {
      qnode_create(&node,i);
      qhead_ins(head,node);
    }

    assert(qhead_empty(head) == QUEUE_FALSE);
    assert(qhead_empty(other) == QUEUE_OK);
    assert(qhead_transfer(head,other,1) == QUEUE_OK);
    assert(qhead_empty(head) == QUEUE_FALSE);
    assert(qhead_empty(other) == QUEUE_FALSE);
    assert(qhead_transfer(head,other,9) == QUEUE_OK);
    assert(qhead_empty(head) == QUEUE_OK);
    assert(qhead_empty(other) == QUEUE_FALSE);
    assert(qhead_transfer(other,head,QFLAG_TRANSFER_ALL) == QUEUE_OK);
    assert(qhead_empty(head) == QUEUE_FALSE);
    assert(qhead_empty(other) == QUEUE_OK);

    qhead_destroy(&head);
    qhead_destroy(&other);

    // show log
    show_log();

    return 0;
  }
