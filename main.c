int uboot_main()
{
	int num;
	int x;
	int count;

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
	
	led_on();
		
	while(1) {
		printf("\n Plese input timing time(s):");
		scanf("%d", &count);
		timer_2_timing(count);
		
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

