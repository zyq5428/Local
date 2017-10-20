#include "vsprintf.h"

unsigned char outbuff[1024];
unsigned char inbuff[1024];

int printf(const char* format, ...)
{
	va_list args;
	int i;

	//1.Converts a variable parameter to a string
	va_start(args,format);
	
	vsprintf((char *)outbuff,format,args);
	
	va_end();

	//2.print string
	for (i=0; i < strlen((char *)outbuff); i++)
	{
		uart_put_char(outbuff[i]);
	}

	return i;
}

int scanf(const char* format, ...)
{
	unsigned char c;
	int i = 0;
	va_list args;

	//1.get input char
	while (1) {
		c = uart_get_char();
		if ((c == 0x0d) || (c == 0x0a)) {
			inbuff[i] = '\n';
			break;
		}
		else {
			inbuff[i++] = c;
		}
	}
	//2.format convert
	va_start(args,format);
	vsscanf((char *)inbuff,format,args);
	va_end(args);

	return i;
}

