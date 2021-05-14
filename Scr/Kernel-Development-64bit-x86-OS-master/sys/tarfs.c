#include <sys/defs.h>
#include <sys/string.h>
#include <sys/tarfs.h>
#include <sys/elf64.h>
#include <sys/kprintf.h>

tarfs_entry tarfs_fs[100];

unsigned int getSize(const char *input)
{   
    unsigned int size = 0;
    unsigned int k;
    unsigned int count = 1;
    
    for (k = 11; k > 0; k--, count *= 8){
        size += ((input[k - 1] - '0') * count);
    }
    
    return size;
}

int sstrncmp(const char *str1,const char *str2,int count)
{
        while(count) {
                if(*str1 && *str2 && *str1 == *str2) {
                str1++;
                str2++;
                } else {
                        return *str1-*str2;
                        break;
                }
                count--;

        }

        return 0;
}


uint64_t stoi(char *s) // the message and then the line #
{
	uint64_t i;
	i = 0;
	while(*s >= '0' && *s <= '9')
	{
		i = i * 10 + (*s - '0');
		s++;
	}
	return i;
}


char* sstrcpy(char *dst, const char *src) {
	char *ret;

	ret = dst;
	while ((*dst++ = *src++) != '\0')
		/* do nothing */;
	return ret;
}

int sstrcmp(const char *p, const char *q) {
	while (*p || *q) {
		if (*p != *q)
			return -1;
		p++, q++;
	}
	return 0;
}

uint64_t octalToDecimal(uint64_t n)
{

	uint64_t decimal_equiv = 0, i = 0, rem = 0, pow_oct = 1;
	while (n!=0)
	{
		rem = (int)n%10;
		n /= 10;
		if(i>0)			
			pow_oct *=8;
		decimal_equiv += rem*pow_oct;
		i++;
	}
	return decimal_equiv;
}


int get_per_ind(char* dir)
{
	char name[100];
    	int len = strlen(dir);
    	sstrcpy(&name[0], dir);
    	len = len-2;
   	// kkprintff("  {%d} ", len); 
    	while(name[len] != '/')
    	{
        	len--;
        	if(len == 0)
            		return 999;
    	}
   	// kkprintff("  {%d} ", len); 
    	name[++len] = '\0';
    	int i = 0;
    	while(sstrcmp(&name[0], &(tarfs_fs[i].name[0])) !=  0)
        	i++;
    	// kkprintff("parent {%d}", i);
    	return i;
}

int get_per_ind_file(char* dir)
{
    	char name[100];
    	int len = strlen(dir);
    	sstrcpy(&name[0], dir);
    	len = len-1;
   	// kkprintff("  {%d} ", len); 
    	while(name[len] != '/')
    	{
        	len--;
        	if(len == 0)
            		return 999;
    	}
   	// kkprintff("  {%d} ", len); 
    	name[++len] = '\0';
    	int i = 0;
    	while(sstrcmp(&name[0], &(tarfs_fs[i].name[0])) !=  0)
        	i++;
    	// kkprintff("parent {%d}", i);
    	return i;
}

void tarfs_init()
{
       	struct posix_header_ustar *tarfs_var = (struct posix_header_ustar *)&_binary_tarfs_start;
	int i = 0, temp = 0;
	uint64_t size;
       	tarfs_entry tarfs_e;
       	//int curr_dir= 999;
	while(1)
	{
		tarfs_var = (struct posix_header_ustar *)(&_binary_tarfs_start + temp);
           	if(strlen(tarfs_var->name) == 0)
                	break;
          
		size = octalToDecimal(stoi(tarfs_var->size));
           	sstrcpy(&tarfs_e.name[0], tarfs_var->name);
           	tarfs_e.size = size;
           	tarfs_e.addr_hdr = (uint64_t)&_binary_tarfs_start + temp;
           	tarfs_e.typeflag = stoi(tarfs_var->typeflag);
           	if(tarfs_e.typeflag == FILE_TYPE)
                	tarfs_e.par_ind = get_per_ind_file(&(tarfs_e.name[0]));
           	else if(tarfs_e.typeflag == DIRECTORY)
           	{
                	tarfs_e.par_ind = get_per_ind(&(tarfs_e.name[0]));
               		// curr_dir = i;
           	}
           
            
           	tarfs_fs[i] = tarfs_e;
           	//kprintf("%s", tarfs_fs[i].name);
		//kprintf("\n"); 
           	//kprintf("   I[%d]     P[%d] \n", i, tarfs_fs[i].par_ind);
           	i++;
		if(size == 0)
			temp = temp + 512;
		else
			temp +=  (size%512==0) ? size + 512 : size + (512 - size%512) + 512;
	  }
        
}

/*
void tarfs_init()
{       
        uint64_t address= (uint64_t)&_binary_tarfs_start;
        unsigned int i;
        for(i =0 ; address <(uint64_t)&_binary_tarfs_end ;i++){
                struct posix_header_ustar *header = (struct posix_header_ustar*)address;
                unsigned int size = getSize(header->size);
                strcpy(tarfs_fs[i].name,header->name);
                tarfs_fs[i].size = size;            
                tarfs_fs[i].typeflag = atoi(header->typeflag);
                tarfs_fs[i].addr_hdr = address;
                if(tarfs_e.typeflag == FILE_TYPE)
                        tarfs_e.par_ind = get_per_ind_file(&(tarfs_e.name[0]));
                else if(tarfs_e.typeflag == DIRECTORY)
                {
                        tarfs_e.par_ind = get_per_ind(&(tarfs_e.name[0]));
                        // curr_dir = i;
                }     
                address += ((size/512) +1) * 512;
                if(size %512)
                        address+=512;
        }
	kprintf("\nRuns Fine");
}
*/

uint64_t getElfHeader(char * fileName)
{
        uint64_t address= (uint64_t)&_binary_tarfs_start; // starting from the first binary tarfs address 
        unsigned int i;
        for(i =0 ; address <(uint64_t)&_binary_tarfs_end ;i++){ // traversing till the end of the tarfs header address to find the address we need.
                struct posix_header_ustar *header = (struct posix_header_ustar*)address;
                unsigned int size = getSize(header->size);
                if(strcmp(header->name,fileName)==0) 
                {
                        return (uint64_t) (header+1); // the elf header sits next to the tarf header. So returning the address of the elf header.
                }
                address += ((size/512) +1) * 512; // rounding off the address.
                if(size %512)
                        address+=512;
        }
return (uint64_t)0;
}/*
uint64_t tarfs_open_file(char * filename)
{       
        int i=0;
        while(1)
        {       
                if(tarfs_fs[i].name==0) break;
                if(strcmp(&(tarfs_fs[i].name[0]), filename) == 0 && tarfs_fs[i].typeflag == FILE_TYPE)
                {       
                        return tarfs_fs[i].addr_hdr;
                }
        }
        kprintf("\n No such file ");
        kprintf("%s", filename);
        return 0;
}
*/

uint64_t opendir(char * directory)
{
	tarfs_entry tarfs_e;
        int i = 0;
        while(1)
        {
            tarfs_e = tarfs_fs[i];
            i++;
            if(strlen(tarfs_e.name) == 0)
                    break;

           //if(strcmp(&(tarfs_e.name[0]), directory) == 0 && tarfs_e.typeflag == DIRECTORY)
                if(strcmp(&(tarfs_e.name[0]), directory) ==0)   
                 return tarfs_e.addr_hdr;
                    // return i;
        }
        //kprintf("No such directory exist");
        kprintf("%s\n", directory);
        return 0;
}


uint64_t openfile(char * file)
{
	tarfs_entry tarfs_e;
        int i = 0;
        while(1)
        {
            tarfs_e = tarfs_fs[i];
            i++;
	   // kprintf("\n%s",tarfs_e);	
            if(strlen(tarfs_e.name) == 0)
                    break;

            if(strcmp(&(tarfs_e.name[0]), file) == 0 && tarfs_e.typeflag == FILE_TYPE)
               //if(strcmp(tarfs_e.name, file) == 0 && tarfs_e.typeflag == FILE_TYPE)
		{
	//		kprintf("%x",tarfs_e.addr_hdr);
			return i;	
		        //return tarfs_e.addr_hdr;
		}
        }
        kprintf("\n No such file exits !!");
        //kprintf("%s\n", file);
        return 0;
}

uint64_t readdir(char* direct)
{
	tarfs_entry tarfs_e;
    	int i = 0, parent = -1;
	if(strlen(direct)==0)
	{
		for(int k =0;k<100;k++)
		{
			if(strlen(tarfs_fs[k].name)!=0)
			kprintf("%s  ",tarfs_fs[k].name);
		}
		kprintf("\n");
		return 0;
	}
    	while(1)
    	{
        	tarfs_e = tarfs_fs[i];
        	if(strlen(tarfs_e.name) == 0)
            		break;
        
        	if( strcmp(&(tarfs_e.name[0]), direct) == 0)
         	{  
            		parent = i;
			i++; 
			continue; 
         	}
         	if((sstrncmp(&(tarfs_e.name[0]), direct, strlen(direct)) == 0 ) &&( tarfs_e.par_ind == parent))
         	{
            		kprintf("%s     ", tarfs_e.name + strlen(direct));
         	}
        	i++;
    	}
	kprintf("\n");
    	return 0;
}


int readfile(uint64_t fd, int size, uint64_t buf)
{
	uint64_t file_addr;	

	
	file_addr = tarfs_fs[fd - 1].addr_hdr;        

         //kprintf("\n No such file ");
   	struct posix_header_ustar* file_hdr = (struct posix_header_ustar *) file_addr; 
    	//struct posix_header_ustar* file_hdr = (struct posix_header_ustar *) 0xFFFFFFFF8020D560; 
	kprintf("%s", file_hdr->name);
    	
	int file_size =  octalToDecimal(stoi(file_hdr->size));
    	if(file_size < size)
        	size = file_size;
    	char* tmp =(char *)buf;
    	char* file_start_addr = (char *)(file_addr + 512);
    	// kprintf("\nsize %d file %x", size, file_start_addr);
    	int j = 0;
    	while(size-- > 0)
    	{
        	tmp[j++] = *file_start_addr++;
    	}    
    	tmp[j]='\0';
    	kprintf("\n");
    	kprintf("%s", tmp);

    	return size;

}

void closefile(uint64_t file_addr)
{

	unsigned long* file = (unsigned long* )file_addr;
	*file = 0;
	kprintf("\nFile at %p has been closed",file_addr);
}

void closedir(uint64_t dir_addr)
{

        unsigned long* dir = (unsigned long* )dir_addr;
        *dir = 0;
        kprintf("\nDirectory at %p has been closed",dir_addr);
}

/*
int tarfs_read_file(uint64_t fd, int size, char *buf)
{       
        struct posix_header_ustar * header = (struct posix_header_ustar*)fd;
        unsigned int file_size = getSize(header->size);
        if(file_size<size)
        {       
                size = file_size;
        }
        int len = size;
        char *temp = (char *) buf;
        char* file_addr = (char *)(fd + 512);
        int i = 0;
        while(size-- > 0)
        {       
                temp[i++] = *file_addr++;
        }    
        temp[i]='\0';
        return len;
}*/


uint64_t sy_chdir(char *path, struct task_struct *proc ){
	int ind=0;
	int len_path = strlen(path);
	int j=0;
	char new_path[50];
	tarfs_entry tarfs_e;
	int len = strlen(proc->cur_dir);
	if(path[0]=='.'){
		for(int i=0; i<len_path; i++){
			if(path[i]=='.' && (path[i+1]=='\0' || path[i+1]=='/')){
				strcat(new_path,proc->cur_dir);
				j=j+len;
			}
			else if(path[i]=='.' && path[i+1]=='.' && (path[i+2]=='\0' ||  path[i+2]=='/')){
				int i_mid;int cnt=0;
				for(i_mid=0; i_mid<len; i_mid++){
					if(proc->cur_dir[i_mid]=='/')
						cnt++;
				}
				for(i_mid=0; i_mid<len; i_mid++){
					if(proc->cur_dir[i_mid]=='/' && cnt>0){
						cnt--;
					}
					if(cnt>1){
						new_path[j]=proc->cur_dir[i_mid];
						j++;
					}
				}
				i=i+2;
			}
			else{
					new_path[j]=path[i];
					j++;
			}
	  
		}
	}
	else{
		strcpy(new_path,proc->cur_dir);
		if(new_path[len-1]!='/')
			new_path[len]='/';
		strcat(new_path,path);
	}
	int new_len=strlen(new_path);
	for(ind=0;ind<40;ind++){
		tarfs_e = tarfs_fs[ind];
		if((strncmp(new_path,&(tarfs_e.name[0]),new_len)==0) && (tarfs_e.typeflag==DIRECTORY)){
			strcpy(proc->cur_dir,new_path);
			return 1;
		}
	}
	return 0;
}



