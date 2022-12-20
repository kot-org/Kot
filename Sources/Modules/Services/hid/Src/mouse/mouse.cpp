#include <mouse/mouse.h>

event_t MouseRelative;
event_t MouseAbsolute;

KResult CreateMouseContext(uisd_hid_t* self){
    uint64_t Flags = NULL;
    Keyhole_SetFlag(&Flags, KeyholeFlagPresent, true);
    Keyhole_SetFlag(&Flags, KeyholeFlagDataTypeEventIsBindable, true);
    Keyhole_SetFlag(&Flags, KeyholeFlagDataTypeEventIsTriggerable, true);

    Sys_Event_Create(&MouseRelative);
    Sys_Keyhole_CloneModify(MouseRelative, &self->MouseRelative, NULL, Flags, PriviledgeService);

    Sys_Event_Create(&MouseAbsolute);
    Sys_Keyhole_CloneModify(MouseAbsolute, &self->MouseAbsolute, NULL, Flags, PriviledgeService);
    
    return KSUCCESS;
}