#include <core/core.h>

#include <kot-graphics/orb.h>
#include <kot-graphics/context.h>


shell_t* NewShell(process_t Target){
    shell_t* Shell = (shell_t*)malloc(sizeof(shell_t));

    Shell->Target = Target;

    Shell->ReadRequest = vector_create();

    Shell->HeightUsed = 0;
    
    Sys_Event_Create(&Shell->ShellEvent);
    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&ShellEventEntry, PriviledgeApp, (uint64_t)Shell, &Shell->ShellEventThread);
    Sys_Event_Bind(Shell->ShellEvent, Shell->ShellEventThread, false);
    Shell->Wid = CreateWindow(Shell->ShellEvent, Window_Type_Default);

    ResizeWindow(Shell->Wid, 500, 500);
    ChangeVisibilityWindow(Shell->Wid, true);

    Shell->Framebuffer = &Shell->Wid->Framebuffer;
    Shell->Backbuffer = (framebuffer_t*)malloc(sizeof(framebuffer_t));

    memcpy(Shell->Backbuffer, Shell->Framebuffer, sizeof(framebuffer_t));
    Shell->Backbuffer->Buffer = calloc(Shell->Framebuffer->Size);

    // Load font
    file_t* FontFile = fopen("d0:default-font.sfn", "r");
    fseek(FontFile, 0, SEEK_END);
    size64_t Size = ftell(FontFile);
    uintptr_t Buffer = malloc(Size);
    fseek(FontFile, 0, SEEK_SET);
    fread(Buffer, Size, 1, FontFile);
    Shell->Font = (kfont_t*)LoadFont(Buffer);
    free(Buffer);
    fclose(FontFile);

    font_fb_t FontFB;
    FontFB.Address = Shell->Backbuffer->Buffer;
    FontFB.Width = Shell->Backbuffer->Width;
    FontFB.Height = Shell->Backbuffer->Height;
    FontFB.Pitch = Shell->Backbuffer->Pitch;
    LoadPen(Shell->Font, &FontFB, 0, 0, 16, 0, 0xFFFFFFFF);

    ChangeVisibilityWindow(Shell->Wid, true);

    return Shell;
}

void ShellPrint(shell_t* Shell, uintptr_t Buffer, size64_t Size){
    char* Text = (char*)malloc(Size + 1);
    memcpy(Text, Buffer, Size);
    Text[Size] = NULL;
    int64_t TextHeight = 16; // font size
    if(TextHeight + Shell->HeightUsed > Shell->Backbuffer->Height){
        uint64_t HeightToMove = TextHeight;
        size64_t SizeToMove = HeightToMove * Shell->Backbuffer->Pitch;
        uintptr_t Src = (uintptr_t)((uint64_t)Shell->Backbuffer->Buffer + SizeToMove);
        uintptr_t Dst = Shell->Backbuffer->Buffer;
        size64_t Size = Shell->Backbuffer->Size - SizeToMove;
        memcpy(Dst, Src, Size);
        memset((uintptr_t)((uint64_t)Dst + Size), 0, SizeToMove);
        Shell->HeightUsed -= HeightToMove;
        EditPen(Shell->Font, NULL, -1, Shell->HeightUsed, -1, -1, -1);
    }
    Shell->HeightUsed += TextHeight;
    DrawFont(Shell->Font, Text);
    free(Text);
    BlitFramebuffer(Shell->Framebuffer, Shell->Backbuffer, 0, 0);
}

KResult ShellSendRequest(shell_t* Shell, read_request_shell_t* Request){
    arguments_t arguments{
        .arg[0] = KSUCCESS,             /* Status */
        .arg[1] = Request->CallbackArg, /* CallbackArg */
        .arg[2] = NULL,                 /* Key to buffer */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    ksmem_t BufferKey;
    Sys_CreateMemoryField(Sys_GetProcess(), Request->SizeGet, (uintptr_t*)&Request->Buffer, &BufferKey, MemoryFieldTypeSendSpaceRO);
    Sys_Keyhole_CloneModify(BufferKey, &arguments.arg[2], Shell->Target, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagEditable, PriviledgeApp);
    KResult Status = Sys_ExecThread(Request->Callback, &arguments, ExecutionTypeQueu, NULL);
    vector_remove(Shell->ReadRequest, 0);
    return Status;
}

KResult ShellCreateRequest(shell_t* Shell, thread_t Callback, uint64_t CallbackArg, size64_t SizeRequest){
    read_request_shell_t* Request = (read_request_shell_t*)malloc(sizeof(read_request_shell_t));
    Request->Callback = Callback;
    Request->CallbackArg = CallbackArg;

    Request->SizeRequest = SizeRequest;
    Request->SizeGet = NULL;
    Request->Buffer = NULL;

    vector_push(Shell->ReadRequest, Request);

    return KSUCCESS;
}

void ShellEventEntry(uint64_t EventType, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3, uint64_t GP4){
    shell_t* Shell = (shell_t*)Sys_GetExternalDataThread();
    if(EventType == Window_Event_Keyboard){
        // Only handle Keyboard
        if(Shell->ReadRequest->length){
            read_request_shell_t* CurrentRequest = (read_request_shell_t*)vector_get(Shell->ReadRequest, 0);
            if(CurrentRequest->SizeGet == 0){
                int64_t TextHeight = 16; // font size
                if(TextHeight + Shell->HeightUsed > Shell->Backbuffer->Height){
                    uint64_t HeightToMove = TextHeight;
                    size64_t SizeToMove = HeightToMove * Shell->Backbuffer->Pitch;
                    uintptr_t Src = (uintptr_t)((uint64_t)Shell->Backbuffer->Buffer + SizeToMove);
                    uintptr_t Dst = Shell->Backbuffer->Buffer;
                    size64_t Size = Shell->Backbuffer->Size - SizeToMove;
                    memcpy(Dst, Src, Size);
                    memset((uintptr_t)((uint64_t)Dst + Size), 0, SizeToMove);
                    Shell->HeightUsed -= HeightToMove;
                    EditPen(Shell->Font, NULL, -1, Shell->HeightUsed, -1, -1, -1);
                }
                Shell->HeightUsed += TextHeight;
            }

            uint64_t ScanCode = GP0;
            char Char;
            bool IsPressed;
            GetCharFromScanCode(ScanCode, TableConverter, TableConverterCharCount, &Char, &IsPressed, &Shell->PressedCache);
            if(IsPressed){
                DrawFontSize(Shell->Font, &Char, 1);
                BlitFramebuffer(Shell->Framebuffer, Shell->Backbuffer, 0, 0);
                
                CurrentRequest->SizeGet += sizeof(char);
                CurrentRequest->Buffer = (char*)realloc((uintptr_t)CurrentRequest->Buffer, CurrentRequest->SizeGet);
                CurrentRequest->Buffer[CurrentRequest->SizeGet - 1] = Char;

                if(Char == '\n' || CurrentRequest->SizeGet == CurrentRequest->SizeRequest){
                    ShellSendRequest(Shell, CurrentRequest);
                }
            }
        }
    }
    Sys_Event_Close();
}