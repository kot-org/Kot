#include <event/event.h>

namespace Event{
    static uint64_t mutexEvent;

    uint64_t Create(kevent_t** event, enum EventType Type, uint64_t AdditionnalData){
        kevent_t* self = NULL;

        switch (Type){
            case EventTypeIRQLines: {
                IRQLinekevent_t* event = (IRQLinekevent_t*)malloc(sizeof(IRQLinekevent_t));
                self = &event->Header;
                event->IRQLine = (uint8_t)AdditionnalData;
                event->IsEnable = false;
                break;
            }                
            case EventTypeIRQ: {
                IRQkevent_t* event = (IRQkevent_t*)malloc(sizeof(IRQkevent_t));
                self = &event->Header;
                event->IRQ = (uint8_t)AdditionnalData;
                break;                
            }
            case EventTypeIPC: {
                IPCkevent_t* event = (IPCkevent_t*)malloc(sizeof(IPCkevent_t));
                self = &event->Header;
                event->Master = (kthread_t*)AdditionnalData;
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

    uint64_t Bind(kthread_t* task, kevent_t* self, bool IgnoreMissedEvents){
        Aquire(&self->Lock);

        if(self->Type == EventTypeIRQLines){
            IRQLinekevent_t* event = (IRQLinekevent_t*)self;
            if(!event->IsEnable){
                event->IsEnable = true;
                APIC::IoChangeIrqState(event->IRQLine, 0, event->IsEnable);
            }
        }
        
        self->NumTask++;
        self->Tasks = (kevent_tasks_t**)realloc(self->Tasks, self->NumTask * sizeof(kevent_tasks_t*));

        kevent_tasks_t* TasksEvent = (kevent_tasks_t*)calloc(sizeof(kevent_tasks_t));
        TasksEvent->Thread = task;
        TasksEvent->Event = self;

        TasksEvent->NumberOfMissedEvents = NULL;

        TasksEvent->IgnoreMissedEvents = IgnoreMissedEvents;
        
        task->IsEvent = true;   
        task->EventTask = TasksEvent;
        self->Tasks[self->NumTask - 1] = TasksEvent;

        Release(&self->Lock);
        return KSUCCESS;
    }

    uint64_t Unbind(kthread_t* task, kevent_t* self){
        if(self->NumTask <= 0) return KFAIL;
        Aquire(&self->Lock);

        self->NumTask--;
        for(size64_t i = 0; i < self->NumTask; i++){
            if(self->Tasks[i]->Thread == task){
                uintptr_t newPos = malloc(self->NumTask * sizeof(kevent_tasks_t));
                memcpy(newPos, self->Tasks[i], sizeof(kevent_tasks_t) * i);
                i++;
                memcpy((uintptr_t)((uint64_t)newPos + sizeof(kevent_tasks_t) * (i - 1)), (uintptr_t)((uint64_t)self->Tasks[i] + sizeof(kevent_tasks_t) * i), sizeof(kevent_tasks_t) * i);
                self->Tasks = (kevent_tasks_t**)newPos;
            }
        }

        if(self->Type == EventTypeIRQLines && self->NumTask == 0){
            IRQLinekevent_t* event = (IRQLinekevent_t*)self;
            if(event->IsEnable){
                event->IsEnable = false;
                APIC::IoChangeIrqState(event->IRQLine, 0, event->IsEnable);
            }
        }
        Release(&self->Lock);
        return KSUCCESS;
    }
    
    uint64_t Trigger(kevent_t* self, arguments_t* parameters){
        if(self == NULL) return KFAIL;

        for(size64_t i = 0; i < self->NumTask; i++){
            kevent_tasks_t* task = self->Tasks[i];
            Aquire(&task->Lock);
            if(task->Thread->IsClose){
                task->Thread->Launch(parameters);
            }else{
                if(!task->IgnoreMissedEvents){
                    task->NumberOfMissedEvents++;
                }
            }
            Release(&task->Lock);
        }

        return KSUCCESS;
    } 
    
    uint64_t TriggerIRQ(kevent_t* self, arguments_t* parameters){
        for(size64_t i = 0; i < self->NumTask; i++){
            kevent_tasks_t* task = self->Tasks[i];
            if(task->Thread->IsClose){
                task->Thread->Launch(parameters);
            }else{
                if(!task->IgnoreMissedEvents){
                    task->NumberOfMissedEvents++;
                }
            }
        }

        return KSUCCESS;
    } 

    uint64_t Close(ContextStack* Registers, kthread_t* task){
        if(!task->IsEvent) return KFAIL;
        Aquire(&task->EventTask->Lock);

        if(task->EventTask->NumberOfMissedEvents){
            Aquire(&globalTaskManager->MutexScheduler);
            task->ResetContext(task->Regs);
            task->Launch_WL(&task->EventTask->Event->Parameters);

            task->EventTask->NumberOfMissedEvents--;
            Release(&task->EventTask->Lock);
            ForceSelfDestruction();
        }else{
            Aquire(&globalTaskManager->MutexScheduler);
            task->Regs->rsp = (uint64_t)StackTop;
            task->Regs->rip = (uint64_t)task->EntryPoint;
            task->Regs->cs = Registers->threadInfo->CS;
            task->Regs->ss = Registers->threadInfo->SS;
            task->IsClose = true;
            task->IsBlock = true;
            Release(&task->EventTask->Lock);
            ForceSelfDestruction();
        }

        return KSUCCESS;
    }
}
