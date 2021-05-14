#include <sys/terminal.h>
#include <sys/kprintf.h>
#include <sys/string.h>
static volatile int stdin_flag = 0; 

enum keyboard_scancodes {
	ESC = 0x01,
	SCHR_1,
	SCHR_2,
	SCHR_3,
	SCHR_4,
	SCHR_5,
	SCHR_6,
	SCHR_7,
	SCHR_8,
	SCHR_9,
	SCHR_0,
	UNDERSCORE_HYPHEN,
	EQUALS_PLUS,
	BCKSPC,
	TAB,
	CHR_Q,
	CHR_W,
	CHR_E,
	CHR_R,
	CHR_T,
	CHR_Y,
	CHR_U,
	CHR_I,
	CHR_O,
	CHR_P,
	CURL_BRACK_OPEN,
	CURL_BRACK_CLOSE,
	ENTER,
	LEFT_CTRL,
	CHR_A,
	CHR_S,
	CHR_D,
	CHR_F,
	CHR_G,
	CHR_H,
	CHR_J,
	CHR_K,
	CHR_L,
	COLON_SEMICLN,
	SNGL_DBL_QUOTES,
	TILDE,
	LEFT_SHIFT,
	OR_BCKSLASH,
	CHR_Z,
	CHR_X,
	CHR_C,
	CHR_V,
	CHR_B,
	CHR_N,
	CHR_M,
	COMMA_OPENDLM,
	DOT_CLOSEDLM,
	FRWDSLASH_QUESTION,
	RIGHT_SHIFT,
	PRINT_SCREEN,
	ALT,
	SPACE,
	CAPSLOCK,
	FK_F1,
	FK_F2,
	FK_F3,
	FK_F4,
	FK_F5,
	FK_F6,
	FK_F7,
	FK_F8,
	FK_F9,
	FK_F10,
	NUM_LOCK,
	SCROLL_LOCK,
	KEYPAD_HOME_7,
	KEYPAD_UP_8,
	KEYPAD_PGUP_9,
	KEYPAD_MINUS,
	KEYPAD_LFT_4,
	KEYPAD_5,
	KEYPAD_RGHT_6,
	KEYPAD_PLUS,
	KEYPAD_END_1,
	KEYPAD_DOWN_2,
	KEYPAD_PGDWN_3,
	KEYPAD_INS_0,
	KEYPAD_DOT_DEL
};

unsigned char noShiftChar[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    '^',			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0, // 67
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */ //88
    '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+'
 
};

unsigned char shiftChar[128] =
{
    0,  27,
    '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+',
    '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
   '^',			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
 '\"', '~',   0,		/* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', '<', '>', '?',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0, // 67
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */ //88
 
};

int shift=0;
int control=0;
static volatile char t_buf[1024];
static volatile int scan_len = 0;

void out(uint16_t p, uint16_t value)
{
        __asm__ volatile ("outb %%al, %%dx"::"d" (p), "a" (value));

}


char in(uint16_t p) 
{
        char input;
        __asm__ volatile ("inb %%dx,%%al":"=a"(input):"d"(p));
        return input;
}


int readInput(char *buf,int count) {
	__asm__ __volatile__("sti;");
	 stdin_flag = 1;
	 scan_len =0;
	 kprintf("");
	while(stdin_flag); 

//	{
	//wait
//	kprintf("Waiting, stdin-flag %d", stdin_flag);
//	while(1);
//	if(stdin_flag ==0) break;
//	}
//	while(1);
 	

	if (count <  scan_len)
	scan_len = count;
	t_buf[scan_len]= '\0';
	int len = scan_len;	
    	memcpy((void *)buf,(void *)t_buf,scan_len);

//	memset(t_buf, 0, sizeof(t_buf));
	for(int i=0 ; i<scan_len; i++){
	t_buf[i]= '\0';	
	}

	scan_len = 0;
//	  while(1);
	return len;
}



void interpret_scancode(uint16_t scancode){
	char t_char = '\0';
	if(scancode == SPACE)
		t_char = noShiftChar[scancode];
	if(scancode == TAB) {
                t_char = noShiftChar[scancode];
                tab();
        }
        
        if(scancode == BCKSPC){
        t_char = noShiftChar[scancode];
        /*if (stdin_flag == 1)
		{
		      if(scan_len == 0){
                      return;
		}
			else if(scan_len >0 )
		{
			t_buf[--scan_len] = ' ';
		}	
        	}
	
	 else	{
                 backspace();
	       	return;

		}*/
		if (stdin_flag == 1 && scan_len>=1)
		{
		t_buf[--scan_len] = ' ';
		}	
	        backspace();
		return;
	}
	if(scancode == ENTER)
	{
//	kprintf(" Entered ");
        t_char = ' ';//noShiftChar[scancode];
        if (stdin_flag == 1) 
	{
		stdin_flag =0;
//		kprintf(" Buffer : %s ", t_buf);
	}
	else  kprintf("");
	nextline();
        }	
	
	if(shift){
        t_char = shiftChar[scancode];
        }
        else{
        t_char = noShiftChar[scancode];
        }

	if(t_char) {
		
		kprintf("%c", t_char);
		if (stdin_flag == 1)
		{
			t_buf[scan_len++] = t_char;
		//	kprintf("buff %c...", t_buf[scan_len]);
		}
	}

}


void keyboard_handler(){
        uint16_t scancode = 0;
        uint16_t status = in(0x64);

        if(status & 0x01) {
                if((scancode = in(0x60)) < 0) {
                         out(0x20, 0x20);
			 return;
			
                } else {
                        if(scancode & 0x80) {
                                shift = 0;
				scancode = (shift & (1<<6) ? scancode : scancode & 0x7F);				
                                out(0x20, 0x20); 
			        return;
				
                        }
                        else {
                                if(scancode == RIGHT_SHIFT || scancode == LEFT_SHIFT) {
                                        shift = 1;
                                }
                        }
                }
                interpret_scancode(scancode);
        }
        out(0x20, 0x20);
}
