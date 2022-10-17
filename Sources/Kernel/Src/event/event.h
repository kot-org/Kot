#pragma once

#include <kot/sys.h>
#include <kot/types.h>
#include <lib/node/node.h>
#include <scheduler/scheduler.h>
#include <arch/x86-64/apic/apic.h>
#include <arch/x86-64/atomic/atomic.h>

struct kevent_t{    
    locker_t Lock;

    struct kevent_tasks_t** Tasks;
    struct arguments_t Parameters;
    size64_t NumTask;

    enum EventType Type;
}__attribute__((packed));

struct kevent_tasks_t{
    bool IgnoreMissedEvents;
    uint64_t NumberOfMissedEvents;
    struct kthread_t* Thread;
    struct kevent_t* Event;
    locker_t Lock;
}__attribute__((packed));

struct IRQLinekevent_t{
    kevent_t Header;
    uint8_t IRQLine;
    bool IsEnable;
}__attribute__((packed));

struct IRQkevent_t{
    kevent_t Header;
    uint8_t IRQ;
}__attribute__((packed));

struct IPCkevent_t{
    kevent_t Header;
    kthread_t* Master;
}__attribute__((packed));

namespace Event{
    uint64_t Create(kevent_t** event, enum EventType Type, uint64_t AdditionnalData);
    uint64_t Bind(struct kthread_t* task, struct kevent_t* self, bool IgnoreMissedEvents);
    uint64_t Unbind(struct kthread_t* task, struct kevent_t* self);
    uint64_t Trigger(struct kevent_t* self, arguments_t* parameters);
    uint64_t TriggerIRQ(kevent_t* self, arguments_t* parameters);
    uint64_t Close(struct ContextStack* Registers, kthread_t* task);
}