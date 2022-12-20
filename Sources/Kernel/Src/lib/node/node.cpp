#include <lib/node/node.h>
#include <heap/heap.h>

Node* CreateNode(uintptr_t data){
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->parent = node;
    node->lastNodeCreate = node;
    AtomicClearLock(&node->lock);
    return node;
}

Node* Node::GetNode(uint64_t position){
    AtomicAquire(&lock);
    Node* node = this->parent;
    for(int i = 0; i < position; i++){
        if(node->next != NULL){
            node = node->next;
        }else{
            AtomicRelease(&lock);
            return 0;
        }
    }
    AtomicRelease(&lock);
    return node;
}

uint64_t Node::GetSize(){
    AtomicAquire(&lock);
    uint64_t size = 1;
    Node* node = this->parent;
    while(true){
        if(node->next != NULL){
            node = node->next;
            size++;
        }else{
            AtomicRelease(&lock);
            return size;
        }
    }
    AtomicRelease(&lock);
}

Node* Node::Add(uintptr_t data){
    AtomicAquire(&lock);
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    parent->lastNodeCreate->next = newNode;
    newNode->last = newNode;
    newNode->next = NULL;
    newNode->parent = this->parent;
    parent->lastNodeCreate = newNode;
    AtomicRelease(&lock);
    return newNode;    
}

void Node::ModifyData(uintptr_t data){
    AtomicAquire(&lock);
    this->data = data;
    AtomicRelease(&lock);
}

void Node::Delete(){
    AtomicAquire(&lock);
    if(parent->lastNodeCreate == this){
        parent->lastNodeCreate = this->last;
    }

    if(last != NULL){
        last->next = next;
    }

    if(next != NULL){
        next->last = last;
    }

    if(this->parent == this){
        if(next != NULL){
            next->parent = next;
        }        
    }
    AtomicRelease(&lock);

    free(this);
}

void Node::FreeAllNode(){
    Node* node = this->parent;
    while(true){
        if(node->next != NULL){
            node = node->next;
            node->last->Delete();
        }else{
            return;
        }
    }
}