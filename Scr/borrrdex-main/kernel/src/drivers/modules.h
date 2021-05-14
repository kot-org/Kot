#pragma once

#include <cstdint>

#define MODULE_DEFINE(type, name, module) \
    static module_t __module__##name \
    __attribute__((section("modules_ptr"))) = {type, #name, module};

#define MODULE_INIT(name, fn) \
    static module_general_t __module_general__##name \
    __attribute__((section("real_modules_ptr"))) = {fn}; \
    MODULE_DEFINE(MODULE_TYPE_GENERAL, name, &__module_general__##name)

extern "C" {
    typedef enum {
        MODULE_TYPE_GENERAL,
        MODULE_TYPE_PCI
    } module_type_t;

    typedef int(*module_init_t)(void);

    typedef struct {
        module_init_t init_func;
    } module_general_t;

    typedef struct {
        module_type_t module_type;
        char module_name[20];
        void* module;
    } module_t;

    extern module_t _ModulesStart;
    extern module_t _ModulesEnd;

    void modules_init();
}