/* 20160233 ParkNaHyeon syntactic.c */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dynarray.h"
#include "lexical.h"
#include "syntactic.h"

/*--------------------------------------------------------------------------
syntactic_analyzer
-----
return value: 0 on failure, 1 othewise
parameter: DynArray_T dyn_ptr
this function does syntactic analysis
  --------------------------------------------------------------------------*/
int syntactic_analyzer(DynArray_T dyn_ptr)
{
  int i;
  int token_number;
  struct Token* token;
  struct Token* prev_token;
  struct Token* next_token;

  assert(dyn_ptr!=NULL);

  token_number=DynArray_getLength(dyn_ptr);
  i=0;
  while(i<=token_number){
    i++;
    for(;i<token_number;i++){
      token=(struct Token*)(DynArray_get(dyn_ptr,i));
      if((token->token_type)==TOKEN_PIPE) {break;}
    }
    if(i==token_number) return 1;//if there is no pipe
    if(i==0){
      fprintf(stderr,"./ish: Missing command name\n");
      printf("%% ");
      fflush(NULL);
      return 0;
    }
    if(i==token_number-1){
      fprintf(stderr,"./ish: Pipe or redirection destination not specified\n");
      printf("%% ");
      fflush(NULL);
      return 0;
    }
    prev_token=(struct Token*)(DynArray_get(dyn_ptr,i-1));
    if((prev_token->token_type)==TOKEN_PIPE){
      fprintf(stderr,"./ish: Pipe or redirection destination not specified\n");
      printf("%% ");
      fflush(NULL);
      return 0;
    }
    next_token=(struct Token*)(DynArray_get(dyn_ptr,i+1));
    if((next_token->token_type)==TOKEN_PIPE){
      fprintf(stderr,"./ish: Pipe or redirection destination not specified\n");
      printf("%% ");
      fflush(NULL);
      return 0;
    }
  }
  return 1;
}
