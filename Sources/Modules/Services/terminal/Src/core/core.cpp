#include <core/core.h>

uint64_t LineCount(char* Str){
    uint64_t Count = 0;
    
    while(*Str){
        if(*Str == '\n'){
            Count++;
        }
        Str++;
    }
    
    return Count;
}

char* GetNextLine(char* Str){
    char* Tmp = Str;
    while(*Str){
        if(*Str == '\n'){
            return Str + 1;
        }
        Str++;
    }
    return Tmp;
}

void WindowRenderer(kui_Context *Ctx){
    shell_t* Shell = (shell_t*)Ctx->opaque;
    kui_Container* Cnt;
    kui_Rect R;


    kui_begin(Ctx);
    if(kui_begin_window(Ctx, "Shell", kui_rect(0, 0, Shell->Width, Shell->Height))){
        kui_layout_row(Ctx, 1, (int[]) { -1 }, -1);
        R = kui_layout_next(Ctx);
        Cnt = kui_get_current_container(Ctx);
        if(Cnt->window_parent){
            if(!Shell->TextFramebuffer.Buffer){
                Shell->Event = Cnt->window_parent->window_event;

                Shell->TextFramebuffer.Width = R.w;
                Shell->TextFramebuffer.Height = R.h;
                Shell->TextFramebuffer.Btpp = sizeof(color_t);
                Shell->TextFramebuffer.Bpp = Shell->TextFramebuffer.Btpp * 8;
                Shell->TextFramebuffer.Pitch = Shell->TextFramebuffer.Btpp * Shell->TextFramebuffer.Width;
                Shell->TextFramebuffer.Size = Shell->TextFramebuffer.Pitch * Shell->TextFramebuffer.Height;
                Shell->TextFramebuffer.Buffer = malloc(Shell->TextFramebuffer.Size);
                LoadPen(Shell->ShellFont, &Shell->TextFramebuffer, 0, 0, 16, 0, 0xffffff);
                Shell->LineNumberMax = Shell->TextFramebuffer.Height / GetLineHeight(Shell->ShellFont) - 1;
            }
            int Len = strlen(Shell->InputBuffer);
            int N = kui_min(Shell->InputBufferSize - Len - 1, (int)strlen(Ctx->input_text));
            if (N > 0) {
                memcpy(Shell->InputBuffer + Len, Ctx->input_text, N);
                Len += N;
                Shell->InputBuffer[Len] = '\0';
            }
            if(Ctx->key_pressed & KUI_KEY_BACKSPACE && Len > 0) {
                while ((Shell->InputBuffer[--Len] & 0xc0) == 0x80 && Len > 0);
                Shell->InputBuffer[Len] = '\0';
            }
            if(Shell->ReadRequest->length){
                read_request_shell_t* CurrentRequest = (read_request_shell_t*)kot_vector_get(Shell->ReadRequest, 0);
                if(Ctx->key_pressed & KUI_KEY_RETURN || Len == CurrentRequest->SizeRequest){
                    Shell->InputBuffer[Len] = '\n';
                    Len++;
                    Shell->InputBuffer[Len] = '\0';
                    CurrentRequest->SizeGet = Len;
                    CurrentRequest->Buffer = Shell->InputBuffer;
                    ShellSendRequest(Shell, CurrentRequest);
                    ShellPrintWU(Shell, Shell->InputBuffer, Len);
                    Shell->InputBuffer[0] = '\0';
                }
            }

            atomicAcquire(&Shell->Lock, 0);
            SetPenPosX(Shell->ShellFont, 2);
            SetPenPosY(Shell->ShellFont, 0);
            memset(Shell->TextFramebuffer.Buffer, NULL, Shell->TextFramebuffer.Size);
            DrawFont(Shell->ShellFont, Shell->OuputBufferLastShow);
            DrawFont(Shell->ShellFont, Shell->InputBuffer);
            kui_set_framebuffer(Ctx, R, &Shell->TextFramebuffer);
            atomicUnlock(&Shell->Lock, 0);
        }
        kui_end_window(Ctx);
    }
    kui_end(Ctx);
    kui_r_present(Cnt);
}

shell_t* NewShell(kot_process_t Target){
    shell_t* Shell = (shell_t*)malloc(sizeof(shell_t));

    Shell->Target = Target;

    Shell->ReadRequest = kot_vector_create();

    Shell->InputBufferSize = 0x1000;
    Shell->InputBuffer = (char*)malloc(Shell->InputBufferSize);
    Shell->InputBuffer[0] = '\0';

    Shell->OutputBufferSize = 1;
    Shell->OutputBuffer = (char*)malloc(Shell->OutputBufferSize);
    Shell->OutputBuffer[0] = '\0';

    Shell->OuputBufferLastShow = Shell->OutputBuffer;

    Shell->TextFramebuffer.Buffer = NULL;

    /* Load default font */
    FILE* DefaultFontFile = fopen("d0:arial.ttf", "r");
    fseek(DefaultFontFile, 0, SEEK_END);
    size64_t DefaultFontSize = ftell(DefaultFontFile);
    void* DefaultFontBuffer = malloc(DefaultFontSize);
    fseek(DefaultFontFile, 0, SEEK_SET);
    fread(DefaultFontBuffer, DefaultFontSize, 1, DefaultFontFile);
    Shell->ShellFont = LoadFont((void*)DefaultFontBuffer, DefaultFontSize);
    free(DefaultFontBuffer);
    fclose(DefaultFontFile);

    Shell->Width = 900;
    Shell->Height = 500;
    Shell->LineNumberShow = 0;

    Shell->Lock = 0;

    Shell->Ctx = kui_init(WindowRenderer, Shell);

    return Shell;
}

void ShellPrintWU(shell_t* Shell, void* Buffer, size64_t Size){
    atomicAcquire(&Shell->Lock, 0);

    /* Don't forget to remove 1 to the start because we start with size of 1 to have NULL char at the end */
    size64_t OldSize = Shell->OutputBufferSize-1;
    Shell->OutputBufferSize += Size;
    void* OldBuffer = Shell->OutputBuffer;
    Shell->OutputBuffer = (char*)realloc(Shell->OutputBuffer, Shell->OutputBufferSize);
    Shell->OuputBufferLastShow = (char*)((uintptr_t)Shell->OuputBufferLastShow - (uintptr_t)OldBuffer + (uintptr_t)Shell->OutputBuffer);
    uint64_t BufferLineCount = LineCount((char*)Buffer);
    Shell->LineNumberShow += BufferLineCount;
    if(BufferLineCount < Shell->LineNumberMax){
        for(uint64_t i = Shell->LineNumberMax; Shell->LineNumberMax < Shell->LineNumberShow;){
            Shell->OuputBufferLastShow = GetNextLine(Shell->OuputBufferLastShow);
            Shell->LineNumberShow--;
        }
    }else{
        char* Tmp = (char*)Buffer;
        for(uint64_t i = Shell->LineNumberMax; Shell->LineNumberMax < Shell->LineNumberShow;){
            Tmp = GetNextLine(Tmp);
            Shell->LineNumberShow--;
        }
        Shell->OuputBufferLastShow = Shell->OuputBufferLastShow + OldSize + ((uintptr_t)Tmp - (uintptr_t)Buffer);
    }
    memcpy(&Shell->OutputBuffer[OldSize], (void*)Buffer, Size);
    Shell->OutputBuffer[Shell->OutputBufferSize-1] = '\0';
    
    atomicUnlock(&Shell->Lock, 0);
}

void ShellPrint(shell_t* Shell, void* Buffer, size64_t Size){
    ShellPrintWU(Shell, Buffer, Size);

    kot_arguments_t Parameters;
    Parameters.arg[0] = Window_Event_Update;
    kot_Sys_Event_Trigger(Shell->Event, &Parameters);
}

KResult ShellSendRequest(shell_t* Shell, read_request_shell_t* Request){
    kot_arguments_t arguments{
        .arg[0] = KSUCCESS,             /* Status */
        .arg[1] = Request->CallbackArg, /* CallbackArg */
        .arg[2] = NULL,                 /* Key to buffer */
        .arg[3] = NULL,                 /* GP1 */
        .arg[4] = NULL,                 /* GP2 */
        .arg[5] = NULL,                 /* GP3 */
    };

    kot_key_mem_t BufferKey;
    kot_Sys_CreateMemoryField(kot_Sys_GetProcess(), Request->SizeGet, (void**)&Request->Buffer, &BufferKey, MemoryFieldTypeSendSpaceRO);
    kot_Sys_Keyhole_CloneModify(BufferKey, &arguments.arg[2], Shell->Target, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagEditable, PriviledgeApp);
    
    KResult Status = kot_Sys_ExecThread(Request->Callback, &arguments, ExecutionTypeQueu | ExecutionTypeAwait, NULL);
    kot_vector_remove(Shell->ReadRequest, 0);
    return Status;
}

KResult ShellCreateRequest(shell_t* Shell, kot_thread_t Callback, uint64_t CallbackArg, size64_t SizeRequest){
    read_request_shell_t* Request = (read_request_shell_t*)malloc(sizeof(read_request_shell_t));
    Request->Callback = Callback;
    Request->CallbackArg = CallbackArg;

    Request->SizeRequest = SizeRequest;
    Request->SizeGet = NULL;
    Request->Buffer = NULL;

    kot_vector_push(Shell->ReadRequest, (void*)Request);

    return KSUCCESS;
}