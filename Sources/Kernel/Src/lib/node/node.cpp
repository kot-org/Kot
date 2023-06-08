#include <lib/node/node.h>
#include <heap/heap.h>

Node* CreateNode(void* data){
    Node* node = (Node*)kmalloc(sizeof(Node));
    node->data = data;
    node->parent = node;
    node->lastNodeCreate = node;
    AtomicClearLock(&node->lock);
    return node;
}

Node* Node::GetNode(uint64_t position){
    AtomicAcquire(&parent->lock);
    Node* node = this->parent;
    for(int i = 0; i < position; i++){
        if(node->next != NULL){
            node = node->next;
        }else{
            AtomicRelease(&parent->lock);
            return 0;
        }
    }
    AtomicRelease(&parent->lock);
    return node;
}

uint64_t Node::GetSize(){
    AtomicAcquire(&parent->lock);
    uint64_t size = 1;
    Node* node = this->parent;
    while(true){
        if(node->next != NULL){
            node = node->next;
            size++;
        }else{
            AtomicRelease(&parent->lock);
            return size;
        }
    }
    AtomicRelease(&parent->lock);
}

Node* Node::Add(void* data){
    AtomicAcquire(&parent->lock);
    Node* newNode = (Node*)kmalloc(sizeof(Node));
    newNode->data = data;
    parent->lastNodeCreate->next = newNode;
    newNode->last = this;
    newNode->next = NULL;
    newNode->parent = this->parent;
    parent->lastNodeCreate = newNode;
    AtomicRelease(&parent->lock);
    return newNode;    
}

void Node::ModifyData(void* data){
    AtomicAcquire(&parent->lock);
    this->data = data;
    AtomicRelease(&parent->lock);
}

void Node::Delete(){
    AtomicAcquire(&parent->lock);
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
    AtomicRelease(&parent->lock);

    kfree(this);
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