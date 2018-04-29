#ifndef LEXER_H
#define LEXER_H

#include "str.h"

enum { TOK_NAME, TOK_NUM, TOK_CHAR, TOK_STRING };

typedef struct token
{
	int type;
	union
	{
		string_t string_value;
		char char_value;
		double num_value;
	};
} token;

typedef struct token_list
{
	token tok;
	struct token_list * next;
} token_list;

token_list * lex_raw(const char * str);

void token_print(token tok);

#endif // LEXER_H
