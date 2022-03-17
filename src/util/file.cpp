#include "file.h"

#include <string.h>


int source_file(FILE * __restrict file, char* __restrict ptr, int & n) {
    n = 0;
    int c;
    while ( (c = fgetc(file)) && c != EOF ) {
        ptr[n++] = c;
    }
    // Zero temrminate
    ptr[n] = 0;
    return 0;
}

int get_resource(char const* __restrict path, char* __restrict source, int & n) {
    int bytes = strlen(path);
    memcpy(RESOURCE.DIR + RESOURCE.BYTES, path, bytes);

    // Source file
    FILE* sfile = fopen(RESOURCE.DIR, "r");
    if (!sfile) {
        fprintf(stderr, "Couldn't read the file: %s\n", RESOURCE.DIR);
        return -1;
    }
    int status = source_file(sfile, source, n);

    // Failed to read the file
    if (status) {
        fprintf(stderr, "Couldn't get the resource: %s\n", RESOURCE.DIR);
    }

    fclose(sfile);
    RESOURCE.DIR[RESOURCE.BYTES] = 0;
    return status;
}
