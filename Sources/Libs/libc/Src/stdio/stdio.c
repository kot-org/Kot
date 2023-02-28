#include <kot/stdio.h>

int sprintf(char *str, const char *format, ...){
    va_list args;
    int i, j, k;

    va_start(args, format);

    i = j = 0;
    while (format[i] != '\0') {
        if (format[i] == '%') {
        i++;
        switch (format[i]) {
            case 'c': {
                char c = va_arg(args, int);
                str[j] = c;
                break;
            }
            case 'd': {
                int d = va_arg(args, int);
                k = 0;
                if (d < 0) {
                    str[j++] = '-';
                    d = -d;
                }
                int power_of_10 = 1;
                while (power_of_10 * 10 <= d) {
                    power_of_10 *= 10;
                }
                while (power_of_10 > 0) {
                    int digit = d / power_of_10;
                    str[j++] = digit + '0';
                    d %= power_of_10;
                    power_of_10 /= 10;
                }
                break;
            }
            case 'f': {
                double f = va_arg(args, double);
                int d1, d2;
                d1 = (int)f;
                d2 = (int)((f - d1) * 1000000);
                k = 0;
                if (d1 < 0) {
                    str[j++] = '-';
                    d1 = -d1;
                }
                do {
                    str[j++] = d1 % 10 + '0';
                    k++;
                } while ((d1 /= 10) > 0);
                for (k = k / 2; k > 0; k--) {
                    char c = str[j - k - 1];
                    str[j - k - 1] = str[j - k];
                    str[j - k] = c;
                }
                str[j++] = '.';
                k = 0;
                do {
                    str[j++] = d2 % 10 + '0';
                    k++;
                } while ((d2 /= 10) > 0);
                for (k = k / 2; k > 0; k--) {
                    char c = str[j - k - 1];
                    str[j - k - 1] = str[j - k];
                    str[j - k] = c;
                }
                break;
            }
            case 's': {
                char *s = va_arg(args, char*);
                for (k = 0; s[k] != '\0'; k++) {
                str[j++] = s[k];
                }
                break;
            }
            default: {
                str[j++] = '%';
                str[j++] = format[i];
                break;
                }
            }
        } else {
        str[j++] = format[i];
        }
    i++;
    }

    str[j] = '\0';

    va_end(args);

    return j;
}