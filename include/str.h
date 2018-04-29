#ifndef STR_H
#define STR_H

#include <stddef.h>

typedef struct string_t
{
	const char * raw;
	size_t offset;
	size_t length;
} string_t;

string_t string_from_raw(const char * raw);
string_t string_from_raw_n(const char * raw, size_t size);
string_t string_substr(string_t s, size_t offset, size_t length);
char * string_to_raw(string_t s);
int string_cmp(string_t s1, string_t s2);
int string_cmp_raw(string_t s1, const char * raw);
void string_print(string_t s);

void string_print_stats();

#endif // STR_H
