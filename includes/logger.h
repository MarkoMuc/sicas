#ifndef STD_IO
#include <stdio.h>
#endif

// macros

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
    fprintf(stderr,"\n");         \
    exit(1);                      \
  } while(0)

#define LOG_XERR(...)              \
  do {                            \
    fprintf(stderr, __VA_ARGS__); \
    exit(1);                      \
  } while(0)

#define LOG_LERR(sl, l,...) \
  do {                      \
    fprintf(stderr, "[%ld,%ld]:[%ld,%ld] ", (sl).s_row, (sl).s_col, (l).e_row, (l).e_col);\
    LOG_ERR(__VA_ARGS__);\
  } while(0);

#define LOG_XLERR(sl, l,...) \
  do {                      \
    fprintf(stderr, "[%ld,%ld]:[%ld,%ld] ", (sl).s_row, (sl).s_col, (l).e_row, (l).e_col);\
    LOG_XERR(__VA_ARGS__);\
  } while(0);
