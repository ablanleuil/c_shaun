#include <parser.h>
#include <lexer.h>

#include <stdio.h>
#include <stdlib.h>

#define PARSE_ASSERT(cond, msg) \
	if (!(cond)) \
	{ \
		fprintf(stderr, "%s\n", msg); \
		exit(-1); \
	}

shaun_t parse_number(token_list **);
shaun_t parse_raw_object(token_list **);
shaun_t parse_object(token_list **);
shaun_t parse_list(token_list **);
shaun_t parse_value(token_list **);


int is_char(char c, token t)
{
	if (t.type == TOK_CHAR)
		return c == t.char_value;
	return 0;
}

int is_name(token t)
{
	return t.type == TOK_NAME;
}

int is_string(token t)
{
	return t.type == TOK_STRING;
}

int is_number(token t)
{
	return t.type == TOK_NUM;
}

shaun_t parse_raw(const char * raw)
{
	token_list * tokens = lex_raw(raw);
	if (!tokens)
		return shaun_create_null();
	
	if (is_char('{', tokens->tok))
	{
		return parse_object(&tokens);
	}
	else
		return parse_raw_object(&tokens);
}

shaun_t parse_object(token_list ** tokens)
{
	PARSE_ASSERT(is_char('{', (*tokens)->tok), "Expected an opening bracket");
	*tokens = (*tokens)->next;
	shaun_t obj = parse_raw_object(tokens);
	PARSE_ASSERT(is_char('}', (*tokens)->tok), "Expected a closing bracket");
	*tokens = (*tokens)->next;
	return obj;
}

shaun_t parse_raw_object(token_list ** tokens)
{
	shaun_t ret = shaun_create_object();
	while (*tokens && !is_char('}', (*tokens)->tok))
	{
		PARSE_ASSERT(is_name((*tokens)->tok), "Expected an attribute name");
		string_t name = (*tokens)->tok.string_value;
		*tokens = (*tokens)->next;

		PARSE_ASSERT(is_char(':', (*tokens)->tok), "Expected an attribute separator");
		*tokens = (*tokens)->next;

		shaun_t value = parse_value(tokens);
		shaun_object_insert(&ret, name, value);
	}

	return ret;
}

shaun_t parse_value(token_list ** tokens)
{
	token tok = (*tokens)->tok;
	if (is_char('[', tok))
		return parse_list(tokens);
	if (is_char('{', tok))
		return parse_object(tokens);
	if (is_name(tok) && !string_cmp_raw(tok.string_value, "true"))
	{
		(*tokens) = (*tokens)->next;
		return shaun_create_bool(1);
	}
	if (is_name(tok) && !string_cmp_raw(tok.string_value, "false"))
	{
		(*tokens) = (*tokens)->next;
		return shaun_create_bool(0);
	}
	if (is_name(tok) && !string_cmp_raw(tok.string_value, "null"))
	{
		(*tokens) = (*tokens)->next;
		return shaun_create_null();
	}
	if (is_number(tok))
		return parse_number(tokens);
	if (is_string(tok))
	{
		shaun_t ret = shaun_create_string(tok.string_value);
		(*tokens) = (*tokens)->next;
		return ret;
	}

}

shaun_t parse_list(token_list ** tokens)
{
	PARSE_ASSERT(is_char('[', (*tokens)->tok), "Expected left hook");
	*tokens = (*tokens)->next;

	shaun_t ret = shaun_create_list();
	while (*tokens != NULL && !is_char(']', (*tokens)->tok))
	{
		shaun_list_push(&ret, parse_value(tokens));
	}

	PARSE_ASSERT(is_char(']', (*tokens)->tok), "Expected right hook");
	*tokens = (*tokens)->next;

	return ret;
}

shaun_t parse_number(token_list ** tokens)
{
	PARSE_ASSERT(is_number((*tokens)->tok), "Expected a number");
	double val = (*tokens)->tok.num_value;
	*tokens = (*tokens)->next;

	if (is_name((*tokens)->tok))
	{
		if (!(*tokens)->next || !is_char(':', (*tokens)->next->tok))
		{
			string_t s = (*tokens)->tok.string_value;
			(*tokens) = (*tokens)->next;
			return shaun_create_number(val, s);
		}
	}

	return shaun_create_number(val, string_from_raw(""));
}
