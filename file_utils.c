#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int read_file(char* filename, char** buffer){
	FILE* ptr_file;
	struct stat st;
	int size;

	ptr_file = fopen(filename, "r");

	/* CHeck that file has been opened */
	if (!ptr_file){
		fprintf(stderr, "Input file could not be opened.\n");
		return -1;
	}

	/* Get file size */
	stat(filename, &st);
	size = st.st_size;

	/* Read from the third param (FILE*) and store data int othe string pointed to by the first param (char*) */
	fgets(*buffer, size, ptr_file);

	return size;
}

int write_file(char* filename, char* buffer, int size){
	FILE* ptr_file;
	int i;

	ptr_file = fopen(filename, "w");

	if (!ptr_file){
		fprintf(stderr, "Output file cold not be opened.\n");
		return -1;
	}

	for (i = 0; i<size; i++){
		fputc( (int)buffer[i], ptr_file);
	}

	return 0;
}
