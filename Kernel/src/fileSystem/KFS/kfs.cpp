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
        for(count = 0; FoldersSlit[count] != 0; count++); 

        for(int i = 0; i < count; i++){ 
            //find the folder name

            uint64_t byteToScan = folderInfo->firstByte;
            for(int i = 0; i < folderInfo->numberFiles; i++){
                memset(buffer, 0, sizeof(HeaderInfo));
                partition->Read(byteToScan, sizeof(HeaderInfo), buffer); 
                if(!((HeaderInfo*)buffer)->IsFile){
                    memset(buffer, 0, sizeof(FolderInfo));
                    partition->Read(byteToScan + sizeof(HeaderInfo), sizeof(FolderInfo), buffer); 


                    if(((FolderInfo*)buffer)->name == FoldersSlit[i]){ //check the name
                        byteToScan = ((FolderInfo*)buffer)->firstByte;
                    }else{
                        byteToScan = ((FolderInfo*)buffer)->bottomHeader;
                    }

                }else{
                    memset(buffer, 0, sizeof(FileInfo));
                    partition->Read(byteToScan + sizeof(HeaderInfo), sizeof(FileInfo), buffer);

                    //verify if the search is finish 
                    if(((FileInfo*)buffer)->name == FoldersSlit[count]){
                        File* file;
                        file->fileInfo = (FileInfo*)buffer;
                        return file;
                    }
                }                    
            }            
        }        
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