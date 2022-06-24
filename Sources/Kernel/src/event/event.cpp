#include <event/event.h>

namespace Event{
    static uint64_t mutexEvent;

    uint64_t Creat(event_t** event, enum EventType Type, uint64_t AdditionnalData){
        event_t* self = NULL;

        switch (Type){
            case EventTypeIRQ: {
                IRQEvent_t* event = (IRQEvent_t*)malloc(sizeof(IRQEvent_t));
                self = &event->header;
                event->IRQ = (uint8_t)AdditionnalData;
                event->IsEnable = false;
                break;
            }                
            case EventTypeIVT: {
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
        Atomic::atomicAcquire(&task->EventLock, 0);

        if(self->Type == EventTypeIRQ){
            IRQEvent_t* event = (IRQEvent_t*)self;
            if(!event->IsEnable){
                event->IsEnable = true;
                APIC::IoChangeIrqState(event->IRQ, 0, event->IsEnable);
            }
        }
        
        self->NumTask++;
        self->Tasks = (event_tasks_t**)realloc(self->Tasks, self->NumTask * sizeof(event_tasks_t*));

        self->Tasks[self->NumTask - 1] = (event_tasks_t*)calloc(sizeof(event_tasks_t));
        self->Tasks[self->NumTask - 1]->Thread = task;

        self->Tasks[self->NumTask - 1]->NumberOfMissedEvents = NULL;

        if(!task->IsEvent){
            task->DataNode = (event_data_node_t*)calloc(sizeof(event_data_node_t));
            task->DataNode->LastData = (event_data_t*)malloc(sizeof(event_data_t));
            task->DataNode->LastData->Next = (event_data_t*)malloc(sizeof(event_data_t));
            task->DataNode->CurrentData = task->DataNode->LastData;
        }
        self->Tasks[self->NumTask - 1]->DataNode = task->DataNode;

        self->Tasks[self->NumTask - 1]->IgnoreMissedEvents = IgnoreMissedEvents;
        
        task->IsEvent = true;   


        Atomic::atomicUnlock(&task->EventLock, 0);
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

        if(self->Type == EventTypeIRQ && self->NumTask == 0){
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
        Atomic::atomicAcquire(&mutexEvent, 0);

        for(size_t i = 0; i < self->NumTask; i++){
            if(self->Tasks[i]->Thread->IsBlock){
                self->Tasks[i]->DataNode->CurrentData->Task = self->Tasks[i];
                self->Tasks[i]->Thread->Launch(parameters);
            }else{
                if(!self->Tasks[i]->IgnoreMissedEvents){
                    self->Tasks[i]->DataNode->LastData = self->Tasks[i]->DataNode->LastData->Next;
                    self->Tasks[i]->DataNode->LastData->Next = (event_data_t*)malloc(sizeof(event_data_t));
                    self->Tasks[i]->DataNode->LastData->Task = self->Tasks[i];
                    if(parameters != NULL){
                        memcpy(&self->Tasks[i]->DataNode->LastData->Parameters, parameters, sizeof(parameters_t));
                    }else{
                        memset(&self->Tasks[i]->DataNode->LastData->Parameters, 0, sizeof(parameters_t));
                    }

                    self->Tasks[i]->DataNode->NumberOfMissedEvents++;
                    self->Tasks[i]->NumberOfMissedEvents++;
                }
            }
        }
        Atomic::atomicUnlock(&mutexEvent, 0);
        return KSUCCESS;
    } 

    uint64_t Close(ContextStack* Registers, thread_t* task){
        // Reset task
        CPU::DisableInterrupts();
        task->Regs->rsp = (uint64_t)StackTop;
        task->Regs->rip = (uint64_t)task->EntryPoint;
        task->Regs->cs = Registers->ThreadInfo->CS;
        task->Regs->ss = Registers->ThreadInfo->SS;
        
        if(task->DataNode->NumberOfMissedEvents){
            event_data_t* Next = task->DataNode->CurrentData->Next;
            Registers->rip = task->Regs->rip;
            Registers->rsp = task->Regs->rsp;

            free(task->DataNode->CurrentData);
            task->DataNode->CurrentData = Next;
            task->DataNode->CurrentData->Task->NumberOfMissedEvents--;
            task->DataNode->NumberOfMissedEvents--;
        }else{
            globalTaskManager->ThreadExecutePerCore[task->CoreID] = NULL;
            ExitAndSetBit((uint8_t*)&task->IsBlock);
        }  

        return KSUCCESS;
    }
}
