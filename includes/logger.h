#ifndef STD_IO
#include <stdio.h>
#endif

#define LOG(...)                                                               \
  do {                                                                         \
    fprintf(stdout, __VA_ARGS__);                                              \
  } while (0)

#define LOG_ERR(...)                                                           \
  do {                                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
  } while (0)
