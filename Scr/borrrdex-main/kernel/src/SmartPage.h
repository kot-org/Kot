#pragma once

#ifndef __cplusplus
#error C++ only
#endif

class SmartPage {
public:
    SmartPage(int pageCount);
    ~SmartPage();

    operator void*();
private:
    void* _start;
    int _count;
};