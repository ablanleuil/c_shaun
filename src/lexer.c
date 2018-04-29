#include <lexer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void list_append_char(token_list ** ret, char c)
{
	token_list * l = (token_list*)malloc(sizeof(token_list));
	l->tok.type = TOK_CHAR;
	l->tok.char_value = c;
	l->next = NULL;
	(*ret)->next = l;
	*ret = l;
}

void list_append_name(token_list ** ret, string_t s)
{
	token_list * l = (token_list*)malloc(sizeof(token_list));
	l->tok.type = TOK_NAME;
	l->tok.string_value = s;
	l->next = NULL;
	(*ret)->next = l;
	*ret = l;
}

void list_append_num(token_list ** ret, double s)
{
	token_list * l = (token_list*)malloc(sizeof(token_list));
	l->tok.type = TOK_NUM;
	l->tok.num_value = s;
	l->next = NULL;
	(*ret)->next = l;
	*ret = l;
}

void list_append_string(token_list ** ret, string_t s)
{
	token_list * l = (token_list*)malloc(sizeof(token_list));
	l->tok.type = TOK_STRING;
	l->tok.string_value = s;
	l->next = NULL;
	(*ret)->next = l;
	*ret = l;
}

string_t lexer_name(const char ** str)
{
	const char * it = *str;

	while (isalnum(*it) || *it == '_')
		++it;

	string_t ret = string_from_raw_n(*str, it-*str);

	*str = it;

	return ret;
}

string_t lexer_string(const char ** str)
{
	const char * it = *str+1;

	while (*it != '"')
	{
		if (*it == '\\')
			++it;
		++it;
	}

	char * raw_buffer = (char*)malloc(it - *str)
	   , * buffer_write = raw_buffer;
	
	it = *str+1;
	while (*it != '"')
	{
		char c = *it;
		if (c == '\\')
		{
			switch (*(++it))
			{
				case 'n' : c = '\n'; break;
				case 't' : c = '\t'; break;
				case '"' : c = '"' ; break;
				case '\\': c = '\\'; break;
				case 'r' : c = '\r'; break;
				default  : c = *it ; break;
			}
		}

		*(buffer_write++) = c;
		++it;
	}

	*buffer_write = 0;

	string_t ret = string_from_raw(raw_buffer);

	free(raw_buffer);

	*str = it+1;
	return ret;
}

double lexer_num(const char ** str)
{
	const char * it = *str;
	float num;
	int point_encounter;


	int read = sscanf(*str, "%f", &num);

	while (isdigit(*it) || *it == '.' || *it == 'e' || *it == 'E' || *it == '+' || *it == '-')
		++it;

	double ret = num;
	*str = it;
	return ret;
}

token_list * lex_raw(const char * str)
{
	token_list fst;
	fst.next = NULL;
	token_list * ret = &fst;
	while (*str)
	{
		switch (*str)
		{
			case ':':
			case ',':
			case '{':
			case '}':
			case '[':
			case ']':
				list_append_char(&ret, *str);
				++str;
				break;
			case '"':
			{
				string_t s = lexer_string(&str);
				list_append_string(&ret, s);
				break;
			}
			default:
				if (isdigit(*str))
				{
					double s = lexer_num(&str);
					list_append_num(&ret, s);
				}
				if (isalpha(*str) || *str == '_')
				{
					string_t s = lexer_name(&str);
					list_append_name(&ret, s);
				}
				if (isspace(*str) || *str == ',')
					++str;
		}
	}

	return fst.next;
}

void token_print(token tok)
{
	switch (tok.type)
	{
		case TOK_NAME:
			string_print(tok.string_value);
			break;
		case TOK_NUM:
			printf("%f", tok.num_value);
			break;
		case TOK_STRING:
			printf("\""); string_print(tok.string_value); printf("\"");
			break;
		case TOK_CHAR:
			printf("%c", tok.char_value);
			break;
	}
}
