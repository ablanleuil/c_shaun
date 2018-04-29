#include <lexer.h>
#include <parser.h>
#include <shaun.h>

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
	FILE * f = fopen(argv[1], "r");
	char buf[4096];
	int read = fread(buf, 1, 4065, f);

	if (!feof(f))
		return -1;

	buf[read] = 0;

	shaun_t obj = parse_raw(buf);
	shaun_print(obj);

	printf("\n");
	string_print_stats();

	return 0;
}
