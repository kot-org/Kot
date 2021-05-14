#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
int puts(const char *s)
{
  for( ; *s; ++s) if (putchar(*s) != *s) return EOF;
  return (putchar('\n') == '\n') ? 0 : EOF;
}
*/
int puts(const char *p)
{
    write(1, p, strlen (p));
    return 0;
}
