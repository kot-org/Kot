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

    struct arguments_t Parameters;

    enum EventType Type;
}__attribute__((packed));

struct kevent_tasks_t{
    bool IgnoreMissedEvents;
    struct kthread_t* thread;
    struct event_data_node_t* DataNode;
    struct kevent_t* Event;
}__attribute__((packed));

struct event_data_node_t{
    uint64_t NumberOfMissedEvents;
    uint64_t IRQNumberOfMissedEvents;
    struct kevent_t* Event;
    struct event_data_t* CurrentData;
    struct event_data_t* LastData;
}__attribute__((packed));

struct event_data_t{
    struct kevent_tasks_t* Task;
    struct arguments_t Parameters;
    struct event_data_t* Next;
}__attribute__((packed));

struct IRQLinekevent_t{
    struct kevent_t Header;
    uint8_t IRQLine;
    bool IsEnable;
}__attribute__((packed));

struct IRQkevent_t{
    struct kevent_t header;
    uint8_t IRQ;
}__attribute__((packed));

struct IPCkevent_t{
    struct kevent_t header;
    kthread_t* master;
}__attribute__((packed));

namespace Event{
    uint64_t Create(kevent_t** event, enum EventType Type, uint64_t AdditionnalData);
    uint64_t Bind(struct kthread_t* task, struct kevent_t* self, bool IgnoreMissedEvents);
    uint64_t Unbind(struct kthread_t* task, struct kevent_t* self);
    uint64_t Trigger(struct kevent_t* self, arguments_t* parameters);
    uint64_t TriggerIRQ(kevent_t* self);
    uint64_t Close(struct ContextStack* Registers, kthread_t* task);
}