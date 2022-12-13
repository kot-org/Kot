#include <kot/uisd/srvs/storage.h>

KResult fopen(char* Path, char* Mode){

}

KResult fclose(file_t* File){

}

KResult fread(uintptr_t Buffer, size_t BlocSize, size_t BlocCount, file_t* File){

}

KResult fwrite(uintptr_t Buffer, size_t BlocSize, size_t BlocCount, file_t* File){

}


KResult opendir(char* Path){

}

KResult readdir(directory_t* Directory){

}

KResult closedir(directory_t* Directory){

}

KResult removefile(char* Path){

}

KResult rename(char* OldName, char* NewName){

}


KResult mkdir(char* Path, mode_t Mode){

}

KResult rmdir(char* Path){

}