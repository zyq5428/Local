#define GPNCON	((volatile unsigned long *)0x7f008830)
#define GPNPUD  ((volatile unsigned long *)0x7f008838)

void button_init_ext_int(void)
{
	*(GPNCON) = ((0b10) | (0b10 << 2));  //set key1 and key6
}
