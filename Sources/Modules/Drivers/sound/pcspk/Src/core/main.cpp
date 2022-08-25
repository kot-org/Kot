#include <core/main.h>

void playSound(uint32_t freq) {
    uint32_t div;
    uint8_t tmp;

    div = PIT_FREQUENCY / freq;

    IoWrite8(PIT_COMMAND, 0xB6);
    IoWrite8(PIT_DATA_2, (uint8_t) div);
    IoWrite8(PIT_DATA_2, (uint8_t) div >> 8);

    tmp = IoRead8(PIT_PCS_OUTPUT);
    if(tmp != (tmp | 3))
        IoWrite8(PIT_PCS_OUTPUT, tmp | 3);

    Printlog("[SOUND/PCSPK] Beep!");
}

void stopSound() {
 	IoWrite8(PIT_PCS_OUTPUT, IoRead8(PIT_PCS_OUTPUT) & 0xFC);
}

void playMusic() {
    // todo
}

extern "C" int main(int argc, char* argv[]) {
    Printlog("[SOUND/PCSPK] Initialization ...");

    // playSound(1000);

    Printlog("[SOUND/PCSPK] Driver initialized successfully");

    return KSUCCESS;
}