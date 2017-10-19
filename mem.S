.text
.global men_ctrl_asm_init


#define HCLK 133 /* 133MHz */
#define DDR_tREFRESH	7800
#define DDR_tRAS	45
#define DDR_tRC		68
#define DDR_tRCD	23
#define DDR_tRFC	80
#define DDR_tRP		23
#define DDR_tRRD	15
#define DDR_tWR		15
#define DDR_tXSR	120
#define DDR_CASL	3

#define DMC_DDR_REFRESH_PRD	((DDR_tREFRESH * HCLK / 1000) + 1)
#define DMC_DDR_CAS_LATENCY	(DDR_CASL << 1)
#define DMC_DDR_t_DQSS	1
#define DMC_DDR_t_MRD	2
#define DMC_DDR_t_RAS	((DDR_tRAS * HCLK / 1000) + 1)
#define DMC_DDR_t_RC	((DDR_tRC * HCLK / 1000) + 1)
#define DMC_DDR_t_RCD	((DDR_tRCD * HCLK / 1000) + 1)
#define DMC_DDR_schedule_RCD	((DMC_DDR_t_RCD-3)  << 3)
#define DMC_DDR_t_RFC   	((DDR_tRFC * HCLK / 1000) + 1)
#define DMC_DDR_schedule_RFC    ((DMC_DDR_t_RFC-3)  << 5)
#define DMC_DDR_t_RP   		((DDR_tRP * HCLK / 1000) + 1)
#define DMC_DDR_schedule_RP     ((DMC_DDR_t_RP-3)  << 3)
#define DMC_DDR_t_RRD           ((DDR_tRRD * HCLK / 1000) + 1)
#define DMC_DDR_t_WR            ((DDR_tWR * HCLK / 1000) + 1)
#define DMC_DDR_t_WTR	4
#define DMC_DDR_t_XP	2
#define DMC_DDR_t_XSR   ((DDR_tXSR * HCLK / 1000) + 1)
#define DMC_DDR_t_ESR	DMC_DDR_t_XSR


#define DMC_MEMC_CFG	0x1001a /* Supports one CKE control, Chip 1, Burst 4, Row 14 /Column 10 bit */
#define DMC_MEMC_CFG2   0xb45
#define DMC_CHIP0_CFG	0x150f0	
#define DMC_DDR_32_CFG	0x0

#define MEM_SYS_CFG 0x7e00f120
#define DMC1_BASE 0x7e001000


#define DMC_MEMC_CMD_OFFSET	0x04
#define DIRECT_CMD_OFFSET	0x08

#define DMC_REFRESH_PRD_OFFSET	0x10
#define DMC_CAS_LATENCY_OFFSET  0x14
#define DMC_T_DQSS_OFFSET	0x18
#define DMC_T_MRD_OFFSET	0x1c
#define DMC_T_RAS_OFFSET	0x20
#define DMC_T_RC_OFFSET		0x24
#define DMC_T_RCD_OFFSET	0x28
#define DMC_T_RFC_OFFSET        0x2c
#define DMC_T_RP_OFFSET         0x30
#define DMC_T_RRD_OFFSET        0x34
#define DMC_T_WR_OFFSET         0x38
#define DMC_T_WTR_OFFSET        0x3c
#define DMC_T_XP_OFFSET         0x40
#define DMC_T_XSR_OFFSET        0x44
#define DMC_T_ESR_OFFSET        0x48

#define DMC_MEMC_CFG_OFFSET	0x0c
#define DMC_MEMC_CFG2_OFFSET    0x4c
#define DMC_CHIP_0_CFG_OFFSET   0x200
#define DMC_USER_CONFIG_OFFSET  0x304

#define DMC_MEMC_STATUS_OFFSET  0x00

men_ctrl_asm_init:
	@Set data pin
	ldr r0, =MEM_SYS_CFG
	mov r1, #0xd
	str r1, [r0]

	@Make DMC into config state
	ldr r0, =DMC1_BASE
	ldr r1, =0x04
	str r1, [r0, #DMC_MEMC_CMD_OFFSET]

	@Config registers

	ldr r1, =DMC_DDR_REFRESH_PRD
	str r1, [r0, #DMC_REFRESH_PRD_OFFSET]

	ldr r1, =DMC_DDR_CAS_LATENCY
	str r1, [r0, #DMC_CAS_LATENCY_OFFSET]

	ldr r1, =DMC_DDR_t_DQSS
	str r1, [r0, #DMC_T_DQSS_OFFSET]

	ldr r1, =DMC_DDR_t_MRD
	str r1, [r0, #DMC_T_MRD_OFFSET]


	ldr r1, =DMC_DDR_t_RAS
	str r1, [r0, #DMC_T_RAS_OFFSET]

	ldr r1, =DMC_DDR_t_RC
	str r1, [r0, #DMC_T_RC_OFFSET]

	ldr r1, =DMC_DDR_t_RCD
	ldr r2, =DMC_DDR_schedule_RCD
	orr r1, r1, r2
	str r1, [r0, #DMC_T_RCD_OFFSET]

	ldr r1, =DMC_DDR_t_RFC
	ldr r2, =DMC_DDR_schedule_RFC
	orr r1, r1, r2
	str r1, [r0, #DMC_T_RFC_OFFSET]

	ldr r1, =DMC_DDR_t_RP
	ldr r2, =DMC_DDR_schedule_RP
	orr r1, r1, r2
	str r1, [r0, #DMC_T_RP_OFFSET]

	ldr r1, =DMC_DDR_t_RRD
	str r1, [r0, #DMC_T_RRD_OFFSET]

	ldr r1, =DMC_DDR_t_WR
	str r1, [r0, #DMC_T_WR_OFFSET]

	ldr r1, =DMC_DDR_t_WTR
	str r1, [r0, #DMC_T_WTR_OFFSET]

	ldr r1, =DMC_DDR_t_XP
	str r1, [r0, #DMC_T_XP_OFFSET]

	ldr r1, =DMC_DDR_t_XSR
	str r1, [r0, #DMC_T_XSR_OFFSET]

	ldr r1, =DMC_DDR_t_ESR
	str r1, [r0, #DMC_T_ESR_OFFSET]

	ldr r1, =DMC_MEMC_CFG
	str r1, [r0, #DMC_MEMC_CFG_OFFSET]
	
	ldr r1, =DMC_MEMC_CFG2
	str r1, [r0, #DMC_MEMC_CFG2_OFFSET]

	ldr r1, =DMC_CHIP0_CFG
	str r1, [r0, #DMC_CHIP_0_CFG_OFFSET]

	ldr r1, =DMC_DDR_32_CFG
	str r1, [r0, #DMC_USER_CONFIG_OFFSET]
	
	@DMC0 DDR Chip 0 configuration direct command reg

	@NOP
	ldr r1, =0xc0000
	str r1, [r0, #DIRECT_CMD_OFFSET]

	@Precharg all
	ldr r1, =0x0
	str r1, [r0, #DIRECT_CMD_OFFSET]

	@Autore fresh 2 time
	ldr r1, =0x40000
	str r1, [r0, #DIRECT_CMD_OFFSET]
	str r1, [r0, #DIRECT_CMD_OFFSET]

	@MRS
	ldr r1, =0xa0000
	str r1, [r0, #DIRECT_CMD_OFFSET]

	@Mode reg
	ldr r1, =0x080032
	str r1, [r0, #DIRECT_CMD_OFFSET]

	ldr r1, =0x0
	str r1, [r0, #DMC_MEMC_CMD_OFFSET]

check_dmc1_ready:
	ldr r1, [r0, #DMC_MEMC_STATUS_OFFSET]
	mov r2, #0x3
	and r1, r1, r2
	cmp r1, #0x1
	bne check_dmc1_ready
	nop

	mov pc, lr
