#include "node.h"
#include "../../memory/heap/heap.h"

Node* CreatNode(void* data){
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->parent = node;
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

Node* Node::AddNext(void* data){
    Node* next = (Node*)malloc(sizeof(Node));
    next->data = data;
    this->next = next;
    next->last = this;
    next->parent = this->parent;
    return next;
}

void Node::ModifyData(void* data){
    this->data = data;
}

void Node::Delete(){
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