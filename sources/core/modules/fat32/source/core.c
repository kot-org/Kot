#include <core.h>

#define FAT32_SIGNATURE (((uint64_t)'F' << (0)) | ((uint64_t)'A' << (8)) | ((uint64_t)'T' << (16)) | ((uint64_t)'3' << (24)) | ((uint64_t)'2' << (32)) | ((uint64_t)' ' << (40)) | ((uint64_t)' ' << (48)) | ((uint64_t)' ' << (56)))

static uint64_t cluster_to_lba(fat_context_t* ctx, uint32_t cluster){
    return ctx->first_usable_lba + ctx->bpb->sectors_per_cluster * (cluster - 2);
}

static uint32_t lba_to_cluster(fat_context_t* ctx, uint64_t lba){
    return ((lba - ctx->first_usable_lba) / ctx->bpb->sectors_per_cluster) + 2;
}

static int read_partition(partition_t* partition, uint64_t start, size_t size, void* buffer){
    if(start + size > partition->start + partition->size){
		return EINVAL;
	}
    return partition->device->read(partition->device, start + partition->start, size, buffer);
}

static int write_partition(partition_t* partition, uint64_t start, size_t size, void* buffer){
    if(start + size > partition->start + partition->size){
		return EINVAL;
	}
    return partition->device->write(partition->device, start + partition->start, size, buffer);
}

static inline uint64_t bytes_to_lba(uint64_t value){
    return value >> 9;
}

static inline uint64_t lba_to_bytes(uint64_t value){
    return value << 9;
}

static inline uint32_t fat_get_cluster_entry_without_root_check(fat_short_entry_t* dir){
    return ((uint32_t)dir->cluster_low) | ((uint32_t)dir->cluster_high << 16);
}

static inline uint32_t fat_get_cluster_file(fat_short_entry_t* dir){
    return fat_get_cluster_entry_without_root_check(dir);
}

static inline uint32_t fat_get_cluster_entry(fat_context_t* ctx, fat_short_entry_t* dir){
    uint32_t cluster = fat_get_cluster_entry_without_root_check(dir);

    if(cluster == 0){
        cluster = ctx->bpb->root_cluster_number; // root directory
    }

    return cluster;
}

static inline void fat_set_cluster_entry(fat_short_entry_t* dir, uint32_t cluster){
    dir->cluster_low = cluster & 0xFFFF;
    dir->cluster_high = (cluster >> 16) & 0xFFFF;
}

static uint16_t fat_get_current_time(void){
    uint16_t time = 0;
    time |= (get_current_day() & 0x1F);
    time |= (get_current_month() & 0xF) << 5;
    time |= ((get_current_year() - 1980) & 0x7F) << 9;
    return time;
}

static uint16_t fat_get_current_date(void){
    uint16_t date = 0;
    date |= (get_current_second() & 0x1F);
    date |= (get_current_minute() & 0x3F) << 5;
    date |= (get_current_hour() & 0x1F) << 11;
    return date;
}


static int fat_read_boot_sector(fat_context_t* ctx){
    return read_partition(ctx->partition, 0, sizeof(bpb_t), ctx->bpb);
}

static int fat_read_fs_info_sector(fat_context_t* ctx){
    int err = read_partition(ctx->partition, lba_to_bytes(ctx->bpb->sector_number_fs_info), sizeof(fs_info_t), ctx->fsi);
    if(err){
        return err;
    }
    
    if(ctx->fsi->lead_signature != FSI_LEAD_SIGNATURE){
        return EINVAL;
    }

    if(ctx->fsi->struct_signature != FSI_STRUCT_SIGNATURE){
        return EINVAL;
    }

    if(ctx->fsi->trail_signature != FSI_TRAIL_SIGNATURE){
        return EINVAL;
    }

    return 0;
}

static int fat_write_fs_info_sector(fat_context_t* ctx){
    return write_partition(ctx->partition, lba_to_bytes(ctx->bpb->sector_number_fs_info), sizeof(fs_info_t), ctx->fsi);
}

static int fat_read_fat(fat_context_t* ctx){
    return read_partition(ctx->partition, ctx->fat1_position, ctx->fat_size, ctx->fat);
}

static inline uint32_t fat_get_next_cluster(fat_context_t* ctx, uint32_t current){
    return ctx->fat[current] & 0x0FFFFFFF;
}

static inline int fat_set_next_cluster(fat_context_t* ctx, uint32_t current, uint32_t next_cluster){
    ctx->fat[current] = (next_cluster & 0x0FFFFFFF) | (ctx->fat[current] & 0xF0000000);
    // write fat 1
    write_partition(ctx->partition, (uint64_t)ctx->fat1_position + (uint64_t)current * (uint64_t)sizeof(uint32_t), sizeof(uint32_t), &ctx->fat[current]);
    // write fat 2
    write_partition(ctx->partition, (uint64_t)ctx->fat2_position + (uint64_t)current * (uint64_t)sizeof(uint32_t), sizeof(uint32_t), &ctx->fat[current]);
    return 0;
}

static spinlock_t fat_allocate_cluster_lock = {};

static int fat_allocate_cluster(fat_context_t* ctx, uint32_t* cluster){
    spinlock_acquire(&fat_allocate_cluster_lock);
    for(uint64_t i = ctx->next_free_cluster; i < ctx->fat_entry_count; i++){
        if((fat_get_next_cluster(ctx, i)) == 0){
            fat_set_next_cluster(ctx, i, END_OF_CLUSTERCHAIN);
            ctx->next_free_cluster = i;
            ctx->fsi->free_cluster_count--;
            fat_write_fs_info_sector(ctx);
            spinlock_release(&fat_allocate_cluster_lock);
            *cluster = i;
            return 0;
        }
    }
    spinlock_release(&fat_allocate_cluster_lock);
    return EINVAL;
}

static int fat_free_cluster(fat_context_t* ctx, uint32_t cluster){
    spinlock_acquire(&fat_allocate_cluster_lock);
    fat_set_next_cluster(ctx, cluster, 0);
    ctx->fsi->free_cluster_count++;
    fat_write_fs_info_sector(ctx);
    spinlock_release(&fat_allocate_cluster_lock);
    return 0;
}

static int fat_free_all_following_clusters(fat_context_t* ctx, uint32_t cluster){
    uint32_t next_cluster = fat_get_next_cluster(ctx, cluster);

    fat_free_cluster(ctx, cluster);

    if(next_cluster < 0x0FFFFFF8){
        return fat_free_all_following_clusters(ctx, next_cluster);
    }else{
        return 0;
    }
}


static int fat_read_cluster(fat_context_t* ctx, uint32_t cluster, uint32_t alignement, uint64_t size, void* buffer){
    uint64_t start = ctx->partition->start + lba_to_bytes(cluster_to_lba(ctx, cluster)) + alignement;
    if(start + size > ctx->partition->start + ctx->partition->size){
        return EINVAL;
    }
    return ctx->partition->device->read(ctx->partition->device, start, size, buffer);
}


static int fat_write_cluster(fat_context_t* ctx, uint32_t cluster, uint32_t alignement, uint64_t size, void* buffer){
    uint64_t start = ctx->partition->start + lba_to_bytes(cluster_to_lba(ctx, cluster)) + alignement;
    if(start + size > ctx->partition->start + ctx->partition->size){
        return EINVAL;
    }
    return ctx->partition->device->write(ctx->partition->device, start, size, buffer);
}

static int fat_get_cluster_chain_count(fat_context_t* ctx, uint32_t cluster){
    int i = 0;
    while(cluster < 0xFFFFFF8 && cluster != 0){
        cluster = fat_get_next_cluster(ctx, cluster);
        i++;
    }
    return i;
}

static int fat_read_cluster_chain(fat_context_t* ctx, uint32_t current_cluster, uint32_t alignement, uint64_t size, uint64_t* size_read,  void* buffer){
    uintptr_t buffer_iteration = (uintptr_t)buffer;
    size_t size_to_read = 0;


    if(alignement < ctx->cluster_size){
        size_to_read = size;
        if(alignement + size_to_read > ctx->cluster_size){
            size_to_read = ctx->cluster_size - alignement;
        }
        assert(!fat_read_cluster(ctx, current_cluster, alignement, size_to_read, (void*)buffer_iteration));
        alignement = 0;
    }else{
        alignement -= ctx->cluster_size;
    }
    

    size -= size_to_read;
    *size_read += size_to_read;
    buffer_iteration += size_to_read;

    uint32_t next_cluster = fat_get_next_cluster(ctx, current_cluster);

    if(size > 0){
        if(next_cluster >= 0xFFFFFF8 || next_cluster == 0){
            return EINVAL; // this is still a success for the size store in size_read field
        }
        return fat_read_cluster_chain(ctx, next_cluster, alignement, size, size_read, (void*)buffer_iteration);
    }else{
        return 0;
    }
}

static int fat_write_cluster_chain(fat_context_t* ctx, uint32_t current_cluster, uint32_t alignement, uint64_t size, uint64_t* size_write,  void* buffer, uint8_t flags){    
    uintptr_t buffer_iteration = (uintptr_t)buffer;
    size_t size_to_write = 0;
    uint64_t next_alignement;


    if(alignement < ctx->cluster_size){
        size_to_write = size;
        if(alignement + size_to_write > ctx->cluster_size){
            size_to_write = ctx->cluster_size - alignement;
        }
        if(size_to_write){
            assert(!fat_write_cluster(ctx, current_cluster, alignement, size_to_write, (void*)buffer_iteration));
        }
        next_alignement = 0;
    }else{
        next_alignement = alignement - ctx->cluster_size;
    }

    size -= size_to_write;
    *size_write += size_to_write;
    buffer_iteration += size_to_write;

    uint32_t next_cluster = fat_get_next_cluster(ctx, current_cluster);

    if(size > 0 || next_alignement > 0){
        if(next_cluster >= 0xFFFFFF8 || next_cluster == 0){
            if(fat_allocate_cluster(ctx, &next_cluster)){
                return EIO;
            }
            fat_set_next_cluster(ctx, current_cluster, next_cluster);
        }
        return fat_write_cluster_chain(ctx, next_cluster, next_alignement, size, size_write, (void*)buffer_iteration, flags);
    }else{
        if(flags & WRITE_CLUSTER_CHAIN_FLAG_FWZ){
            uint32_t fill_start = alignement + size_to_write;
            size_t fill_size_to_write = ctx->cluster_size - fill_start;
            assert(!fat_write_cluster(ctx, current_cluster, fill_start, fill_size_to_write, (void*)ctx->cluster_zero_buffer));
        }

        if(!(flags & WRITE_CLUSTER_CHAIN_FLAG_EOC) || next_cluster >= 0xFFFFFF8 || next_cluster == 0){
            return 0;
        }

        // we have to free every following clusters
        fat_free_all_following_clusters(ctx, next_cluster);

        // put the end of file after the current cluster
        fat_set_next_cluster(ctx, current_cluster, END_OF_CLUSTERCHAIN);

        return 0;
    }

}

static bool fat_entry_valid(fat_short_entry_t* dir){
    return (dir->name[0] != 0) && (dir->name[0] != 0x05) && (dir->name[0] != 0xE5);
}

static bool fat_is_lfn(fat_short_entry_t* dir){
    return (*(uint8_t*)&dir->attributes) == 0xF;
}

static void fat_set_entry_as_lfn(fat_short_entry_t* dir){
    *(uint8_t*)&dir->attributes = 0xF;
}

static void fat_link_entry_to_lfn(fat_short_entry_t* dir){
    *(uint8_t*)&dir->attributes |= 0x20;
}

static int fat_parse_lfn(char* name, fat_long_entry_name_t* lfn){
    uint8_t index = 0;
    for(uint8_t i = 0; i < 5; i++){
        name[index++] = lfn->name1[i] & 0xFF;
    }
    for(uint8_t i = 0; i < 6; i++){
        name[index++] = lfn->name2[i] & 0xFF;
    }
    for(uint8_t i = 0; i < 2; i++){
        name[index++] = lfn->name3[i] & 0xFF;
    }

    return 0;
}

static int fat_set_name_to_lfn(char* name, fat_long_entry_name_t* lfn){
    char name_buffer[LFN_NAME_SIZE];
    size_t name_len = strlen(name);
    char* name_to_copy;
    bool fill_name = false;
    if(name_len < LFN_NAME_SIZE){
        name_to_copy = (char*)name_buffer;
        memcpy(name_to_copy, name, name_len);
        name_to_copy[name_len] = '\0';
        if(name_len + 1 < LFN_NAME_SIZE){
            fill_name = true;
        }
    }else{
        name_to_copy = (char*)name;
    }
    uint8_t index = 0;
    for(uint8_t i = 0; i < 5; i++){
        lfn->name1[i] = name_to_copy[index++];
    }
    for(uint8_t i = 0; i < 6; i++){
        lfn->name2[i] = name_to_copy[index++];
    }
    for(uint8_t i = 0; i < 2; i++){
        lfn->name3[i] = name_to_copy[index++];
    }
    
    if(fill_name){
        for(uint8_t i = name_len + 1; i < LFN_NAME_SIZE; i++){
            if(i < 5){
                lfn->name1[i] = 0xffff;
            }else if(i < 11){
                lfn->name2[i - 5] = 0xffff;
            }else{
                lfn->name3[i - 11] = 0xffff;
            }
        }
    }

    return 0;
}

static uint8_t fat_get_lfn_check_sum(char* name){
    short name_len;
    unsigned char check_sum;
    check_sum = 0;
    for(name_len = 11; name_len != 0; name_len--){
        check_sum = ((check_sum & 1) ? 0x80 : 0) + (check_sum >> 1) + *name++;
    }
    return (check_sum);
}

static bool fat_is_need_lfn(char* name, bool is_file){
    size_t name_len = strlen(name);
    if(is_file){
        if(name_len > 8){
            return true;
        }
    }else{
        if(name_len > 11){
            return true;
        }
    }

    for(int i = 0; i < name_len; i++){
        if(name[i] != to_upper(name[i])){
            return true;
        }
    }    

    return false;
}

static int fat_parse_sfn(char* name, fat_short_entry_t* dir){
    int last = 0;
    for(; last < 8; last++){
        if(dir->name[last] == ' '){
            break;
        }
    }

    memcpy(name, dir->name, last);

    if(!dir->attributes.directory){
        name[last] = '.';
        last++;
        memcpy(name + last, &dir->name[8], 3);
        last += 3;
    }

    name[last] = '\0';

    return 0;
}

static int fat_set_name_to_sfn(char* name, fat_short_entry_t* dir){
    size_t name_len = strlen(name);
    if(dir->attributes.directory){
        bool over_size = false;
        if(name_len > 11){
            name_len = 11;
            over_size = true;
        }
        memcpy(dir->name, name, name_len);
        if(over_size){
            dir->name[9] = '~';
            dir->name[10] = '1';
        }else{
            for(int i = name_len; i < 11; i++){
                dir->name[i] = ' ';
            }
        }
    }else{
        size_t name_len = 0;
        char* name_iteration = name;
        while(*name_iteration){
            if(*name_iteration == '.'){
                break;
            }
            name_len++;
            name_iteration++;
        }
        bool over_size = false;
        memcpy(&dir->name[8], (void*)((uintptr_t)name_iteration + (uintptr_t)1), 3);
        if(name_len > 8){
            name_len = 8;
            over_size = true;
        }
        memcpy(dir->name, name, name_len);
        if(over_size){
            dir->name[6] = '~';
            dir->name[7] = '1';
        }else{
            for(int i = name_len; i < 7; i++){
                dir->name[i] = ' ';
            }
        }
    }
    for(int i = 0; i < 11; i++){
        dir->name[i] = to_upper(dir->name[i]);
    }
    return 0;
}

static int fat_read_one_entry(fat_context_t* ctx, uint32_t cluster, uint32_t entry_number, fat_short_entry_t* dir){
    return fat_read_cluster(ctx, cluster, entry_number * ENTRY_SIZE, ENTRY_SIZE, dir);
}

static fat_short_entry_t* fat_read_entry_with_cache(fat_context_t* ctx, uint32_t cluster_base, uint32_t entry_number, void* cluster_buffer, uint32_t* cluster_cache_id_count_from_base, uint32_t* last_cluster_read){
    uint32_t cluster_count_from_base = entry_number / ctx->entries_per_cluster;

    /* Caching system */
    if(cluster_count_from_base != *cluster_cache_id_count_from_base - 1){ // we begin to one
        *cluster_cache_id_count_from_base -= 1; // we begin to one
        uint32_t cluster;
        uint32_t cluster_index;

        if(*cluster_cache_id_count_from_base && cluster_count_from_base > *cluster_cache_id_count_from_base){
            cluster = *last_cluster_read;
            cluster_index = *cluster_cache_id_count_from_base;
        }else{
            cluster = cluster_base;
            cluster_index = 0;
        }

        for(; cluster_index < cluster_count_from_base; cluster_index++){
            cluster = fat_get_next_cluster(ctx, cluster);
            if(cluster >= 0xFFFFFF8 || cluster == 0){
                return NULL;
            }
        }

        fat_read_cluster(ctx, cluster, 0, ctx->cluster_size, cluster_buffer);
        *cluster_cache_id_count_from_base = cluster_count_from_base + 1; // we begin to one
        *last_cluster_read = cluster;
    }

    /* Read entry from cache */
    uint32_t entry_number_in_cluster = entry_number % ctx->entries_per_cluster;
    return (fat_short_entry_t*)((uintptr_t)cluster_buffer + (uintptr_t)entry_number_in_cluster * (uintptr_t)ENTRY_SIZE);
}

static size_t fat_get_dir_size(fat_context_t* ctx, fat_short_entry_t* dir, void* cluster_buffer){
    uint32_t cluster_cache_id_count_from_base = 0;
    uint32_t last_cluster_read = 0;

    uint32_t entry_index = 0;
    uint32_t last_valid_index = 0;

    uint32_t cluster_base = fat_get_cluster_entry(ctx, dir);
    while((dir = fat_read_entry_with_cache(ctx, cluster_base, entry_index, cluster_buffer, &cluster_cache_id_count_from_base, &last_cluster_read)) != NULL){
        entry_index++;
        if(fat_entry_valid(dir)){
            last_valid_index = entry_index;
        }
    }

    return (size_t)last_valid_index * ENTRY_SIZE;
}

static int fat_find_entry_info(fat_context_t* ctx, uint32_t current_cluster, const char* name, void* cluster_buffer, uint64_t* sfn_position, uint32_t* sfn_entry_index, uint32_t* lfn_first_entry_index){
    char entry_name[256];

    uint32_t cluster_cache_id_count_from_base = 0;
    uint32_t last_cluster_read = 0;

    uint32_t entry_index = 0;

    uint64_t last_entry_index_lfn = 0;

    bool is_last_entry_lfn = false;

    fat_short_entry_t* dir;
    while((dir = fat_read_entry_with_cache(ctx, current_cluster, entry_index, cluster_buffer, &cluster_cache_id_count_from_base, &last_cluster_read)) != NULL){
        if(fat_entry_valid(dir)){
            if(fat_is_lfn(dir)){
                fat_long_entry_name_t* lfn = (fat_long_entry_name_t*)dir;
                uint8_t order = lfn->order & ~0x40;

                last_entry_index_lfn = entry_index;

                for(uint8_t y = 0; y < order; y++){
                    lfn = (fat_long_entry_name_t*)fat_read_entry_with_cache(ctx, current_cluster, entry_index + y, cluster_buffer, &cluster_cache_id_count_from_base, &last_cluster_read);
                    fat_parse_lfn(&entry_name[LFN_NAME_SIZE * (order - 1 - y)], lfn);
                }

                entry_index += order - 1;

                is_last_entry_lfn = true;
            }else{
                if(!is_last_entry_lfn){
                    fat_parse_sfn(entry_name, dir);
                    last_entry_index_lfn = -1;
                }else{
                    is_last_entry_lfn = false;
                }
                // Only sfn entry can be return because lfn are just use to store string not the entry data
                if(!strcmp(name, entry_name)){
                    if(sfn_position != NULL){
                        *sfn_position = lba_to_bytes(cluster_to_lba(ctx, last_cluster_read)) + (entry_index % ctx->entries_per_cluster) * ENTRY_SIZE;
                    }
                    if(sfn_entry_index != NULL){
                        *sfn_entry_index = entry_index;
                    }
                    if(lfn_first_entry_index != NULL){
                        *lfn_first_entry_index = last_entry_index_lfn;
                    }
                    return 0;
                }            
            }
        }
        
        entry_index++;
    }

    return ENOENT;
}

static fat_short_entry_t* fat_find_entry(fat_context_t* ctx, uint32_t current_cluster, const char* name, void* cluster_buffer){
    char entry_name[256];

    uint32_t cluster_cache_id_count_from_base = 0;
    uint32_t last_cluster_read = 0;

    uint32_t entry_index = 0;

    bool is_last_entry_lfn = false;

    fat_short_entry_t* dir;
    while((dir = fat_read_entry_with_cache(ctx, current_cluster, entry_index, cluster_buffer, &cluster_cache_id_count_from_base, &last_cluster_read)) != NULL){
        if(fat_entry_valid(dir)){
            if(fat_is_lfn(dir)){
                fat_long_entry_name_t* lfn = (fat_long_entry_name_t*)dir;
                uint8_t order = lfn->order & ~0x40;

                for(uint8_t y = 0; y < order; y++){
                    lfn = (fat_long_entry_name_t*)fat_read_entry_with_cache(ctx, current_cluster, entry_index + y, cluster_buffer, &cluster_cache_id_count_from_base, &last_cluster_read);
                    fat_parse_lfn(&entry_name[LFN_NAME_SIZE * (order - 1 - y)], lfn);
                }

                entry_index += order - 1;

                is_last_entry_lfn = true;
            }else{
                if(!is_last_entry_lfn){
                    fat_parse_sfn(entry_name, dir);
                }else{
                    is_last_entry_lfn = false;
                }
                // Only sfn entry can be return because lfn are just use to store string not the entry data
                if(!strcmp(name, entry_name)){
                    return dir;
                }            
            }
        }
        
        entry_index++;
    }

    return NULL;
}

static int fat_find_entry_info_with_path(fat_context_t* ctx, fat_short_entry_t* dir, const char* path, void* cluster_buffer, uint64_t* sfn_position, uint32_t* sfn_entry_index, uint32_t* lfn_first_entry_index){
    char* entry_name = (char*)path;
    char* next_entry_name = strchr(entry_name, '/');
    uint32_t current_cluster = fat_get_cluster_entry(ctx, dir);
    while(next_entry_name != NULL){
        *next_entry_name = '\0';

        dir = fat_find_entry(ctx, current_cluster, entry_name, cluster_buffer);

        *next_entry_name = '/';

        if(dir == NULL){
            return ENOENT;
        }
        if(!dir->attributes.directory){
            return ENOENT;
        }

        current_cluster = fat_get_cluster_entry(ctx, dir);

        entry_name = next_entry_name + 1;
        next_entry_name = strchr(entry_name, '/');
    }

    return fat_find_entry_info(ctx, current_cluster, entry_name, cluster_buffer, sfn_position, sfn_entry_index, lfn_first_entry_index);
}

static fat_short_entry_t* fat_find_last_directory(fat_context_t* ctx, fat_short_entry_t* dir, const char* path, void* cluster_buffer, char** entry_name_out){
    char* entry_name = (char*)path;
    char* next_entry_name = strchr(entry_name, '/');
    uint32_t current_cluster = fat_get_cluster_entry(ctx, dir);
    while(next_entry_name != NULL){
        *next_entry_name = '\0';

        dir = fat_find_entry(ctx, current_cluster, entry_name, cluster_buffer);

        *next_entry_name = '/';

        if(dir == NULL){
            return NULL;
        }

        if(!dir->attributes.directory){
            return NULL;
        }
        
        current_cluster = fat_get_cluster_entry(ctx, dir);

        entry_name = next_entry_name + 1;
        next_entry_name = strchr(entry_name, '/');
    }

    if(entry_name_out != NULL){
        *entry_name_out = entry_name;
    }

    return dir;
}

static fat_short_entry_t* fat_find_entry_with_path(fat_context_t* ctx, fat_short_entry_t* dir, const char* path, void* cluster_buffer){
    char* entry_name;
    dir = fat_find_last_directory(ctx, dir, path, cluster_buffer, &entry_name);
    if(dir == NULL){
        return NULL;
    }
    uint32_t current_cluster = fat_get_cluster_entry(ctx, dir);
    return fat_find_entry(ctx, current_cluster, entry_name, cluster_buffer);
}

static int fat_find_entry_info_with_path_from_root(fat_context_t* ctx, const char* path, void* cluster_buffer, uint64_t* sfn_position, uint32_t* sfn_entry_index, uint32_t* lfn_first_entry_index){
    return fat_find_entry_info_with_path(ctx, ctx->root_dir, path, cluster_buffer, sfn_position, sfn_entry_index, lfn_first_entry_index);
}

static fat_short_entry_t* fat_find_entry_with_path_from_root(fat_context_t* ctx, const char* path, void* cluster_buffer){
    return fat_find_entry_with_path(ctx, ctx->root_dir, path, cluster_buffer);
}

static int fat_update_entry_with_path(fat_context_t* ctx, fat_short_entry_t* dir, const char* path, fat_short_entry_t* entry){
    void* cluster_buffer = malloc(ctx->cluster_size);
    uint64_t sfn_position;
    int err = fat_find_entry_info_with_path(ctx, dir, path, cluster_buffer, &sfn_position, NULL, NULL);
    free(cluster_buffer);
    if(err){
        return err;
    }
    return write_partition(ctx->partition, sfn_position, sizeof(fat_short_entry_t), entry);
}

static int fat_update_entry_with_path_from_root(fat_context_t* ctx, const char* path, fat_short_entry_t* entry){
    return fat_update_entry_with_path(ctx, ctx->root_dir, path, entry);
}

static int fat_add_entry_with_path(fat_context_t* ctx, fat_short_entry_t* dir, const char* path, fat_short_entry_t* entry, bool use_lfn,void* cluster_buffer){
    char* entry_name;

    dir = fat_find_last_directory(ctx, dir, path, cluster_buffer, &entry_name);

    if(dir == NULL){
        return ENOENT;
    }

    /* Remove dir from the cluster_buffer */
    fat_short_entry_t dir_tmp = *dir;
    dir = &dir_tmp;

    uint32_t base_cluster = fat_get_cluster_entry(ctx, dir);

    /* Check if the entry already exit */
    if(fat_find_entry(ctx, base_cluster, entry_name, cluster_buffer)){
        return EEXIST;
    }

    size_t dir_size = fat_get_dir_size(ctx, dir, cluster_buffer);

    uint8_t lfn_entries_count;
    if(use_lfn){
        lfn_entries_count = DIV_ROUNDUP(strlen(entry_name), LFN_NAME_SIZE);
    }else{
        lfn_entries_count = 0;
    }
    size_t size_of_new_entry = ENTRY_SIZE * (1 + lfn_entries_count); // sizeof(sfn entry) + (sizeof(lfn entry) * lfn_entries_count)
    void* entry_buffer = malloc(size_of_new_entry);

    fat_short_entry_t* sfn = (fat_short_entry_t*)((uintptr_t)entry_buffer + (uintptr_t)lfn_entries_count * ENTRY_SIZE);
    memcpy(sfn, entry, sizeof(fat_short_entry_t));

    fat_link_entry_to_lfn(sfn);
    fat_set_name_to_sfn(entry_name, sfn);

    if(use_lfn){
        uint8_t checksum = fat_get_lfn_check_sum((char*)sfn->name);

        for(uint8_t i = 0; i < lfn_entries_count; i++){
            fat_long_entry_name_t* lfn = (fat_long_entry_name_t*)((uintptr_t)entry_buffer + (uintptr_t)i * ENTRY_SIZE);
            fat_set_entry_as_lfn((fat_short_entry_t*)lfn);
            fat_set_name_to_lfn(&entry_name[LFN_NAME_SIZE * (lfn_entries_count - 1 - i)], lfn);
            lfn->order = (lfn_entries_count - i) | ((i == 0) ? LAST_LONG_ENTRY : 0);
            lfn->type = 0; 
            lfn->reserved = 0; 
            lfn->checksum = checksum; 
        }
    }


    size_t size_write;
    fat_write_cluster_chain(ctx, base_cluster, dir_size, size_of_new_entry, &size_write, entry_buffer, WRITE_CLUSTER_CHAIN_FLAG_EOC);
    
    return 0;
}

static int fat_add_entry_with_path_from_root(fat_context_t* ctx, const char* path, fat_short_entry_t* entry, bool use_lfn, void* cluster_buffer){
    return fat_add_entry_with_path(ctx, ctx->root_dir, path, entry, use_lfn, cluster_buffer);
}

static int fat_remove_entry_with_path(fat_context_t* ctx, fat_short_entry_t* dir, const char* path, void* cluster_buffer){
    char* entry_name;

    dir = fat_find_last_directory(ctx, dir, path, cluster_buffer, &entry_name);

    if(dir == NULL){
        return ENOENT;
    }

    /* Remove dir from the cluster_buffer */
    fat_short_entry_t dir_tmp = *dir;
    dir = &dir_tmp;

    uint32_t base_cluster = fat_get_cluster_entry(ctx, dir);

    uint32_t sfn_index;
    uint32_t lfn_index;

    int err = fat_find_entry_info_with_path_from_root(ctx, path, cluster_buffer, NULL, &sfn_index, &lfn_index);

    if(err){
        return err;
    }

    uint32_t first_index_to_remove;
    uint32_t first_index_to_move;

    if(lfn_index != -1){
        first_index_to_remove = lfn_index;
        first_index_to_move = sfn_index + 1;
    }else{
        first_index_to_remove = sfn_index;
        first_index_to_move = sfn_index + 1;
    }

    uint64_t alignement_source = first_index_to_move * ENTRY_SIZE;
    uint64_t alignement_destination = first_index_to_remove * ENTRY_SIZE;
    uint64_t size_to_copy = fat_get_dir_size(ctx, dir, cluster_buffer) - alignement_source;
    void* buffer_to_copy = malloc(size_to_copy);

    uint64_t size_read_tmp;
    assert(!fat_read_cluster_chain(ctx, base_cluster, alignement_source, size_to_copy, &size_read_tmp, buffer_to_copy));
    
    uint64_t size_write_tmp;
    assert(!fat_write_cluster_chain(ctx, base_cluster, alignement_destination, size_to_copy, &size_write_tmp, buffer_to_copy, WRITE_CLUSTER_CHAIN_FLAG_EOC | WRITE_CLUSTER_CHAIN_FLAG_FWZ));
    return 0;
}

static int fat_remove_entry_with_path_from_root(fat_context_t* ctx, const char* path, void* cluster_buffer){
    return fat_remove_entry_with_path(ctx, ctx->root_dir, path, cluster_buffer);
}

static int fat_clear_entry_data(fat_context_t* ctx, fat_short_entry_t* dir, void* cluster_buffer){
    uint32_t base_cluster = fat_get_cluster_entry(ctx, dir);
    fat_free_all_following_clusters(ctx, base_cluster);
    return 0;
}

static int fat_clear_entry_data_with_path(fat_context_t* ctx, fat_short_entry_t* dir, const char* path, void* cluster_buffer){
    dir = fat_find_entry_with_path(ctx, dir, path, cluster_buffer);
    if(dir == NULL){
        return ENOENT;
    }
    uint32_t base_cluster = fat_get_cluster_entry(ctx, dir);
    fat_free_all_following_clusters(ctx, base_cluster);
    return 0;
}

static int fat_clear_entry_data_with_path_from_root(fat_context_t* ctx, const char* path, void* cluster_buffer){
    return fat_clear_entry_data_with_path(ctx, ctx->root_dir, path, cluster_buffer);
}


static int fat_remove_and_clear_entry_with_path(fat_context_t* ctx, fat_short_entry_t* dir, const char* path, void* cluster_buffer){
    char* entry_name;

    fat_short_entry_t* dir_parent = fat_find_last_directory(ctx, dir, path, cluster_buffer, &entry_name);
    
    if(dir_parent == NULL){
        free(cluster_buffer);
        return ENOENT;
    }

    /* Remove dir from the cluster_buffer */
    fat_short_entry_t dir_parent_tmp = *dir_parent;
    dir_parent = &dir_parent_tmp;


    fat_short_entry_t* entry = fat_find_entry_with_path(ctx, dir_parent, entry_name, cluster_buffer);

    if(entry == NULL){
        free(cluster_buffer);
        return ENOENT;
    }

    /* Remove entry from the cluster_buffer */
    fat_short_entry_t entry_tmp = *entry;
    entry = &entry_tmp;

    fat_clear_entry_data(ctx, entry, cluster_buffer);

    assert(!fat_remove_entry_with_path(ctx, dir_parent, entry_name, cluster_buffer));

    return 0;
}

static int fat_remove_and_clear_entry_with_path_from_root(fat_context_t* ctx, const char* path, void* cluster_buffer){
    return fat_remove_and_clear_entry_with_path(ctx, ctx->root_dir, path, cluster_buffer);
}


static int fat_rename_entry_with_path(fat_context_t* ctx, fat_short_entry_t* old_dir, fat_short_entry_t* new_dir, const char* old_path, const char* new_path, void* cluster_buffer){
    if(new_path[0] == '\0' || old_path[0] == '\0'){
        return ENOENT;
    }

    fat_short_entry_t* entry = fat_find_entry_with_path(ctx, old_dir, old_path, cluster_buffer);

    if(fat_get_cluster_entry(ctx, entry) == ctx->bpb->root_cluster_number){
        /* Don't allow to move or rename root dir*/
        return EACCES;
    }

    /* Remove entry from the cluster_buffer */
    fat_short_entry_t entry_tmp = *entry;
    entry = &entry_tmp;

    char* new_entry_name = strrchr(new_path, '/');

    if(new_entry_name == NULL){
        new_entry_name = (char*)new_entry_name;
    }

    int err = fat_add_entry_with_path(ctx, new_dir, new_path, entry, fat_is_need_lfn(new_entry_name, !entry->attributes.directory), cluster_buffer);
    if(err){
        return err;
    }

    err = fat_remove_entry_with_path(ctx, old_dir, old_path, cluster_buffer);
    if(err){
        return err;
    }

    if(entry->attributes.directory){
        fat_short_entry_t* parent_dir = fat_find_last_directory(ctx, new_dir, new_path, cluster_buffer, NULL);
        
        uint32_t cluster_parent = fat_get_cluster_entry(ctx, parent_dir);

        fat_short_entry_t special_entry = *entry;
        fat_set_name_to_sfn("..", &special_entry);
        fat_set_cluster_entry(&special_entry, cluster_parent);
        if(fat_update_entry_with_path(ctx, entry, "..", &special_entry)){
            return EIO; // If there is no dotdot entry in a directory other than the root directory, the filesystem may be corrupted.
        }
    }
    
    return 0;
}

static int fat_rename_entry_with_path_from_root(fat_context_t* ctx, const char* old_path, const char* new_path, void* cluster_buffer){
    return fat_rename_entry_with_path(ctx, ctx->root_dir, ctx->root_dir, old_path, new_path, cluster_buffer);
}


/* file */

static int fat_update_file_entry(fat_file_internal_t* file){
    return fat_update_entry_with_path_from_root(file->ctx, file->path, &file->entry);
}

static int fat_create_file(fat_context_t* ctx, const char* path, void* cluster_buffer){
    if(path[0] == '\0'){
        return ENOENT;
    }

    char* file_name = strrchr(path, '/');

    if(file_name == NULL){
        file_name = (char*)path;
    }

    fat_short_entry_t file_entry = {};
    file_entry.creation_time = fat_get_current_time();
    file_entry.creation_date = fat_get_current_date();
    file_entry.last_access_date = fat_get_current_date();
    file_entry.last_write_time = fat_get_current_time();
    file_entry.last_write_date = fat_get_current_date();

    return fat_add_entry_with_path_from_root(ctx, path, &file_entry, fat_is_need_lfn(file_name, true), cluster_buffer);
}

int fat_remove_file(fat_context_t* ctx, const char* path){
    void* cluster_buffer = malloc(ctx->cluster_size);

    int err = fat_remove_and_clear_entry_with_path_from_root(ctx, path, cluster_buffer);

    free(cluster_buffer);

    return err;
}

fat_file_internal_t* fat_open(fat_context_t* ctx, const char* path, int flags, mode_t mode, int* error){
    void* cluster_buffer = malloc(ctx->cluster_size);
    
    fat_short_entry_t* dir = fat_find_entry_with_path_from_root(ctx, path, cluster_buffer);

    if(dir == NULL){
        if(flags & O_CREAT){
            if((*error = fat_create_file(ctx, path, cluster_buffer))){
                free(cluster_buffer);
                return NULL;
            }
            dir = fat_find_entry_with_path_from_root(ctx, path, cluster_buffer);
            assert(dir != NULL);
        }else{
            *error = ENOENT;
            return NULL;
        }
    }

    if(dir->attributes.directory){
        free(cluster_buffer);
        return NULL;
    }

    fat_file_internal_t* file = malloc(sizeof(fat_file_internal_t));
    memcpy(&file->entry, dir, sizeof(fat_short_entry_t));

    size_t path_size = strlen(path) + 1;
    file->path = malloc(path_size);
    memcpy(file->path, path, path_size);

    file->ctx = ctx;

    free(cluster_buffer);

    uint16_t current_date = fat_get_current_date();
    if(file->entry.last_access_date != current_date){
        file->entry.last_access_date = current_date;
        fat_update_file_entry(file);
    }

    return file;
}

int fat_read(fat_file_internal_t* file, uint64_t start, size_t size, size_t* size_read, void* buffer){
    uint64_t size_read_tmp = 0;

    uint32_t base_cluster = fat_get_cluster_file(&file->entry);

    if(base_cluster == 0){
        return EIO;
    }

    int err = fat_read_cluster_chain(file->ctx, base_cluster, start, (uint64_t)size, &size_read_tmp, buffer);

    *size_read = (size_t)size_read_tmp;

    return err;
}

int fat_write(fat_file_internal_t* file, uint64_t start, size_t size, size_t* size_write, void* buffer, bool is_end_of_file){
    if(size == 0){
        return 0;
    }
    
    uint64_t size_write_tmp = 0;

    uint32_t base_cluster = fat_get_cluster_file(&file->entry);

    if(base_cluster == 0){
        // Allocate the first cluster of the entry
        if(fat_allocate_cluster(file->ctx, &base_cluster)){
            return EIO;
        }
        fat_set_cluster_entry(&file->entry, base_cluster);
    }

    int err = fat_write_cluster_chain(file->ctx, base_cluster, start, (uint64_t)size, &size_write_tmp, buffer, is_end_of_file ? WRITE_CLUSTER_CHAIN_FLAG_EOC : 0);

    *size_write = (size_t)size_write_tmp;

    if(is_end_of_file || start + *size_write > file->entry.size){
        file->entry.size = start + *size_write;
    }

    file->entry.last_write_time = fat_get_current_time();
    file->entry.last_write_date = fat_get_current_date();


    fat_update_file_entry(file);

    return err;
}


/* directory */

int fat_create_dir(fat_context_t* ctx, const char* path){
    if(path[0] == '\0'){
        return ENOENT;
    }

    char* dir_name = strrchr(path, '/');

    if(dir_name == NULL){
        dir_name = (char*)path;
    }

    fat_short_entry_t dir_entry = {};
    dir_entry.attributes.directory = true;
    dir_entry.creation_time = fat_get_current_time();
    dir_entry.creation_date = fat_get_current_date();
    dir_entry.last_access_date = fat_get_current_date();
    dir_entry.last_write_time = fat_get_current_time();
    dir_entry.last_write_date = fat_get_current_date();

    uint32_t base_cluster;

    if(fat_allocate_cluster(ctx, &base_cluster)){
        return ENOMEM;
    }

    /* clear cluster data */
    assert(!fat_write_cluster(ctx, base_cluster, 0, ctx->cluster_size, (void*)ctx->cluster_zero_buffer));

    fat_set_cluster_entry(&dir_entry, base_cluster);

    void* cluster_buffer = malloc(ctx->cluster_size);

    int err = fat_add_entry_with_path_from_root(ctx, path, &dir_entry, fat_is_need_lfn(dir_name, true), cluster_buffer);

    if(err){
        free(cluster_buffer);
        fat_free_cluster(ctx, base_cluster);
        return err;
    }


    fat_short_entry_t special_entry = dir_entry;

    fat_set_cluster_entry(&special_entry, base_cluster);
    assert(!fat_add_entry_with_path(ctx, &dir_entry, ".", &special_entry, false, cluster_buffer));

    fat_short_entry_t* parent_dir = fat_find_last_directory(ctx, ctx->root_dir, path, cluster_buffer, NULL);
    
    uint32_t cluster_parent = fat_get_cluster_entry(ctx, parent_dir);
    if(cluster_parent == ctx->bpb->root_cluster_number){
        cluster_parent = 0; // starting cluster of the parent of this directory (which is 0 if this directories parent is the root directory)
    }

    fat_set_cluster_entry(&special_entry, cluster_parent);
    assert(!fat_add_entry_with_path(ctx, &dir_entry, "..", &special_entry, false, cluster_buffer));


    free(cluster_buffer);

    return 0;
}

int fat_remove_dir(fat_context_t* ctx, const char* path){
    void* cluster_buffer = malloc(ctx->cluster_size);

    fat_short_entry_t* dir = fat_find_entry_with_path_from_root(ctx, path, cluster_buffer);

    /* Remove dir from the cluster_buffer */
    fat_short_entry_t dir_tmp = *dir;
    dir = &dir_tmp;

    if(fat_get_cluster_entry(ctx, dir) == ctx->bpb->root_cluster_number){
        return EACCES; // can't delete root dir
    }else{
        if(fat_get_dir_size(ctx, dir, cluster_buffer) > DIR_MINIMUM_SIZE){
            return ENOTEMPTY;
        }
    }

    int err = fat_remove_and_clear_entry_with_path_from_root(ctx, path, cluster_buffer);

    free(cluster_buffer);

    return err;
}


/* directory and file */

int fat_rename(fat_context_t* ctx, const char* old_path, const char* new_path){
    void* cluster_buffer = malloc(ctx->cluster_size);

    int err = fat_rename_entry_with_path_from_root(ctx, old_path, new_path, cluster_buffer);

    free(cluster_buffer);

    return err;
}

/* mount */

int fat_mount(partition_t* partition){
    fat_context_t* ctx = malloc(sizeof(fat_context_t));
    ctx->partition = partition;

    ctx->bpb = malloc(sizeof(bpb_t));
    assert(!fat_read_boot_sector(ctx));

    if(ctx->bpb->identifier != FAT32_SIGNATURE){
        free(ctx->bpb);
        free(ctx);
        return EINVAL;
    }

    ctx->fsi = malloc(sizeof(fs_info_t));
    assert(!fat_read_fs_info_sector(ctx));


    uint64_t total_sector = 0;
    if(ctx->bpb->total_sectors16 != 0){
        total_sector = ctx->bpb->total_sectors16;
    }else{
        total_sector = ctx->bpb->total_sectors32;
    }

    if(ctx->bpb->sectors_per_cluster){
        ctx->cluster_count = total_sector / ctx->bpb->sectors_per_cluster;
    }else{
        ctx->cluster_count = total_sector;
    }
    ctx->data_cluster_count = total_sector - (ctx->bpb->root_cluster_number + (ctx->bpb->fats * ctx->bpb->sectors_per_fat));

    if((uint64_t)ctx->fsi->next_free_cluster <= ctx->cluster_count){
        ctx->next_free_cluster = (uint64_t)ctx->fsi->next_free_cluster;
    }else{
        ctx->next_free_cluster = 0;
    }

    ctx->fat_size = lba_to_bytes(ctx->bpb->sectors_per_fat);
    ctx->fat1_position = lba_to_bytes(ctx->bpb->reserved_sectors);
    ctx->fat2_position = ctx->fat1_position + ctx->fat_size;
    
    ctx->fat = malloc(lba_to_bytes(ctx->bpb->sectors_per_fat));
    assert(!fat_read_fat(ctx));

    ctx->first_usable_lba = ctx->bpb->reserved_sectors + ctx->bpb->fats * ctx->bpb->sectors_per_fat;
    ctx->cluster_size = lba_to_bytes(ctx->bpb->sectors_per_cluster);
    ctx->entries_per_cluster = lba_to_bytes(ctx->bpb->sectors_per_cluster) / ENTRY_SIZE;
    ctx->fat_entry_count = lba_to_bytes(ctx->bpb->sectors_per_fat) / sizeof(uint32_t);
    ctx->cluster_zero_buffer = calloc(1, ctx->cluster_size);

    ctx->root_dir = calloc(1, sizeof(fat_short_entry_t));
    ctx->root_dir->attributes.directory = true;
    fat_set_cluster_entry(ctx->root_dir, ctx->bpb->root_cluster_number);

    fat_create_dir(ctx, "heyiamdir");
    int error;
    fat_file_internal_t* file = fat_open(ctx, "heyiamdir/./hello_world0.txt", O_CREAT, 0, &error);
    assert(file != NULL);
    char* buffer = "hello world !!";
    size_t size = strlen(buffer);
    size_t size_tmp = 0;
    for(int i = 0; i < 37; i++){
        size_t size_tmp = 0;
        fat_write(file, i * size, size, &size_tmp, buffer, true);
    }

    fat_create_dir(ctx, "test0");
    fat_create_dir(ctx, "test1");
    // fat_create_dir(ctx, "test2");
    fat_create_dir(ctx, "test3");
    fat_create_dir(ctx, "test4");
    fat_create_dir(ctx, "test5");
    fat_remove_dir(ctx, "test0");
    // fat_remove_dir(ctx, "heyiamdir");

    fat_remove_file(ctx, "pci0.ksys");

    // assert(!fat_rename(ctx, "pci.ksys", "heyiamdir/pci.ksys"));
    assert(!fat_rename(ctx, "limine", "heyiamdir/testmove"));

    return 0;
}