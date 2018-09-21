/******************************************************************************
// FILE:    l3_main.h
// TITLE:   NPC 3-Level converter controller main program's include .h
// DESCRIPTION:
//          .
//
// 2009-10-15	V1.0
******************************************************************************/

#ifndef __L3_MAIN_H__
#define __L3_MAIN_H__


/*结构体定义
*****************************************************************************/
/* 运行状态字 */
typedef struct{						//	bits	description
	Uint16	start		:1;			//	0
	Uint16	stop		:1;			//	1
	Uint16	reset		:1;			//	2
	Uint16	err			:1;			//	3
	Uint16	err_idcoc	:1;			//	4
	Uint16	err_udcov	:1;			//	5
	Uint16	err_1		:1;			//	6
	Uint16	err_2		:1;			//	7
	Uint16	NpcOn		:1;			//	8
}RUNFLAG_BIT;

typedef union{
   Uint16		all;
   RUNFLAG_BIT	bit;
}RUNFLAG;


/* L3C型结构体 */
typedef struct{
//状态量
	RUNFLAG	flag;					//运行标志
	RUNFLAG	flagBak;				//历史运行标志
//角度
	_iq		qTheta;					//同步角度
	_iq		qDTheta;				//步距角度
	Uint16	uStep;					//步数
//	_iq 	iqSina;
//	_iq 	iqCosa;
//计数器
	int16	iFaultC;				//总故障计数器
	int16	iStartC;				//Start按键计数器
	int16	iStopC;					//Stop按键计数器
	int16	iResetC;				//Reset按键计数器
	int16	iNpcOnC;				//启动NPC按键计数器
	int16	iNpcOffC;				//停止NPC按键计数器
	int16	iErrTwinkC;				//Err闪灯计数器
	//3个故障
	int16	iPdpintC;				//Pdpint计数器
	int16	iUdcovC;				//Udc过压计数器
	int16	iIdcocC;				//Idc过流计数器
//一般变量
	Uint16 uCap;					//Capture值
	_iq qAD[6];						//AD测量值 IA,IB,IDC,INP,UDC,UNP
	_iq qIdR;						//d轴电流给定
	_iq qIqR;						//q轴电流给定
	_iq qVa,qVb,qVc;				//三相输出电压指令值
	_iq qTa,qTb,qTc;				//三相作用时间
	_iq	qUnpe;						//直流中点电压差值	=Unp-Unpref

}L3C;

/* L3C型结构体初始化值 */
#define L3C_DEFAULTS 	{\
			0xffff*KEY_OFF, 0xffff*KEY_OFF, \
			0, _IQ(DTHETA_DEFAULT_q), STEP_DEFAULT, \
			CINI, CINI, CINI, CINI, CINI, CINI, 1000,\
			CPDPINT, CINI, CINI, \
			0,{_IQ(0), _IQ(0), _IQ(0), _IQ(0), _IQ(0), _IQ(0)}, \
			_IQ(OPEN_REF_ID_q), _IQ(OPEN_REF_IQ_q), \
			_IQ(0), _IQ(0), _IQ(0), _IQ(0), _IQ(0), _IQ(0), _IQ(0)}


/* NPC型结构体 */
typedef struct{
//直流电平滞环变幅(HZDCL)策略
	_iq	qKb1;					//下电平变幅系数
	_iq	qKb2;					//上电平变幅系数
//直流电平滞环偏移(HSDCL)策略
	_iq	qKs;						//偏移系数
	_iq qKh;					//上下移动系数
	_iq qKr;					//上下移动系数
	_iq Km_sv;
//直流电平柔性平衡(FBDCL)策略
	_iq	qlbd;					//前馈补偿修正系数
	/*	out(k) = ki0*in(k)+kin1*in(k-1)+kin2*in(k-2)
		 	  - [kout1*out(k-1)+kout2*out(k-2)]			*/
	_iq	qLpfKin0;				//低通滤波器输入 0次系数
	_iq	qLpfKin1;				//低通滤波器输入-1次系数
	_iq	qLpfKin2;				//低通滤波器输入-2次系数
	_iq	qLpfKout1;				//低通滤波器输出-1次系数
	_iq	qLpfKout2;				//低通滤波器输出-2次系数
}NPC;

/* NPC型结构体初始化值 */
#define NPC_DEFAULTS 	{\
			1,1,\
			0,\
			0,\
			0,\
			0,\
			0.2,\
			_IQ(LPF_KIN0_q), _IQ(LPF_KIN1_q), _IQ(LPF_KIN2_q),\
			_IQ(LPF_KOUT1_q), _IQ(LPF_KOUT2_q)}


/*对外声明变量
*****************************************************************************/
extern L3C		l3c;				// L3C型结构初始化值
extern NPC		npc;
extern ADC		ad;
extern PIDREG3	pi_id;
extern PIDREG3	pi_iq;
extern PIDREG3	pi_vdc;
extern IPARK	ipark;
extern SVGENDQ	sv;
extern LPF		lpfUnpe;
extern LPF		lpfUdc;
//extern TLSVGENDQ  tlsv;
extern long		GlobalQ;

#if	CLOSE_CURRENT						// 0:电流开环(给定idref, iqref)	1:电流闭环模式
extern CLARKE	clarke;
extern PARK		park;
#endif

/*对外声明函数
*****************************************************************************/
extern void IDEL();					// 进入待机状态



#if	FLASH_OR_RAM==1
extern Uint16 ramfuncs_loadstart;
extern Uint16 ramfuncs_loadend;
extern Uint16 ramfuncs_runstart;
#endif


#endif
