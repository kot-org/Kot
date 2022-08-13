#pragma once
#include <lib/va_args.h>
#include <arch/x86-64/atomic/atomic.h>
#include <arch/x86-64/io/serial/serial.h>

void Message(const char* str, ...);
void MessageProcess(const char* str, uint64_t charNum, uint64_t PID, uint64_t TID);
void Successful(const char* str, ...);
void Warning(const char* str, ...);
void Error(const char* str, ...);
void PrintRegisters(struct ContextStack* registers);