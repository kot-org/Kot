#pragma once
#include <kot/types.h>

struct Node{
        Node* next;   
        Node* last;   
        Node* parent;
        uintptr_t data;
        Node* lastNodeCreate; //only work for parent node
        Node* GetNode(uint64_t position);
        Node* Add(uintptr_t data);
        void ModifyData(uintptr_t data);
        void Delete();
        uint64_t GetSize();
        void FreeAllNode();
};

Node* CreateNode(uintptr_t data);
Node* GetNode(Node* MainNode, uint64_t position);