#ifndef BUF_H
#define BUF_H

#include <stddef.h>

typedef enum buf_type {
  TRI = 0,
  TRI_DATA,
  INDEX,
  BVH_NODE,
  TLAS_NODE,
  INST,
  MAT
} buf_type;

void    buf_init(size_t buf_cnt);
void    buf_reserve(buf_type type, size_t buf_type_size, size_t elem_cnt);
void    *buf_acquire(buf_type type, size_t elem_cnt);
void    *buf_ptr(buf_type type, size_t idx);
size_t  buf_idx(buf_type type, void *ptr);
size_t  buf_len(buf_type type);
void    buf_release();

#endif
