// Queue test
// Guilherme Dantas

#include <stdio.h>
#include <assert.h>
#include "queue.h"

int main( void )
{
  qhead head;
  qnode node;

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
  
  // free it all
  qhead_destroy(&head);
  qnode_destroy(&node);
}
