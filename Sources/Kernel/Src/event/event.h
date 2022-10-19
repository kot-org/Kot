#pragma once

#include <kot/types.h>
#include <kot/sys.h>
#include <lib/node/node.h>
#include <scheduler/scheduler.h>
#include <arch/x86-64/apic/apic.h>

struct kevent_t{    
    locker_t Lock;

    struct kevent_tasks_t** Tasks;
    size64_t NumTask;

    enum EventType Type;
}__attribute__((packed));

struct kevent_tasks_t{
    bool IgnoreMissedEvents;
    uint64_t NumberOfMissedEvents;
    struct kthread_t* thread;
    struct event_data_node_t* DataNode;
    struct kevent_t* Event;
}__attribute__((packed));

struct event_data_node_t{
    uint64_t NumberOfMissedEvents;
    struct event_data_t* CurrentData;
    struct event_data_t* LastData;
}__attribute__((packed));

struct event_data_t{
    kevent_tasks_t* Task;
    arguments_t Parameters;
    event_data_t* Next;
}__attribute__((packed));

struct IRQLinekevent_t{
    kevent_t header;
    uint8_t IRQLine;
    bool IsEnable;
}__attribute__((packed));

struct IRQkevent_t{
    kevent_t header;
    uint8_t IRQ;
}__attribute__((packed));

struct IPCkevent_t{
    kevent_t header;
    kthread_t* master;
}__attribute__((packed));

namespace Event{
    uint64_t Create(kevent_t** event, enum EventType Type, uint64_t AdditionnalData);
    uint64_t Bind(struct kthread_t* task, struct kevent_t* self, bool IgnoreMissedEvents);
    uint64_t Unbind(struct kthread_t* task, struct kevent_t* self);
    uint64_t Trigger(struct kevent_t* self, arguments_t* parameters);
    uint64_t Close(struct ContextStack* Registers, kthread_t* task);
}