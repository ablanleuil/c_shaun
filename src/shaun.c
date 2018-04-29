#include <shaun.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const shaun_t shaun_null = { SHAUN_NULL, 0 };

int object_index_of(shaun_object_t, string_t);
int object_insert(shaun_object_t *, string_t, shaun_t);

shaun_t shaun_list_get(shaun_t sn, int id)
{
	if (sn.type != SHAUN_LIST || sn.list.size <= id)
		return shaun_null;

	return sn.list.content[id];
}

shaun_t shaun_object_get(shaun_t sn, string_t key)
{
	if (sn.type != SHAUN_OBJECT)
		return shaun_null;

	int id = object_index_of(sn.object, key);
	if (id < 0)
		return shaun_null;

	return sn.object.values[id];
}

int shaun_list_set(shaun_t * sn, int id, shaun_t value)
{
	if (sn->type != SHAUN_LIST || sn->list.size <= id)
		return 0;

	shaun_destroy(sn->list.content[id]);
	sn->list.content[id] = value;

	return 1;
}

int shaun_list_push(shaun_t * sn, shaun_t value)
{
	if (sn->type != SHAUN_LIST)
		return 0;

	if (sn->list.size == sn->list.capacity)
	{
		shaun_t * new_content = (shaun_t*)realloc(sn->list.content
				, (sn->list.capacity+1)*2*sizeof(shaun_t));
		if (!new_content)
			return 0;

		sn->list.content = new_content;
		sn->list.capacity = (sn->list.capacity+1)*2;
	}

	sn->list.content[sn->list.size++] = value;
	return 1;
}

int shaun_object_set(shaun_t * sn, string_t key, shaun_t value)
{
	if (sn->type != SHAUN_OBJECT)
		return 0;

	int id = object_index_of(sn->object, key);
	if (id < 0)
		return 0;

	shaun_destroy(sn->object.values[id]);
	sn->object.values[id] = value;
	return 1;
}

int shaun_object_insert(shaun_t * sn, string_t key, shaun_t value)
{
	if (sn->type != SHAUN_OBJECT)
		return 0;

	int id = object_index_of(sn->object, key);
	if (id >= 0)
		return 0;

	return object_insert(&sn->object, key, value);
}

void shaun_destroy(shaun_t sn)
{
	if (sn.type == SHAUN_LIST)
	{
		int i;
		for (i = 0; i < sn.list.size; ++i)
			shaun_destroy(sn.list.content[i]);
		free(sn.list.content);
	}

	if (sn.type == SHAUN_OBJECT)
	{
		int i;
		for (i = 0; i < sn.list.size; ++i)
			shaun_destroy(sn.list.content[i]);
		free(sn.object.keys);
		free(sn.object.values);
	}
}

int object_index_of(shaun_object_t o, string_t k)
{
	if (o.size == 0)
		return -1;

	int beg = 0, end = o.size-1;
	while (beg <= end)
	{
		int mid = (beg + end) / 2;
		int cmp = string_cmp(k, o.keys[mid]);
		if (cmp > 0)
			beg = mid + 1;
		else if (cmp < 0)
			end = mid - 1;
		else
			return mid;
	}


	return -1;
}

int object_insert(shaun_object_t * o, string_t k, shaun_t v)
{
	if (o->capacity == o->size)
	{
		string_t * new_keys = (string_t*)realloc(o->keys, (o->capacity+1)*2*sizeof(string_t));
		if (!new_keys)
			return 0;
		o->keys = new_keys;

		shaun_t * new_values = (shaun_t*)realloc(o->values, (o->capacity+1)*2*sizeof(shaun_t));
		if (!new_values)
			return 0;
		o->values = new_values;
		o->capacity = (o->capacity+1)*2;
	}

	int beg = 0, end = o->size-1;
	while (beg <= end)
	{
		int mid = (beg + end) / 2;
		int cmp = string_cmp(k, o->keys[mid]);
		if (cmp > 0)
			beg = mid + 1;
		else if (cmp < 0)
			end = mid - 1;
	}

	o->size++;
	int i;
	for (i = o->size-1; i > beg; --i)
	{
		o->values[i] = o->values[i-1];
		o->keys[i] = o->keys[i-1];
	}
	o->keys[beg] = k;
	o->values[beg] = v;
	return 1;
}

shaun_t shaun_create_object()
{
	shaun_t ret;
	ret.type = SHAUN_OBJECT;
	ret.object.values = NULL;
	ret.object.keys = NULL;
	ret.object.capacity = 0;
	ret.object.size = 0;

	return ret;
}

shaun_t shaun_create_list()
{
	shaun_t ret;
	ret.type = SHAUN_LIST;
	ret.list.content = NULL;
	ret.list.capacity = 0;
	ret.list.size = 0;

	return ret;
}

shaun_t shaun_create_number(double value, string_t unit)
{
	shaun_t ret;
	ret.type = SHAUN_NUMBER;
	ret.number.value = value;
	ret.number.unit = unit;
	
	return ret;
}

shaun_t shaun_create_bool(int boolean)
{
	shaun_t ret;
	ret.type = SHAUN_BOOL;
	ret.boolean = boolean;

	return ret;
}

shaun_t shaun_create_null()
{
	shaun_t ret;
	ret.type = SHAUN_NULL;

	return ret;
}

shaun_t shaun_create_string(string_t string)
{
	shaun_t ret;
	ret.type = SHAUN_STRING;
	ret.string = string;

	return ret;
}


void shaun_print(shaun_t sn)
{
	int i;
	switch (sn.type)
	{
		case SHAUN_NUMBER:
			printf("%f", sn.number.value);
			string_print(sn.number.unit);
			break;
		case SHAUN_STRING:
			printf("\"");
			string_print(sn.string);
			printf("\"");
			break;
		case SHAUN_BOOL:
			if (sn.boolean)
				printf("true");
			else
				printf("false");
			break;
		case SHAUN_LIST:
			printf("[ ");
			for (i = 0; i < sn.list.size; ++i)
			{
				shaun_print(sn.list.content[i]);
				if (i != sn.list.size-1)
					printf(", ");
			}
			printf(" ]");
			break;
		case SHAUN_OBJECT:
			printf("{ ");
			for (i = 0; i < sn.object.size; ++i)
			{
				string_print(sn.object.keys[i]);
				printf(": ");
				shaun_print(sn.object.values[i]);
				if (i != sn.object.size-1)
					printf(", ");
			}
			printf(" }");
			break;
		case SHAUN_NULL:
			printf("null");
			break;
		default: break;
	}
}
