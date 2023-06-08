#pragma once
#include <lib/types.h>
#include <arch/x86-64/atomic/atomic.h>

struct Node{
        struct Node* next;   
        struct Node* last;   
        struct Node* parent;
        void* data;
        struct Node* lastNodeCreate; //only work for parent node
        locker_t lock;
        struct Node* GetNode(uint64_t position);
        struct Node* Add(void* data);
        void ModifyData(void* data);
        void Delete();
        uint64_t GetSize();
        void FreeAllNode();
};

struct Node* CreateNode(void* data);
struct Node* GetNode(struct Node* MainNode, uint64_t position);