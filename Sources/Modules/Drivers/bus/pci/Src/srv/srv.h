#pragma once
#include <core/main.h>

void InitSrv(struct PCIDeviceArrayInfo_t* DevicesArray);

KResult CountDevices(kot_thread_t Callback, uint64_t CallbackArg, kot_srv_pci_search_parameters_t* SearchParameters);
KResult FindDevice(kot_thread_t Callback, uint64_t CallbackArg, kot_srv_pci_search_parameters_t* SearchParameters, uint64_t Index);

KResult GetInfoDevice(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t Device);
KResult GetBARDevice(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t Device, uint8_t BarIndex);
KResult BindMSI(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t DeviceIndex, uint8_t IRQVector, uint8_t Processor, uint16_t LocalDeviceVector);
KResult UnbindMSI(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t DeviceIndex, uint16_t LocalDeviceVector);
KResult ConfigReadWord(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t DeviceIndex, uint16_t Offset);
KResult ConfigWriteWord(kot_thread_t Callback, uint64_t CallbackArg, kot_PCIDeviceID_t DeviceIndex, uint16_t Offset, uint16_t Value);