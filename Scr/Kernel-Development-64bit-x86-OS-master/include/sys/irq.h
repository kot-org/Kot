#ifndef IRQ_H
#define IRQ_h

#define HZ 100
extern void handler_timer();
extern void handler_keyboard();

void irq_install();
void irq_remap();
void irq_hander_install(int req, void(* handler)(void));
void irq_hander_uninstall(int req);
void PIC_remap();

#endif
