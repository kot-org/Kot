#include <stdarg.h>

#ifndef LIB_H
#define LIB_H

//#define DEBUG

/* variables */
#define SIZEOF(object) (char *)(&object+1) - (char *)(&object)

#define ORANGE 0xffffa500
#define CYAN   0xff00ffff
#define RED    0xffff0000
#define GREEN  0xff00ff00
#define BLUE   0xff0000ff
#define GRAY   0xff888888
#define WHITE  0xffffffff
#define BLACK  0xff000000

EFI_HANDLE ImageHandle;
EFI_SYSTEM_TABLE* SystemTable;
EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL GraphicsColor;
EFI_STATUS Status;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Volume;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* InfoModeGop;

//graphic variable
struct color{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
struct colorDouble{
    long double r;
    long double g;
    long double b;
};
struct Point{
    int x;
    int y;
};
struct GraphicModeInfo{
    uint_t BetterModeNumber;
    uint_t BetterHorizontalResolution;
    uint_t BetterVerticalResolution;
};

struct GraphicModeInfo BetterGraphicModeInfo; 

/* functions */
void ResetScreen()
{
    SystemTable->ConOut->Reset(SystemTable->ConOut, 1);
}

void ClearScreen()
{
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
}

void SetTextPosition(uint32_t Col, uint32_t Row)
{
    SystemTable->ConOut->SetCursorPosition(SystemTable->ConOut, Col, Row);
}

void SetColor(uint_t Attribute)
{
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, Attribute);
}

void Print(char16_t* str)
{
    #ifdef DEBUG
        if(str == CheckStandardEFIError(EFI_SUCCESS)){
            SetColor(EFI_GREEN); 
        }else{
            SetColor(EFI_RED); 
        }
        SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
    #endif  
}

void HitAnyKey()
{
    SystemTable->ConIn->Reset(SystemTable->ConIn, 1);

    EFI_INPUT_KEY Key;
	while((SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY);
}
void SetGraphicsColor(int32_t color)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL GColor;
    GColor.Reserved = color >> 24;
    GColor.Red      = color >> 16;
    GColor.Green    = color >> 8;
    GColor.Blue     = color;
    GraphicsColor = GColor;
}

void SetPixel(int32_t xPos, int32_t yPos, int32_t color)
{
    SetGraphicsColor(color);
    gop->Blt(gop, &GraphicsColor, 0, 0, 0, xPos, yPos, 1, 1, 0);
}
void SetPixelRGB(int32_t xPos, int32_t yPos, uint8_t r, uint8_t g, uint8_t b)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL GColor;
    GColor.Reserved = 0xff;
    GColor.Red      = r;
    GColor.Green    = g;
    GColor.Blue     = b;
    gop->Blt(gop, &GColor, 0, 0, 0, xPos, yPos, 1, 1, 0);
}

void CreateFilledBox(int32_t xPos, int32_t yPos, int32_t w, int32_t h, int32_t color)
{
    SetGraphicsColor(color);
    gop->Blt(gop, &GraphicsColor, 0, 0, 0, xPos, yPos, w, h, 0);
}

void putpixel(unsigned char* screen, int x, int y, int r, int g, int b) {
    int where = (x + (y * gop->Mode->Info->HorizontalResolution)) * 4;
    screen[where] = b;          // BLUE
    screen[where + 1] = g;      // GREEN
    screen[where + 2] = r;      // RED
}

void fillrect(unsigned char *vram, int w, int h, int x, int y, int r, int g, int b) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            putpixel(vram, j + x, i + y, r, g, b);
        }
    }
}

void Gradientrect(unsigned char *vram, int32_t xPos, int32_t yPos, int32_t w, int32_t h, struct color fromColor, struct color toColor, double middle) {
    
    struct colorDouble pixelColor;
    pixelColor.r = fromColor.r;
    pixelColor.g = fromColor.g;
    pixelColor.b = fromColor.b;

    struct colorDouble jumpPerPixel;
    jumpPerPixel.r = (long double)(fromColor.r - toColor.r)/w;
    jumpPerPixel.g = (long double)(fromColor.g - toColor.g)/w;
    jumpPerPixel.b = (long double)(fromColor.b - toColor.b)/w;
    
    for (int j = 0; j < w; j++) {
        pixelColor.r -= jumpPerPixel.r;
        pixelColor.g -= jumpPerPixel.g;
        pixelColor.b -= jumpPerPixel.b;
        for (int i = 0; i < h; i++) {
            putpixel(vram, j + xPos, i + yPos, pixelColor.r, pixelColor.g, pixelColor.b);
        }
    }
}

void InitializeGOP(){
    Status = SystemTable->BootServices->LocateProtocol(&EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID, 0, (void**)&gop);
    Print(CheckStandardEFIError(Status));
    if(Status == EFI_SUCCESS)
    {
        return;
    }
    else{
        ColdReboot();
    }  
       
}

//RuntimeServices

void ColdReboot()
{
    SystemTable->RuntimeServices->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, 0);
}

void WarmReboot()
{
    SystemTable->RuntimeServices->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, 0);
}

void Shutdown()
{
    SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, 0);
}


//Files system
void InitializeFileSystem()
{
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
    Status = SystemTable->BootServices->HandleProtocol(ImageHandle, &EFI_LOADED_IMAGE_PROTOCOL_GUID, (void**)&LoadedImage);
    Print(CheckStandardEFIError(Status));
    EFI_DEVICE_PATH_PROTOCOL *DevicePath;
    Status = SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &EFI_DEVICE_PATH_PROTOCOL_GUID, (void**)&DevicePath);
    Print(CheckStandardEFIError(Status));
    Status = SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID, (void**)&Volume);
    Print(CheckStandardEFIError(Status));
}

EFI_FILE_PROTOCOL* openFile(char16_t* FileName)
{
    EFI_FILE_PROTOCOL* RootFS;
    Status = Volume->OpenVolume(Volume, &RootFS);
    Print(CheckStandardEFIError(Status));
    EFI_FILE_PROTOCOL* FileHandle = NULL;
    Status = RootFS->Open(RootFS, &FileHandle, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    Print(CheckStandardEFIError(Status));
    
    return FileHandle;
}

EFI_FILE_PROTOCOL* LoadFile(EFI_FILE_PROTOCOL* Directory, char16_t* Path){
    EFI_FILE_PROTOCOL* LoadedFile;
	if (Directory == NULL){
		Volume->OpenVolume(Volume, &Directory);
	}
	EFI_STATUS s = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	Print(CheckStandardEFIError(s));
    if (s != EFI_SUCCESS){
		return NULL;
	}
	return LoadedFile;

}

void closeFile(EFI_FILE_PROTOCOL* FileHandle)
{
    Status = FileHandle->Close(FileHandle);
    Print(CheckStandardEFIError(Status));
}

//font

typedef struct GCP{
    unsigned int X;
    unsigned int Y;
} GCP;

struct PSF_FONT* LinuxFont;
GCP GraphicCursorPosition;

void GraphicChar(char chr, unsigned int xOff, unsigned int yOff)
{
    char* fontPtr = (char*)LinuxFont->glyphBuffer + (chr * LinuxFont->charsize) - 3;
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                   gop->Blt(gop, &GraphicsColor, 0, 0, 0, x, y, 1, 1, 0);
                }
        }
        fontPtr++;
    }
}

void Printgraphictext(const char* str)
{
    char* chr = (char*)str;
    while(*chr != 0){
        GraphicChar(*chr, GraphicCursorPosition.X, GraphicCursorPosition.Y);
        GraphicCursorPosition.X+=8;
        if(GraphicCursorPosition.X + 8 > gop->Mode->Info->HorizontalResolution)
        {
            GraphicCursorPosition.X = 0;
            GraphicCursorPosition.Y += 16;
        }
        chr++;
    }
}

void SetGraphicCursorPosition(unsigned int x, unsigned int y)
{
    GraphicCursorPosition.X = x;
    GraphicCursorPosition.Y = y;
}

int initFont(char16_t* FileName){
    EFI_FILE_PROTOCOL* font = openFile(FileName);

    uint_t lfsize = sizeof(struct PSF_FONT);
    Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, lfsize, (void**)&LinuxFont);
    Print(CheckStandardEFIError(Status));
    Status = font->Read(font, &lfsize, LinuxFont);
    Print(CheckStandardEFIError(Status));
    closeFile(font);
    
    if(LinuxFont == NULL)
    {
        return 0;
    } else {
        return 1;
    }
}

int strlen(char *str)
{
  int  len;
 
  len = 0;
  while (*str++){
     ++len; 
  }     
  return (len);
}

void readBMP(char* filename, int height, int Lx, int Ly, bool AlignMiddle)
{
    EFI_FILE_PROTOCOL* f = openFile(filename);
    uint64_t fileSize = 0;
    void* Buffer;
    if(f != NULL){         
        EFI_FILE_INFO* FileInfo;
        uint_t FileInfoSize = sizeof(EFI_FILE_INFO);
        f->GetInfo(f, &EFI_FILE_INFO_GUID, &FileInfoSize, NULL);
        SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
        f->GetInfo(f, &EFI_FILE_INFO_GUID, &FileInfoSize, (void*)FileInfo);
        fileSize = FileInfo->FileSize;  

        SystemTable->BootServices->AllocatePool(EfiLoaderData, fileSize, (void**)&Buffer);       
        f->Read(f, &fileSize, Buffer);
        
        closeFile(f);
    }
    uint8_t* ReadBuffer = (uint8_t*)Buffer;

    //read the header info
    int i = 54;
    int y = 0;
    unsigned char info[54];
    while(i > 0)
    {
        uint8_t g = *ReadBuffer;
        info[y] = g;
        ReadBuffer++;
        y++;
        i--;
    }
    int dataOffset = *(int*)&info[10]; 
    int src_width = *(int*)&info[18];
    int src_height = *(int*)&info[22];
    int width = src_width * height / src_height;
    int bitCount = (*(short*)&info[28]) / 8;
    //location   
    if(AlignMiddle){
        Lx = (gop->Mode->Info->HorizontalResolution - width) / 2;
        Ly = (gop->Mode->Info->VerticalResolution - height) / 2;
    }
    int location = (Lx + Ly * gop->Mode->Info->HorizontalResolution) * 4;

    unsigned char *screen = (unsigned char*)gop->Mode->FrameBufferBase;

    int pixelSize = 2;
    ReadBuffer = (uint8_t*)Buffer;
    ReadBuffer += dataOffset;
    //resize image to fill the screen
    for (int i = height; 0 < i; i--) { 
        for (int j = 0; j < width; j++) {
            int where = (j + (i * gop->Mode->Info->HorizontalResolution)) * 4 + location;          
            for(int c = pixelSize; 0 <= c; c--){
                uint8_t g = ReadBuffer[((j * src_width) / width + (((height - i) * src_height) / height) * src_width) * bitCount + c];
                //verify if the picture is in the screen
                if(where < gop->Mode->FrameBufferSize){
                    //so you can draw the pixel 
                    switch(c){
                        case 0: 
                            screen[where] = g; 
                            break;
                        case 1:
                            screen[where + 1] = g; 
                            break;
                        case 2:
                            screen[where + 2] = g;
                            break;
                        default: 
                            break;
                    }            
                            
                }                                                       
            }                        
        }
    }
}

void itoa(unsigned long int n, char16_t *buffer, int basenumber)
{
	unsigned long int hold;
	int i, j;
	hold = n;
	i = 0;
	
	do{
		hold = n % basenumber;
		buffer[i++] = (hold < 10) ? (hold + '0') : (hold + 'a' - 10);
	} while(n /= basenumber);
	buffer[i--] = 0;
	
	for(j = 0; j < i; j++, i--)
	{
		hold = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = hold;
	}
}

//printf

void Printf(char *s, ...)
{
    va_list ap;

    char buf[16];
    int i, j, size, buflen, neg;

    unsigned char c;
    int ival;
    unsigned int uival;

    va_start(ap, s);

    while ((c = *s++)) {
        size = 0;
        neg = 0;

        if (c == 0)
            break;
        else if (c == '%') {
            c = *s++;
            if (c >= '0' && c <= '9') {
                size = c - '0';
                c = *s++;
            }

            if (c == 'd') {
                ival = va_arg(ap, int);
                if (ival < 0) {
                    uival = 0 - ival;
                    neg++;
                } else
                    uival = ival;
                itoa(buf, uival, 10);

                buflen = strlen(buf);
                if (buflen < size)
                    for (i = size, j = buflen; i >= 0;
                         i--, j--)
                        buf[i] =
                            (j >=
                             0) ? buf[j] : '0';

                if (neg){
                    Printf("-%s", buf);
                }
                else{
                    Printf(buf);
                }
            } else if (c == 'u') {
                uival = va_arg(ap, int);
                itoa(buf, uival, 10);

                buflen = strlen(buf);
                if (buflen < size)
                    for (i = size, j = buflen; i >= 0;
                         i--, j--)
                        buf[i] =
                            (j >=
                             0) ? buf[j] : '0';

                Printf(buf);
            } else if (c == 'x' || c == 'X') {
                uival = va_arg(ap, int);
                itoa(buf, uival, 16);

                buflen = strlen(buf);
                if (buflen < size)
                    for (i = size, j = buflen; i >= 0;
                         i--, j--)
                        buf[i] =
                            (j >=
                             0) ? buf[j] : '0';

                Printf("0x%s", buf);
            } else if (c == 'p') {
                uival = va_arg(ap, int);
                itoa(buf, uival, 16);
                size = 8;

                buflen = strlen(buf);
                if (buflen < size)
                    for (i = size, j = buflen; i >= 0;
                         i--, j--)
                        buf[i] =
                            (j >=
                             0) ? buf[j] : '0';

                Printf("0x%s", buf);
            } else if(c == 's') {
                Printf((char *) va_arg(ap, int));
            }
        } else if (c == 10){
            //CR-NL
            GraphicCursorPosition.X = 0;
            GraphicCursorPosition.Y += 16;
        } else if (c == 13){
            //CR
            GraphicCursorPosition.X = 0;
        }else
            SetGraphicsColor(0xffffff);
            Print(c);
            GraphicCursorPosition.X+=8;
            if(GraphicCursorPosition.X + 8 > gop->Mode->Info->HorizontalResolution)
            {
                GraphicCursorPosition.X = 0;
                GraphicCursorPosition.Y += 16;
            }
    }

    return;
}

int memcmp(const void* aptr, const void* bptr, size_t n){
	const unsigned char* a = aptr, *b = bptr;
	for (size_t i = 0; i < n; i++){
		if (a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
	}
	return 0;
}

void memset(void* start, uint8_t value, uint64_t num){
    for (uint64_t i = 0; i < num; i++){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}

UINTN strcmp(CHAR8* a, CHAR8* b, UINTN length){
	for (UINTN i = 0; i < length; i++){
		if (*a != *b) return 0;
	}
	return 1;
}

INT32 CompareGuid(EFI_GUID *Guid1, EFI_GUID *Guid2)
{
    INT32       *g1, *g2, r;

    //
    // Compare 32 bits at a time
    //

    g1 = (INT32 *) Guid1;
    g2 = (INT32 *) Guid2;

    r  = g1[0] - g2[0];
    r |= g1[1] - g2[1];
    r |= g1[2] - g2[2];
    r |= g1[3] - g2[3];

    return r;
}

void GetRamFS(char* filename, struct RamFs* ramfs){
    EFI_FILE_PROTOCOL* f = openFile(filename);
    uint64_t fileSize = 0;
    void* Buffer;
    if(f != NULL){         
        EFI_FILE_INFO* FileInfo;
        uint_t FileInfoSize = sizeof(EFI_FILE_INFO);
        f->GetInfo(f, &EFI_FILE_INFO_GUID, &FileInfoSize, NULL);
        SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
        f->GetInfo(f, &EFI_FILE_INFO_GUID, &FileInfoSize, (void*)FileInfo);
        fileSize = FileInfo->FileSize;  

        SystemTable->BootServices->AllocatePool(EfiLoaderData, fileSize, (void**)&Buffer);       
        f->Read(f, &fileSize, Buffer);
        
        closeFile(f);
        ramfs->RamFsBase = Buffer;
        ramfs->Size = fileSize;
    }
}

#endif