// Queue test
// Guilherme Dantas

#include <stdio.h>
#include <assert.h>
#include "queue.h"

int main( void )
{
  qhead head;
  qnode node;
  int aux;

  // testing NULL as parameter
  assert(qnode_create(NULL,123)!=0);
  assert(qhead_create(NULL,456)!=0);
  assert(qnode_getid(NULL)!=0);
  assert(qhead_getid(NULL)!=0);
  qhead_ins(NULL,NULL);
  assert(qhead_rm(NULL)==NULL);
  qnode_destroy(NULL);
  qhead_destroy(NULL);
  
  // empty queue
  assert(qhead_create(&head,2019)==0);
  assert(head!=NULL);
  assert(qhead_getid(head)==2019);
  assert(qhead_rm(head)==NULL);
  qhead_ins(head,NULL);
  
  // one node
  assert(qnode_create(&node,1019)==0);
  assert(node!=NULL);
  assert(qnode_getid(node)==1019);
  
  // test if won't add twice
  qhead_ins(head,node);
  qhead_ins(head,node);
  assert(node==qhead_rm(head));
  assert(NULL==qhead_rm(head));
  
  // check if destroying head destroys nodes
  // [!] wrong way of doing it
  qhead_ins(head,node);
  qhead_destroy(&head);
  assert(head==NULL);
  assert(node!=NULL); // [!] contains trash (bad)
  
  // correct way of destroying queue
  assert(qhead_create(&head,10)==0);
  assert(qnode_create(&node,64)==0);
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
  
  return 0;
}
