#pragma once

#include <stdio.h>


// - OH, no, you can't have global state - it's so evil.
// - Ha ha static go brrr
struct Resource {
    char DIR[512];  // resource/ directory
    int  BYTES;     // Bytes used to store the directory name including the 0 terminator
};

// The resources. Access only from 'main.cpp'
// and 'file.cpp'
extern Resource RESOURCE;

/// Read the content of the file to ptr.
/// @param n bytes written
/// @returns The read status
int source_file(FILE * file, char* ptr, int & n);

/**
 *  @param path to the resource
 *  @param bytes bytes of path
 *  @param n elements read
 */
int get_resource(char const* path, char* source, int & n);
