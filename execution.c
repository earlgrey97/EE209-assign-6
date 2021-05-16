/*20160233 ParkNaHyeon execution.c*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "dynarray.h"
#include "lexical.h"
#include "syntactic.h"

/*-------------------------------------------------------------------------
free_token
-----
return value: none
parameter: void* pcElement, void* pvExtra
this function frees token
  -------------------------------------------------------------------------*/
void free_token(void* pvElement, void* pvExtra)
{
  free(pvElement);
}

/*-------------------------------------------------------------------------
make_argvsets
-----
return value: number of argv sets for ex) ls -al | grep drw -> 2
parameter: DynArray_T dyn_ptr, char*** argv_set, int* argc_array
this function makes argv sets and returns the number of it
  -------------------------------------------------------------------------*/
int make_argvsets(DynArray_T dyn_ptr, char*** argv_set, int* argc_array)
{
  struct Token* token;
  int i=0;
  int count=0;
  char** chunk;
  int j=0;
  int argv_index=0;
  int token_count=0;
  int number_of_tokens=DynArray_getLength(dyn_ptr);

  assert(dyn_ptr!=NULL);

  while(token_count!=number_of_tokens){
    count=0;
    while(i<number_of_tokens){
      token_count++;
      token=DynArray_get(dyn_ptr,i);
      if(token->token_type==TOKEN_PIPE) break;
      i++;
      count++;
    }
    //make chunk
    chunk=(char**)malloc(count*sizeof(char*));
    //insert
    for(j=0;j<count;j++){
      chunk[j]=(char *)malloc(strlen(((struct Token*)DynArray_get(dyn_ptr,i-count+j))->token_value)*sizeof(char));
      strcpy(chunk[j],(char*)((struct Token*)DynArray_get(dyn_ptr,i-count+j))->token_value);
    }
    argc_array[argv_index]=count;
    argv_set[argv_index]=chunk;
    argv_index++;
    i++;
  }
  return argv_index;
}

/*-------------------------------------------------------------------------
change_dir
-----
return value: 0 on failure, otherwise returns 1
parameter: int argc, char* argv[]
this function changes directory if argv[0] is cd
  -------------------------------------------------------------------------*/
int change_dir(int argc, char* argv[])
{
  int res;

  if(argc==1) chdir(getenv("HOME"));
  else if(argc==2){
    res=chdir(argv[1]);
    if(res==-1){
      fprintf(stderr, "./ish: No such file or directory\n");
      printf("%% ");
      fflush(NULL);
      return 0;
    }
  }
  else{
    fprintf(stderr, "./ish: No such file or directory\n");
    printf("%% ");
    fflush(NULL);
    return 0;
  }
  return 1;
}

/*-------------------------------------------------------------------------
my_setenv
-----
return value: 0 on failure, 1 otherwise
parameter: int argc, char* argv[]
this function sets environment variables
  -------------------------------------------------------------------------*/
int my_setenv(int argc, char* argv[])
{
  char* new_var;

  if(argc==3){
    if(strcmp(argv[1],"HOME")==0){
      fprintf(stderr,"setenv failed\n");
      printf("%% ");
      fflush(NULL);
      return 0;
    }
    if(setenv(argv[0],argv[1],1)==-1){
      fprintf(stderr,"setenv failed\n");
      printf("%% ");
      fflush(NULL);
      return 0;
    }
  }
  else if(argc==2){
    if(strcmp(argv[1],"HOME")==0){
      fprintf(stderr,"setenv failed\n");
      printf("%% ");
      fflush(NULL);
      return 0;
    }
    new_var=argv[1];
   if(setenv(new_var,argv[1],1)==-1){
     fprintf(stderr,"setenv failed\n");
     printf("%% ");
     fflush(NULL);
     return 0;
   }
  }
  else{
    fprintf(stderr,"setenv failed\n");
    printf("%% ");
    fflush(NULL);
    return 0;
  }
  return 1;
}

/*-------------------------------------------------------------------------
execute
-----
return value: 0 on failure, 1 otherwise
parameter: char*** argv_set, int number_of_argv, int* argc_array, char* argv[]
this function executes
-------------------------------------------------------------------------*/
int execute(char*** argv_set, int number_of_argv, int* argc_array, char* argv[])
{
   pid_t pid;
   int status;
   static int i=0;
   int p[2];
   int fd=0;
   int cd_res;
   int argc;
   int set_res;
   int unset_res;

   //---if there is only one process without pipe----
   if(number_of_argv==1){
     //------set argv and argc-----------
     argv=argv_set[0];
     argc=argc_array[0];
     //-----make child process-----------
     pid=fork();
     fflush(NULL);
     //------------execute---------------
     if(pid==0){//child
       if(strcmp(argv[0],"cd")==0){
	 cd_res=change_dir(argc, argv);
	 if(cd_res==0){
	   return 0;
	 }
	 printf("%% ");
	 fflush(NULL);
	 return 1;
       }
       else if(strcmp(argv[0],"setenv")==0){
	 set_res=my_setenv(argc, argv);
	 if(set_res==0){
	   return 0;
	 }
	 printf("%% ");
	 fflush(NULL);
	 return 1;
       }
       else if(strcmp(argv[0],"unsetenv")==0){
	 unset_res=unsetenv(argv[1]);
	 if(unset_res!=0){
	   printf("%% ");
	   fflush(NULL);
	   return 0;
	 }
	 printf("%% ");
	 fflush(NULL);
	 return 1;
       }
       else if(strcmp(argv[0],"exit")==0){
	 exit(0);
       }
       else{
	 execvp(argv[0],argv);
	 fprintf(stderr, "%s: No such file or directory\n",argv[0]);
	 printf("%% ");
	 fflush(NULL);
	 return 0;
       }
     }
     else if(pid==-1){
       fprintf(stderr,"failed to make child process\n");
       printf("%% ");
       fflush(NULL);
       return 0;
     }
     else{//parent
       pid=wait(&status);
       printf("%% ");
       fflush(NULL);
       return 1;
     }
   }
   
   //-----if there is at least one pipe---------------
   for(i=0;i<number_of_argv;i++){
     if(pipe(p)==-1) return 0;;
     //-----make child process-----------
     pid=fork();
     fflush(NULL);
     //------------execute---------------
     if(pid==0){//child
       //------set argv and argc---------
       argv=argv_set[i];
       argc=argc_array[i];
       //-----set pipe-------------------
       close(p[0]);
       dup2(fd,0);
       if(i!=number_of_argv-1) dup2(p[1],1);
       if(strcmp(argv[0],"cd")==0){
	 cd_res=change_dir(argc, argv);
	 if(cd_res==0){
	   printf("%% ");
	   fflush(NULL);
	   return 0;
	 }
       }
       else if(strcmp(argv[0],"setenv")==0){
	 set_res=my_setenv(argc, argv);
	 if(set_res==0){
	   printf("%% ");
	   fflush(NULL);
	   return 0;
	 }
       }
       else if(strcmp(argv[0],"unsetenv")==0){
	 unset_res=unsetenv(argv[1]);
	 if((unset_res!=0)){
	   printf("%% ");
	   fflush(NULL);
	   return 0;
	 }
       }
       else if(strcmp(argv[0],"exit")==0){
	 exit(0);
       }
       else {
	 execvp(argv[0],argv);
	 fprintf(stderr, "execution failed\n");
	 fflush(NULL);
	 return 0;
       }
     }
     else if(pid==-1){
       fprintf(stderr,"failed to make child process\n");
       printf("%% ");
       fflush(NULL);
       return 0;
     }
     else{//parent     
       close(p[1]);
       wait(&status);
       fd=p[0];
     }
   }
   printf("%% ");
   fflush(NULL);
   return 0;
}

/*-------------------------------------------------------------------------
main
-----
return value: 0
parameter: int argc, char* argv[]
this function use fgets to get the command line and call proper functions
  -------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{

  DynArray_T dyn_ptr;//dynamic array ptr
  char input[MAX_LINE_SIZE];
  char*** argv_set;
  int number_of_tokens;
  int lex_res;
  int syn_res;
  int number_of_argv;
  int* argc_array=(int*)calloc(MAX_LINE_SIZE,sizeof(int)); //holds each argv's argc values
  char** check_exit;

  printf("%% ");
  fflush(NULL);
  while(fgets(input, MAX_LINE_SIZE, stdin) != NULL){ 
    dyn_ptr = DynArray_new(0); // Create a new dynamic array
    //-----------check--------------------------
    lex_res=lexical_analyzer(input,dyn_ptr);
    if(lex_res){
      syn_res=syntactic_analyzer(dyn_ptr);
      if(syn_res){
	//----------make argv set-------------------
	number_of_tokens=DynArray_getLength(dyn_ptr);
	argv_set=(char***)calloc(number_of_tokens,sizeof(char**));
	number_of_argv=make_argvsets(dyn_ptr,argv_set,argc_array);
	check_exit=argv_set[0];
	if(strcmp(check_exit[0],"exit")==0) exit(0);
	//-----------execute-----------------------
	execute(argv_set,number_of_argv, argc_array, argv);
      }
    }
    //---------free original dyn_ptr------------
    DynArray_map(dyn_ptr, free_token, NULL);
  }
  return 0;
}
