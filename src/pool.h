#ifndef POOL_H
#define POOL_H

#include <stddef.h>

typedef enum buf_type {
  TRI = 0,
  TRI_DATA,
  INDEX,
  BVH_NODE,
  BVH_INST,
  TLAS_NODE,
  MAT
} buf_type;

void    pool_init(size_t type_cnt);
void    pool_reserve(buf_type type, size_t type_sz, size_t total_cnt);
void    *pool_acquire(buf_type type, size_t cnt);
void    *pool_ptr(buf_type type, size_t ofs);
size_t  pool_ofs(buf_type type, void *ptr);
void    pool_release();

#endif
