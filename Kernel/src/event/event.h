#pragma once

#include <kot/types.h>
#include <kot/sys.h>
#include <lib/node/node.h>
#include <scheduler/scheduler.h>
#include <arch/x86-64/apic/apic.h>

struct event_t{    
    uint64_t Lock;

    struct thread_t** Tasks;
    size_t NumTask;

    enum EventType Type;
}__attribute__((packed));

struct IRQEvent_t{
    event_t header;
    uint8_t IRQ;
    bool IsEnable;
}__attribute__((packed));

struct IVTEvent_t{
    event_t header;
    uint8_t IVT;
}__attribute__((packed));

struct IPCEvent_t{
    event_t header;
    thread_t* master;
}__attribute__((packed));

namespace Event{
    uint64_t Creat(event_t** event, enum EventType Type, uint64_t AdditionnalData);
    uint64_t Bind(struct thread_t* task, struct event_t* self);
    uint64_t Unbind(struct thread_t* task, struct event_t* self);
    uint64_t Trigger(struct thread_t* author, struct event_t* self, uintptr_t Data, size_t Size);
    void Exit(thread_t* task);
}