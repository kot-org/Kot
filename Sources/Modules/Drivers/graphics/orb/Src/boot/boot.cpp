#include <boot/boot.h>

#define BootLogoBottomMargin 0x20

void LoadBootGraphics(kot_framebuffer_t* Framebuffer){
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
            uint32_t PosX = (Framebuffer->Width - BGRTBMPImageHeader->Width) / 2;
            uint32_t PosY = (Framebuffer->Height - (BGRTBMPImageHeader->Height + LogoHeight)) / 2 ;
            uint8_t* Buffer = (uint8_t*)((uint64_t)MapPhysical((uintptr_t)BGRTTable->ImageAddress, BGRTBMPImageHeader->Size) + (uint64_t)BGRTBMPImageHeader->ImageOffset); // map all the image
            ParseBootImage(Framebuffer, Buffer, BGRTBMPImageHeader->Width, BGRTBMPImageHeader->Height, BGRTBMPImageHeader->Bpp, PosX, PosY);
        }
    }

    // Draw kot logo
    if(IsLogo){
        uint8_t* Buffer = (uint8_t*)((uint64_t)LogoImageHeader + (uint64_t)LogoImageHeader->ImageOffset); // map all the image
        uint32_t PosX = (Framebuffer->Width - LogoImageHeader->Width) / 2;
        uint32_t PosY = (Framebuffer->Height - LogoImageHeader->Height) / 2;
        if(IsBGRT){
            PosY = Framebuffer->Height - (LogoImageHeader->Height + BootLogoBottomMargin);
        }
        ParseBootImage(Framebuffer, Buffer, LogoImageHeader->Width, LogoImageHeader->Height, LogoImageHeader->Bpp, PosX, PosY);
        LoadBootAnimation(Framebuffer, PosX, PosY + LogoImageHeader->Height, LogoImageHeader->Width, 10);
    }
}

void ParseBootImage(kot_framebuffer_t* Framebuffer, uint8_t* IGA, uint32_t Width, uint32_t Height, uint8_t Bpp, uint32_t XPosition, uint32_t YPosition){
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
            uint32_t Data = ((uint32_t)IGA[Position + 1] << 8) | ((uint32_t)IGA[Position + 2] << 16) | (uint32_t)IGA[Position] | 0xff << 24;
            PutPixel(Framebuffer, j + XPosition, i + YPosition, Data);
        }
    }
}

thread_t bootAnimationThread = NULL;

void LoadBootAnimation(kot_framebuffer_t* Framebuffer, uint64_t XPosition, uint64_t YPosition, uint64_t Width, uint64_t Height){
    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&BootAnimation, PriviledgeDriver, NULL, &bootAnimationThread);
    arguments_t parameters{
        .arg[0] = (uint64_t)Framebuffer,
        .arg[1] = XPosition,
        .arg[2] = YPosition,
        .arg[3] = Width,
        .arg[4] = Height,
    };
    DrawRect(Framebuffer, XPosition, YPosition, Width, Height, 0xffffffff);

    Sys_ExecThread(bootAnimationThread, &parameters, ExecutionTypeQueu, NULL);
}

void BootAnimation(kot_framebuffer_t* Framebuffer, uint64_t XPosition, uint64_t YPosition, uint64_t Width, uint64_t Height){
    // Remove rectangle
    XPosition += 1;
    YPosition += 1;
    Width -= 1;
    Height -= 1;

    kot_framebuffer_t Backbuffer;
    Backbuffer.Size = Width * Framebuffer->Btpp * Height;
    Backbuffer.Buffer = calloc(Backbuffer.Size);
    Backbuffer.Bpp = Framebuffer->Bpp;
    Backbuffer.Btpp = Framebuffer->Btpp;
    Backbuffer.Height = Height;
    Backbuffer.Width = Width;
    Backbuffer.Pitch = Width * Framebuffer->Btpp;

    uint64_t x = 0;
    uint64_t tick;
    GetTickFromTime(&tick, 3000);
    uint64_t divider = tick / Width;
    while(true){
        GetActualTick(&tick);

        FillRect(&Backbuffer, x, 0, x, Height, 0x0);
        x = ((uint64_t)(tick / divider) % Width);
        FillRect(&Backbuffer, x, 0, x, Height, 0xffffffff);
        BlitFramebuffer(Framebuffer, &Backbuffer, XPosition, YPosition);
    }
    free(Backbuffer.Buffer);
    Sys_Exit(KSUCCESS);
}