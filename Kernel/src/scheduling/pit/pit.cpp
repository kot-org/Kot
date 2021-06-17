#include "pit.h"

namespace PIT{
    double TimeSinceBoot = 0;

    uint16_t Divisor = 65535;

    void Sleepd(double seconds){
        double startTime = TimeSinceBoot;
        double endTime = startTime + seconds;
        while(true){
            if((double)TimeSinceBoot >= (double)endTime){
                break;
            }
            asm("hlt");
        }
    }

    void Sleep(uint64_t milliSeconds){
        Sleepd((double)milliSeconds / 1000);
    }

    void SetDivisor(uint16_t divisor){
        if(divisor < 100) divisor = 100;
        Divisor = divisor;
        IoWrite8(0x40, (uint8_t)(divisor & 0x00ff));
        io_wait();
        IoWrite8(0x40, (uint8_t)(divisor & 0xff00) >> 8);
    }

    uint64_t GetFrequency(){
        return BaseFrequency / Divisor;
    }

    void SetFrequency(uint64_t frequency){
        SetDivisor(BaseFrequency / frequency);
    }

    void Tick(){  
        TimeSinceBoot += (double)1 / (double)GetFrequency();
    }
}
/*void local_apic_start_lints() {
	void* local_apic_ptr = madt_get_info()->local_apic_ptr;
	uint8_t local_apic_id = local_apic_get_register(local_apic_ptr, LOCAL_APIC_REG_OFFSET_ID) >> 24;
	// Set Spurious interrupts
	uint32_t spurious_reg = local_apic_get_register(local_apic_ptr, LOCAL_APIC_REG_OFFSET_SPURIOUS_INT_VECTOR);
	local_apic_set_register(local_apic_ptr, LOCAL_APIC_REG_OFFSET_SPURIOUS_INT_VECTOR, spurious_reg | LOCAL_APIC_INTERRUPT_VECTOR_SPURIOUS);
	// Set NMI from MADT
	size_t nmis_num = madt_get_info()->nmis_num;
	for(size_t i = 0; i < nmis_num; i++) {
		struct MADTNonMaskableInterrupt* nmi = madt_get_info()->nmis[i];
		if(nmi->local_apic_id != 0xff && nmi->local_apic_id != local_apic_id) {
			continue;
		}
		uint32_t nmi_entry = local_apic_create_register_value((struct LocalAPICInterruptRegister){
			.vector = LOCAL_APIC_INTERRUPT_VECTOR_LINT,
			.message_type = LOCAL_APIC_INTERRUPT_REGISTER_MESSAGE_TYPE_NMI,
			.trigger_mode = nmi->flags & 8 ? LOCAL_APIC_INTERRUPT_REGISTER_TRIGGER_MODE_LEVEL : LOCAL_APIC_INTERRUPT_REGISTER_TRIGGER_MODE_EDGE,
			.mask = LOCAL_APIC_INTERRUPT_REGISTER_MASK_DISABLE	// TODO figure the rest of this out
		});
		switch(nmi->lint) {
			case 0:
				local_apic_set_register(local_apic_ptr, LOCAL_APIC_REG_OFFSET_LVT_LINT0, nmi_entry);
				break;
			case 1:
				local_apic_set_register(local_apic_ptr, LOCAL_APIC_REG_OFFSET_LVT_LINT1, nmi_entry);
				break;
		}
	}
	// Setup Local APIC timer
	local_apic_set_register(local_apic_ptr, LOCAL_APIC_REG_OFFSET_INITIAL_COUNT, 0x100000);
	uint32_t divide_reg = local_apic_get_register(local_apic_ptr, LOCAL_APIC_REG_OFFSET_DIVIDE_CONFIG);
	local_apic_set_register(local_apic_ptr, LOCAL_APIC_REG_OFFSET_DIVIDE_CONFIG, (divide_reg & 0xfffffff4) | 0b1010);
	uint32_t timer_reg = local_apic_get_register(local_apic_ptr, LOCAL_APIC_REG_OFFSET_LVT_TIMER);
	local_apic_set_register(local_apic_ptr, LOCAL_APIC_REG_OFFSET_LVT_TIMER, local_apic_create_register_value((struct LocalAPICInterruptRegister){
		.vector = LOCAL_APIC_INTERRUPT_VECTOR_TIMER,
		.mask = LOCAL_APIC_INTERRUPT_REGISTER_MASK_ENABLE,
		.timer_mode = LOCAL_APIC_INTERRUPT_REGISTER_TIMER_MODE_PERIODIC
	}) | (timer_reg & 0xfffcef00));
}*/