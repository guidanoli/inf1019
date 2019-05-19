
  // Queue test
  // Guilherme Dantas

  #include <stdio.h>
  #include <string.h>
  #include <assert.h>
  #include "queue.h"

  /* TEST MODULE TOOLKIT */

  #define DEFAULT_COLOR "\033[0m"
  #define RED "\033[0;31m"
  #define GREEN "\033[0;32m"
  #define YELLOW "\033[0;33m"
  #define assert(x) assertcolor(x,#x,__LINE__)

  static void printcolor(const char * tag, const char * color, const char * msg);
  static void printerror(const char * msg);
  static void printmsg(const char * msg);
  static void printsuccess(const char * msg);
  static void assertcolor(int boolean, const char * label, const line);
  static void show_log();

  int n_tests = 0;
  int n_failed = 0;

  /* CONSTANTS */

  int main( void )
  {
    qhead head, other;
    qnode node;
    int aux, i;

    // testing NULL as parameter
    assert(qnode_create(NULL,123)==QUEUE_NULL);
    assert(qhead_create(NULL,456)==QUEUE_NULL);
    assert(qnode_getid(NULL)==-1);
    assert(qhead_getid(NULL)==-1);
    qhead_ins(NULL,NULL);
    assert(qhead_rm(NULL)==NULL);
    qnode_destroy(NULL);
    qhead_destroy(NULL);
    assert(qhead_empty(NULL)==QUEUE_NULL);
    
    // empty queue
    assert(qhead_create(&head,2019)==QUEUE_OK);
    assert(head!=NULL);
    assert(qhead_getid(head)==2019);
    assert(qhead_rm(head)==NULL);
    qhead_ins(head,NULL);
    assert(qhead_empty(head)==QUEUE_OK);
    
    // one node
    assert(qnode_create(&node,1019)==QUEUE_OK);
    assert(node!=NULL);
    assert(qnode_getid(node)==1019);
    
    // test if won't add twice
    // queue will contain only one
    qhead_ins(head,node);
    qhead_ins(head,node);
    assert(qhead_empty(head)==QUEUE_FALSE);
    assert(node==qhead_rm(head));
    assert(qhead_empty(head)==QUEUE_OK);
    assert(NULL==qhead_rm(head));
    assert(qhead_empty(head)==QUEUE_OK);
    
    // check if destroying head destroys nodes
    // [!] wrong way of doing it
    qhead_ins(head,node);
    qhead_destroy(&head);
    assert(head==NULL);
    assert(node!=NULL); // [!] contains trash (bad)
    
    // correct way of destroying queue
    assert(qhead_create(&head,10)==QUEUE_OK);
    assert(qnode_create(&node,64)==QUEUE_OK);
    qhead_ins(head,node);
    assert(qhead_rm(head)==node);
    qnode_destroy(&node);
    qhead_destroy(&head);
    assert(head==NULL);
    assert(node==NULL); // both clean!
    
    // for many items...
    qhead_create(&head,32);
    aux = 0;
    for( int i = 0 ; i < 10 ; i++ )
    {
      // [!] dangerous. loss of data about nodes
      // just from test conciseness sake
      qnode_create(&node,i);
      qhead_ins(head,node);
      aux++;
    }
    
    // destroy until empty
    while(( node = qhead_rm(head) ) != NULL )
    {
      qnode_destroy(&node);
      assert(node==NULL);
      aux--;
    }
    qhead_destroy(&head);
    assert(head==NULL);
    assert(aux==0); // #inserted = #removed
    
    qhead_create(&head,123);
    qhead_create(&other,456);
    for( int i = 0 ; i < 10 ; i++ )
    {
      qnode_create(&node,i);
      qhead_ins(head,node);
    }
    
    assert(qhead_empty(head)==QUEUE_FALSE);
    assert(qhead_empty(other)==QUEUE_OK);
    assert(qhead_transfer(head,other,1)==QUEUE_OK);
    assert(qhead_empty(head)==QUEUE_FALSE);
    assert(qhead_empty(other)==QUEUE_FALSE);
    assert(qhead_transfer(head,other,9)==QUEUE_OK);
    assert(qhead_empty(head)==QUEUE_OK);
    assert(qhead_empty(other)==QUEUE_FALSE);
    assert(qhead_transfer(other,head,QFLAG_TRANSFER_ALL)==QUEUE_OK);
    assert(qhead_empty(head)==QUEUE_FALSE);
    assert(qhead_empty(other)==QUEUE_OK);
    
    qhead_destroy(&head);
    qhead_destroy(&other);
        
    // show log
    show_log();
    
    return 0;
  }

  static void show_log()
  {
    printf("[");
    printf(YELLOW);
    printf("LOG");
    printf(DEFAULT_COLOR);
    printf("] ");
    printf(GREEN);
    printf("%d",n_tests);
    printf(DEFAULT_COLOR);
    printf(" asserts\n");
    if( n_failed == 0 )
    {
      printf("[");
      printf(YELLOW);
      printf("LOG");
      printf(DEFAULT_COLOR);
      printf("] ");
      printf(GREEN);
      printf("0");
      printf(DEFAULT_COLOR);
      printf(" errors found\n");
    }
    else
    {
      printf("[");
      printf(YELLOW);
      printf("LOG");
      printf(DEFAULT_COLOR);
      printf("] ");
      printf(RED);
      printf("%d",n_failed);
      printf(DEFAULT_COLOR);
      printf(" errors found\n");
    }
  }

  static void printcolor(const char * tag, const char * color, const char * msg)
  {
    printf(DEFAULT_COLOR);  printf("[");
    printf(color);          printf(tag);
    printf(DEFAULT_COLOR);  printf("] %s\n",msg);
  }

  static void printerror(const char * msg)
  {
    printcolor("ERRO",RED,msg);
    n_failed++;
  }

  static void printmsg(const char * msg)
  {
    printcolor("MSG",DEFAULT_COLOR,msg);
  }

  static void printsuccess(const char * msg)
  {
    printcolor("SUCCESS",GREEN,msg);
  }

  static void assertcolor(int boolean, const char * label, const line)
  {
    if( boolean ) printsuccess( label );
    else {
      char msg[256] = "";
      char line_s[8] = "";
      strcpy(msg,label);
      strcat(msg," failed at line ");
      sprintf(line_s,"%d.",line);
      strcat(msg,line_s);
      printerror(msg);
    }
    n_tests++;
  }

