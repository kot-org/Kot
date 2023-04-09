#include <desktop/desktop.h>

desktopc::desktopc(orbc* Parent){

}

KResult desktopc::AddMonitor(monitorc* Monitor){
    Monitor->Desktop = this;
    return KSUCCESS;
}

KResult desktopc::RemoveMonitor(monitorc* Monitor){
    Monitor->Desktop = this;
    return KSUCCESS;
}

KResult desktopc::Update(monitorc* Monitor){
    return KSUCCESS;
}