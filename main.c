int time,count;

void input_time(void)
{
	printf("\n Plese input timing time(s):");
	scanf("%d", &time);
	printf("\n\r you input is %d s!\n\r", time);
}

int uboot_main()
{
	int num;
	int x;

#ifdef MMU_ON
	mmu_init();
#endif

	uart_init();
	
	led_init();

	button_init_ext_int();

	irq_init();
	
	timer_2_init();

	ts_init();

	lcd_init();

	dma_init();	
		
	led4_off();
	
	dma_start();
	
	lcd_test();
	
	led_off();
	
	input_time();
	
	count = 0;
		
	while(0) {
		if(!(count == time)) {
			printf("\n\r count is : %d \n\r", count);
		
			printf("\n\r time is : %d \n\r", time);
			
			count = time;
			timer_2_s(count);
		} else {
			//printf("\n\r The value you entered is the same as last time!!!\n\r");
			//input_time();
			;
		}
		}

	while(1) {		
		printf("\n Plese input delay time(s):");
		scanf("%d", &count);
		sdelay(count);
		led4_xor();		
		}

	while(0) {
		printf("\n*****************************\n\r");
		printf("\n************U-Boot***********\n\r");
		printf("[1]:Download Linux Kerel from TFTP server!\n\r");
		printf("[2]:Boot Linux from RAM!\n\r");
		printf("[3]:Boot Linux from Nand Flash!\n\r");
		printf("\n Plese Select:");

		scanf("%d", &num);

		switch (num) {
			case 1:
				//tftp_load();
				break;

			case 2:
				//boot_linux_arm();
				break;

			case 3:
				//boot_linux_nand();
				break;

			default:
				printf("Error: Wrong selection!\n\r");
				break;
		}
		 
	}

	return 0;
}

