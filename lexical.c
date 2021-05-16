/* 20160233 ParkNaHyeon lexical.c */
/* This file's job: decides whether the input is valid or not. 
  Namely, lexical analysis */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "dynarray.h"
#include "lexical.h"

/*--------------------------------------------------------------------------
add_token
-----
return value: none
parameter: DynArray_T dyn_ptr, char* set_value, int set_type
this function adds_token
  --------------------------------------------------------------------------*/
void add_token(DynArray_T dyn_ptr, char* set_value, int set_type)
{
  struct Token* new_token;

  assert(dyn_ptr!=NULL);
  assert(set_value!=NULL);
  
  new_token=(struct Token*)calloc(1,sizeof(struct Token));
  new_token->token_type=set_type;
  new_token->token_value=(char *)malloc(strlen(set_value)*sizeof(char));
  strcpy(new_token->token_value,set_value);
  DynArray_add(dyn_ptr,(const void*)new_token);
}

/*--------------------------------------------------------------------------
lexical_analyzer
-----
return value: 0 on failure, 1 othewise
parameter: char* input, DynArray_T dyn_ptr
this function does lexical analysis
  --------------------------------------------------------------------------*/
int lexical_analyzer(char* input, DynArray_T dyn_ptr)
{
  char* input_ptr;//ptr used to check every char of input
  char* set_value;//value to add to new token
  int index=0;
  enum lexstate LexState;//dfa state
  int error_flag=0;

  assert(dyn_ptr!=NULL);
  
  input_ptr=input;

  //set first LexState
  if((*input_ptr)=='|') LexState=PIPE;
  else if((*input_ptr)=='"') LexState=IN_QUOTE;
  else if((*input_ptr)==' ') LexState=IS_SPACE;
  else if((*input_ptr)=='\n'){
    printf("%% ");
    fflush(NULL);
    return 0;
  }
  else if((*input_ptr)==EOF){
    printf("%% ");
    fflush(NULL);
    return 0;
  }
  else if((*input_ptr)=='\0'){
    printf("%% ");
    fflush(NULL);
    return 0;
  }
  else{
    LexState=IN_WORD;
    set_value=(char*)malloc(strlen(input)*sizeof(char));
    set_value[index]=(*input_ptr);
    index++;
  }
  //go to DFA state

  while(((*input_ptr)!='\n')&&((*input_ptr)!='\0')){
    input_ptr++;
    switch(LexState){
    case IN_WORD://----------------------------------------
      if((*input_ptr)=='"'){
	LexState=IN_QUOTE;
      }
      else if((*input_ptr)=='|'){
	LexState=PIPE;
	set_value[index]='\0';
	add_token(dyn_ptr,set_value,TOKEN_WORD);
	index=0;
	free(set_value);
	set_value=(char*)malloc(strlen(input)*sizeof(char));
      }
      else if(((*input_ptr)=='\n')||((*input_ptr)=='\0')){
	set_value[index]='\0';
	add_token(dyn_ptr,set_value,TOKEN_WORD);
	index=0;
	free(set_value);
	set_value=(char*)malloc(strlen(input)*sizeof(char));
      }
      else if((*input_ptr)==' '){
	LexState=IS_SPACE;
	set_value[index]='\0';
	add_token(dyn_ptr,set_value,TOKEN_WORD);
	index=0;
	free(set_value);
	set_value=(char*)malloc(strlen(input)*sizeof(char));
      }
      else{
	set_value[index]=(*input_ptr);
	index++;
      }
      break;
    case IN_QUOTE://--------------------------------------
      if(((*input_ptr)=='\n')||((*input_ptr)=='\0')){error_flag=1;}
      else if((*input_ptr)=='"'){LexState = IN_WORD;}
      else{
	set_value[index]=(*input_ptr);
	index++;
      }
      break;
	case PIPE://-----------------------------------------
	  if(((*input_ptr)=='\n')||((*input_ptr)=='\0'));
	  else if((*input_ptr)=='"'){LexState=IN_QUOTE;}
	  else if((*input_ptr)==' '){LexState=IS_SPACE;}
	  else if((*input_ptr)=='|') ; 
	  else{
	    LexState=IN_WORD;
	    set_value[index]=(*input_ptr);
	    index++;
	  }
	  add_token(dyn_ptr,"|",TOKEN_PIPE);
	  break;
    case IS_SPACE://-------------------------------------
      if((*input_ptr)==' ');
      else if((*input_ptr)=='|'){LexState=PIPE;}
      else if((*input_ptr)=='"'){LexState=IN_QUOTE;}
      else{
	LexState=IN_WORD;
	set_value[index]=(*input_ptr);
	index++;
	  }
      break;
    }
    if(error_flag==1){
      free(set_value);
      fprintf(stderr,"./ish: Could not fine quote pair \n");
      printf("%% ");
      fflush(NULL);
      return 0;
    }
  }
  
  return 1;
  
}
