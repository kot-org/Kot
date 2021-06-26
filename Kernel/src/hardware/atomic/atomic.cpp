#include "atomic.h"

namespace Atomic{
    void* atomicLoker(void* mutex){
        if(mutex == 0){
            mutex = globalAllocator.RequestPage();
            Atomic::atomicUnlock((void*)mutex);
        }
        while(true){
            atomicWait(mutex);
            atomicLock(mutex);
            if(atomicCheck(mutex)){
                break;
            }            
        }
        return mutex;
    }
}