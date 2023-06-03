#include <mouse/mouse.h>

kot_event_t MouseRelative;
kot_event_t MouseAbsolute;

KResult CreateMouseContext(kot_uisd_hid_t* self){
    kot_Sys_Event_Create(&MouseRelative);
    kot_Sys_Keyhole_CloneModify(MouseRelative, &self->MouseRelative, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable | KeyholeFlagDataTypeEventIsTriggerable, PriviledgeService);

    kot_Sys_Event_Create(&MouseAbsolute);
    kot_Sys_Keyhole_CloneModify(MouseAbsolute, &self->MouseAbsolute, NULL, KeyholeFlagPresent | KeyholeFlagDataTypeEventIsBindable | KeyholeFlagDataTypeEventIsTriggerable, PriviledgeService);
    
    return KSUCCESS;
}