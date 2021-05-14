#include "power.h"

void* PowerDownVoid;
void* PowerRestartVoid;

void Power::InitPower(void* pdwn,void* rest) {
    PowerDownVoid = pdwn;
    PowerRestartVoid = rest;
}

void Power::Shutdown() {
	((void(*)())PowerDownVoid)();
}

void Power::Restart() {
	((void(*)())PowerRestartVoid)();
}