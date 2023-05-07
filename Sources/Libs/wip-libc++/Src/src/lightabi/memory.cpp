

void operator delete(void *)
{
	//TODO:  Having virtual destructor may require an implementation of operator delete(void*) even if there is no dynamic memory allocation
	// For no dynamic memory enabled, just passthrough?
}
