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
        SystemDataPathBuilder->append(":Kot/System/Starter.json");
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
                JsonArray* Array = (JsonArray*) Parser->getValue();

                arguments_t* InitParameters = (arguments_t*)calloc(sizeof(arguments_t));

                for(uint64_t i = 0; i < Array->length(); i++){
                    JsonObject* Service = (JsonObject*) Array->Get(i);
                    JsonString* File = (JsonString*) Service->Get("file");
                    JsonNumber* Priviledge = (JsonNumber*) Service->Get("priviledge"); // default: 3
                    
                    if (File->getType() == JSON_STRING) {
                        if(strcmp(File->Get(), "")) continue;
                        int32_t ServicePriledge = 3;
                        if(Priviledge != NULL){
                            if(Priviledge->getType() == JSON_NUMBER){ 
                                if(Priviledge->Get() >= 1 && Priviledge->Get() <= 3){
                                    ServicePriledge = Priviledge->Get();
                                }
                            }
                        }

                        StringBuilder* ServicePathBuilder = new StringBuilder("d");
                        ServicePathBuilder->append(DiskNumberBuffer);
                        ServicePathBuilder->append(":");
                        ServicePathBuilder->append(File->Get());
                        char* FilePath = ServicePathBuilder->toString();
                        delete ServicePathBuilder;

                        srv_system_callback_t* Callback = Srv_System_LoadExecutable(Priviledge->Get(), FilePath, true);
                        
                        size_t Filenamelen = strlen(FilePath);
                        char** CharArray = (char**)malloc((sizeof(char*) * 0x1) + (sizeof(char) * Filenamelen));
                        CharArray[0] = (char*)&CharArray[1];
                        memcpy(CharArray[0], FilePath, Filenamelen);

                        InitParameters->arg[0] = 1;
                        ShareDataWithArguments_t Data{
                            .Data = &CharArray,
                            .Size = (sizeof(char*) * 0x1) + (sizeof(char) * Filenamelen),
                            .ParameterPosition = 0x1,
                        };
                        Sys_ExecThread((thread_t)Callback->Data, InitParameters, ExecutionTypeQueu, &Data);
                        free(Callback);
                    }
                }
                free(InitParameters);
                Status = KSUCCESS;
            }
            delete Parser;
            free(SystemDataFile);
            free(BufferSystemDataFile);
        }
        free(SystemDataPath);

        return Status;
    }

    return KNOTALLOW;
}