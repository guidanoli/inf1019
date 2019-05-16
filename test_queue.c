// Queue test
// Guilherme Dantas

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "queue.h"

#define VAR1 12
#define VAR2 55
#define VAR3 -4
#define VAR4 100
#define VAR5 500

int main( void )
{
	int * var1 = (int *) malloc(sizeof(int));
	int * var2 = (int *) malloc(sizeof(int));
	int * var3 = (int *) malloc(sizeof(int));
	int * var4 = (int *) malloc(sizeof(int));
	int * var5 = (int *) malloc(sizeof(int));
  qhead head = NULL;
  qnode node = NULL;
  int aux, i;
  
  // initiate variables
  *var1 = VAR1;
  *var2 = VAR2;
  *var3 = VAR3;
  *var4 = VAR4;
  *var5 = VAR5;

  // testing NULL as parameter
  assert(qnode_create(NULL,NULL)!=0);
  assert(qnode_create(&node,NULL)!=0);
  assert(node == NULL);
  assert(qnode_create(NULL,var1)!=0);
  assert(*var1==VAR1);
  assert(qhead_create(NULL,NULL)!=0);
  assert(qhead_create(&head,NULL)!=0);
  assert(head==NULL);
  assert(qhead_create(NULL,var3)!=0);
  assert(*var3==VAR3);
  assert(qnode_getinfo(NULL)==NULL);
  assert(qhead_getinfo(NULL)==NULL);
  qhead_ins(NULL,NULL);
  assert(qhead_rm(NULL)==NULL);
  qnode_destroy(NULL);
  qhead_destroy(NULL);
  
  // empty queue
  assert(qhead_create(&head,var3)==0);
  assert(head!=NULL);
  assert(qhead_getinfo(head)==var3);
  assert(*var3==VAR3);
  assert(qhead_rm(head)==NULL);
  qhead_ins(head,NULL);
  
  // one node
  assert(qnode_create(&node,var1)==0);
  assert(node!=NULL);
  assert(qnode_getinfo(node)==var1);
  assert(*var1 == VAR1);
  
  // test if won't add twice
  qhead_ins(head,node);
  qhead_ins(head,node);
  assert(node==qhead_rm(head));
  assert(NULL==qhead_rm(head));
  
  // check if destroying head destroys nodes
  // [!] wrong way of doing it
  qhead_ins(head,node);
  assert(*var1==VAR1);
  assert(*var3==VAR3);
  qhead_destroy(&head);
  assert(*var1!=VAR1);
  assert(*var3!=VAR3);
  assert(head==NULL);
  assert(node!=NULL); // [!] contains trash (bad)
  
  // correct way of destroying queue
  assert(qhead_create(&head,var4)==0);
  assert(qnode_create(&node,var2)==0);
  qhead_ins(head,node);
  assert(qhead_rm(head)==node);
  assert(*var2==VAR2);
  assert(*var4==VAR4);
  qnode_destroy(&node);
  assert(*var2!=VAR2);
  assert(*var4==VAR4);
  qhead_destroy(&head);
  assert(*var4!=VAR4);
  assert(head==NULL);
  assert(node==NULL); // both clean!
  
  // for many items...
  qhead_create(&head,var5);
  aux = 0;
  for( i = 0 ; i < 10 ; i++ )
  {
    // [!] dangerous. loss of data about nodes
    // just from test conciseness sake
    int * info = (int *) malloc(sizeof(int));
    *info = i+1;
    qnode_create(&node,info);
    qhead_ins(head,node);
    aux++;
  }
  
  // destroy until empty
  i = 0;
  while(( node = qhead_rm(head) ) != NULL )
  {
  	int * info = (int *) qnode_getinfo(node);
  	assert(info!=NULL);
  	assert(*info==i+1);
    qnode_destroy(&node);
    assert(*info!=i+1);
    assert(node==NULL);
    aux--;
    i++;
  }
  assert(qhead_getinfo(head)==var5);
  qhead_destroy(&head);
  assert(head==NULL);
  assert(aux==0); // #inserted = #removed
  
  return 0;
}
