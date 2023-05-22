#include <main/core.h>

extern "C" int main(int argc, char* argv[]){
    kot_Printlog("[TIMER/HPET] Initializing ...");

    srv_system_callback_t* Callback = Srv_System_GetTableInRootSystemDescription("HPET", true);
    HPETHeader_t* HPETHeader = (HPETHeader_t*)Callback->Data;
    free(Callback);

    if(!HPETHeader){
        kot_Printlog("[TIMER/HPET] HPET table not found !");
        return KFAIL;
    }

    if(HPETHeader->Address.AddressSpace != SPACE_MEMORY){
        kot_Printlog("[TIMER/HPET] Registers acces system not supported");
        return KFAIL;
    }

    HPET_t* Hpet = (HPET_t*)malloc(sizeof(HPET_t));
    Hpet->Header = HPETHeader;
    Hpet->RegistersAddress = (void*)MapPhysical((void*)HPETHeader->Address.Address, REGISTER_SIZE);
    Hpet->MainCounterAddress = (uint64_t)Hpet->RegistersAddress + RegisterMainCounterValues;
    Hpet->TickPeriod = Hpet->ReadRegister(RegisterGeneralCapabilitiesAndIDRegister) >> GeneralCapabilitiesAndIDRegisterCounterPeriod;

    Srv_Time_SetTickPointerKey(&Hpet->MainCounterAddress, Hpet->TickPeriod, true);

    kot_Printlog("[TIMER/HPET] Initialized with success");
    return KSUCCESS;
}

uint64_t HPET_t::ReadRegister(uint64_t offset){
    return *((volatile uint64_t*)((void*)((uint64_t)RegistersAddress + offset)));
}

void HPET_t::WriteRegister(uint64_t offset, uint64_t value){
    *((volatile uint64_t*)((void*)((uint64_t)RegistersAddress + offset))) = value;
}


uint64_t HPET_t::GetMainCounterTick(){
    return ReadRegister(RegisterMainCounterValues);
}


void HPET_t::ChangeMainTimerInterruptState(bool IsEnable){
    uint64_t GeneralConfigurationRegisterData = ReadRegister(RegisterGeneralConfigurationRegister);
    GeneralConfigurationRegisterData |= IsEnable << TimerInterrupt; 
    WriteRegister(RegisterGeneralConfigurationRegister, GeneralConfigurationRegisterData);
} 

void HPET_t::ChangeMainTimerLegacyMappingState(bool IsEnable){
    uint64_t GeneralConfigurationRegisterData = ReadRegister(RegisterGeneralConfigurationRegister);
    GeneralConfigurationRegisterData |= IsEnable << LegacyMapping; 
    WriteRegister(RegisterGeneralConfigurationRegister, GeneralConfigurationRegisterData);
}