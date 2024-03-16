#ifndef COMMON_MACROS
#define COMMON_MACROS

#define KILOBYTES(val) ((val)*1024LL)
#define MEGABYTES(val) (KILOBYTES(val)*1024LL)
#define GIGABYTES(val) (MEGABYTES(val)*1024LL)
#define TERABYTES(val) (GIGABYTES(val)*1024LL)

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#define NULL 0

#endif
