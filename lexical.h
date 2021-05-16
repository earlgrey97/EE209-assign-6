#ifndef _LEXICAL_H
#define _LEXICAL_H_

enum { MAX_LINE_SIZE = 1024 }; /* Including newline character */
enum tokentype { TOKEN_WORD, TOKEN_PIPE };
enum lexstate { IN_WORD, IN_QUOTE, PIPE, IS_SPACE};

/* A Token is either a number or a word, expressed as a string. */
struct Token {
  enum tokentype token_type; // The type of the token
  char* token_value;// The string which is the token's value. 
};

void add_token(DynArray_T dyn_ptr, char* set_value, int set_type);
int lexical_analyzer(char* input, DynArray_T dyn_ptr);

#endif
