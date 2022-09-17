#include <keyboard/keyboard.h>

event_t KeyboardServer;
event_t KeyboardClient;

KResult CreateKeyboardContext(uisd_hid_t* self){
    uint64_t Flags = NULL;
    Keyhole_SetFlag(&Flags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&Flags, KeyholeFlagDataTypeEventIsTriggerable, true);

    Sys_Event_Create(&KeyboardServer);
    Sys_Keyhole_CloneModify(KeyboardServer, &self->KeyboardServer, NULL, Flags, PriviledgeService);


    Flags = NULL;
    Keyhole_SetFlag(&Flags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&Flags, KeyholeFlagDataTypeEventIsBindable, true);

    Sys_Event_Create(&KeyboardClient);
    Sys_Keyhole_CloneModify(KeyboardClient, &self->KeyboardClient, NULL, Flags, PriviledgeService);

    return KSUCCESS;
}