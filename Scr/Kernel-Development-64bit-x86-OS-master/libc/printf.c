#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

char asceding_hex_array[16] = "0123456789ABCDEF";
char final_value_to_print[2048];

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

    int len = strlen(string_to_append) ;
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
//                  final_value_to_print[curr_pos] = *input_string;
//                  curr_pos += 1;
                    break;
            }

        }

       else

         {
                final_value_to_print[curr_pos] = *input_string;
                curr_pos += 1;

          } 
    }

    final_value_to_print[curr_pos] = '\0'; // current position always points to where the char ends. hence termination with\0
    write(1, final_value_to_print, strlen(final_value_to_print));
    return 0;
}


int printf(const char *fmt, ...)
{

va_list variables;
va_start(variables, fmt);
vprintfs(variables, fmt);
 
return 0;
}
