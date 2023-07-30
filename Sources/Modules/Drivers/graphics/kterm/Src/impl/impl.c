#include <impl/impl.h>

void* alloc_mem(size_t size){
   return calloc(size, sizeof(uint8_t));
}

void free_mem(void *ptr, size_t size){
   free(ptr);
}

void callback(struct term_t*, uint64_t, uint64_t, uint64_t, uint64_t){
    
}

kot_term_t* CreateTerminal(kot_framebuffer_t* Fb, char* FontPath, char* ImagePath){
    kot_term_t* TerminalHandler = (kot_term_t*)malloc(sizeof(kot_term_t));

    TerminalHandler->ReadRequest = kot_vector_create();

    memset(&TerminalHandler->Term, 0, sizeof(struct term_t));

    struct framebuffer_t Framebuffer = {
        Fb->Buffer,
        Fb->Width,
        Fb->Height,
        Fb->Pitch
    };

    // Load font
    FILE* FontFile = fopen(FontPath, "rb");
    assert(FontFile != NULL);

    fseek(FontFile, 0, SEEK_END);
    size_t FontFileSize = ftell(FontFile);
    fseek(FontFile, 0, SEEK_SET);
    void* FontAdress = malloc(FontFileSize);
    fread(FontAdress, FontFileSize, 1, FontFile);

    struct font_t Font = {
        FontAdress, 
        8,
        16,
        1,
        1,
        1
    };

    struct style_t Style = {
        DEFAULT_ANSI_COLOURS,
        DEFAULT_ANSI_BRIGHT_COLOURS,
        DEFAULT_BACKGROUND,
        DEFAULT_FOREGROUND,
        DEFAULT_MARGIN,
        DEFAULT_MARGIN_GRADIENT
    };


    // Load image
    FILE* ImageFile = fopen(ImagePath, "rb");
    assert(ImageFile != NULL);

    fseek(ImageFile, 0, SEEK_END);
    size_t ImageFileSize = ftell(ImageFile);
    fseek(ImageFile, 0, SEEK_SET);
    void* ImageAdress = malloc(ImageFileSize);
    fread(ImageAdress, ImageFileSize, 1, ImageFile);

    struct image_t Image;
    image_open(&Image, ImageAdress, ImageFileSize);

    struct background_t Back = {
        &Image,
        STRETCHED,
        DEFAULT_BACKGROUND
    };

    term_init(&TerminalHandler->Term, callback, false, TERM_TABSIZE);

    term_vbe(&TerminalHandler->Term, Framebuffer, Font, Style, Back);

    TerminalHandler->InputBufferSize = 0x1000;
    TerminalHandler->InputBuffer = (char*)malloc(TerminalHandler->InputBufferSize);
    TerminalHandler->InputBuffer[0] = '\0';
    TerminalHandler->InputCursorPos = 0;

    TerminalHandler->Winsize.ws_row = TerminalHandler->Term.rows;
    TerminalHandler->Winsize.ws_col = TerminalHandler->Term.cols;
    TerminalHandler->Winsize.ws_xpixel = Fb->Width;
    TerminalHandler->Winsize.ws_ypixel = Fb->Height;

    return TerminalHandler;
}

void PutCharTerminal(kot_term_t* Handler, char Char){
    term_putchar(&Handler->Term, Char);
    term_double_buffer_flush(&Handler->Term);
}

void PrintTerminal(kot_term_t* Handler, const char* Text){
    WriteTerminal(Handler, Text, strlen(Text));
}

void WriteTerminal(kot_term_t* Handler, const char* Text, size_t Size){
    term_write(&Handler->Term, Text, Size);
}

void BackspaceTerminal(kot_term_t* Handler){
    if(Handler->InputCursorPos){
        int Len = strlen(Handler->InputBuffer);
        memcpy((void*)((uintptr_t)Handler->InputBuffer + (uintptr_t)Handler->InputCursorPos - 1), (void*)((uintptr_t)Handler->InputBuffer + (uintptr_t)Handler->InputCursorPos), Len - Handler->InputCursorPos + 1);
        size_t XCursorPos;
        size_t YCursorPos;
        term_get_cursor_pos(&Handler->Term, &XCursorPos, &YCursorPos);
        Handler->InputCursorPos--;

        if(XCursorPos == 0){
            XCursorPos = Handler->Term.cols;
            YCursorPos--;
        }else{
            XCursorPos--;
        }

        term_set_cursor_pos(&Handler->Term, XCursorPos, YCursorPos);

        term_write(&Handler->Term, (void*)((uintptr_t)Handler->InputBuffer + (uintptr_t)Handler->InputCursorPos), Len - Handler->InputCursorPos);
        term_putchar(&Handler->Term, ' ');
        
        term_set_cursor_pos(&Handler->Term, XCursorPos, YCursorPos);
        term_double_buffer_flush(&Handler->Term);
    }
}

KResult SendRequestTerminal(kot_term_t* Handler, read_request_shell_t* Request){
    struct kot_arguments_t arguments;
    arguments.arg[0] = KSUCCESS;             /* Status */
    arguments.arg[1] = Request->CallbackArg; /* CallbackArg */
    arguments.arg[2] = NULL;                 /* Key to buffer */
    arguments.arg[3] = NULL;                 /* GP1 */
    arguments.arg[4] = NULL;                 /* GP2 */
    arguments.arg[5] = NULL;                 /* GP3 */

    kot_key_mem_t BufferKey;
    kot_Sys_CreateMemoryField(kot_Sys_GetProcess(), Request->SizeGet, (void**)&Request->Buffer, &BufferKey, MemoryFieldTypeSendSpaceRO);
    kot_Sys_Keyhole_CloneModify(BufferKey, &arguments.arg[2], Request->TargetDataProc, KeyholeFlagPresent | KeyholeFlagCloneable | KeyholeFlagEditable, PriviledgeApp);
    
    KResult Status = kot_Sys_ExecThread(Request->Callback, &arguments, ExecutionTypeQueu | ExecutionTypeAwait, NULL);
    kot_vector_remove(Handler->ReadRequest, 0);
    return Status;
}

KResult CreateRequestTerminal(kot_term_t* Handler, kot_thread_t Callback, uint64_t CallbackArg, size64_t SizeRequest, kot_process_t TargetDataProc){
    read_request_shell_t* Request = (read_request_shell_t*)malloc(sizeof(read_request_shell_t));
    Request->Callback = Callback;
    Request->CallbackArg = CallbackArg;

    Request->SizeRequest = SizeRequest;
    Request->SizeGet = NULL;
    Request->Buffer = NULL;
    Request->TargetDataProc = TargetDataProc;

    kot_vector_push(Handler->ReadRequest, (void*)Request);

    return KSUCCESS;
}

void PressKeyTerminal(kot_term_t* Handler, uint64_t Key){
    if(!Handler->ReadRequest->length) return;
    static uint64_t Cache = 0;
    bool IsPressed;
    kot_GetCharFromScanCode(Key, TableConverter, TableConverterCharCount, NULL, &IsPressed, NULL);
    if(IsPressed){
        switch(Key){
            case 0x0E:{
                // Backspace
                BackspaceTerminal(Handler);
                break;
            }
            case 0x1C:{
                // Enter
                if(Handler->ReadRequest->length){
                    int Len = strlen(Handler->InputBuffer);
                    read_request_shell_t* CurrentRequest = (read_request_shell_t*)kot_vector_get(Handler->ReadRequest, 0);
                    Handler->InputBuffer[Len] = '\n';
                    Len++;
                    Handler->InputBuffer[Len] = '\0';
                    CurrentRequest->SizeGet = Len;
                    CurrentRequest->Buffer = Handler->InputBuffer;
                    SendRequestTerminal(Handler, CurrentRequest);
                    Handler->InputBuffer[0] = '\0';
                    Handler->InputCursorPos = 0;
                    PutCharTerminal(Handler, '\n');
                }
                break;
            }
            case 0x4B:{
                // Left arrow
                if(Handler->InputCursorPos){
                    Handler->InputCursorPos--;
                    size_t XCursorPos;
                    size_t YCursorPos;
                    term_get_cursor_pos(&Handler->Term, &XCursorPos, &YCursorPos);
                    if(XCursorPos == 0){
                        XCursorPos = Handler->Term.cols;
                        YCursorPos--;
                    }else{
                        XCursorPos--;
                    }
                    term_set_cursor_pos(&Handler->Term, XCursorPos, YCursorPos);
                    term_double_buffer_flush(&Handler->Term);
                }
                break;
            }
            case 0x4D:{
                // Right arrow
                int Len = strlen(Handler->InputBuffer);
                if(Handler->InputCursorPos < Len){
                    Handler->InputCursorPos++;
                    size_t XCursorPos;
                    size_t YCursorPos;
                    term_get_cursor_pos(&Handler->Term, &XCursorPos, &YCursorPos);
                    if(XCursorPos == Handler->Term.cols){
                        XCursorPos = 0;
                        YCursorPos++;
                    }else{
                        XCursorPos++;
                    }
                    term_set_cursor_pos(&Handler->Term, XCursorPos, YCursorPos);
                    term_double_buffer_flush(&Handler->Term);
                }
                break;
            }
            case 0x48:{
                // Up arrow
                break;
            }
            case 0x50:{
                // Down arrow
                break;
            }
            default: {
                char Char = '\0';
                kot_GetCharFromScanCode(Key, TableConverter, TableConverterCharCount, &Char, &IsPressed, &Cache);
                Handler->InputBuffer[Handler->InputCursorPos] = Char;
                Handler->InputBuffer[Handler->InputCursorPos + 1] = '\0';

                PutCharTerminal(Handler, Char);
                Handler->InputCursorPos++;

                break;
            }
        }
    }else{
        // Update cache
        char Char = '\0';
        kot_GetCharFromScanCode(Key, TableConverter, TableConverterCharCount, &Char, &IsPressed, &Cache);
    }
}