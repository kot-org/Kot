#include <event/event.h>

namespace Event{
    static uint64_t mutexEvent;

    uint64_t Create(event_t** event, enum EventType Type, uint64_t AdditionnalData){
        event_t* self = NULL;

        switch (Type){
            case EventTypeIRQLines: {
                IRQEvent_t* event = (IRQEvent_t*)malloc(sizeof(IRQEvent_t));
                self = &event->header;
                event->IRQ = (uint8_t)AdditionnalData;
                event->IsEnable = false;
                break;
            }                
            case EventTypeIRQ: {
                if(AdditionnalData < IRQ_START + IRQ_MAX) return KFAIL;
                IVTEvent_t* event = (IVTEvent_t*)malloc(sizeof(IVTEvent_t));
                self = &event->header;
                event->IVT = (uint8_t)AdditionnalData;
                break;                
            }
            case EventTypeIPC: {
                IPCEvent_t* event = (IPCEvent_t*)malloc(sizeof(IPCEvent_t));
                self = &event->header;
                event->master = (thread_t*)AdditionnalData;
                break;                
            }

            default:
                return KFAIL;

        }

        self->Type = Type;
        self->Tasks = NULL;
        self->NumTask = 0;

        *event = self;

        return KSUCCESS;
    }

    uint64_t Bind(thread_t* task, event_t* self, bool IgnoreMissedEvents){
        Atomic::atomicAcquire(&self->Lock, 0);

        if(self->Type == EventTypeIRQLines){
            IRQEvent_t* event = (IRQEvent_t*)self;
            if(!event->IsEnable){
                event->IsEnable = true;
                APIC::IoChangeIrqState(event->IRQ, 0, event->IsEnable);
            }
        }
        
        self->NumTask++;
        self->Tasks = (event_tasks_t**)realloc(self->Tasks, self->NumTask * sizeof(event_tasks_t*));

        event_tasks_t* TasksEvent = (event_tasks_t*)calloc(sizeof(event_tasks_t));
        TasksEvent->Thread = task;
        TasksEvent->Event = self;

        TasksEvent->NumberOfMissedEvents = NULL;

        if(!task->IsEvent){
            task->EventDataNode = (event_data_node_t*)calloc(sizeof(event_data_node_t));
            task->EventDataNode->LastData = (event_data_t*)malloc(sizeof(event_data_t));
            task->EventDataNode->LastData->Next = (event_data_t*)malloc(sizeof(event_data_t));
            task->EventDataNode->CurrentData = task->EventDataNode->LastData;
        }
        TasksEvent->DataNode = task->EventDataNode;

        TasksEvent->IgnoreMissedEvents = IgnoreMissedEvents;
        
        task->IsEvent = true;   

        self->Tasks[self->NumTask - 1] = TasksEvent;

        Atomic::atomicUnlock(&self->Lock, 0);
        return KSUCCESS;
    }

    uint64_t Unbind(thread_t* task, event_t* self){
        if(self->NumTask <= 0) return KFAIL;
        Atomic::atomicAcquire(&self->Lock, 0);

        self->NumTask--;
        for(size_t i = 0; i < self->NumTask; i++){
            if(self->Tasks[i]->Thread == task){
                uintptr_t newPos = malloc(self->NumTask * sizeof(event_tasks_t));
                memcpy(newPos, self->Tasks[i], sizeof(event_tasks_t) * i);
                i++;
                memcpy((uintptr_t)((uint64_t)newPos + sizeof(event_tasks_t) * (i - 1)), (uintptr_t)((uint64_t)self->Tasks[i] + sizeof(event_tasks_t) * i), sizeof(event_tasks_t) * i);
                self->Tasks = (event_tasks_t**)newPos;
            }
        }

        if(self->Type == EventTypeIRQLines && self->NumTask == 0){
            IRQEvent_t* event = (IRQEvent_t*)self;
            if(event->IsEnable){
                event->IsEnable = false;
                APIC::IoChangeIrqState(event->IRQ, 0, event->IsEnable);
            }
        }
        Atomic::atomicUnlock(&self->Lock, 0);
        return KSUCCESS;
    }
    
    uint64_t Trigger(thread_t* author, event_t* self, parameters_t* parameters){
        if(self == NULL) return KFAIL;

        for(size_t i = 0; i < self->NumTask; i++){
            event_tasks_t* task = self->Tasks[i];
            Atomic::atomicAcquire(&task->Thread->EventLock, 0);

            if(task->Thread->IsBlock){
                task->DataNode->CurrentData->Task = task;
                task->Thread->Launch(parameters);
            }else{
                if(!task->IgnoreMissedEvents){
                    task->DataNode->LastData = task->DataNode->LastData->Next;
                    task->DataNode->LastData->Next = (event_data_t*)malloc(sizeof(event_data_t));
                    task->DataNode->LastData->Task = task;
                    if(parameters != NULL){
                        memcpy(&task->DataNode->LastData->Parameters, parameters, sizeof(parameters_t));
                    }else{
                        memset(&task->DataNode->LastData->Parameters, 0, sizeof(parameters_t));
                    }

                    task->DataNode->NumberOfMissedEvents++;
                    task->NumberOfMissedEvents++;
                }
            }
            Atomic::atomicUnlock(&task->Thread->EventLock, 0);
        }

        return KSUCCESS;
    } 

    uint64_t Close(ContextStack* Registers, thread_t* task){
        Atomic::atomicAcquire(&task->EventLock, 0);
        /* Reset task */
        task->Regs->rsp = (uint64_t)StackTop;
        task->Regs->rip = (uint64_t)task->EntryPoint;
        task->Regs->cs = Registers->ThreadInfo->CS;
        task->Regs->ss = Registers->ThreadInfo->SS;
        
        if(task->EventDataNode->NumberOfMissedEvents){
            event_data_t* Next = task->EventDataNode->CurrentData->Next;
            Registers->rip = task->Regs->rip;
            Registers->rsp = task->Regs->rsp;

            free(task->EventDataNode->CurrentData);
            task->EventDataNode->CurrentData = Next;
            task->EventDataNode->CurrentData->Task->NumberOfMissedEvents--;
            task->EventDataNode->NumberOfMissedEvents--;
            Atomic::atomicUnlock(&task->EventLock, 0);
        }else{
            globalTaskManager->ThreadExecutePerCore[task->CoreID] = NULL;
            task->IsBlock = true;
            Atomic::atomicUnlock(&task->EventLock, 0);
            ForceSchedule();
        }

        return KSUCCESS;
    }
}
