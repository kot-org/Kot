#include "boot.h"

#define BootLogoBottomMargin 0x20

void loadBootGraphics(framebuffer_t* Framebuffer){
    bool IsBGRT = false;
    bool IsLogo = false;

    uint32_t LogoHeight = 0;
    
    // Get kot logo informations
    srv_system_callback_t* CallbackLogo = Srv_System_ReadFileInitrd("bootlogo.bmp", true);
    BMPImageHeader_t* LogoImageHeader = (BMPImageHeader_t*)CallbackLogo->Data;
    if(CallbackLogo->Data != NULL){
        if(LogoImageHeader->ImageOffset != NULL){
            LogoHeight = LogoImageHeader->Height;
            IsLogo = true;
        }
    } 

    // Draw BGRT image
    srv_system_callback_t* CallbackBGRT = Srv_System_GetTableInRootSystemDescription("BGRT", true);
    if(CallbackBGRT->Data != NULL){
        BGRTHeader_t* BGRTTable = (BGRTHeader_t*)CallbackBGRT->Data;
        BMPImageHeader_t* BGRTBMPImageHeader = (BMPImageHeader_t*)MapPhysical((uintptr_t)BGRTTable->ImageAddress, sizeof(BMPImageHeader_t)); // map the header only
        if(BGRTBMPImageHeader->ImageOffset != NULL){
            IsBGRT = true;
            uint32_t PosX = (Framebuffer->width - BGRTBMPImageHeader->Width) / 2;
            uint32_t PosY = (Framebuffer->height - (BGRTBMPImageHeader->Height + LogoHeight)) / 2 ;
            uint8_t* Buffer = (uint8_t*)((uint64_t)MapPhysical((uintptr_t)BGRTTable->ImageAddress, BGRTBMPImageHeader->Size) + (uint64_t)BGRTBMPImageHeader->ImageOffset); // map all the image
            parseBootImage(Framebuffer, Buffer, BGRTBMPImageHeader->Width, BGRTBMPImageHeader->Height, BGRTBMPImageHeader->Bpp, PosX, PosY);
        }
    }

    // Draw kot logo
    if(IsLogo){
        uint8_t* Buffer = (uint8_t*)((uint64_t)LogoImageHeader + (uint64_t)LogoImageHeader->ImageOffset); // map all the image
        uint32_t PosX = (Framebuffer->width - LogoImageHeader->Width) / 2;
        uint32_t PosY = (Framebuffer->height - LogoImageHeader->Height) / 2;
        if(IsBGRT){
            PosY = Framebuffer->height - (LogoImageHeader->Height + BootLogoBottomMargin);
        }
        parseBootImage(Framebuffer, Buffer, LogoImageHeader->Width, LogoImageHeader->Height, LogoImageHeader->Bpp, PosX, PosY);
        loadBootAnimation(Framebuffer, PosX, PosY + LogoImageHeader->Height, LogoImageHeader->Width, 10);
    }
}

void parseBootImage(framebuffer_t* Framebuffer, uint8_t* IGA, uint32_t Width, uint32_t Height, uint8_t Bpp, uint32_t XPos, uint32_t YPos){
    uint8_t BytePerPixel = Bpp / 8;
    uint32_t RowSize = Width * (uint32_t)BytePerPixel;
    // Add padding up to 4 bit padding
    if(RowSize % 4){
        RowSize -= RowSize % 4;
        RowSize += 4;
    }
    for(uint32_t i = Height; 0 < i; i--){ 
        for(uint32_t j = 0; j < Width; j++){
            uint32_t Position = (j * (uint32_t)BytePerPixel) + ((Height - i) * RowSize);
            uint32_t Data = ((uint32_t)IGA[Position + 1] << 8) | ((uint32_t)IGA[Position + 2] << 16) | (uint32_t)IGA[Position];
            putPixel(Framebuffer, j + XPos, i + YPos, Data);
        }
    }
}

thread_t bootAnimationThread = NULL;

void loadBootAnimation(framebuffer_t* Framebuffer, uint64_t XPos, uint64_t YPos, uint64_t Width, uint64_t Height){
    Sys_Createthread(Sys_GetProcess(), (uintptr_t)&bootAnimation, PriviledgeDriver, NULL, &bootAnimationThread);
    arguments_t parameters{
        .arg[0] = (uint64_t)Framebuffer,
        .arg[1] = XPos,
        .arg[2] = YPos,
        .arg[3] = Width,
        .arg[4] = Height,
    };
    drawRect(Framebuffer, XPos, YPos, Width, Height, 0xffffff);

    Sys_Execthread(bootAnimationThread, &parameters, ExecutionTypeQueu, NULL);
}

void bootAnimation(framebuffer_t* Framebuffer, uint64_t XPos, uint64_t YPos, uint64_t Width, uint64_t Height){

    // Remove rectangle
    XPos += 1;
    YPos += 1;
    Width -= 1;
    Height -= 1;

    framebuffer_t Backbuffer;
    Backbuffer.size = Width * Framebuffer->btpp * Height;
    Backbuffer.addr = calloc(Backbuffer.size);
    Backbuffer.bpp = Framebuffer->bpp;
    Backbuffer.btpp = Framebuffer->btpp;
    Backbuffer.height = Height;
    Backbuffer.width = Width;
    Backbuffer.pitch = Width * Framebuffer->btpp;

    uint64_t x = 0;
    uint64_t tick;
    GetTickFromTime(&tick, 1000);
    uint64_t divider = tick / Width;
    while(true){
        GetActualTick(&tick);
        fillRect(&Backbuffer, x, 0, x, Height, 0x0);
        x = ((uint64_t)(tick / divider) % Width);
        fillRect(&Backbuffer, x, 0, x, Height, 0xffffff);
        blitFramebuffer(Framebuffer, &Backbuffer, XPos, YPos);
    }
    free(Backbuffer.addr);
    Sys_Close(KSUCCESS);
}