#include <sys/timer.h>
#include <sys/irq.h>
#include <stdio.h>
#include <sys/defs.h>
#include <sys/kprintf.h>

int jiffy=0;
static unsigned char* v_ptr =(unsigned char*) 0xB8000 +(2*80*24+39*2);

void outpb(uint16_t port, uint16_t val)
{
        __asm__ volatile ("outb %%al, %%dx"::"d" (port), "a" (val));

}

/*void timer_install(){
	irq_hander_install(0, timer_handler); // setting IRQ0
	timer_set();
	
}*/

void timer_handler(){
  outpb(0x20, 0x20);
// timer_set();

  jiffy++;
//  kprintf("tick ");
  
  if(jiffy % 18==0){
//  char * str = "Seconds";
//  v_ptr =(unsigned char*) 0xB8000 +(2*80*24+60*2);
	v_ptr= (unsigned char *)0xFFFFFFFF800B8000+(2*80*24+60*2);
        char str[20];
        int sec = jiffy/18;
        int len = 0;
        while(sec > 0){
	str[len] = '0' + sec%10;
	// kprintf("%c,value = ",str[len]);
        len++;
	sec= sec/10;
	}
	str[len] = '\0';		

	while((len--) >0)
    {
        // kprintf("digit = %c ",str[len]);
        *v_ptr  = str[len];
         v_ptr +=2;

    }
        // kprintf("\n ");
	//kprintf( "Seconds %d :",jiffy/18);	
}

}

void timer_set(){
int divisor = 1193180/HZ;

//kprintf("%d\n", divisor);
outpb(0X40, 0X36);   // command byte
outpb(0X40, divisor & 0xFF); //low byte
outpb(0X40, divisor >> 8); //high byte

}

void call_timer() {
kprintf("%s",jiffy);
        

     
}


