#include <srv/srv.h>

struct SrvInfo_t* SrvInfo;

void InitializeSrv(struct KernelInfo* kernelInfo){
    uintptr_t address = getFreeAlignedSpace(sizeof(uisd_system_t));
    ksmem_t key = NULL;
    Sys_CreateMemoryField(proc, sizeof(uisd_system_t), &address, &key, MemoryFieldTypeShareSpaceRO);

    uisd_system_t* SystemSrv = (uisd_system_t*)address;
    SystemSrv->ControllerHeader.IsReadWrite = false;
    SystemSrv->ControllerHeader.Version = System_Srv_Version;
    SystemSrv->ControllerHeader.VendorID = Kot_VendorID;
    SystemSrv->ControllerHeader.Type = ControllerTypeEnum_System;
    SystemSrv->ControllerHeader.Process = ShareProcessKey(proc);

    /* Setup threads */
    thread_t GetFrameBufferThread = NULL;
    Sys_Createthread(proc, (uintptr_t)&GetFrameBuffer, PriviledgeApp, &GetFrameBufferThread);
    SystemSrv->GetFramebuffer = MakeShareableThread(GetFrameBufferThread, PriviledgeService);

    /* Setup data */
    SrvInfo = (SrvInfo_t*)malloc(sizeof(SrvInfo_t));

    SrvInfo->Framebuffer = (srv_system_framebuffer_t*)malloc(sizeof(srv_system_framebuffer_t));
    SrvInfo->Framebuffer->address = kernelInfo->Framebuffer.framebuffer_addr;
    SrvInfo->Framebuffer->width = kernelInfo->Framebuffer.framebuffer_width;
    SrvInfo->Framebuffer->height = kernelInfo->Framebuffer.framebuffer_height;
    SrvInfo->Framebuffer->pitch = kernelInfo->Framebuffer.framebuffer_pitch;
    SrvInfo->Framebuffer->bpp = kernelInfo->Framebuffer.framebuffer_bpp;

    CreateControllerUISD(ControllerTypeEnum_System, key, true);
}

KResult GetFrameBuffer(thread_t Callback, uint64_t CallbackArg){
    ShareDataWithArguments_t data{
        .Data = SrvInfo->Framebuffer,
        .Size = sizeof(srv_system_framebuffer_t),
        .ParameterPosition = 0x2, 
    };

    arguments_t arguments{
        .arg[0] = KSUCCESS,         /* Statu */
        .arg[1] = CallbackArg,      /* CallbackArg */
        .arg[2] = NULL,             /* GP0 */
        .arg[3] = NULL,             /* GP1 */
        .arg[4] = NULL,             /* GP2 */
        .arg[5] = NULL,             /* GP3 */
    };

    Sys_Execthread(Callback, &arguments, ExecutionTypeQueu, &data);
    Sys_Close(KSUCCESS);
}