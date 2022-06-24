#include <lib/node/node.h>
#include <heap/heap.h>

Node* CreatNode(uintptr_t data){
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->parent = node;
    node->lastNodeCreat = node;
    return node;
}

Node* Node::GetNode(uint64_t position){
    Node* node = this->parent;
    for(int i = 0; i < position; i++){
        if(node->next != NULL){
            node = node->next;
        }else{
            return 0;
        }
    }
    return node;
}

uint64_t Node::GetSize(){
    uint64_t size = 1;
    Node* node = this->parent;
    while(true){
        if(node->next != NULL){
            node = node->next;
            size++;
        }else{
            return size;
        }
    }
}

Node* Node::Add(uintptr_t data){
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    parent->lastNodeCreat->next = newNode;
    newNode->last = newNode;
    newNode->next = NULL;
    newNode->parent = this->parent;
    parent->lastNodeCreat = newNode;
    return newNode;    
}

void Node::ModifyData(uintptr_t data){
    this->data = data;
}

void Node::Delete(){
    if(parent->lastNodeCreat == this){
        parent->lastNodeCreat = this->last;
    }

    if(last != NULL){
        last->next = next;
    }

    if(next != NULL){
        next->last = last;
    }

    if(this->parent = this){
        if(next != NULL){
            next->parent = next;
        }        
    }

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