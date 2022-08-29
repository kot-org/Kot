#pragma once
#include <core/main.h>
#include <controller/controller.h>

struct HBAPort_t{
    uint32_t CommandListBase;
    uint32_t CommandListBaseUpper;
    uint32_t FisBaseAddress;
    uint32_t FisBaseAddressUpper;
    uint32_t InterruptStatus;
    uint32_t InterruptEnable;
    uint32_t CommandStatus;
    uint32_t Reserved0;
    uint32_t TaskFileData;
    uint32_t Signature;
    uint32_t SataStatus;
    uint32_t SataControl;
    uint32_t SataError;
    uint32_t SataActive;
    uint32_t CommandIssue;
    uint32_t SataNotification;
    uint32_t FisSwitchControl;
    uint32_t Reserved1[11];
    uint32_t Vendor[4];
};

class Port {
    public:
        HBAPort_t* HbaPort;
        PortType portType;
};