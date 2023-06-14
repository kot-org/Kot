#include <core/core.h>


void WindowRenderer(kui_Context *Ctx){
    shell_t* Shell = (shell_t*)Ctx->opaque;

    kui_begin(Ctx);
    if(kui_begin_window(Ctx, "Shell", kui_rect(0, 0, 900, 500))){
        /* output text panel */
        kui_layout_row(Ctx, 1, (int[]) { -1 }, -25);
        kui_begin_panel(Ctx, "Shell Output");
        kui_Container* Panel = kui_get_current_container(Ctx);
        kui_layout_row(Ctx, 1, (int[]) { -1 }, -1);
        kui_text(Ctx, Shell->OutputBuffer);
        kui_end_panel(Ctx);
        if(Shell->OutputUpdated){
            Panel->scroll.y = Panel->content_size.y;
            Shell->OutputUpdated = false;
        }

        /* input textbox + submit button */
        bool IsSubmited = false;
        kui_layout_row(Ctx, 1, (int[]) { -1 }, -1);
        if(kui_textbox(Ctx, Shell->InputBuffer, Shell->InputBufferSize - 1) & KUI_RES_SUBMIT){
            kui_set_focus(Ctx, Ctx->last_id);
            if(Shell->ReadRequest->length){
                read_request_shell_t* CurrentRequest = (read_request_shell_t*)kot_vector_get(Shell->ReadRequest, 0);
                CurrentRequest->SizeGet = strlen(Shell->InputBuffer) + 2;
                Shell->InputBuffer[CurrentRequest->SizeGet-1] = '\n';
                Shell->InputBuffer[CurrentRequest->SizeGet] = '\0';
                CurrentRequest->Buffer = Shell->InputBuffer;
                kot_Printlog(Shell->InputBuffer);
                ShellPrint(Shell, CurrentRequest->Buffer, CurrentRequest->SizeGet);
                ShellSendRequest(Shell, CurrentRequest);
            }
            Shell->InputBuffer[0] = '\0';
        }

        kui_end_window(Ctx);
    }
    kui_end(Ctx);
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

    Shell->Ctx = kui_init(WindowRenderer, Shell);

    return Shell;
}

void ShellPrint(shell_t* Shell, void* Buffer, size64_t Size){
    size64_t OldSize = Shell->OutputBufferSize-1;
    Shell->OutputBufferSize += Size;
    Shell->OutputBuffer = (char*)realloc(Shell->OutputBuffer, Shell->OutputBufferSize);
    /* Don't forget to remove 1 to the start because we start with size of 1 to have NULL char at the end */
    memcpy(&Shell->OutputBuffer[OldSize-1], (void*)Buffer, Size);
    Shell->OutputBuffer[Shell->OutputBufferSize] = '\0';
    Shell->OutputUpdated = true;
    kot_Printlog(Shell->OutputBuffer);
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
    
    KResult Status = kot_Sys_ExecThread(Request->Callback, &arguments, ExecutionTypeQueu, NULL);
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