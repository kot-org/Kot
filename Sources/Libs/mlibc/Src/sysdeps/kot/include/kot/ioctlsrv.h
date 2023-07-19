#include <kot/sys.h>
#include <kot/types.h>

static KResult GetArgData(void* Buffer, size_t Size, void* Arg, kot_process_t Target){
    return kot_Sys_InterProcessMemoryCopy(kot_Sys_GetProcess(), Buffer, Target, Arg, Size);
}

static KResult SetArgData(void* Buffer, size_t Size, void* Arg, kot_process_t Target){
    return kot_Sys_InterProcessMemoryCopy(Target, Arg, kot_Sys_GetProcess(), Buffer, Size);
}