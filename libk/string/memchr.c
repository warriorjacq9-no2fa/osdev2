#include <string.h>

void *
memchr (void *src_void, int c, size_t length)
{
  unsigned char *src = (unsigned char *)src_void;
  
  while (length-- > 0)
  {
    if (*src == c)
     return (void *)src;
    src++;
  }
  return NULL;
}