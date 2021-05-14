#include <sys/kprintf.h>
#include  <stdarg.h>
#include <sys/defs.h>

#define KERNBASE 0xffffffff80000000
char asceding_hex_array[16] = "0123456789ABCDEF";
char final_value_to_print[2048];
//static char* vga_start_address =(char*) 0xb8000;
//static char* vga_address_pointer = (char*)0xb8000;

//changes made by Banani
static char* vga_start_address = (char *)0xFFFFFFFF800B8000;
static char* vga_address_pointer = (char *)0xFFFFFFFF800B8000;

int number_of_rows = 25;
int number_of_columns =80;
uint64_t  get_vga_address()
{
return  (uint64_t)vga_address_pointer;
}

void set_vga_address(uint64_t new_pointer_address)
{
vga_address_pointer = (char*)new_pointer_address; 
}
 
int current_row_number()
{
int row_number = (vga_address_pointer-vga_start_address)/160;
return row_number;
}

int current_column_no()
{
	int col =(((vga_address_pointer - vga_start_address)/2)%80);
	return col;
}

void character_print_position(int row_number,int column_number)
{
uint64_t address_to_print = (uint64_t)vga_start_address+(row_number*0xA0 + column_number*2);
set_vga_address(address_to_print);
}
void nextline()
{
        int linenumber = current_row_number() + 1;
        character_print_position(linenumber,0);

}

void clrscr() {
        char *v_ptr = vga_start_address;
        int count =0;
        while (count < (80*24*2)){
        *(v_ptr+count) = ' ';
        count= count+2;
        }
        vga_address_pointer = v_ptr;

}

void tab()
{

	set_vga_address(get_vga_address()+8);

}


void backspace(){
	if((uint64_t)current_column_no() <= 8){
		return;
	}
		
	if (get_vga_address() >= (uint64_t)vga_start_address) 	
	{	
		set_vga_address(get_vga_address()-2);
                char ch = ' '; 
		*vga_address_pointer = ch;
		//vga_address_pointer = vga_address_pointer-2;
		set_vga_address(get_vga_address());	
	}

}


void escape_sequence(const char *escape_character)
{
        if (*escape_character == '\n' )
        {
		nextline();
               
        }  
        else if (*escape_character == '\t')
	{
	       tab();
	
	} 
	
}

int  strlenn(const char* string)
{

        int len = 0;

        while(*string++)
                len++;

        return len;

}


char* strrev(char* str){

    int i;
    int len = strlenn(str) -1;
    for(i=0;i<strlenn(str)/2;i++)
        {
         str[i]+=str[len];
         str[len]=str[i]-str[len];
         str[i]=str[i]-str[len--];
        }

    return str;
}



int console_output(char *final_value_to_print,int curr_pos)
{
int length =0;
while((*final_value_to_print)&&(length<curr_pos))
{
*vga_address_pointer = *final_value_to_print;

final_value_to_print = final_value_to_print+1;

vga_address_pointer= vga_address_pointer+2;

length++;
}
return 0;
}

int convert_hex_to_string(char *final_value_to_print, unsigned long hex_to_convert, int curr_pos)
{

    if(hex_to_convert < 0){
        final_value_to_print[curr_pos] = '-';
        curr_pos += 1;
        hex_to_convert *= -1;
    }
   
    int start_of_number = curr_pos;

   if(hex_to_convert == 0){
        final_value_to_print[curr_pos] = '0';
        return ++curr_pos;
    }

      
    while (hex_to_convert > 0)
    {
        final_value_to_print[curr_pos] = asceding_hex_array[hex_to_convert%16];
        hex_to_convert /= 16;
        curr_pos=curr_pos+1;
    }

//int start_of_number = curr_pos;

     int count = 0;
     while(count < (curr_pos - start_of_number)/2){
        char temp = final_value_to_print[start_of_number + count];
        final_value_to_print[start_of_number + count] = final_value_to_print[curr_pos - 1 - count];
        final_value_to_print[curr_pos - 1 - count] = temp;
        count += 1;
}
    return (curr_pos);


}

int convert_pointer_to_string(char *final_value_to_print, unsigned long hex_to_convert, int curr_pos)
{

 if(hex_to_convert < 0){
        final_value_to_print[curr_pos] = '-';
        curr_pos += 1;
        hex_to_convert *= -1;
    }

int start_of_number = curr_pos;
  
if(hex_to_convert == 0){
        final_value_to_print[curr_pos] = '0';
        return ++curr_pos;
    }

    while (hex_to_convert > 0)
    {
        final_value_to_print[curr_pos] = asceding_hex_array[hex_to_convert%16];
        hex_to_convert /= 16;
        curr_pos=curr_pos+1;
    }

//int start_of_number = curr_pos;

     int count = 0;
     while(count < (curr_pos - start_of_number)/2){
        char temp = final_value_to_print[start_of_number + count];
        final_value_to_print[start_of_number + count] = final_value_to_print[curr_pos - 1 - count];
        final_value_to_print[curr_pos - 1 - count] = temp;
        count += 1;
}
    return (curr_pos);


}


int convert_int_to_string(char *final_value_to_print, int int_to_convert, int curr_pos){

    if(int_to_convert < 0){
        final_value_to_print[curr_pos] = '-';
        curr_pos += 1;
        int_to_convert *= -1;
    }

    int start_of_number = curr_pos;

    if(int_to_convert == 0){
        final_value_to_print[curr_pos] = '0';
        curr_pos += 1;
        return curr_pos;
    }


    while(int_to_convert > 0){

        unsigned int digit = int_to_convert % 10;
        final_value_to_print[curr_pos] = '0' + digit;
        curr_pos += 1;
        int_to_convert /= 10;

    }

    int count = 0;
    while(count < (curr_pos - start_of_number)/2){
        char temp = final_value_to_print[start_of_number + count];
        final_value_to_print[start_of_number + count] = final_value_to_print[curr_pos - 1 - count];
        final_value_to_print[curr_pos - 1 - count] = temp;
        count += 1;
    }

    return curr_pos;

}


int convert_string_to_string(char *final_value_to_print, char *string_to_append, int curr_pos){

    int len = strlenn(string_to_append) ;
    int i = 0;

    while(i < len){
        final_value_to_print[curr_pos] = *string_to_append;
        string_to_append += 1;
        curr_pos += 1;
        i += 1;
    }

    return curr_pos;

}

int vprintfs(va_list variables, const char *input_string)
{

    int curr_pos = 0;
   
    for(;*input_string!=0; input_string += 1){

        if(*input_string == '%'){
            input_string += 1;

            char c = *input_string;

            switch(c){

                case 's':
                   
                    curr_pos = convert_string_to_string(final_value_to_print, va_arg(variables, char *), curr_pos);
                    break;

                case 'd':
                    
                    curr_pos = convert_int_to_string(final_value_to_print, va_arg(variables, int), curr_pos);
                    break;

                case 'c':
                    
                    final_value_to_print[curr_pos] = va_arg(variables, int);
                    curr_pos += 1;
                    break;

                case 'x':
               curr_pos = convert_hex_to_string(final_value_to_print, va_arg(variables, unsigned long), curr_pos);
                             break;


                case 'p':
            curr_pos = convert_pointer_to_string(final_value_to_print, va_arg(variables, unsigned long), curr_pos);
                 break;

                default:
//                final_value_to_print[curr_pos] = *input_string;
//                 curr_pos += 1;
                    break;

            }

        }
        else if ( *input_string == '\n')
        {
        nextline();   
            
        }
       else if(*input_string =='\t')
       {
        tab();  
       }

       else

         {
         final_value_to_print[curr_pos] = *input_string;
                 curr_pos += 1;

          } 
    }

final_value_to_print[curr_pos] = '\0'; // current position always points to where the char ends. hence termination with\0

return console_output(final_value_to_print,strlenn(final_value_to_print));
}


void kprintf(const char *fmt, ...)
{

va_list variables;
va_start(variables, fmt);
vprintfs(variables, fmt);

}
