#include <system/system.h>

KResult ExecuteSystemAction(uint64_t PartitonID){
    // Load filesystem handler
    if(!KotSpecificData.VFSHandler){
        srv_storage_callback_t* Callback = Srv_Storage_VFSLoginApp(Sys_GetProcess(), FS_AUTHORIZATION_HIGH, Storage_Permissions_Admin | Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, "d0:", true);
        KotSpecificData.VFSHandler = Callback->Data;
        free(Callback);
    }

    // TODO certify disk as system
    bool IsSystem = true;
    if(IsSystem){
        KResult Status = KFAIL;

        char DiskNumberBuffer[20];
        char* DiskNumber = itoa(PartitonID, (char*)&DiskNumberBuffer, 10);

        StringBuilder* SystemDataPathBuilder = new StringBuilder("d");
        SystemDataPathBuilder->append(DiskNumberBuffer);
        SystemDataPathBuilder->append(":System/Starter.json");
        char* SystemDataPath = SystemDataPathBuilder->toString();
        delete SystemDataPathBuilder;
        file_t* SystemDataFile = fopen(SystemDataPath, "r");
        if(SystemDataFile){
            fseek(SystemDataFile, 0, SEEK_END);
            size_t SystemDataFileSize = ftell(SystemDataFile);
            fseek(SystemDataFile, 0, SEEK_SET);

            char* BufferSystemDataFile = (char*)malloc(SystemDataFileSize);
            fread(BufferSystemDataFile, SystemDataFileSize, 1, SystemDataFile);

            JsonParser* Parser = new JsonParser(BufferSystemDataFile);

            if(Parser->getCode() == JSON_SUCCESS && Parser->getValue()->getType() == JSON_ARRAY){
                Printlog("Hey");
                Status = KSUCCESS;
            }
            delete Parser;
            free(BufferSystemDataFile);
        }
        free(SystemDataPath);

        return Status;
    }

    return KNOTALLOW;
}