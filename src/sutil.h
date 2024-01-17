#ifndef SUTIL_H
#define SUTIL_H

#include <stddef.h>

#ifdef NOSTDLIB

void *malloc(size_t size);
void free(void *ptr);

void *memset(void *dest, int c, size_t cnt);
void *memcpy(void *dest, const void *src, size_t cnt);

extern double time();

#else

#include <stdlib.h>

#endif

#endif
