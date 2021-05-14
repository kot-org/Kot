#ifndef _ahcicomm_h
#define _ahcicomm_h
#include <sys/ahci.h>

int ahci_read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t *buf);

int ahci_write(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t* buffer );

#endif        
