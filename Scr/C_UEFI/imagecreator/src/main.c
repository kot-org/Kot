// Original ImageCreator created by ThatOSDev ( ExoVerse ) - 2020 - 2021
#include <stdio.h>
#include <string.h>
void createblankimage()
{
    char data[512];
    FILE * fPtr;
    fPtr = fopen("drive.hdd", "w");

    int t;
    for(t = 0; t < 524288; t++)
    {
          memset(data, '\0', sizeof(data));
          fwrite(data, sizeof(data), 1, fPtr );
    }
    fclose(fPtr);
}

int main()
{
    createblankimage();
    return 0;
}
