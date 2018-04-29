#include <str.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define TABLE_SIZE 10

typedef struct strlist
{
	const char * str;
	struct strlist * next;
} strlist;

strlist * table[TABLE_SIZE];
int libinit = 0;

void ht_init()
{
	size_t i;
	for (i = 0; i < TABLE_SIZE; ++i)
		table[i] = NULL;
}

size_t ht_hash(const char * obj)
{
	long long n = 0;
	while (*obj)
		n += *(obj++);
	return n % TABLE_SIZE;
}

const char * ht_insert(const char * raw)
{
	if (!libinit)
	{
		ht_init();
		libinit = 1;
	}

	size_t len = strlen(raw);
	size_t id = ht_hash(raw);
	strlist * fst = table[id];

	if (fst == NULL)
	{
		fst = (strlist*)malloc(sizeof(strlist));
		fst->next = NULL;
		fst->str = strdup(raw);

		table[id] = fst;

		return fst->str;
	}

	while (1)
	{
		if (!strncmp(raw, fst->str, len) && (len != 0 || *fst->str == 0))
		{
			return fst->str;
		}

		if (fst->next == NULL)
		{
			strlist * item = (strlist*)malloc(sizeof(strlist));
			item->next = NULL;
			item->str  = strdup(raw);
			
			fst->next = item;

			return item->str;
		}

		fst = fst->next;
	}
}

const char * ht_insert_n(const char * raw, size_t size)
{
	char * buffer = (char*)malloc(size+1);
	memcpy(buffer, raw, size);
	buffer[size] = 0;
	const char * ret = ht_insert(buffer);
	free(buffer);
	return ret;
}

string_t string_from_raw(const char * raw)
{
	const char * ptr = ht_insert(raw);
	string_t str = { ptr, 0, strlen(raw) };
	return str;
}

string_t string_from_raw_n(const char * raw, size_t size)
{
	const char * ptr = ht_insert_n(raw, size);
	string_t str = { ptr, 0, size };
	return str;
}

string_t string_substr(string_t s, size_t offset, size_t length)
{
	if (offset < 0 || length <= 0 || offset >= s.length)
	{
		string_t ret = { NULL, 0, 0 };
		return ret;
	}
	if (offset+length >= s.length)
	{
		string_t ret = { s.raw, offset, s.length-offset+1 };
		return ret;
	}

	string_t ret = { s.raw, offset, length };
	return ret;
}

char * string_to_raw(string_t s)
{
	if (s.raw == NULL) return NULL;
	return strndup(s.raw+s.offset, s.length);
}

int string_cmp(string_t s1, string_t s2)
{
	if (s1.raw == s2.raw && s1.offset == s2.offset)
		return s1.length - s2.length;
	
	char a = s1.raw[s1.offset+s1.length], b = s2.raw[s2.offset+s2.length];

	((char*)s1.raw)[s1.offset+s1.length] = 0;
	((char*)s2.raw)[s2.offset+s2.length] = 0;

	int ret = strcmp(s1.raw+s1.offset, s2.raw+s2.offset);

	((char*)s1.raw)[s1.offset+s1.length] = a;
	((char*)s2.raw)[s2.offset+s2.length] = b;

	return ret;
}

int string_cmp_raw(string_t s1, const char * raw)
{
	char a = s1.raw[s1.offset+s1.length];
	((char*)s1.raw)[s1.offset+s1.length] = 0;
	int ret = strcmp(s1.raw+s1.offset, raw);
	((char*)s1.raw)[s1.offset+s1.length] = a;

	return ret;
}

void string_print(string_t s)
{
	printf("%.*s", s.length, s.raw+s.offset);
}

void string_print_stats()
{
	int i, max = 0, min = INT_MAX, num = 0;
	int slot_num[TABLE_SIZE] = {0};
	for (i = 0; i < TABLE_SIZE; ++i)
	{
		printf("table[%d]: ", i);
		strlist * l = table[i];
		while (l)
		{
			slot_num[i]++;
			printf("\"%s\" -> ", l->str);
			l = l->next;
		}
		num += slot_num[i];
		max = max < slot_num[i] ? slot_num[i] : max;
		min = min > slot_num[i] ? slot_num[i] : min;
		printf("[]\n");
	}

	printf("Total number of strings: %d\n"
	       "Minimum list size: %d\n"
	       "Maximum list size: %d\n", num, min, max);
}
