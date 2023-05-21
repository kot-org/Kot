#include <keyboard/keyboard.h>

kot_event_t KeyboardEvent;

KResult CreateKeyboardContext(uisd_hid_t* self){
    Sys_Event_Create(&KeyboardEvent);
    Sys_Keyhole_CloneModify(KeyboardEvent, &self->KeyboardEvent, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable, PriviledgeService);

    return KSUCCESS;
}