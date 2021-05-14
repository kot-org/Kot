#pragma once

class Spinlock {
public:
    Spinlock() {
        reset();
    }

    void acquire();
    void release();

    void reset();
    
private:
    int _handle;
};