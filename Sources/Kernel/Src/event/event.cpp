#include <event/event.h>

namespace Event{
    static uint64_t mutexEvent;

    uint64_t Create(event_t** event, enum EventType Type, uint64_t AdditionnalData){
        event_t* self = NULL;

        switch (Type){
            case EventTypeIRQLines: {
                IRQLineEvent_t* event = (IRQLineEvent_t*)malloc(sizeof(IRQLineEvent_t));
                self = &event->header;
                event->IRQLine = (uint8_t)AdditionnalData;
                event->IsEnable = false;
                break;
            }                
            case EventTypeIRQ: {
                IRQEvent_t* event = (IRQEvent_t*)malloc(sizeof(IRQEvent_t));
                self = &event->header;
                event->IRQ = (uint8_t)AdditionnalData;
                break;                
            }
            case EventTypeIPC: {
                IPCEvent_t* event = (IPCEvent_t*)malloc(sizeof(IPCEvent_t));
                self = &event->header;
                event->master = (kthread_t*)AdditionnalData;
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

    uint64_t Bind(kthread_t* task, event_t* self, bool IgnoreMissedEvents){
        Atomic::atomicAcquire(&self->Lock, 0);

        if(self->Type == EventTypeIRQLines){
            IRQLineEvent_t* event = (IRQLineEvent_t*)self;
            if(!event->IsEnable){
                event->IsEnable = true;
                APIC::IoChangeIrqState(event->IRQLine, 0, event->IsEnable);
            }
        }
        
        self->NumTask++;
        self->Tasks = (event_tasks_t**)realloc(self->Tasks, self->NumTask * sizeof(event_tasks_t*));

        event_tasks_t* TasksEvent = (event_tasks_t*)calloc(sizeof(event_tasks_t));
        TasksEvent->thread = task;
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

    uint64_t Unbind(kthread_t* task, event_t* self){
        if(self->NumTask <= 0) return KFAIL;
        Atomic::atomicAcquire(&self->Lock, 0);

        self->NumTask--;
        for(size64_t i = 0; i < self->NumTask; i++){
            if(self->Tasks[i]->thread == task){
                uintptr_t newPos = malloc(self->NumTask * sizeof(event_tasks_t));
                memcpy(newPos, self->Tasks[i], sizeof(event_tasks_t) * i);
                i++;
                memcpy((uintptr_t)((uint64_t)newPos + sizeof(event_tasks_t) * (i - 1)), (uintptr_t)((uint64_t)self->Tasks[i] + sizeof(event_tasks_t) * i), sizeof(event_tasks_t) * i);
                self->Tasks = (event_tasks_t**)newPos;
            }
        }

        if(self->Type == EventTypeIRQLines && self->NumTask == 0){
            IRQLineEvent_t* event = (IRQLineEvent_t*)self;
            if(event->IsEnable){
                event->IsEnable = false;
                APIC::IoChangeIrqState(event->IRQLine, 0, event->IsEnable);
            }
        }
        Atomic::atomicUnlock(&self->Lock, 0);
        return KSUCCESS;
    }
    
    uint64_t Trigger(kthread_t* author, event_t* self, arguments_t* parameters){
        if(self == NULL) return KFAIL;

        for(size64_t i = 0; i < self->NumTask; i++){
            event_tasks_t* task = self->Tasks[i];
            Atomic::atomicAcquire(&task->thread->EventLock, 0);
            if(task->thread->IsClose){
                task->DataNode->CurrentData->Task = task;
                task->thread->Launch(parameters);
            }else{
                if(!task->IgnoreMissedEvents){
                    task->DataNode->LastData = task->DataNode->LastData->Next;
                    task->DataNode->LastData->Next = (event_data_t*)malloc(sizeof(event_data_t));
                    task->DataNode->LastData->Task = task;
                    if(parameters != NULL){
                        memcpy(&task->DataNode->LastData->Parameters, parameters, sizeof(arguments_t));
                    }else{
                        memset(&task->DataNode->LastData->Parameters, 0, sizeof(arguments_t));
                    }

                    task->DataNode->NumberOfMissedEvents++;
                    task->NumberOfMissedEvents++;
                }
            }
            Atomic::atomicUnlock(&task->thread->EventLock, 0);
        }

        return KSUCCESS;
    } 

    uint64_t Close(ContextStack* Registers, kthread_t* task){
        if(!task->IsEvent) return KFAIL;
        Atomic::atomicAcquire(&task->EventLock, 0);
        Atomic::atomicAcquire(&globalTaskManager->MutexScheduler, 0);

        if(task->EventDataNode->NumberOfMissedEvents){
            event_data_t* Next = task->EventDataNode->CurrentData->Next;
            task->ResetContext(task->Regs);
            task->Launch_WL(&task->EventDataNode->CurrentData->Parameters);

            free(task->EventDataNode->CurrentData);
            task->EventDataNode->CurrentData = Next;
            task->EventDataNode->CurrentData->Task->NumberOfMissedEvents--;
            task->EventDataNode->NumberOfMissedEvents--;
            Atomic::atomicUnlock(&task->EventLock, 0);
            Atomic::atomicUnlock(&globalTaskManager->MutexScheduler, 0);
            ForceSelfDestruction();
        }else{
            task->Regs->rsp = (uint64_t)StackTop;
            task->Regs->rip = (uint64_t)task->EntryPoint;
            task->Regs->cs = Registers->threadInfo->CS;
            task->Regs->ss = Registers->threadInfo->SS;
            task->IsClose = true;
            task->IsBlock = true;
            Atomic::atomicUnlock(&task->EventLock, 0);
            ForceSelfDestruction();
        }

        return KSUCCESS;
    }
}
