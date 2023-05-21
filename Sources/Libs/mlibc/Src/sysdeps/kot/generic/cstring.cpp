#include <kot/cstring.h>

// alpha size of int
int64_t asi(int64_t n, int basenumber){
    int64_t i = 0;
    do {
        i++;
    } while(n /= basenumber);
    return i;
}

char* itoa(int64_t n, char* buffer, int basenumber){
	int64_t hold;
	int64_t i, j;
	hold = n;
	i = 0;

    bool IsNegative = (n < 0);

    if(IsNegative){
        n = -n;
    }
	
	do{
		hold = n % basenumber;
		buffer[i++] = (hold < 10) ? (hold + '0') : (hold + 'a' - 10);
	} while(n /= basenumber);

    if(IsNegative){
        buffer[i++] = '-';
    }

	buffer[i--] = NULL;
	
	for(j = 0; j < i; j++, i--)
	{
		hold = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = hold;
	}


    return buffer;
}