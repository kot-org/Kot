#include <keyboard/keyboard.h>

kot_event_t KeyboardEvent;

KResult CreateKeyboardContext(kot_uisd_hid_t* self){
    kot_Sys_Event_Create(&KeyboardEvent);
    kot_Sys_Keyhole_CloneModify(KeyboardEvent, &self->KeyboardEvent, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable, PriviledgeService);

    return KSUCCESS;
}