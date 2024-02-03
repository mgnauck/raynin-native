#include "pool.h"
#include "sutil.h"

struct buf {
  char    *base;
  size_t  sz;
  size_t  ofs;
} *buf;

size_t buf_type_cnt = 0;

void pool_init(size_t type_cnt)
{
  buf_type_cnt = type_cnt;

  buf = malloc(buf_type_cnt * sizeof(*buf));

  for(size_t i=0; i<buf_type_cnt; i++)
    buf[i].base = NULL;
}

void pool_reserve(buf_type type, size_t type_size, size_t total_cnt)
{
  buf[type].base = malloc(total_cnt * type_size);
  buf[type].sz = type_size;
  buf[type].ofs = 0;
}

void *pool_acquire(buf_type type, size_t cnt)
{
  size_t curr_ofs = buf[type].ofs;
  buf[type].ofs += cnt;
  return buf[type].base + curr_ofs * buf[type].sz;
}

void *pool_ptr(buf_type type, size_t ofs)
{
  return buf[type].base + ofs * buf[type].sz;
}

size_t pool_ofs(buf_type type, void *ptr)
{
  return ((char *)ptr - buf[type].base) / buf[type].sz;
}

void pool_release()
{
  for(size_t i=0; i<buf_type_cnt; i++)
    free(buf[i].base);
  free(buf);
  buf_type_cnt = 0;
}
