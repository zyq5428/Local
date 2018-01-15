#define GPMCON        0x7f008020
#define GPMDAT        0x7f008024

unsigned int *led_config;
unsigned int *led_data;

#define LED_MAGIC	'l'
#define LED_ON		_IO(LED_MAGIC, 0)
#define LED_OFF		_IO(LED_MAGIC, 1)