#include <mouse/mouse.h>

kot_event_t MouseRelative;
kot_event_t MouseAbsolute;

KResult CreateMouseContext(uisd_hid_t* self){
    Sys_Event_Create(&MouseRelative);
    Sys_Keyhole_CloneModify(MouseRelative, &self->MouseRelative, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable | KeyholeFlagDataTypeEventIsTriggerable, PriviledgeService);

    Sys_Event_Create(&MouseAbsolute);
    Sys_Keyhole_CloneModify(MouseAbsolute, &self->MouseAbsolute, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable | KeyholeFlagDataTypeEventIsTriggerable, PriviledgeService);
    
    return KSUCCESS;
}