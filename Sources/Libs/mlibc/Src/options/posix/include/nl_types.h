#ifndef NL_TYPES_H
#define NL_TYPES_H

#include <features.h>

#define NL_SETD 1
#define NL_CAT_LOCALE 1

typedef void* nl_catd;
typedef int nl_item;

extern nl_catd catopen (const char *__cat_name, int __flag) __nonnull ((1));

extern char *catgets (nl_catd __catalog, int __set, int __number, const char *__string) __THROW __nonnull ((1));

extern int catclose (nl_catd __catalog) __THROW __nonnull ((1));

#endif // NL_TYPES_H 