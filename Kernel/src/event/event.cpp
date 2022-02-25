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
                IVTEvent_t* event = (IVTEvent_t*)malloc(sizeof(IVTEvent_t));
                self = &event->header;
                event->IVT = (uint8_t)AdditionnalData;
                break;                
            }
            case EventTypeIPC: {
                IPCEvent_t* event = (IPCEvent_t*)malloc(sizeof(IPCEvent_t));
                self = &event->header;
                event->additionnalData = AdditionnalData;
                event->master = (thread_t*)AdditionnalData;
                break;                
            }

        }

        self->Type = Type;
        self->Tasks = NULL;
        self->NumTask = 0;

        *event = self;

        return KSUCCESS;
    }

    uint64_t Bind(thread_t* task, event_t* self){
        Atomic::atomicSpinlock(&self->Lock, 0);
        Atomic::atomicLock(&self->Lock, 0);

        self->NumTask++;
        self->Tasks = (thread_t**)realloc(self->Tasks, self->NumTask * sizeof(thread_t));
        self->Tasks[self->NumTask - 1] = task;

        task->IsEvent = true;
        task->Event = self;

        if(self->Type == EventTypeIRQ){
            IRQEvent_t* event = (IRQEvent_t*)self;
            if(!event->IsEnable){
                event->IsEnable = true;
                APIC::IoChangeIrqState(event->IRQ, 0, event->IsEnable);
            }
        }

        Atomic::atomicUnlock(&self->Lock, 0);
        return KSUCCESS;
    }

    uint64_t Unbind(thread_t* task, event_t* self){
        if(self->NumTask <= 0) return KFAIL;
        Atomic::atomicSpinlock(&self->Lock, 0);
        Atomic::atomicLock(&self->Lock, 0);

        self->NumTask--;
        for(size_t i = 0; i < self->NumTask; i++){
            if(self->Tasks[i] == task){
                void* newPos = malloc(self->NumTask * sizeof(thread_t));
                memcpy(newPos, self->Tasks[i], sizeof(thread_t) * i);
                i++;
                memcpy((void*)((uint64_t)newPos + sizeof(thread_t) * (i - 1)), (void*)((uint64_t)self->Tasks[i] + sizeof(thread_t) * i), sizeof(thread_t) * i);
                self->Tasks = (thread_t**)newPos;
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
    
    uint64_t Trigger(thread_t* author, event_t* self, void* Data, size_t Size){
        if(self == NULL) return KFAIL;
        Atomic::atomicSpinlock(&self->Lock, 0);
        Atomic::atomicLock(&self->Lock, 0);

        Parameters* FunctionParameters = (Parameters*)calloc(sizeof(Parameters));

        size_t EventStructSize = 0;
        switch (self->Type){
            case EventTypeIRQ: 
                EventStructSize = sizeof(IRQEvent_t);    
                break;     
            case EventTypeIVT: 
                EventStructSize = sizeof(IVTEvent_t);  
                break;                
            case EventTypeIPC:
                EventStructSize = sizeof(IPCEvent_t);  
                break;                
        }

        FunctionParameters->Parameter0 = self->Type;

        for(size_t i = 0; i < self->NumTask; i++){
            if(!self->Tasks[i]->IsBlock){

            }

            FunctionParameters->Parameter1 = (uint64_t)self->Tasks[i]->ShareDataInStack(Data, Size);
            FunctionParameters->Parameter2 = (uint64_t)self->Tasks[i]->ShareDataInStack(Data, Size);
        }

        Atomic::atomicUnlock(&self->Lock, 0);
        return KSUCCESS;
    } 

    void Exit(thread_t* task){
        // Reset task
        task->Regs->rip = (uint64_t)task->EntryPoint;
        task->IsBlock = false;
    }
}
