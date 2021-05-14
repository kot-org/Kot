#pragma once
#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include <stddef.h>

int memcmp(const void *aptr, const void *bptr, size_t n)
{
	const unsigned char *a = aptr, *b = bptr;
	for (size_t i = 0; i < n; i++)
	{
		if (a[i] < b[i])
			return -1;
		else if (a[i] > b[i])
			return 1;
	}
	return 0;
}

int strlen(char *s)
{
	int z = 0;
	for (int i = 0; s[i] != 0; i++)
		z = i;
	return z;
}

UINTN strcmp(CHAR8* a,CHAR8* b,UINTN length) {
	for(UINTN i = 0;i < length;i++) {
		if(*a != *b) return 0;
	}
	return 1;
}
