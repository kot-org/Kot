


/**
* The __dso_handle is a global pointer to the current address where the next
* {destructor_ptr, object_ptr} pair will be stored. The main function of most
* bare metal applications is not supposed to return and global/static objects
* will not be destructed. In this case it will be enough to implement the
* required function the following way:
*/
extern "C" int __aeabi_atexit(
    void *object,
    void (*destructor)(void *),
    void *dso_handle)
{
    static_cast<void>(object);
    static_cast<void>(destructor);
    static_cast<void>(dso_handle);
    return 0;
}

void* __dso_handle = nullptr;
