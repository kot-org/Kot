#include "sounddriver.h"

void Sound::play(long long nFrequence) {
    long long Div;
 	int tmp;
 
 	Div = 1193180 / nFrequence;
 	outportb(0x43, 0xb6);
 	outportb(0x42, (int) (Div) );
 	outportb(0x42, (int) (Div >> 8));
 
 	tmp = inportb(0x61);
  	if (tmp != (tmp | 3)) {
 		outportb(0x61, tmp | 3);
 	}
}

void Sound::stop() {
    int tmp = inportb(0x61) & 0xFC;
 
 	outportb(0x61, tmp);
}