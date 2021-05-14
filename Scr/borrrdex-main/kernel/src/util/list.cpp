#include "list.h"
#include "memory/heap.h"

template<typename T>
List<T>::List() 
    :_capacity(0)
    ,_mem(nullptr)
{

}

template<typename T>
List<T>::List(size_t capacity) 
    :_capacity(capacity)
{
    _mem = kcalloc(capacity, sizeof(T));
}

template<typename T>
List<T>::~List() {
    for(size_t i = 0; i < _capacity; i++) {
        if(_mem[i]) {
            
        }
    }
}