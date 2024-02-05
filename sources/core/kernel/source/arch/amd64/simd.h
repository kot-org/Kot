#ifndef _AMD64_SIMD_H
#define _AMD64_SIMD_H 1

void simd_init(void);

void* simd_create_context(void);
void simd_free_context(void* ctx);
void simd_save_context(void* ctx);
void simd_restore_context(void* ctx);

#endif