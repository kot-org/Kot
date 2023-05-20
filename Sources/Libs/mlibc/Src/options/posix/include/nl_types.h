#ifndef NL_TYPES_H
#define NL_TYPES_H

#include <features.h>

#define NL_SETD 1
#define NL_CAT_LOCALE 1

typedef void* nl_catd;
typedef int nl_item;

nl_catd catopen (const char *__cat_name, int __flag){
    return NULL;
}

char *catgets (nl_catd __catalog, int __set, int __number, const char *__string){
    return NULL;
}

int catclose (nl_catd __catalog){
    return -1;
}

#endif // NL_TYPES_H 