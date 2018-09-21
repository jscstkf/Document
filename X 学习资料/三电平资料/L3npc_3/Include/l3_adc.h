/******************************************************************************
// FILE:    l3_adc.h
// TITLE:   NPC 3-Level converter controller ADC program's include .h
// DESCRIPTION:
//          .
//
// 2009-10-24	V1.0
******************************************************************************/

#ifndef __L3_ADC_H__
#define __L3_ADC_H__

//#include "IQmathLib.h"
//#include "DSP281x_Device.h"

/*结构体定义
*****************************************************************************/
/* ADC型结构体 */
/*****************************************************************************/
typedef struct {
	_iq	qGain[8];
	_iq	qOffs[8];
	_iq	qOut[8];

	_iq	qAll_gain;
	_iq	qAll_offs;

	void (*init)();
	void (*start)();
	void (*read)();
	void (*offset)();
}ADC;

/* ADC型结构体初始化值 */
#define ADC_DEFAULTS {\
	{_IQ(1),_IQ(1),_IQ(1),_IQ(1),_IQ(1),_IQ(1),_IQ(1),_IQ(1)},\
	{AD_OFF_IA,\
	AD_OFF_IB,\
	AD_OFF_IDC,\
	AD_OFF_INP,\
	AD_OFF_UDC,\
	AD_OFF_UNP,0,0},\
	{0,0,0,0,0,0,0,0},\
	_IQ(1),0,\
	(void (*)(Uint32))ADC_init, \
	(void (*)(Uint32))ADC_start, \
	(void (*)(Uint32))ADC_read, \
	(void (*)(Uint32))ADC_offset \
	}
typedef ADC *ADC_handle;

/*对外声明函数
*****************************************************************************/
extern void ADC_init(ADC_handle);		// ADC initialation
extern void ADC_start(ADC_handle);		// ADC Start
extern void ADC_read(ADC_handle);		// ADC results
extern void ADC_offset(ADC_handle);		// ADC Gain & Offset calculation


#endif
