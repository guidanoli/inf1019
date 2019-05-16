// Interpretador e Escalonador
// Guilherme Dantas

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "queue.h"

#define QTD_FILAS 3
#define STR_PROGNAME_MAX 64
#define PROG_FILENAME "prog"
#define _DEBUG

qhead filas[QTD_FILAS];
qnode current_process = NULL;

int main(int argc, char ** argv)
{
  char c;
  if( argc != 1 )
  {
    fprintf(stderr,"Número de argumentos inesperado.");
    return EXIT_FAILURE;
  }
  
  while((c = fgetc(stdin)) == 'e')
  {
    char prog[STR_PROGNAME_MAX] = "";
    int r[3];
    
    if( scanf("xec %s (%d,%d,%d)\n",prog,r+0,r+1,r+2) != 4 )
    {
      fprintf(stderr,"String mal formatada.\n");
      return EXIT_FAILURE;
    }
    
    // add program to ready list (?)
    #ifdef _DEBUG
    printf("Carregado programa %s com rajadas %d, %d e %d.\n",prog,r[0],r[1],r[2]);
    #endif
    
    scanf(" ");
    
  }
	return EXIT_SUCCESS;
}
