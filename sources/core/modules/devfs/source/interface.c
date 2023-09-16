devfs_context_t* devfs_ctx = NULL;
devfs_handler_t devfs_internal_handler;

int add_dev(const char* path, devfs_functions_t* functions){
    return devfs_add_dev(devfs_ctx, path, functions);
}

void interface_init(devfs_context_t* devfs_context){
    devfs_ctx = devfs_context;

    devfs_internal_handler.add_dev = &add_dev;
    devfs_handler = &devfs_internal_handler;
}