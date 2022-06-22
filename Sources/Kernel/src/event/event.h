#pragma once

#include <kot/types.h>
#include <kot/sys.h>
#include <lib/node/node.h>
#include <scheduler/scheduler.h>
#include <arch/x86-64/apic/apic.h>

struct event_t{    
    uint64_t Lock;

    struct event_tasks_t** Tasks;
    size_t NumTask;

    enum EventType Type;
}__attribute__((packed));

struct event_tasks_t{
    bool IgnoreMissedEvents;
    uint64_t NumberOfMissedEvents;
    struct thread_t* Thread;
    struct event_data_node_t* DataNode;
}__attribute__((packed));

struct event_data_node_t{
    uint64_t NumberOfMissedEvents;
    struct event_data_t* CurrentData;
    struct event_data_t* LastData;
}__attribute__((packed));

struct event_data_t{
    event_tasks_t* Task;
    parameters_t Parameters;
    event_data_t* Next;
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
    uint64_t Bind(struct thread_t* task, struct event_t* self, bool IgnoreMissedEvents);
    uint64_t Unbind(struct thread_t* task, struct event_t* self);
    uint64_t Trigger(struct thread_t* author, struct event_t* self, parameters_t* parameters);
    void Exit(thread_t* task);
}