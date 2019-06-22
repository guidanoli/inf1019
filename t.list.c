
  // List module test
  // Guilherme Dantas

  #include <stdlib.h>
  #include "test.h"
  #include "list.h"

  int sum = 0;

  void subfunc ( void * info ) {
    sum -= *((unsigned int *)info);
    free(info);
  }

  void delfunc ( void * info ) {
    *((unsigned int *)info) = 0;
  }

  int main (void)
  {

    plist h = NULL;
    unsigned int uaux;
    void * paux;
    char caux;

    // NULL parameters
    assert(list_create(NULL)==LIST_PARAM);
    assert(list_destroy(NULL)==LIST_PARAM);
    assert(list_destroy(&h)==LIST_OK);
    assert(list_count(NULL,&uaux)==LIST_PARAM);
    assert(list_count(h,NULL)==LIST_PARAM);
    assert(list_count(NULL,NULL)==LIST_PARAM);
    assert(list_isempty(NULL)==LIST_PARAM);
    assert(list_ins(NULL,NULL,NULL)==LIST_PARAM);
    assert(list_ins(h,NULL,NULL)==LIST_PARAM);
    assert(list_ins(NULL,&uaux,NULL)==LIST_PARAM);
    assert(list_rand(NULL,NULL)==LIST_PARAM);
    assert(list_rand(h,NULL)==LIST_PARAM);
    assert(list_rand(NULL,&paux)==LIST_PARAM);

    // empty list
    fatal_assert(list_create(&h)!=LIST_MEM);
    assert(list_count(h,&uaux)==LIST_OK);
    assert(uaux==0);
    assert(list_isempty(h)==LIST_EMPTY);
    assert(list_destroy(&h)==LIST_OK);

    // one node
    fatal_assert(list_create(&h)!=LIST_MEM);
    fatal_assert(list_ins(h,&uaux,delfunc)!=LIST_MEM);
    assert(list_count(h,&uaux)==LIST_OK);
    assert(uaux==1);
    assert(list_isempty(h)==LIST_NOT_EMPTY);
    assert(list_destroy(&h)==LIST_OK);

    // del func
    fatal_assert(list_create(&h)!=LIST_MEM);
    uaux = 10;
    fatal_assert(list_ins(h,&uaux,delfunc)!=LIST_MEM);
    assert(uaux==10);
    assert(list_destroy(&h)==LIST_OK);
    assert(uaux==0);

    // no del func
    fatal_assert(list_create(&h)!=LIST_MEM);
    uaux = 10;
    fatal_assert(list_ins(h,&uaux,NULL)!=LIST_MEM);
    assert(uaux == 10);
    assert(list_destroy(&h)==LIST_OK);
    assert(uaux == 10);

    // multiple nodes
    fatal_assert(list_create(&h)!=LIST_MEM);
    for(int i = 0 ; i < 10; i++)
    {
      assert(list_count(h,&uaux)==LIST_OK);
      assert(uaux==i);
      unsigned int * x = (unsigned int *) malloc(sizeof(unsigned int));
      *x = i*i;
      sum += *x;
      fatal_assert(list_ins(h,x,subfunc)!=LIST_MEM);
    }
    assert(sum!=0);
    assert(list_destroy(&h)==LIST_OK);
    assert(sum==0);

    // func is stdlib free
    fatal_assert(list_create(&h)!=LIST_MEM);
    for(int i = 0 ; i < 10; i++)
      fatal_assert(list_ins(h,malloc(sizeof(char)),free)!=LIST_MEM);
    assert(list_destroy(&h)==LIST_OK);

    // random item
    fatal_assert(list_create(&h)!=LIST_MEM);
    uaux = 10;
    assert(list_rand(h,&paux)==LIST_EMPTY);
    fatal_assert(list_ins(h,&uaux,NULL)!=LIST_MEM);
    assert(list_rand(h,&paux)==LIST_OK);
    assert(paux==&uaux);
    assert(list_destroy(&h)==LIST_OK);

    // random item in a certain range
    fatal_assert(list_create(&h)!=LIST_MEM);
    for(char i = 'a' ; i <= 'z'; i++)
    {
      char * c = (char *) malloc(sizeof(char));
      *c = i;
      fatal_assert(list_ins(h,c,free)!=LIST_MEM);
    }
    assert(list_rand(h,&paux)==LIST_OK);
    caux = *((char *)paux);
    printmsg("INFO",CYAN,"Chosen character: %c\n",caux);
    assert(caux >= 'a' && caux <= 'z');
    assert(list_destroy(&h)==LIST_OK);

    // emptying list
    fatal_assert(list_create(&h)!=LIST_MEM);
    for(int i = 0 ; i < 10; i++)
      fatal_assert(list_ins(h,malloc(sizeof(char)),free)!=LIST_MEM);
    assert(list_empty(h)==LIST_OK);
    assert(list_isempty(h)==LIST_EMPTY);
    assert(list_destroy(&h)==LIST_OK);

    show_log();

    return 0;
  }
