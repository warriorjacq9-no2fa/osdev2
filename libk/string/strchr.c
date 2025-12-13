#include <string.h>
#include <stddef.h>

char *strchr(const char *s, int c)
{
    while (*s != (char) c) {
        if (!*s++) {
            return NULL;
        }
    }
    return (char *)s;
}

char *strrchr(register const char *s, int c)
{
  char *ret = 0;

  do {
    if (*s == c)
      ret = (char*) s;
  } while (*s++);
  return (ret);
}