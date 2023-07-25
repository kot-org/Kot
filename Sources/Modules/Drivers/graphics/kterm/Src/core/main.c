#include <core/main.h>

#include <impl/impl.h>


void* TableConverter;
size64_t TableConverterCharCount;

kot_term_t* TermHandler = NULL;

int main(){
   kot_GetTableConverter("/d0:/azerty.bin", &TableConverter, &TableConverterCharCount);

   struct kot_srv_system_callback_t* Callback = kot_Srv_System_GetFramebuffer(true);
   kot_srv_system_framebuffer_t* BootFramebuffer = (kot_srv_system_framebuffer_t*)Callback->Data;
   free(Callback);

   size64_t FbSize = BootFramebuffer->Pitch * BootFramebuffer->Height;

   void* VirtualAddress = kot_MapPhysical((void*)BootFramebuffer->Address, FbSize);

   kot_framebuffer_t Fb;
   Fb.Buffer = VirtualAddress;
   Fb.Width = BootFramebuffer->Width;
   Fb.Height = BootFramebuffer->Height;
   Fb.Pitch = BootFramebuffer->Pitch;
   Fb.Size = FbSize;
    
   TermHandler = CreateTerminal(&Fb, "/d0:/vgafont.bin", "/d1:/kot/Wallpapers/term.bmp");
   
   PrintTerminal(TermHandler, "\e[0;33m--- Welcome to Kot terminal ---\n");
   PrintTerminal(TermHandler, "If you encounter any bugs during your session, please report them on the github repository: \nhttps://github.com/kot-org/Kot\e[0;37m\n");

   kot_thread_t KeyboardInterruptThread;
   kot_Sys_CreateThread(kot_Sys_GetProcess(), (void*)&KeyboardInterruptEntry, PriviledgeApp, (uint64_t)TermHandler, &KeyboardInterruptThread);
   kot_BindKeyboardEvent(KeyboardInterruptThread, false);

   SrvInitalize();

   kot_Sys_Close(KSUCCESS); /* Don't close the process */
}

kot_term_t* NewTerminal(){
   static int TerminalCount = 0;
   if(TerminalCount >= TERMINAL_MAX){
      return 0;
   }else{
      TerminalCount++;
      return TermHandler;
   }
}

void KeyboardInterruptEntry(uint64_t Key){
   kot_term_t* Handler = (kot_term_t*)kot_Sys_GetExternalDataThread();
   PressKeyTerminal(Handler, Key);
   kot_Sys_Event_Close();   
}