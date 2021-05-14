#include "kfs.h"

namespace FileSystem{

    /* KFS */
    KFS::KFS(){
       GPT::AssignNamePartitonsGUID();
    }

    File* KFS::OpenFile(char* filePath){
        FolderInfo* folderInfo = NULL;
        GPT::Partition partitionTemp = GPT::Partition(NULL, NULL);
        GPT::Partition* partition = &partitionTemp;
        void* buffer = globalAllocator.RequestPage();


        char** SplitPath = split(filePath, "://");
        char* PartitionName = SplitPath[0];
        char* Folders = SplitPath[1];
        char** FoldersSlit = split(Folders, "/");
        int count;

        for(int i = 0; FoldersSlit[i] != 0; i++){ 
            count++;
        }

        for(int i = 0; i < count; i++){ 
            //this->OpenFolderInFolder(NULL, NULL, FoldersSlit[i]);

            printf("%s\n", FoldersSlit[i]);
        }
        /*char* nameToCheck;
        while(*filePath != 0){
            if(*filePath == '/'){ //open new folder or file
                //find the folder name
                char* tempName = filePath;
                tempName++;
                while(*tempName != '/'){
                    *nameToCheck = *tempName;
                    nameToCheck++;
                    tempName++;
                }

                printf("%s\n", tempName);

                /*uint64_t byteToScan = folderInfo->firstByte;
                for(int i = 0; i < folderInfo->numberFiles; i++){
                    memset(buffer, 0, sizeof(HeaderInfo));
                    partition->Read(byteToScan, sizeof(HeaderInfo), buffer); 
                    if(((HeaderInfo*)buffer)->IsFile == false){
                        memset(buffer, 0, sizeof(FolderInfo));
                        partition->Read(byteToScan + sizeof(HeaderInfo), sizeof(FolderInfo), buffer); 


                        if(((FolderInfo*)buffer)->name == nameToCheck){ //check the name
                            byteToScan = ((FolderInfo*)buffer)->firstByte;
                        }else{
                            byteToScan = ((FolderInfo*)buffer)->bottomHeader;
                        }

                    }else{
                        memset(buffer, 0, sizeof(FileInfo));
                        partition->Read(byteToScan + sizeof(HeaderInfo), sizeof(FileInfo), buffer);

                        //verify if the search is finish 
                        if(((FileInfo*)buffer)->name == nameToCheck){
                            File* file;
                            file->fileInfo = (FileInfo*)buffer;
                            return file;
                        }
                    }                    
                }              
            }
            filePath++;
        }*/
    }

    void KFS::Close(File* file){

    }
    
    FolderInfo* OpenFolderInFolder(GPT::Partition* Partition, FolderInfo* FolderOpened, char* FolderName){
        void* buffer = globalAllocator.RequestPage();
        uint64_t firstByte = 0;
        uint64_t byteToScan = 0;

        if(FolderOpened == NULL){
            firstByte = Partition->partition->FirstLBA;
        }else{
            firstByte = FolderOpened->firstByte;
        }

        byteToScan = firstByte;

        while(true){            
            memset(buffer, 0, sizeof(HeaderInfo));
            Partition->Read(byteToScan, sizeof(HeaderInfo), buffer); 

            if(((HeaderInfo*)buffer)->IsFile == false){
                memset(buffer, 0, sizeof(FolderInfo));
                Partition->Read(byteToScan + sizeof(HeaderInfo), sizeof(FolderInfo), buffer); 
                if(((FolderInfo*)buffer)->name == FolderName){
                    return (FolderInfo*)buffer;
                }else{
                    byteToScan = ((FolderInfo*)buffer)->bottomHeader;
                }
            }else{
                memset(buffer, 0, sizeof(FileInfo));
                Partition->Read(byteToScan + sizeof(HeaderInfo), sizeof(FileInfo), buffer); 
                byteToScan = ((FileInfo*)buffer)->bottomHeader;
            }

            if(byteToScan == 0){
                return NULL;
            }
        }
    }
}