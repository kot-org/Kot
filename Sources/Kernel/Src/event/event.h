#pragma once

#include <kot/types.h>
#include <kot/sys.h>
#include <lib/node/node.h>
#include <scheduler/scheduler.h>
#include <arch/x86-64/apic/apic.h>

struct event_t{    
    uint64_t Lock;

    struct event_tasks_t** Tasks;
    size64_t NumTask;

    enum EventType Type;
}__attribute__((packed));

struct event_tasks_t{
    bool IgnoreMissedEvents;
    uint64_t NumberOfMissedEvents;
    struct kthread_t* thread;
    struct event_data_node_t* DataNode;
    struct event_t* Event;
}__attribute__((packed));

struct event_data_node_t{
    uint64_t NumberOfMissedEvents;
    struct event_data_t* CurrentData;
    struct event_data_t* LastData;
}__attribute__((packed));

struct event_data_t{
    event_tasks_t* Task;
    arguments_t Parameters;
    event_data_t* Next;
}__attribute__((packed));

struct IRQLineEvent_t{
    event_t header;
    uint8_t IRQLine;
    bool IsEnable;
}__attribute__((packed));

struct IRQEvent_t{
    event_t header;
    uint8_t IRQ;
}__attribute__((packed));

struct IPCEvent_t{
    event_t header;
    kthread_t* master;
}__attribute__((packed));

namespace Event{
    uint64_t Create(event_t** event, enum EventType Type, uint64_t AdditionnalData);
    uint64_t Bind(struct kthread_t* task, struct event_t* self, bool IgnoreMissedEvents);
    uint64_t Unbind(struct kthread_t* task, struct event_t* self);
    uint64_t Trigger(struct kthread_t* author, struct event_t* self, arguments_t* parameters);
    uint64_t Close(ContextStack* Registers, kthread_t* task);
}