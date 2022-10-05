#include "boot.h"

#define BootLogoBottomMargin 0x10

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
