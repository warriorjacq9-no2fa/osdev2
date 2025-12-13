#ifndef _STRING_H
#define _STRING_H 1

#include <kernel/arch.h>

#define NULL (void*)0

#include <stddef.h>

#define ISUPPER(c) (64 < c && c < 91)
#define ISLOWER(c) (96 < c && c < 123)
#define ISALPHA(c) ((64 < c && c < 91) || 96 < c && c < 123)
#define ISDIGIT(c) (47 < c && c < 58)
#define ISSPACE(c) ((8 < c && c < 14) || c == 32)

void* memchr(void *src_void, int c, size_t length);
int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
char *strcat(char *dest, const char *src);
char *strncat(char *s1, const char *s2, size_t n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dst, const char* src, register size_t n);
size_t strlen(const char*);
char *strchr(const char *s, int c);
char *strrchr(register const char *s, int c);
int strcmp(const char *s1, const char *s2);
char* strtok(char *s, char *delim);
char* strpbrk (const char *s, const char *accept);
size_t strspn (const char *s, const char *accept);
unsigned long strtoul(const char *nptr, char **endptr, int base);

#endif