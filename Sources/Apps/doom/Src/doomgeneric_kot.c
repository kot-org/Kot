#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include <stdbool.h>

#include <kot/sys.h>

#include <kot-graphics/orb.h>
#include <kot-graphics/utils.h>

#define KEYQUEUE_SIZE 16

kot_window_t* Window;
kot_thread_t WindowHandlerThread;
kot_event_t WindowEvent;
uint16_t KeyQueue[KEYQUEUE_SIZE];
uint64_t KeyQueueWriteIndex = 0;
uint64_t KeyQueueReadIndex = 0;

void* TableConverter;
size64_t TableConverterCharCount;

static uint8_t ConvertToDoomKey(char key){
  switch (key){
    case 0x1C:
      kot_Printlog("KEY_ENTER");
      key = KEY_ENTER;
      break;
    case 0x01:
      kot_Printlog("KEY_ESCAPE");
      key = KEY_ESCAPE;
      break;
    case 0x4B:
      kot_Printlog("KEY_LEFTARROW");
      key = KEY_LEFTARROW;
      break;
    case 0x4D:
      kot_Printlog("KEY_RIGHTARROW");
      key = KEY_RIGHTARROW;
      break;
    case 0x48:
      kot_Printlog("KEY_UPARROW");
      key = KEY_UPARROW;
      break;
    case 0x50:
      kot_Printlog("KEY_DOWNARROW");
      key = KEY_DOWNARROW;
      break;
    case 0x1D:
      kot_Printlog("KEY_FIRE");
      key = KEY_FIRE;
      break;
    case 0x39:
      kot_Printlog("KEY_USE");
      key = KEY_USE;
      break;
    case 0x2A:
    case 0x26:
      kot_Printlog("KEY_RSHIFT");
      key = KEY_RSHIFT;
      break;
    case 0x38:
      kot_Printlog("KEY_LALT");
      key = KEY_LALT;
      break;
    case 0x3B:
      kot_Printlog("KEY_F1");
      key = KEY_F1;
      break;
    case 0x3C:
      kot_Printlog("KEY_F2");
      key = KEY_F2;
      break;
    case 0x3D:
      kot_Printlog("KEY_F3");
      key = KEY_F3;
      break;
    case 0x3E:
      kot_Printlog("KEY_F4");
      key = KEY_F4;
      break;
    case 0x3F:
      kot_Printlog("KEY_F5");
      key = KEY_F5;
      break;
    case 0x40:
      kot_Printlog("KEY_F6");
      key = KEY_F6;
      break;
    case 0x41:
      kot_Printlog("KEY_F7");
      key = KEY_F7;
      break;
    case 0x42:
      kot_Printlog("KEY_F8");
      key = KEY_F8;
      break;
    case 0x13:
      kot_Printlog("KEY_F9");
      key = KEY_F9;
      break;
    case 0x44:
      kot_Printlog("KEY_F10");
      key = KEY_F10;
      break;
    case 0x57:
      kot_Printlog("KEY_F11");
      key = KEY_F11;
      break;
    case 0x58:
      kot_Printlog("KEY_F12");
      key = KEY_F12;
      break;
    case 0xD:
      kot_Printlog("KEY_EQUALS");
      key = KEY_EQUALS;
      break;
    default:{
      bool IsPressed;
      uint64_t Cache;
      char ScanCode = key;
      kot_GetCharFromScanCode(ScanCode, TableConverter, TableConverterCharCount, &key, &IsPressed, &Cache);
      kot_Printlog(&key);
      break;
    }

    }

  return key;
}

void EventHandler(enum kot_Window_Event EventType, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3, uint64_t GP4){
  if(EventType == Window_Event_Keyboard){
    bool IsPressed;
    kot_GetCharFromScanCode(GP0, TableConverter, TableConverterCharCount, NULL, &IsPressed, NULL);
    if(!IsPressed){
      GP0 -= 0x80;
    }
    uint8_t Key = ConvertToDoomKey(GP0);

    uint16_t KeyData = (IsPressed << 8) | Key;

    KeyQueue[KeyQueueWriteIndex] = KeyData;
    KeyQueueWriteIndex++;
    KeyQueueWriteIndex %= KEYQUEUE_SIZE;
  }
  kot_Sys_Event_Close();
}

void DG_Init(){
  kot_Sys_Event_Create(&WindowEvent);
  kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&EventHandler, PriviledgeApp, NULL, &WindowHandlerThread);
  kot_Sys_Event_Bind(WindowEvent, WindowHandlerThread, false);
  Window = CreateWindow(WindowEvent, Window_Type_Default);
  WindowChangePosition(Window, 0, 0);
  ResizeWindow(Window, DOOMGENERIC_RESX, DOOMGENERIC_RESY);
  ChangeVisibilityWindow(Window, true);
}

void DG_DrawFrame(){
  memcpy(Window->Framebuffer.Buffer, DG_ScreenBuffer, DOOMGENERIC_RESX * DOOMGENERIC_RESY * 4);
}

void DG_SleepMs(uint32_t ms){
  kot_Sleep(ms * 1000000);
}

uint32_t DG_GetTicksMs(){
  uint64_t Time;
  kot_GetTime(&Time);
  return (uint32_t)(Time / 1000000);
}

int DG_GetKey(int* pressed, unsigned char* doomKey){
  if(KeyQueueReadIndex != KeyQueueWriteIndex){
    uint16_t KeyData = KeyQueue[KeyQueueReadIndex];
    KeyQueueReadIndex++;
    KeyQueueReadIndex %= KEYQUEUE_SIZE;

    *pressed = KeyData >> 8;
    *doomKey = KeyData & 0xFF;

    return 1;
  }
  return 0;
}

void DG_SetWindowTitle(const char * title){

}

int main(int argc, char **argv){
    kot_GetTableConverter("d0:azerty.bin", &TableConverter, &TableConverterCharCount);

    doomgeneric_Create(argc, argv);
    while(true){
      doomgeneric_Tick();
    }

    return 0;
}