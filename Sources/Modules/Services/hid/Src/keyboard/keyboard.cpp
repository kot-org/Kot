#include <keyboard/keyboard.h>

event_t KeyboardServer;
event_t KeyboardClient;

KResult CreateKeyboardContext(uisd_hid_t* self){
    Sys_Event_Create(&KeyboardServer);
    Sys_Keyhole_CloneModify(KeyboardServer, &self->KeyboardServer, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsTriggerable, PriviledgeService);

    Sys_Event_Create(&KeyboardClient);
    Sys_Keyhole_CloneModify(KeyboardClient, &self->KeyboardClient, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable, PriviledgeService);

    return KSUCCESS;
}