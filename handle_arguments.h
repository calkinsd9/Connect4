#ifndef H_HANDLE_ARGUMENTS
#define H_HANDLE_ARGUMENTS

#include <argp.h>

struct arguments {
	int width;
	int height;
	int square;
	int connect;
	char* load;
};

error_t parse_opt(int key, char* arg, struct argp_state* state);

void setup(int argc, char** argv, struct arguments* arguments_main);

#endif
