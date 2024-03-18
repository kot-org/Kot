#include <errno.h>
#include <lib/assert.h>
#include <global/resources.h>

static inline int check_index(int index){
    if(index < 0){
        return EINVAL;
    }

    if(index >= MAX_DESCRIPTORS){
        return EINVAL;
    }

    return 0;
}

static inline int allocate_index(descriptors_ctx_t* ctx){
    for(descriptor_index_t i = ctx->index_search_begin; i < MAX_DESCRIPTORS; i++){
        if(ctx->descriptors[i] == NULL){
            ctx->index_search_begin = i;
            return i;
        }
    }

    return -EINVAL;
}

static inline int allocate_target_index(descriptors_ctx_t* ctx, int index){
    if(ctx->descriptors[index] == NULL){
        return 0;
    }

    return EINVAL;
}


static inline int free_index(descriptors_ctx_t* ctx, int index){
    ctx->descriptors[index] = NULL;

    if(index < ctx->index_search_begin){
        ctx->index_search_begin = index;
    }

    return 0;
}


int add_descriptor(descriptors_ctx_t* ctx, descriptor_t* descriptor){
    assert(!spinlock_acquire(&ctx->lock));

    int index = allocate_index(ctx);

    if(index < 0){
        spinlock_release(&ctx->lock);
        return index;
    }

    descriptor->is_parent = true;
    ctx->descriptors[index] = descriptor;

    spinlock_release(&ctx->lock);

    return index;
}

descriptor_t* get_descriptor(descriptors_ctx_t* ctx, int index){
    int error = check_index(index);
    
    if(error){
        return NULL;
    }

    assert(!spinlock_acquire(&ctx->lock));

    descriptor_t* descriptor = ctx->descriptors[index];

    spinlock_release(&ctx->lock);

    return descriptor;
}

int remove_descriptor(descriptors_ctx_t* ctx, int index){
    int error = check_index(index);

    if(error){
        return error;
    }

    assert(!spinlock_acquire(&ctx->lock));

    free_index(ctx, index);

    spinlock_release(&ctx->lock);

    return 0;
}

int move_descriptor(descriptors_ctx_t* ctx, int old_index, int new_index){
    int error = check_index(old_index);

    if(error){
        return error;
    }   

    error = check_index(new_index);

    if(error){
        return error;
    }

    assert(!spinlock_acquire(&ctx->lock));

    if(ctx->descriptors[old_index] == NULL){
        spinlock_release(&ctx->lock); 
        return EINVAL;
    }

    if(allocate_target_index(ctx, new_index)){
        spinlock_release(&ctx->lock); 
        return EINVAL;
    }

    ctx->descriptors[new_index] = ctx->descriptors[old_index];

    free_index(ctx, old_index);

    spinlock_release(&ctx->lock);   

    return 0; 
}

int copy_process_descriptors(descriptors_ctx_t* dst, descriptors_ctx_t* src){
    assert(!(dst->index_search_begin));

    for(int i = 0; i < MAX_DESCRIPTORS; i++){
        dst->descriptors[i] = src->descriptors[i];
        if(dst->descriptors[i] != NULL){
            dst->descriptors[i]->is_parent = false;
        }
    }

    return 0;
}