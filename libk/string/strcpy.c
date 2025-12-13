#include <string.h>

char *strcpy(char *dest, const char *src)
{
	int i = 0;
    while (1)
    {
        dest[i] = src[i];

        if (dest[i] == '\0')
        {
            break;
        }

        i++;
    }
    return dest;
}

char *strncpy(char *dst, const char* src, register size_t n)
{
	if (n != 0) {
		register char *d = dst;
		register const char *s = src;

		do {
			if ((*d++ = *s++) == 0) {
				/* NUL pad the remaining n-1 bytes */
				while (--n != 0)
					*d++ = 0;
				break;
			}
		} while (--n != 0);
	}
	return (dst);
}