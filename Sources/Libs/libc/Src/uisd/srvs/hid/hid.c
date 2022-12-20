#include <kot/uisd/srvs/hid.h>

/* Get event functions */

event_t GetMouseRelativeEvent(){
    uisd_hid_t* ControllerData = (uisd_hid_t*)FindControllerUISD(ControllerTypeEnum_Hid);
    return ControllerData->MouseRelative;
}

event_t GetMouseAbsoluteEvent(){
    uisd_hid_t* ControllerData = (uisd_hid_t*)FindControllerUISD(ControllerTypeEnum_Hid);
    return ControllerData->MouseAbsolute;
}

event_t GetKeyboardServerEvent(){
    uisd_hid_t* ControllerData = (uisd_hid_t*)FindControllerUISD(ControllerTypeEnum_Hid);
    return ControllerData->KeyboardServer;
}

event_t GetKeyboardClientEvent(){
    uisd_hid_t* ControllerData = (uisd_hid_t*)FindControllerUISD(ControllerTypeEnum_Hid);
    return ControllerData->KeyboardClient;
}

/* Bind functions */

KResult BindMouseRelative(thread_t Task, bool IgnoreMissedEvents){
    Sys_Event_Bind(GetMouseRelativeEvent(), Task, IgnoreMissedEvents);
    return KSUCCESS;
}

KResult BindMouseAbsolute(thread_t Task, bool IgnoreMissedEvents){
    Sys_Event_Bind(GetMouseAbsoluteEvent(), Task, IgnoreMissedEvents);
    return KSUCCESS;
}

KResult BindKeyboardClient(thread_t Task, bool IgnoreMissedEvents){
    Sys_Event_Bind(GetKeyboardClientEvent(), Task, IgnoreMissedEvents);
    return KSUCCESS;
}