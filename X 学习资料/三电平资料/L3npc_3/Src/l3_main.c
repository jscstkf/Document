/******************************************************************************
// FILE:    l3_main.c
// TITLE:   NPC 3-Level converter controller main program
// DESCRIPTION:
//          Main Program.
//
// 2009-10-15	V1.0
******************************************************************************/

#include "l3_global.h"

/*ÉùÃ÷º¯Êý
*****************************************************************************/
void InitSysCtrl();				// ³õÊ¼»¯ÏµÍ³¿ØÖÆ¼Ä´æÆ÷
void InitPieCtrl();				// ³õÊ¼»¯ÍâÉèÖÐ¶Ï¿ØÖÆ¡¢±êÖ¾¼Ä´æÆ÷
void InitPieVectTable();		// ³õÊ¼»¯ÖÐ¶ÏÏòÁ¿±í
void InitGpio();				// ³õÊ¼»¯IO
void InitPeripherals();			// ³õÊ¼»¯ÍâÉè
void InitEv();					// ³õÊ¼»¯EV
void InitL3C();					// ³õÊ¼»¯L3C

void CheckNormal();				// ÅÐ¶ÏÔËÐÐ×´Ì¬,Õý³£Ôò¿ªÖÐ¶Ï
void CheckFault();				// ÅÐ¶Ï¹ÊÕÏÊäÈëÐÅºÅ
void IDEL();					// ½øÈë´ý»ú×´Ì¬

void test_io_ad(void);			// IO,PWM,ADµÈ»ù±¾¹¦ÄÜµ÷ÊÔ
void delayx(Uint16);			// Ö´ÐÐn¸öµ¥Î»µÄÐ¡ÑÓÊ±

/*È«¾ÖÒ»°ã±äÁ¿
*****************************************************************************/
int32  GlobalQ = GLOBAL_Q;		// ¶¨Òå´Ë±äÁ¿¿ÉÅäºÏIQmath.gelÖ±½ÓWatch IQÐÍ±äÁ¿

/*È«¾Ö½á¹¹Ìå±äÁ¿
*****************************************************************************/
#pragma DATA_SECTION(l3c, "L3CRegsFile");
L3C		l3c		= L3C_DEFAULTS;			// L3CÐÍ½á¹¹³õÊ¼»¯Öµ
NPC		npc		= NPC_DEFAULTS;			// NPCÐÍ½á¹¹³õÊ¼»¯Öµ
ADC		ad		= ADC_DEFAULTS;
PIDREG3	pi_id	= PIDREG3_DEFAULTS;
PIDREG3	pi_iq	= PIDREG3_DEFAULTS;
PIDREG3	pi_vdc	= PIDREG3_DEFAULTS;
IPARK	ipark	= IPARK_DEFAULTS;
SVGENDQ	sv		= SVGENDQ_DEFAULTS;
LPF		lpfUdc	= LPF_DEFAULTS;
LPF		lpfUnp	= LPF_DEFAULTS;			// AD²ÉÑù´¦ÀíÓÃ
LPF		lpfUnpe	= LPF_DEFAULTS;			// FBDCLÓÃ
TLSVGENDQ tlsv  = TLSVGENDQ_DEFAULTS; 

#if	CLOSE_CURRENT						// 0:µçÁ÷¿ª»·(¸ø¶¨idref, iqref)	1:µçÁ÷±Õ»·Ä£Ê½
CLARKE	clarke	= CLARKE_DEFAULTS;
PARK	park	= PARK_DEFAULTS;
//ICLARKE	iclarke	= ICLARKE_DEFAULTS;
#endif

/*Ö÷º¯Êý
*****************************************************************************/
/*funName:		main()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	Ö÷º¯Êý
******************************************************************************/
void main(void)
{
	InitSysCtrl();				// ³õÊ¼»¯ÏµÍ³¿ØÖÆ¼Ä´æÆ÷
	InitGpio();					// ³õÊ¼»¯IO
	DINT;						// ¹Ø±ÕCPUÖÐ¶Ï
	InitPieCtrl();				// ³õÊ¼»¯ÍâÉèÖÐ¶Ï¿ØÖÆ¡¢±êÖ¾¼Ä´æÆ÷
	IER = 0x0000;				// ½ûÓÃCPUÖÐ¶Ï, ÇåCPUÖÐ¶Ï±êÖ¾
	IFR = 0x0000;
	InitPieVectTable();			// ³õÊ¼»¯ÖÐ¶ÏÏòÁ¿±í
	InitPeripherals();			// ³õÊ¼»¯ÍâÉè
	InitL3C();					// ³õÊ¼»¯L3C

#if	FLASH_OR_RAM==1				// ³õÊ¼»¯Flash
    memcpy(	&ramfuncs_runstart,
			&ramfuncs_loadstart,
			&ramfuncs_loadend - &ramfuncs_loadstart);
	InitFlash();
#endif

// ¹ØÁªÖÐ¶Ï³ÌÐòÓëÖÐ¶ÏÏòÁ¿±í
	EALLOW;
	PieVectTable.T1UFINT = &Eva_T1UF_isr;
	PieVectTable.PDPINTA = &Eva_PDPINT_isr;
	EDIS;

// ³õÊ¼»¯ÖÐ¶Ï
	EvaRegs.EVAIMRA.bit.PDPINTA		= 1;
    EvaRegs.EVAIFRA.bit.PDPINTA		= 1;
	PieCtrlRegs.PIEIER1.bit.INTx1	= 1;	// Ê¹ÄÜPDPINTAÖÐ¶Ï

 	EvaRegs.EVAIMRA.bit.T1UFINT		= 1;
    EvaRegs.EVAIFRA.bit.T1UFINT		= 1;
	PieCtrlRegs.PIEIER2.bit.INTx6	= 1;	// Ê¹ÄÜT1UFÖÐ¶Ï

//	EvaRegs.EVAIMRA.bit.CAP1INT		= 1;
//	EvaRegs.EVAIFRA.bit.CAP1INT		= 1;
//	PieCtrlRegs.PIEIER2.bit.INTx1	= 1;	// Ê¹ÄÜCAP1ÖÐ¶Ï

	IER |= M_INT1;							// Ê¹ÄÜCPU INT1(PDPINTA)
	IER |= M_INT2;							// Ê¹ÄÜCPU INT2(T1UFINT)
	IFR = 0;								// Çå³ýËùÓÐCPUÖÐ¶Ï±êÖ¾
		
// User specific code:
#if CHEAT_START
	l3c.flag.bit.start = KEY_ON;				// ×Ô¶¯¸øÒ»´ÎSTART°´¼ü
#endif 

#if	NORMAL_OR_TEST
	for( ; ;){
	#if CHEAT_STATE								// CHEAT_STATE==1:×ÜÈÏÎªapf.uState==S_OK
		l3c.flag.bit.err = KEY_OFF;
	#else
		if(l3c.flag.bit.start)					//´ò¿ª¿ª¹Ø¹Üºó¿ªÊ¼¼ì²â´íÎó
			CheckFault();
	#endif
		CheckNormal();							// ÎÞ¹ÊÕÏÔòÅÐ¶ÏÔËÐÐ×´Ì¬
	}
#else
 #if	TEST_PWM
    EALLOW;
	EvaRegs.T1CON.bit.TENABLE	= 1;	// Enable T1
	EvaRegs.COMCONA.bit.FCOMPOE	= 1;	// Full compare output: enable
	EDIS;
	for(;;){
//		EvaRegs.COMCONA.bit.FCOMPOE = 1;
	}
 #else
    EALLOW;
	GpioMuxRegs.GPAMUX.all &= 0xFFC0;	// ÅäÖÃpwm1~6Òý½ÅÎªIO¿Ú
	GpioMuxRegs.GPBMUX.all &= 0xFFC0;	// ÅäÖÃpwm7~12Òý½ÅÎªIO¿Ú
 	GpioMuxRegs.GPADIR.all |= 0x003F;	// Êä³ö
	GpioMuxRegs.GPBDIR.all  = 0xF03F;	// B0~5Êä³ö,B12~15Êä³ö
	EDIS;
	for(;;){
    	test_io_ad();
	}
 #endif

#endif

}


/*Ò»°ãº¯Êý
*****************************************************************************/
/*funName:		InitGpio()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	³õÊ¼»¯IO
******************************************************************************/
void InitGpio(void)
{
	// IOÅäÖÃÎ»ÖØÆôÄ¬ÈÏÎª0
	EALLOW;
// GPA
	GpioMuxRegs.GPAMUX.bit.PWM1_GPIOA0		= 1;
	GpioMuxRegs.GPAMUX.bit.PWM2_GPIOA1		= 1;
	GpioMuxRegs.GPAMUX.bit.PWM3_GPIOA2		= 1;
	GpioMuxRegs.GPAMUX.bit.PWM4_GPIOA3		= 1;
	GpioMuxRegs.GPAMUX.bit.PWM5_GPIOA4		= 1;
	GpioMuxRegs.GPAMUX.bit.PWM6_GPIOA5		= 1;	
	GpioMuxRegs.GPAMUX.bit.CAP1Q1_GPIOA8	= 1;
	GpioMuxRegs.GPAMUX.bit.CAP2Q2_GPIOA9	= 1;

// GPB
	GpioMuxRegs.GPBMUX.bit.PWM7_GPIOB0		= 1;
	GpioMuxRegs.GPBMUX.bit.PWM8_GPIOB1		= 1;
	GpioMuxRegs.GPBMUX.bit.PWM9_GPIOB2		= 1;
	GpioMuxRegs.GPBMUX.bit.PWM10_GPIOB3		= 1;
	GpioMuxRegs.GPBMUX.bit.PWM11_GPIOB4		= 1;
	GpioMuxRegs.GPBMUX.bit.PWM12_GPIOB5		= 1;
	
	GpioMuxRegs.GPBDIR.bit.GPIOB8			= 0;	// In0
	GpioMuxRegs.GPBDIR.bit.GPIOB9			= 0;	// In1
	GpioMuxRegs.GPBDIR.bit.GPIOB10			= 0;	// In2
	GpioMuxRegs.GPBDIR.bit.GPIOB11			= 0;	// In3
	GpioMuxRegs.GPBDIR.bit.GPIOB12			= 1;	// Out0
	GpioMuxRegs.GPBDIR.bit.GPIOB13			= 1;	// Out1
	GpioMuxRegs.GPBDIR.bit.GPIOB14			= 1;	// Out2
	GpioMuxRegs.GPBDIR.bit.GPIOB15			= 1;	// Out3

// GPD
	GpioMuxRegs.GPDMUX.bit.T1CTRIP_PDPA_GPIOD0	= 1;	// PDPAINT
	GpioMuxRegs.GPDQUAL.all	= 0xff;						// (0xff*2)¸öSYSCLKOUTÊ±³¤ÅÐÊäÈëÓÐÐ§

// GPE
//	GpioMuxRegs.GPEDIR.bit.GPIOE0	= 0;

// GPF
//	GpioMuxRegs.GPFMUX.bit.MCLKXA_GPIOF8	= 0;

//	GpioMuxRegs.GPFDIR.bit.GPIOF14 = 0;

// GPG_MUX
//	GpioMuxRegs.GPGMUX.bit.SCIRXDB_GPIOG5 = 1;

	EDIS;
}

/*funName:		InitPeripherals()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	³õÊ¼»¯ÍâÉè
******************************************************************************/
void InitPeripherals()
{
//	// Initialize CPU Timers To default State:
//	InitCpuTimers();

//	// Initialize McBSP Peripheral To default State:
//	InitMcbsp();

	// Initialize Event Manager Peripheral To default State:
	InitEv();

    // Initialize ADC Peripheral To default State:
    ad.init(&ad);

//	// Initialize eCAN Peripheral To default State:
//    InitECan();

//	// Initialize SPI Peripherals To default State:
//	InitSpi();

//	// Initialize SCI Peripherals To default State:
//	InitSci();
}

/*funName:		InitEv()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	³õÊ¼»¯EV
******************************************************************************/
void InitEv(void)
{
// EVA Control Register
//	EvaRegs.EXTCONA.bit.INDCOE	= 0;	// Compare output independent mode: disable

// Timer x Control Register
	EvaRegs.T1CON.bit.FREE		= 0;
	EvaRegs.T1CON.bit.SOFT		= 0;	// Stop immediately on emulation suspend
	EvaRegs.T1CON.bit.TMODE		= 1;	// Count Mode: continuous-Up/-Down
	EvaRegs.T1CON.bit.TPS		= T1_TPS;// TxCLK=HSPCLK/(2^TPS)=150MHz	HSPCLK=150MHz
	EvaRegs.T1CON.bit.TCLKS10	= 0;	// Clock source use: internal 
	EvaRegs.T1CON.bit.TCLD10	= 0;	// T1CMPR reload when: T1CNT=0
	EvaRegs.T1CON.bit.TECMPR	= 0;
	EvaRegs.T1CON.bit.TENABLE	= 0;	// Disable T1
//	EvaRegs.T2CON.bit.T2SWT1	= 0;	// T2 enabling use: own TENABLE bit
//	EvaRegs.T2CON.bit.SET1PR	= 0;	// T2PR using T1PR. (Reserved bit in T1CON)

// GP Timer Control Register
	EvaRegs.GPTCONA.bit.T1TOADC = 2;	// ÖÜÆÚÖÐ¶ÏÆô¶¯ADC, ÅäºÏÏÂÒçÖÐ¶ÏÖ´ÐÐÖ÷³ÌÐò
//	EvaRegs.GPTCONA.bit.T2TOADC = 2;
//	EvaRegs.GPTCONA.bit.T2CTRIPE= 1;	// TxCTRIP enabled, TxCMPR output=HI-Z when TxCTRIP=low (active only when EXTCONA.0=1)
//	EvaRegs.GPTCONA.bit.T1CTRIPE= 1;
//	EvaRegs.GPTCONA.bit.TCMPOE	= 0;	// TxPWM_TxCMP are in HI-Z state
//	EvaRegs.GPTCONA.bit.T2CMPOE = 0;
//	EvaRegs.GPTCONA.bit.T1CMPOE = 0;
//	EvaRegs.GPTCONA.bit.T2PIN	= 0;	// Tx compare output polarity: Force low
//	EvaRegs.GPTCONA.bit.T1PIN	= 0;
	
// Compare Control Register
	EvaRegs.COMCONA.bit.CENABLE	= 1;	// Compare operation: Enabled
	EvaRegs.COMCONA.bit.CLD		= 0;	// CMPRx reload when: Underflow
	EvaRegs.COMCONA.bit.SVENABLE= 0;	// SVPWM mode: disable
	EvaRegs.COMCONA.bit.ACTRLD	= 0;	// ACTRx reload when: T1CNT=0
	EvaRegs.COMCONA.bit.FCOMPOE	= 0;	// Full compare output: disable
//	EvaRegs.COMCONA.bit.FCMP3OE = 1;	// CMPRx output: enable (active only when EXTCONA.0=0)
//	EvaRegs.COMCONA.bit.FCMP2OE = 1;
//	EvaRegs.COMCONA.bit.FCMP1OE = 1;
//	EvaRegs.COMCONA.bit.C3TRIPE	= 1;	// CxTRIP enabled, CMPRx output=HI-Z when CxTRIP=low (active only when EXTCONA.0=1)
//	EvaRegs.COMCONA.bit.C2TRIPE	= 1;
//	EvaRegs.COMCONA.bit.C1TRIPE	= 1;

// Compare Action Control Register
//	EvaRegs.ACTRA.bit.SVRDIR	= 0;	// SVPWM rotation direction: positive(CCW)
//	EvaRegs.ACTRA.bit.D			= 0;	// Basic space vector bits
	EvaRegs.ACTRA.all 			= 0x999;// CMPR1,3,5  active low; CMPR2, 4, 6 active high; 

// Dead-Band Timer Control Register
	EvaRegs.DBTCONA.bit.DBT		= VDBT;
	EvaRegs.DBTCONA.bit.DBTPS	= VDBTPS;	// DBtime = T_HSPCLK*(DBT*(2^DBTPS))	(DBTPS<=5)
	EvaRegs.DBTCONA.bit.EDBT1	= 1;		// Dead-band timer x enable
	EvaRegs.DBTCONA.bit.EDBT2	= 1;
	EvaRegs.DBTCONA.bit.EDBT3	= 1;
	
    EvaRegs.T1CNT	= 0x0000;
    EvaRegs.T1PR 	= T1PR_VAL;
//	EvaRegs.CMPR1	= T1PR_VAL>>1;
//	EvaRegs.CMPR2	= T1PR_VAL>>1;
//	EvaRegs.CMPR3	= T1PR_VAL>>1;

// IMR, IFR
	EvaRegs.EVAIMRA.bit.T1UFINT	= 0;
	EvaRegs.EVAIFRA.bit.T1UFINT	= 1;
	EvaRegs.EVAIMRA.bit.PDPINTA	= 0;
	EvaRegs.EVAIFRA.bit.PDPINTA	= 1;
	
#if	DUAL_EV_MODE
// EVB Control Register
// Timer x Control Register
	EvbRegs.T3CON.bit.FREE		= 0;
	EvbRegs.T3CON.bit.SOFT		= 0;	// Stop immediately on emulation suspend
	EvbRegs.T3CON.bit.TMODE		= 1;	// Count Mode: continuous-Up/-Down
	EvbRegs.T3CON.bit.TPS		= T1_TPS;// TxCLK=HSPCLK/(2^TPS)=150MHz	HSPCLK=150MHz
	EvbRegs.T3CON.bit.TCLKS10	= 0;	// Clock source use: internal 
	EvbRegs.T3CON.bit.TCLD10	= 0;	// T3CMPR reload when: T3CNT=0
	EvbRegs.T3CON.bit.TECMPR	= 0;
	EvbRegs.T3CON.bit.TENABLE	= 0;	// Disable T3
//	EvbRegs.T4CON.bit.SET3PR	= 0;	// T4PR using T3PR. (Reserved bit in T3CON)

// GP Timer Control Register
//	EvbRegs.GPTCONB.bit.T3TOADC = 2;	// Starts ADC by interrupt flag of: Tx period
//	EvbRegs.GPTCONB.bit.T4TOADC = 2;
//	EvbRegs.GPTCONB.bit.T4CTRIPE= 1;	// TxCTRIP enabled, TxCMPR output=HI-Z when TxCTRIP=low (active only when EXTCONB.0=1)
//	EvbRegs.GPTCONB.bit.T3CTRIPE= 1;
//	EvbRegs.GPTCONB.bit.TCMPOE	= 0;	// TxPWM_TxCMP are in HI-Z state
//	EvbRegs.GPTCONB.bit.T4CMPOE = 0;
//	EvbRegs.GPTCONB.bit.T3CMPOE = 0;
//	EvbRegs.GPTCONB.bit.T4PIN	= 0;	// Tx compare output polarity: Force low
//	EvbRegs.GPTCONB.bit.T3PIN	= 0;
	
// Compare Control Register
	EvbRegs.COMCONB.bit.CENABLE	= 1;	// Compare operation: Enabled
	EvbRegs.COMCONB.bit.CLD		= 0;	// CMPRx reload when: Underflow
	EvbRegs.COMCONB.bit.SVENABLE= 0;	// SVPWM mode: disable
	EvbRegs.COMCONB.bit.ACTRLD	= 0;	// ACTRx reload when: T3CNT=0
	EvbRegs.COMCONB.bit.FCOMPOE	= 0;	// Full compare output: disable
//	EvbRegs.COMCONB.bit.FCMP3OE = 1;	// CMPRx output: enable (active only when EXTCONB.0=0)
//	EvbRegs.COMCONB.bit.FCMP2OE = 1;
//	EvbRegs.COMCONB.bit.FCMP1OE = 1;
//	EvbRegs.COMCONB.bit.C3TRIPE	= 1;	// CxTRIP enabled, CMPRx output=HI-Z when CxTRIP=low (active only when EXTCONB.0=1)
//	EvbRegs.COMCONB.bit.C2TRIPE	= 1;
//	EvbRegs.COMCONB.bit.C1TRIPE	= 1;

// Compare Action Control Register
//	EvbRegs.ACTRB.bit.SVRDIR	= 0;	// SVPWM rotation direction: positive(CCW)
//	EvbRegs.ACTRB.bit.D			= 0;	// Basic space vector bits
	EvbRegs.ACTRB.all 			= 0x999;// CMPR7,9,11 active low; CMPR8,10,12 active high; 

// Dead-Band Timer Control Register
	EvbRegs.DBTCONB.bit.DBT		= VDBT;
	EvbRegs.DBTCONB.bit.DBTPS	= VDBTPS;	// DBtime = T_HSPCLK*(DBT*(2^DBTPS))	(DBTPS<=5)
	EvbRegs.DBTCONB.bit.EDBT1	= 1;		// Dead-band timer x enable
	EvbRegs.DBTCONB.bit.EDBT2	= 1;
	EvbRegs.DBTCONB.bit.EDBT3	= 1;
	
    EvbRegs.T3CNT	= 0x0000;
    EvbRegs.T3PR 	= T1PR_VAL;
//	EvbRegs.CMPR4	= T1PR_VAL>>1;
//	EvbRegs.CMPR5	= T1PR_VAL>>1;
//	EvbRegs.CMPR6	= T1PR_VAL>>1;

// IMR, IFR
//	EvbRegs.EVBIMRA.bit.T3UFINT	= 0;
//	EvbRegs.EVBIFRA.bit.T3UFINT	= 1;
//	EvbRegs.EVBIMRA.bit.PDPINTB	= 0;
//	EvbRegs.EVBIFRA.bit.PDPINTB	= 1;
#endif

}

/*funName:		InitL3C()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	³õÊ¼»¯L3C
******************************************************************************/
void InitL3C()
{
	int16	i;
	
	l3c.uStep		= STEP_DEFAULT;
	l3c.iFaultC		= CINI;
	l3c.iStartC		= CINI;
	l3c.iStopC		= CINI;
	l3c.iPdpintC	= CINI;
	l3c.iUdcovC		= CINI;
	l3c.iIdcocC		= CINI;
	l3c.uCap		= 0;
	l3c.iErrTwinkC	= 1000;							// ÖÃ´óÊý¹Ø±ÕERRµÆÉÁË¸¼ÆÊýÆ÷

//	l3c.qSina		= _IQ(0);
//	l3c.qSina		= _IQ(1);
	l3c.qTheta		= 0;
	l3c.qDTheta		= _IQ(DTHETA_DEFAULT_q);
	l3c.qIdR		= _IQ(OPEN_REF_ID_q);
	l3c.qIqR		= _IQ(OPEN_REF_IQ_q);
	l3c.qUnpe		= 0;
	
	for(i=0;i<AD_CH_NUM;i++)
		l3c.qAD[i] = 0;
	
	// LPF²ÎÊý
	lpfUdc.qkin[0]	= _IQ(LPF_KIN0_q);
	lpfUdc.qkin[1]	= _IQ(LPF_KIN1_q);
	lpfUdc.qkin[2]	= _IQ(LPF_KIN2_q);
	lpfUdc.qkout[1]	= _IQ(LPF_KOUT1_q);
	lpfUdc.qkout[2]	= _IQ(LPF_KOUT2_q);

	lpfUnp.qkin[0]	= _IQ(LPF_KIN0_q);		// AD²ÉÑù´¦ÀíÓÃ
	lpfUnp.qkin[1]	= _IQ(LPF_KIN1_q);
	lpfUnp.qkin[2]	= _IQ(LPF_KIN2_q);
	lpfUnp.qkout[1] = _IQ(LPF_KOUT1_q);
	lpfUnp.qkout[2] = _IQ(LPF_KOUT2_q);
	
	lpfUnpe.qkin[0]	= _IQ(LPF_KIN0_q);		// FBDCLÓÃ
	lpfUnpe.qkin[1]	= _IQ(LPF_KIN1_q);
	lpfUnpe.qkin[2]	= _IQ(LPF_KIN2_q);
	lpfUnpe.qkout[1]= _IQ(LPF_KOUT1_q);
	lpfUnpe.qkout[2]= _IQ(LPF_KOUT2_q);
	
	// ÃðµÆ
	LED_BIT_START = LED_OFF;				// Ï¨ÃðSTART
	LED_BIT_STOP  = LED_ON;					// µãÁÁSTOP
#if	(L3_SP_MODE!=L3_SP_NONPC)				// !=: Ei0,Ei3ÓÃÓÚÆô¶¯NPC_ON,NPC_OFF¿ØÖÆ	==:ÓÃÓÚFAULT,RESET
	LED_BIT_NpcOn = LED_OFF;				// Ï¨ÃðNPCON
	LED_BIT_NpcOff= LED_OFF;				// Ï¨ÃðNPCOFF
#else
	LED_BIT_RESET = LED_OFF;				// Ï¨ÃðRESET
	LED_BIT_IDCOC = LED_OFF;				// Ï¨ÃðIDCOC
#endif
	
	// Çå±êÖ¾Î»
	l3c.flag.bit.start	= KEY_OFF;
	l3c.flag.bit.stop	= KEY_OFF;
	l3c.flag.bit.reset	= KEY_OFF;
	l3c.flag.bit.err 	= KEY_OFF;
	l3c.flag.bit.err_idcoc = KEY_OFF;
	l3c.flag.bit.err_udcov = KEY_OFF;
	l3c.flag.bit.err_1 = KEY_OFF;
	l3c.flag.bit.err_2 = KEY_OFF;
	l3c.flag.bit.NpcOn = KEY_OFF;
	
	npc.qKb1 = _IQ(1);
	npc.qKb2 = _IQ(1);
	npc.qKs  = _IQ(0);
	
	// CMPR·ù³õÖµ
	EvaRegs.CMPR1	= 16101;
	EvaRegs.CMPR2	= 37500;
	EvaRegs.CMPR3	= 37500;

	EvbRegs.CMPR4	= 0;
	EvbRegs.CMPR5	= 4681;
	EvbRegs.CMPR6	= 16720;
}

/*funName:		CheckNormal()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	ÅÐ¶ÏÔËÐÐ×´Ì¬,Õý³£Ôò¿ªÖÐ¶Ï
******************************************************************************/
void CheckNormal()
{
// ÅÐ¶Ï°´¼üÊäÈë£¬¼ÇÔÚflagÈí¼þ±êÖ¾ÖÐ
//************************************************************************
// ¼ì²éSTART
	if(l3c.flag.bit.start==KEY_OFF){
		if(IN_BIT_START==KEY_ON){					//·À¶¶¼ì²â°´¼ü °´¼üÊäÈëµÍÓÐÐ§
			l3c.iStartC--;
			if(l3c.iStartC<=0){
				l3c.iStartC = CMAX;
				l3c.flag.bit.start = KEY_ON;		//ÖÃSTART
				l3c.flag.bit.stop  = KEY_OFF;		//ÇåSTOP
				l3c.flag.bit.reset = KEY_OFF;		//ÇåRESET
			}
		}
		else
			if(l3c.iStartC<CMAX)					// Ã»ÓÐ¹ÊÕÏÔò¹ÊÕÏ¼ÆÊýÆ÷+1
				l3c.iStartC++;
	}
// ¼ì²éSTOP
	if(l3c.flag.bit.stop==KEY_OFF){
		if(IN_BIT_STOP==KEY_ON){					//·À¶¶¼ì²â°´¼ü °´¼üÊäÈëµÍÓÐÐ§
			l3c.iStopC--;
			if(l3c.iStopC<=0){
				l3c.iStopC=CMAX;
				l3c.flag.bit.stop = KEY_ON;			//ÖÃSTOP
				l3c.flag.bit.start = KEY_OFF;		//ÇåSTART
				l3c.flag.bit.reset = KEY_OFF;		//ÇåRESET
			}
		}
		else
			if(l3c.iStopC<CMAX)						// Ã»ÓÐ¹ÊÕÏÔò¹ÊÕÏ¼ÆÊýÆ÷+1
				l3c.iStopC++;
	}
#if	(L3_SP_MODE!=L3_SP_NONPC)						// !=: Ei0,Ei3ÓÃÓÚÆô¶¯NPC_ON,NPC_OFF¿ØÖÆ	==:ÓÃÓÚFAULT,RESET
// ¼ì²éNPC_ON
	if(l3c.flag.bit.NpcOn==KEY_OFF){
		if(IN_BIT_NpcOn==KEY_ON){					//·À¶¶¼ì²â°´¼ü °´¼üÊäÈëµÍÓÐÐ§
			l3c.iNpcOnC--;
			if(l3c.iNpcOnC<=0){
				l3c.iNpcOnC=CMAX;
				l3c.flag.bit.NpcOn 	= KEY_ON;		//ÖÃNpcOn
			}
		}
		else
			if(l3c.iNpcOnC<CMAX)
				l3c.iNpcOnC++;
	}
// ¼ì²éNPC_OFF
	if(l3c.flag.bit.NpcOn==KEY_ON){
		if(IN_BIT_NpcOff==KEY_ON){					//·À¶¶¼ì²â°´¼ü °´¼üÊäÈëµÍÓÐÐ§
			l3c.iNpcOffC--;
			if(l3c.iNpcOffC<=0){
				l3c.iNpcOffC=CMAX;
				l3c.flag.bit.NpcOn	= KEY_OFF;		//ÇåNpcOn
			}
		}
		else
			if(l3c.iNpcOffC<CMAX)
				l3c.iNpcOffC++;
	}
#else						// (KEY_NPC==0)
// ¼ì²éRESET
	if(l3c.flag.bit.reset==KEY_OFF){
		if(IN_BIT_RESET==KEY_ON){					//·À¶¶¼ì²â°´¼ü °´¼üÊäÈëµÍ§
			l3c.iResetC--;
			if(l3c.iResetC<=0){
				l3c.iResetC=CMAX;
				l3c.flag.bit.reset 	= KEY_ON;		//ÖÃSTOP
				l3c.flag.bit.start = KEY_OFF;		//ÇåSTART
				l3c.flag.bit.stop  = KEY_OFF;		//ÇåSTOP
			}
		}
		else
			if(l3c.iResetC<CMAX)					// Ã»ÓÐ¹ÊÕÏÔò¹ÊÕÏ¼ÆÊýÆ÷+1
				l3c.iResetC++;
	}
#endif

// ¸ù¾ÝuRunflagÈí¼þ±êÖ¾½øÐÐ²Ù×÷
//************************************************************************
// START==1,STOP==0,ERR==0
	if((l3c.flag.bit.start==KEY_ON)
	 &&(l3c.flag.bit.stop ==KEY_OFF)
	 &&(l3c.flag.bit.err  ==KEY_OFF)){
		LED_BIT_STOP  = LED_OFF;				// Ï¨ÃðSTOP
		LED_BIT_RESET = LED_OFF;				// Ï¨ÃðRESET

	// Æô¶¯L3ÏµÍ³
#if DUAL_EV_MODE
		EvbRegs.T3CON.bit.TENABLE	= 1;		// Ê¹ÄÜT3
	#if	PWM_ON
		EvbRegs.COMCONB.bit.FCOMPOE	= 1;		// Ê¹ÄÜÈ«±È½ÏPWMÊä³ö
	#endif
#endif
		EvaRegs.T1CON.bit.TENABLE	= 1;		// Ê¹ÄÜT1
	#if	PWM_ON
		EvaRegs.COMCONA.bit.FCOMPOE	= 1;		// Ê¹ÄÜÈ«±È½ÏPWMÊä³ö
	#endif

		l3c.flag.bit.start = KEY_OFF;			// ÓÃÍê¼´¸´Î»
		EINT;
	}

// STOP==1
	if(l3c.flag.bit.stop==KEY_ON){
		IDEL();
		LED_BIT_STOP  = LED_ON;					// µãÁÁSTOP
		LED_BIT_START = LED_OFF;				// Ï¨ÃðSTART
#if	(L3_SP_MODE==L3_SP_NONPC)
		LED_BIT_RESET = LED_OFF;				// Ï¨ÃðRESET
#endif
		delayx(10);
	
	// ¼ÆÊýÆ÷ÇåÁã
		l3c.iFaultC = 0;
		l3c.iIdcocC = 0;
		l3c.iPdpintC = 0;
//		uGisr1 = 0;
//		uGisr2 = 0;

		l3c.flag.bit.stop = KEY_OFF;			// ÓÃÍê¼´¸´Î»
	}

#if	(L3_SP_MODE!=L3_SP_NONPC)					// !=: Ei0,Ei3ÓÃÓÚÆô¶¯NPC_ON,NPC_OFF¿ØÖÆ	==:ÓÃÓÚFAULT,RESET
// NpcOn==1
	if(l3c.flag.bit.NpcOn==KEY_ON){
		LED_BIT_NpcOn  = LED_ON;
		LED_BIT_NpcOff = LED_OFF;
	}
// NpcOff==1
	if(l3c.flag.bit.NpcOn==KEY_OFF){
		LED_BIT_NpcOff  = LED_ON;
		LED_BIT_NpcOn	= LED_OFF;
	}
#else					// (KEY_NPC==0)
// RESET==1
	if(l3c.flag.bit.reset==KEY_ON){
		IDEL();
		LED_BIT_RESET = LED_ON;					// µãÁÁRESET
		LED_BIT_START = LED_OFF;				// Ï¨ÃðSTART
		LED_BIT_STOP  = LED_OFF;				// Ï¨ÃðSTOP
		LED_BIT_IDCOC = LED_OFF;				// Ï¨ÃðIDCOC
		l3c.iErrTwinkC = 1000;					// ÖÃ´óÊý¹Ø±ÕERRµÆÉÁË¸¼ÆÊýÆ÷
		delayx(10);
		
	// ¼ÆÊýÆ÷ÇåÁã
		l3c.iFaultC = 0;
		l3c.iIdcocC = 0;
		l3c.iPdpintC = 5;
		
		l3c.flag.bit.reset = KEY_OFF;			// ÓÃÍê¼´¸´Î»
	}

// ERR==1
	if(l3c.flag.bit.err==KEY_ON){
		IDEL();
		LED_BIT_START = LED_OFF;				// Ï¨ÃðSTART
		LED_BIT_RESET = LED_OFF;				// Ï¨ÃðRESET
		delayx(10);
		l3c.iErrTwinkC = 0;						// Æô¶¯ERRµÆÉÁË¸¼ÆÊýÆ÷
				
	// ¼ÆÊýÆ÷ÇåÁã
		l3c.iFaultC = 0;
		l3c.iIdcocC = 0;
		l3c.iPdpintC = 0;
//		uGisr1 = 0;
//		uGisr2 = 0;
		l3c.flag.bit.err = KEY_OFF;				// ÓÃÍê¼´¸´Î»
	}

// ÓÐ¹ÊÕÏÊ±ÉÁË¸STOPµÆ(PB.14)
	if(l3c.iErrTwinkC<100){						// ¼ÆÊýÆ÷Öµ<100Ê±²ÅÉÁË¸
		if(l3c.iErrTwinkC>=0){
			l3c.iErrTwinkC--;
			delayx(150);
		}
		else{
			l3c.iErrTwinkC = 20;
			LED_BIT_TWI_IDCOC = 1;				// ÉÁË¸IDCOCµÆ
		}
	}
#endif
}

/*funName:		CheckFault()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	ÅÐ¶Ï¹ÊÕÏÊäÈëÐÅºÅ
******************************************************************************/
void CheckFault()
{
#if	(L3_SP_MODE==L3_SP_NONPC)				// !=: Ei0,Ei3ÓÃÓÚÆô¶¯NPC_ON,NPC_OFF¿ØÖÆ	==:ÓÃÓÚFAULT,RESET
// IDCOC	PB8
 #if (!CHEAT_NO_IDCOC)
	if(IN_BIT_IDCOC==KEY_ON){
		if(l3c.iIdcocC>0)				// ÓÐÒ»´Î¹ÊÕÏÔò¹ÊÕÏ¼ÆÊýÆ÷-1
			l3c.iIdcocC--;
		if(l3c.iIdcocC<=0){				// ¼õµ½0ÔòÅÐ·¢Éú¹ÊÕÏ£¬µãLED¡¢ÖÃ±êÖ¾
			LED_BIT_IDCOC = LED_ON;		// µãÁÁIDCOCµÆ
			l3c.flag.bit.err = KEY_ON;
			l3c.flag.bit.err_idcoc = KEY_ON;
		}
	}
	else{
		if(l3c.iIdcocC<CMAX)			// Ã»ÓÐ¹ÊÕÏÔò¹ÊÕÏ¼ÆÊýÆ÷+1
			l3c.iIdcocC++;
	}
 #endif
#endif
}

/*funName:		IDEL()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	½øÈë´ý»ú×´Ì¬
******************************************************************************/
void IDEL()
{
//	l3c.flag = 0;						// ÕâÀï²»ÄÜÇåÔËÐÐ±êÖ¾
//	l3c.flagBak = 0;

	EvaRegs.CMPR1	= T1PR_VAL>>1;			// Ç¿ÖÆÀ­µ½N
	EvaRegs.CMPR2	= T1PR_VAL>>1;
	EvaRegs.CMPR3	= T1PR_VAL>>1;
	EvaRegs.COMCONA.bit.FCOMPOE = 0;	// ½ûÖ¹±È½ÏÊä³ö
	EvaRegs.T1CON.bit.TENABLE	= 0;	// ¹Ø±ÕT1
#if	DUAL_EV_MODE						// 1:Ê¹ÄÜË«EVÊä³öÈýµçÆ½PWMÄ£Ê½
	EvbRegs.CMPR4	= T1PR_VAL>>1;
	EvbRegs.CMPR5	= T1PR_VAL>>1;
	EvbRegs.CMPR6	= T1PR_VAL>>1;
	EvbRegs.COMCONB.bit.FCOMPOE = 0;	// ½ûÖ¹±È½ÏÊä³ö
	EvbRegs.T3CON.bit.TENABLE	= 0;	// ¹Ø±ÕT3
#endif
	
	LED_BIT_START = LED_OFF;			// Ï¨ÃðSTART
//	LED_BIT_RESET = LED_OFF;			// µãÁÁRESET
//	LED_BIT_STOP  = LED_OFF;			// Ï¨ÃðSTOP
//	LED_BIT_IDCOC = LED_ON;				// Ï¨ÃðIDCOC

	DINT;
}

/*funName:		test_io_ad()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	IO,PWM,ADµÈ±¾¹¦ÄÜµ÷ÊÔ
******************************************************************************/
void test_io_ad(void)
{
	int16	i;
	
//OUT test
   	GpioDataRegs.GPBDAT.bit.GPIOB15 = 0;	// OUT3
	GpioDataRegs.GPBDAT.bit.GPIOB14 = 0;	// OUT2
	GpioDataRegs.GPBDAT.bit.GPIOB13 = 0;	// OUT1
	GpioDataRegs.GPBDAT.bit.GPIOB12 = 0;	// OUT0

	GpioDataRegs.GPBDAT.bit.GPIOB15 = 1;
	GpioDataRegs.GPBDAT.bit.GPIOB14 = 1;
	GpioDataRegs.GPBDAT.bit.GPIOB13 = 1;
	GpioDataRegs.GPBDAT.bit.GPIOB12 = 1;

// PWM test
	GpioDataRegs.GPADAT.bit.GPIOA0 = 0;		// PWM1
	GpioDataRegs.GPADAT.bit.GPIOA0 = 1;

	GpioDataRegs.GPADAT.bit.GPIOA1 = 0;		// PWM2
	GpioDataRegs.GPADAT.bit.GPIOA1 = 1;

	GpioDataRegs.GPADAT.bit.GPIOA2 = 0;		// PWM3
	GpioDataRegs.GPADAT.bit.GPIOA2 = 1;

	GpioDataRegs.GPADAT.bit.GPIOA3 = 0;		// PWM4
	GpioDataRegs.GPADAT.bit.GPIOA3 = 1;

	GpioDataRegs.GPADAT.bit.GPIOA4 = 0;		// PWM5
	GpioDataRegs.GPADAT.bit.GPIOA4 = 1;

	GpioDataRegs.GPADAT.bit.GPIOA5 = 0;		// PWM6
	GpioDataRegs.GPADAT.bit.GPIOA5 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB0 = 0;		// PWM7
	GpioDataRegs.GPBDAT.bit.GPIOB0 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB1 = 0;		// PWM8
	GpioDataRegs.GPBDAT.bit.GPIOB1 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB2 = 0;		// PWM9
	GpioDataRegs.GPBDAT.bit.GPIOB2 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB3 = 0;		// PWM10
	GpioDataRegs.GPBDAT.bit.GPIOB3 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB4 = 0;		// PWM11
	GpioDataRegs.GPBDAT.bit.GPIOB4 = 1;

	GpioDataRegs.GPBDAT.bit.GPIOB5 = 0;		// PWM12
	GpioDataRegs.GPBDAT.bit.GPIOB5 = 1;

//IN test
	;				// ¼Ó¶ÏµãË¢ÐÂ¹Û²ìB8~B11
	
//AD test
	ad.start(&ad);	// ²»¶Ï¶ÁÈ¡ADÖµ, ¹Û²ìad.out[]
	delayx(1);
	ad.read(&ad);

	for(i=0;i<AD_CH_NUM;i++)
		l3c.qAD[i] -= _IQ(1.5);

	l3c.qAD[AD_CH_IA]  = _IQmpy(ad.qOut[AD_CH_IA ], AD_K_IA);
	l3c.qAD[AD_CH_IB]  = _IQmpy(ad.qOut[AD_CH_IB ], AD_K_IB);
	l3c.qAD[AD_CH_IDC] = _IQmpy(ad.qOut[AD_CH_IDC], AD_K_IDC);
	l3c.qAD[AD_CH_INP] = _IQmpy(ad.qOut[AD_CH_INP], AD_K_INP);
	l3c.qAD[AD_CH_UDC] = _IQmpy(ad.qOut[AD_CH_UDC], AD_K_UDC);	// AD_K_UDC¶¨ÒåÊ±ÓÒÒÆÁË4Î»(ÒÔÔö´óÕûÊý·¶Î§), ÔòUdcÎªIQ20¸ñÊ½
	l3c.qAD[AD_CH_UNP] = _IQmpy(ad.qOut[AD_CH_UNP], AD_K_UNP);	// AD_K_UNP¶¨ÒåÊ±ÓÒÒÆÁË4Î»
}


/*funName:		delayx()
* Inputs: 		n
* Outputs: 		NONE
* Description: 	Ö´ÐÐn¸öµ¥Î»µÄÐ¡ÑÓÊ±
******************************************************************************/
void delayx(Uint16 n)
{
    Uint16	i,j;
	for(i=0;i<n;i++){
		for(j=0;j<1000;j++){}
	}
}

//===========================================================================
// Function format demo
//===========================================================================
/*funName:		functionxx()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	º¯ÊýµÄÖ÷Òª¹¦ÄÜ
******************************************************************************/
/*
void functionxx()
{
}
*/
