#pragma once
#include <core/main.h>

void InitSrv();

KResult GetBARNum(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t Device);
KResult GetBARType(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t Device, uint8_t BarIndex);
KResult GetBARSize(thread_t Callback, uint64_t CallbackArg, PCIDeviceID_t Device, uint8_t BarIndex);
KResult PCISearcher(thread_t Callback, uint64_t CallbackArg, srv_pci_search_parameters_t* SearchParameters);
KResult PCISearcherGetDevice(thread_t Callback, uint64_t CallbackArg, srv_pci_search_parameters_t* SearchParameters, uint64_t Index);