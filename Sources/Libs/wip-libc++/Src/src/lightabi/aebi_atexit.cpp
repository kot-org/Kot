


/**
*  if your main function returns and then the code jumps back to the initialisation/reset routine, there is a need to properly perform destruction of global/static objects. You'll have to allocate enough space to store all the necessary {destructor_ptr, object_ptr} pairs, then in __aeabi_atexit function store the pair in the area pointed by __dso_handle, while incrementing value of later. Note, that dso_handle parameter to the __aeabi_atexit function is actually a pointer to the global __dso_handle value. Then, when the main function returns, invoke the stored destructors in the opposite order while passing addresses of the relevant objects as their first arguments.
*/
