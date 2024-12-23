#ifndef STD_IO
#include <stdio.h>
#endif

#define LOG(...)                  \
  do {                            \
    fprintf(stdout, __VA_ARGS__); \
  } while(0)

#define LOG_ERR(...)              \
  do {                            \
    fprintf(stderr, __VA_ARGS__); \
  } while(0)

#define LOG_PANIC(...)            \
  do {                            \
    fprintf(stderr, "%s:%d: Panicked in function %s with error:\n",__FILE__, __LINE__, __func__); \
    fprintf(stderr, __VA_ARGS__); \
    exit(1);                      \
  } while(0)
