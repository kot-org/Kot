#pragma once

#include <cstddef>

template<typename T>
class List {
public:
    List();
    List(size_t capacity);
    ~List();
    
    void push_back(T item);
    void insert(T* item, size_t index);
    T operator[](size_t index) const;
private:
    size_t _capacity;
    T* _mem;
};