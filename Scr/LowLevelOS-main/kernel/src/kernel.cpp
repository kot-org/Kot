#include "kernelInit.h"

#define MOUSE_BORDER_COLOUR WHITE
#define MOUSE_CONTENTS_COLOUR DARKGRAY

/*
Special Thanks to:
- @borrrden - he fixed my buggy keyboard handler
- @AbsurdPoncho - if he didn't do a osdev series i won't started this project
- @nothotscott - he has a very good tutorial on debugging that helped me (https://www.youtube.com/watch?v=llP7zB8HTls)
*/

/*
Bugs:
- Mouse is buggy and crashes the entire os on real hardware with usb keyboard and mice (beacause of crappy ps/2 emulation, so, i need an usb driver....)
- Mouse is moving kinda slow (maybe i can fix this optimizing?)
- When i move the mouse and i type there is a chance that they interfer each other
*/

uint8_t MousePointer[16*25] = {
1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,
1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,
1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,
1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,
1,0,0,0,1,1,1,0,0,0,1,0,0,0,0,0,
1,0,0,1,1,0,1,0,0,0,1,0,0,0,0,0,
1,1,1,1,0,0,1,1,0,0,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,
0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,
0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,
};

uint8_t MousePointerFilled[16*25] = {
1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,
1,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,
1,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,
1,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,
1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,
1,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,
1,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,
1,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,
1,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,
1,2,2,2,2,2,2,2,2,2,2,2,1,0,0,0,
1,2,2,2,2,2,2,2,2,2,2,2,2,1,0,0,
1,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0,
1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,
1,2,2,2,2,1,1,2,2,1,1,0,0,0,0,0,
1,2,2,2,1,1,1,2,2,2,1,0,0,0,0,0,
1,2,2,1,1,0,1,2,2,2,1,0,0,0,0,0,
1,1,1,1,0,0,1,1,2,2,1,1,0,0,0,0,
0,0,0,0,0,0,1,1,2,2,2,1,0,0,0,0,
0,0,0,0,0,0,0,1,2,2,2,1,0,0,0,0,
0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,
};

void displayLogo() {
	printf("%co%s%co",LIGHTRED,LLOSLogo,WHITE);
}

void displayCPU() {
	printf("\n\nCPU: %co%s%co\n",ORANGE,cpu.getName(),WHITE);
	printf("CPU Vendor: %co%s%co\n",ORANGE,cpu.getVendor(),WHITE);
	printf("CPU Features: %co",ORANGE);
	for(int i = 0;i<cpu.cpuFeatures;i++) printf("%s ",CPUFeatures[i]);
	printf("%co",WHITE);
}

void displayRAM(BootInfo* bootInfo) {
	printf("\n\nPhysical RAM: %co%d MB%co",YELLOW,(GlobalAllocator.GetFreeRAM()+GlobalAllocator.GetUsedRAM())/1024/1024,WHITE);
	printf("\nTotal RAM: %co%d MB%co",YELLOW,(GlobalAllocator.GetFreeRAM()+GlobalAllocator.GetUsedRAM()+GlobalAllocator.GetReservedRAM())/1024/1024,WHITE);
	printf("\nFree RAM: %co%d MB%co",YELLOW,GlobalAllocator.GetFreeRAM()/1024/1024,WHITE);
	printf("\nUsed RAM: %co%d MB%co",YELLOW,GlobalAllocator.GetUsedRAM()/1024/1024,WHITE);
	printf("\nReserved RAM: %co%d MB%co",YELLOW,GlobalAllocator.GetReservedRAM()/1024/1024,WHITE);
}

void displayScreen() {
	printf("\n\nScreen Width: %co%dpx%co",LIGHTGREEN,display.getWidth(),WHITE);
	printf("\nScreen Height: %co%dpx%co\n",LIGHTGREEN,display.getHeight(),WHITE);
}

void displayPCI() {
	printf("\n\nDetected %co%d%co PCI devices: \n",LIGHTTURQOISE,pci.DevicesIndex,WHITE);

	for(int i = 0;i<pci.DevicesIndex;i++) {
		TranslatedPCIDevice device = pci.Devices[i];
		printf("\nPCI Device %d:\n",i);
		printf(" Vendor: %co%s%co",LIGHTTURQOISE,device.VendorID,WHITE);
		printf(" Device: %co%s%co",LIGHTTURQOISE,device.DeviceID,WHITE);
		printf(" Class: %co%s%co",LIGHTTURQOISE,device.Class,WHITE);
		printf(" SubClass: %co%s%co",LIGHTTURQOISE,device.Subclass,WHITE);
	}
}

void displayDateTime() {
	printf("\n\nTime: %co%d:%d:%d%co",LIGHTMAGENTA,rtc.readHours(),rtc.readMinutes(),rtc.readSeconds(),WHITE);
	printf("\nDate: %co%d/%d/20%d%co",MAGENTA,rtc.readDay(),rtc.readMonth(),rtc.readYear(),WHITE);
}

void displayFirmware(BootInfo* binfo) {
	printf("\n\nUEFI Firmware Vendor: %co%s%co",LIGHTRED,shorttostr(binfo->Efi->Vendor),WHITE);
	printf("\nUEFI Firmware Version: %co%d%co",LIGHTRED,binfo->Efi->Version,WHITE);
}

void displayRandomNumber() {
	printf("\n\nRandom number: %co%d%co\n",ORANGE,rand(),WHITE);
}

void displayKeyboard() {
	printf("\n\nKey pressed: ");
	if(kbhit())
		printf("%c",getch());
	printf("\nKeyboard buffer: %s_",kb.buffer);
}

void drawPointer() {
	int x = 0;
	int y = 0;
	for(int i = 0;i< 16*25;i++) {
		if(MousePointerFilled[i] == 1)
			display.putpix((x+mouse.state.X)*4,y+mouse.state.Y,MOUSE_BORDER_COLOUR);
		else if (MousePointerFilled[i] == 2)
			display.putpix((x+mouse.state.X)*4,y+mouse.state.Y,MOUSE_CONTENTS_COLOUR);
		x++;
		if(x > 15) {
			x = 0;
			y++;
		}
	}
}

void doMouse() {
	drawPointer();

	if(mouse.state.ButtonLeft || mouse.state.ButtonRight || mouse.state.ButtonMiddle) {		
		power.Shutdown();
	}
}

void displayRSDP() {
	printf("\n\nRSDP Address: %x",(uint64_t)GlobalInfo->RSDP);
	printf("\nRSDP Signature: %c%c%c%c%c%c%c%c",*(uint8_t*)GlobalInfo->RSDP,*((uint8_t*)GlobalInfo->RSDP+1),*((uint8_t*)GlobalInfo->RSDP+2),*((uint8_t*)GlobalInfo->RSDP+3),*((uint8_t*)GlobalInfo->RSDP+4),*((uint8_t*)GlobalInfo->RSDP+5),*((uint8_t*)GlobalInfo->RSDP+6),*((uint8_t*)GlobalInfo->RSDP+7));
}

extern "C" int kernelMain(BootInfo* binfo) {
	InitDrivers(binfo);
	srand(rtc.readTime());
	display.clearScreen(BLACK);

	LOOP {
		display.clearScreen(BLACK);

		displayLogo();
		displayDateTime();

		displayPCI();

		displayKeyboard();

		printf("\n\nClick to shutdown!");

		doMouse();

		display.update();
	}

	LOOP;

	return 0;
} 