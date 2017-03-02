#include <stdlib.h>
#include "handle_arguments.h"

const char* argp_program_version = "1.5.0";
const char* argp_program_bug_address = "calkinda@mail.gvsu.edu";
char doc[] = "Connect 4";
char args_doc[] = "";

struct argp_option options[] = {
	{"width", 'w', "WIDTH", 0, "sets the width of the board"},
	{"height", 'h', "HEIGHT", 0, "sets the height of the board"},
	{"square", 's', "SQUARE", 0, "sets the width and height of a square board"},
	{"connect", 'c', "CONNECT", 0, "sets the number of spaces needed to win"},
	{"load", 'l', "FILE", 0, "load a game"},
	{ 0 }
};

struct argp argp = { options, parse_opt, args_doc, doc };

struct arguments arguments;

error_t parse_opt(int key, char* arg, struct argp_state* state){
	struct arguments* arguments = state->input;
	switch(key){
		case 'w':
			arguments->width = atoi(arg);
			break;
		case 'h':
			arguments->height = atoi(arg);
			break;
		case 's':
			arguments->square = atoi(arg);
			break;
		case 'c':
			arguments->connect = atoi(arg);
			break;
		case 'l':
			arguments->load = arg;
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

void setup(int argc, char** argv, struct arguments* arguments_main){
	int width = 0;
	int height = 0;
	int square = 0;
	int connect = 0;
	char* load = NULL;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	width = arguments.width;
	height = arguments.height;
	square = arguments.square;
	connect = arguments.connect;

	//debug
	//printf("width: %d\nheight: %d\nsquare: %d\nconnect: %d\n\n", width, height, square, connect);

	if(arguments.load != NULL){
		printf("oad file: %s\n\n", arguments.load);
	}

	*arguments_main = arguments;
}
