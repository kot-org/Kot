#include <iconv.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>

size_t iconv(iconv_t cd, char **__restrict inbuf, size_t *__restrict inbytesleft, char **__restrict outbuf, size_t *__restrict outbytesleft) {
	(void)inbytesleft;
	(void)outbytesleft;

	if(cd == (iconv_t)1) { // UTF-8 to UTF-8
        size_t input_length = strlen(*inbuf);
        
        if (*outbytesleft < input_length) {
            return (size_t)-1;
        }

        memcpy(*outbuf, *inbuf, input_length);

        *inbuf += input_length;
        *outbuf += input_length;
        *outbytesleft -= input_length;

        return input_length;
	}
	__ensure(!"iconv() not implemented");
	__builtin_unreachable();
}

int iconv_close(iconv_t) {
	return 0;
}

iconv_t iconv_open(const char *tocode, const char *fromcode) {
	if(!strcmp(tocode, "UTF-8") && !strcmp(fromcode, "UTF-8")) {
		iconv_t cd = (iconv_t)1;
		return cd;
	}
	__ensure(!"iconv_open() not implemented");
	__builtin_unreachable();
}


