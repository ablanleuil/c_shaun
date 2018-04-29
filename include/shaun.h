#ifndef SHAUN_H
#define SHAUN_H

#include "str.h"

enum { SHAUN_NUMBER, SHAUN_STRING, SHAUN_BOOL, SHAUN_LIST, SHAUN_NULL, SHAUN_OBJECT };

typedef struct shaun_number_t shaun_number_t;
typedef struct shaun_list_t shaun_list_t;
typedef struct shaun_t shaun_t;
typedef struct shaun_object_t shaun_object_t;

struct shaun_number_t
{
	double value;
	string_t unit;
};

struct shaun_list_t
{
	shaun_t * content;
	int capacity;
	int size;
};

struct shaun_object_t
{
	string_t * keys;
	shaun_t  * values;
	int capacity;
	int size;
};

struct shaun_t
{
	int type;
	union {
		int boolean;
		shaun_number_t number;
		string_t string;
		shaun_list_t list;
		shaun_object_t object;
	};
};

shaun_t shaun_list_get(shaun_t, int);
shaun_t shaun_object_get(shaun_t, string_t);

int shaun_list_set(shaun_t *, int, shaun_t);
int shaun_list_push(shaun_t *, shaun_t);

int shaun_object_set(shaun_t *, string_t, shaun_t);
int shaun_object_insert(shaun_t *, string_t, shaun_t);

void shaun_destroy(shaun_t);

shaun_t shaun_create_object();
shaun_t shaun_create_list();
shaun_t shaun_create_number(double, string_t);
shaun_t shaun_create_bool(int);
shaun_t shaun_create_null();
shaun_t shaun_create_string(string_t);

void shaun_print(shaun_t);

#endif // SHAUN_H
