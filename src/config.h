#ifndef CONFIG_H
#define CONFIG_H

#define u_int unsigned int
#define ll long long int
#define u_ll unsigned long long int
#define doc_int unsigned int
// compression method 2 can't landle unsigned long long int since BITSIZE is just 64

#define DEBUG false
// size in bytes (16MB=16777216)
// #define MAX_EXTRA_SIZE_POSTING_LISTS 16777216
// size in bytes of a single posting list that i can safely store in memory and continue with other tasks
// #define SAFE_SIZE 1024

#define MAX_EXTRA_SIZE_POSTING_LISTS 10737418240
// size in bytes of a single posting list that i can safely store in memory and continue with other tasks
#define SAFE_SIZE 2048

//#define MAX_EXTRA_SIZE_POSTING_LISTS 16
//#define SAFE_SIZE 8

#define DELIMITERS " \n,.:;\"(){}[]`'"

#define INDEX_FILES_DIR "."

#define TEMP_DIR_PATH "./temp/"

#define CACHED_DICT_SIZE_BYTES 10737418240
// #define CACHED_DICT_SIZE_BYTES 524288

#endif
