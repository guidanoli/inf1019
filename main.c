// Interpretador e Escalonador
// Guilherme Dantas

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define STR_PROGNAME_MAX 64
#define _DEBUG

int main(int argc, char ** argv)
{
  // Input
  FILE * f;
  char c;
  int line = 1;
  
  if( argc != 2 )
  {
    fprintf(stderr,"Número de argumentos esperado: 1 Recebido: %d\n",argc-1);
    return EXIT_FAILURE;
  }
  
  f = fopen(argv[1],"r");
  if( f == NULL )
  {
    fprintf(stderr,"Não foi possível abrir '%s': %s\n",argv[1],strerror(errno));
    return EXIT_FAILURE;
  }
  
  while((c = fgetc(f)) != EOF )
  {
    char prog[STR_PROGNAME_MAX] = "";
    int r[3];
    
    if( c != 'e' )
    {
      fprintf(stderr,"Caracter inesperado na linha %d do arquivo '%s'. Esperado: 'e' Recebido: '%c'\n",line,argv[1],c);
      return EXIT_FAILURE;
    }
    if( fscanf(f,"xec %s (%d,%d,%d)\n",prog,r+0,r+1,r+2) != 4 )
    {
      fprintf(stderr,"Linha %d do arquivo '%s' está mal formatada.\n",line,argv[1]);
      return EXIT_FAILURE;
    }
    
    // add program to ready list (?)
    #ifdef _DEBUG
    printf("Carregado programa %s com rajadas %d, %d e %d.\n",prog,r[0],r[1],r[2]);
    #endif
    
    fscanf(f," ");
    
  }
  fclose(f);
	return EXIT_SUCCESS;
}
