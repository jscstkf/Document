/******************************************************************************
// FILE:    l3_adc.c
// TITLE:   NPC 3-Level converter controller ADC program
// DESCRIPTION:
//          ADC Program.
//
// 2009-10-24	V1.0
******************************************************************************/

#include "l3_global.h"

/*全局一般变量
*****************************************************************************/


/*函数
*****************************************************************************/
#define	ONE_HALF_Q16	98304//_IQ16(1.5)//
/*funName:		ADC_read()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	读取,转换所有6个AD通道值RESULTx(高12位有效数), 并作Gain和Offset处理, 结果为_iq格式
*				从采样板输入端Y到ADC数字值X的对应关系为: [-1.5V, 1.5V]-->[0xfff0, 0]
*				即: Y = 1.5-(X*3/65520) = 1.5 - (3/65520)*X = (98304)q16 - (3)q16*X
******************************************************************************/
void ADC_read(ADC_handle p)
{
    _iq16	dat1,dat2;
	_iq		tmp;

	/* Wait until ADC conversion is completed */
	while (AdcRegs.ADCST.bit.SEQ1_BSY == 1)  	// 等待ADC转换完成
	{};

// AD_CH_IA
	dat1 = AdcRegs.ADCRESULT0 & 0xfff0;					// 281x ADC仅高12位有效
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT1 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_IA];							// 零偏处理
	p->qOut[AD_CH_IA] = _IQmpy(tmp, p->qGain[AD_CH_IA]);// 增益处理
	
// AD_CH_IB
	dat1 = AdcRegs.ADCRESULT2 & 0xfff0;
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT3 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_IB];
	p->qOut[AD_CH_IB] = _IQmpy(tmp, p->qGain[AD_CH_IB]);

// AD_CH_IDC
	dat1 = AdcRegs.ADCRESULT4 & 0xfff0;
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT5 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_IDC];
	p->qOut[AD_CH_IDC] = _IQmpy(tmp, p->qGain[AD_CH_IDC]);
		
// AD_CH_INP
	dat1 = AdcRegs.ADCRESULT6 & 0xfff0;
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT7 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_INP];
	p->qOut[AD_CH_INP] = _IQmpy(tmp, p->qGain[AD_CH_INP]);
		
// AD_CH_UDC
	dat1 = AdcRegs.ADCRESULT8 & 0xfff0;
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT9 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_UDC];
	p->qOut[AD_CH_UDC] = _IQmpy(tmp, p->qGain[AD_CH_UDC]);
		
// AD_CH_UNP
	dat1 = AdcRegs.ADCRESULT10 & 0xfff0;
	dat1 = ONE_HALF_Q16-dat1*3;
	dat2 = AdcRegs.ADCRESULT11 & 0xfff0;
	dat2 = ONE_HALF_Q16-dat2*3;
	tmp	 = _IQ16toIQ( (dat1+dat2)>>1 );
	tmp -= p->qOffs[AD_CH_UNP];
	p->qOut[AD_CH_UNP] = _IQmpy(tmp, p->qGain[AD_CH_UNP]);
	
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
}


/*funName:		ADC_offset()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	根据ADREF_HIGH和ADREF_LOW通道值校正ADC的Gain和Offset
*				Gain 	= (xh-xl)/(yh-yl)
*				Offset	= yl*Gain-xl
******************************************************************************/
void ADC_offset(ADC_handle p)
{
	long 	dx,dy,yl,yh;
	_iq15	tt;
	
	yh = p->qOut[AD_CH_REF25];
	yl = p->qOut[AD_CH_REF05];
	dx = (long)AD_REF_HIGH - AD_REF_LOW;
	dy = yh-yl;
	tt = _IQ15div(dx,dy);
	p->qAll_gain = _IQ15mpy(yl,tt)-AD_REF_LOW;
	p->qAll_gain = tt>>2;							// IQ15toIQ13
}

/*funName:		ADC_init()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	初始化ADC
******************************************************************************/
void ADC_init(ADC_handle p)
{
	DELAY_US(5000L);						// ADC复位前延时8ms
	AdcRegs.ADCTRL1.bit.RESET		= 1;
	NOP();
	NOP();
	AdcRegs.ADCTRL1.bit.RESET		= 0;

	AdcRegs.ADCTRL1.bit.SUSMOD		= 3;	// 
	AdcRegs.ADCTRL1.bit.ACQ_PS		= 1;	// SOC脉冲宽度为1+1个ADCLK
	AdcRegs.ADCTRL1.bit.CPS			= 0;	// ADCLK = FCLK/1
	AdcRegs.ADCTRL1.bit.CONT_RUN	= 0;	// 0:启动才转换;		1:连续自动转换;
	AdcRegs.ADCTRL1.bit.SEQ_CASC	= 1;	// 0:2个单独8通道SEQ; 	1:级联成16通道SEQ;
   
   	// 带隙和参考电路上电
	AdcRegs.ADCTRL3.bit.ADCBGRFDN	= 0x3;
	DELAY_US(8000L);						// ADC上电重启前延时8ms
  	// 除带隙和参考电路之外的ADC其他模拟电路上电
    AdcRegs.ADCTRL3.bit.ADCPWDN		= 1;	// Power up rest of ADC 
 	DELAY_US(20L);

   	// ADC内核时钟分频器 ADCLK＝ HSPCLK/[ADCCLKPS[3:0]*(ADCTRL1[7]+1)]
	AdcRegs.ADCTRL3.bit.ADCCLKPS 	= 3;	// ADCCLK=150M/6=25MHz 
	AdcRegs.ADCTRL3.bit.SMODE_SEL	= 0;	// 0:顺序采样模式;		1:同时采样模式; (281x有A,B两个采保)

	// 确定采样序列数, 及分配各次采样的通道号
	AdcRegs.ADCMAXCONV.bit.MAX_CONV1= 12;	// 6通道过采样2次
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = AD_CH_IA;
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = AD_CH_IA; 
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = AD_CH_IB;
	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = AD_CH_IB;
	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = AD_CH_IDC;
	AdcRegs.ADCCHSELSEQ2.bit.CONV05 = AD_CH_IDC;
	AdcRegs.ADCCHSELSEQ2.bit.CONV06 = AD_CH_INP;
	AdcRegs.ADCCHSELSEQ2.bit.CONV07 = AD_CH_INP;
	AdcRegs.ADCCHSELSEQ3.bit.CONV08 = AD_CH_UDC;
	AdcRegs.ADCCHSELSEQ3.bit.CONV09 = AD_CH_UDC;
	AdcRegs.ADCCHSELSEQ3.bit.CONV10 = AD_CH_UNP;
	AdcRegs.ADCCHSELSEQ3.bit.CONV11 = AD_CH_UNP;

	// 设置是否使用系统事件启动AD
	AdcRegs.ADCTRL2.bit.RST_SEQ1	= 0;	// 0:无动作;			1:立即复位SEQ
	AdcRegs.ADCTRL2.bit.SOC_SEQ1	= 0;	// 0:清除启动触发;		1:软件启动ADC
	AdcRegs.ADCTRL2.bit.EVA_SOC_SEQ1= 1;	// 1:EVA触发SEQ1/SEQ有效
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1= 0;	// 1:SEQ触发的中断有效
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1= 0;	// 0:每次SEQ都中断;		1:隔次SEQ才中断
	AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1= 0;	// 1:ADCSOC引脚启动ADC有效
	AdcRegs.ADCTRL2.bit.RST_SEQ2	= 0;
	AdcRegs.ADCTRL2.bit.SOC_SEQ2	= 0;
	AdcRegs.ADCTRL2.bit.EVB_SOC_SEQ2= 0;
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ2= 0;
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ2= 0;
	AdcRegs.ADCTRL2.bit.EVB_SOC_SEQ2= 0;
		
	// 清ADC标志
	AdcRegs.ADCST.bit.INT_SEQ1_CLR	= 1;	// SEQ1中断标志
	AdcRegs.ADCST.bit.INT_SEQ2_CLR	= 1;	// SEQ2中断标志
}  

/*funName:		ADC_Start()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	启动ADC的SEQ
******************************************************************************/
void ADC_start(ADC_handle p)
{
	AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
}

//===========================================================================
// No more.
//===========================================================================
