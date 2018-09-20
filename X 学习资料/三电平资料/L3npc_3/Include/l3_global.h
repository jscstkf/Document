/******************************************************************************
// FILE:    l3_global.h
// TITLE:   NPC 3-Level converter controller global define file
// DESCRIPTION:
//          Global define.
//
// 2009-10-15	V1.0
******************************************************************************/

#ifndef __L3_GLOBAL_H__
#define __L3_GLOBAL_H__

// !引用后缀带_q的宏定义时应加上_IQ()

// 调试常数
/*###########################################################################*/
// 系统配置变量
//#define	GLOBAL_Q		20//24//	// 在IQmathLib.h中修改
#define	FLASH_OR_RAM		0	 		// 1:FLASH运行			0:RAM运行
#define	NORMAL_OR_TEST		1	 		// 1:正式程序			0:测试程序
#define TEST_PWM			0			// 1:PWM给定占空比		0:PWM为IO引脚	(NORMAL_OR_TEST=0有效)
#define PWM_ON				1//0//		// 1:使能PWM输出		0:禁止PWM输出	(NORMAL_OR_TEST=1有效)
#define	L3_NPC_FB			0//1//		// 1: 使用柔性平衡(Flexible Blance)控制		0:不使用
#define L3_MODE				L3_SV//L3_SP//
#define L3_SV_MODE			L3_SV_SELECTSR//L3_SV_SMALLVEC//L3_SV_NONPC//
#define L3_SP_MODE		   	L3_SP_SR//L3_SP_SA//L3_SP_HS//L3_SP_NONPC//L3_SP_HZ//
#define	UDCREF				_IQ(200)	// 直流母线电压给定值
#define	UNPREF				_IQ(150)//_IQ(40)//UDCREF>>1	// 直流中点电压给定值

#define	CHEAT_STATE			0			// 1:总认为flag.err=S_OK,就不执行CheckFault()了
#define	CHEAT_START			0//1//		// 1:总认为按下START键
#define	CHEAT_NO_IDCOC		0			// 1:不判直流侧过流
#define	DUAL_EV_MODE		1			// 1:使能双EV输出三电平PWM模式
#define	REC_ALLTHETIME		1//0//		// 1:循环记录 			0:单次记录
#define	SAMPLE_PWM			1//4		// 每SAMPLE_PWM次进行一次采样
#define ADUDC_DEAL			1			// 1:进行UDC,UNP通道的滤波算法处理

// 开环值
#define OPEN_REF_ID_q		0.6//0.4//0.2//0.7147//0.8//	// D轴电流环开环给定值
#define OPEN_REF_IQ_q		0			// Q轴电流环开环给定值

// 控制模式
#define CLOSE_CURRENT		0			// 1: 电流闭环模式		0:电流开环给定idref,iqref
#define	L3_SV				10			// 三电平SVPWM方法
	#define	L3_SV_NONPC		11			// SV无中点控制
	#define	L3_SV_SMALLVEC	12			// SV小矢量分配
	#define L3_SV_SELECTSR	13			// SV区间选择型

#define	L3_SP				20			// 三电平SPWM方法
	#define	L3_SP_NONPC		21			// SP无中点控制
	#define	L3_SP_HZ		22			// SP滞环变幅(Hysteresis Zoom)控制
	#define	L3_SP_HS		23			// SP滞环偏移(Hysteresis Shift)控制
	#define	L3_SP_SA		24			// SP载波幅移()控制
	#define	L3_SP_SR		25			// SP载波选择型控制
// NPC方式
#define NPC_HYS_MODE		NPC_HYS_DOUBLE//NPC_HYS_SINGLE//	// 滞环方式
#define NPC_HYS_SINGLE		1			// 单滞环
#define NPC_HYS_DOUBLE		2			// 双滞环

// NPC参数
#define	NPC_DEAD_q			1						// 滞环死区范围 V
#define	NPC_DEAD_IN_q		0.5						// 滞环内死区范围 V
#define	NPC_KB_q			0.1//0.5//0.05//		// 变幅系数
#define	NPC_KB_IN_q			(NPC_KB_q*5)			// 变幅系数(内死区)
#define	NPC_KS_q			0.1//0.05//0.2			// 偏移系数
#define	NPC_KS_IN_q			(NPC_KS_q/5)			// 偏移系数(内死区)
#define	NPC_LPF_FC			1//10//50//				// 柔性控制低通滤波器截止频率 Hz
#define	NPC_LPF_LBD_q		1						// 柔性控制前馈修正系数
#define	NPC_KH_q			0.1//0.2//0.05			// 幅移系数
#define	NPC_KH_IN_q			(NPC_KH_q/5)			// 
#define	NPC_KR_q			0.2//0.05//0.2			// 选择性移动系数
#define	NPC_KR_IN_q			(NPC_KR_q/5)			// 
#define	NPC_KM_SV			0.5//0.05//0.2			// SV选择性移动系数
#define	NPC_KM_SV_q			(NPC_KM_SV/5)			// 
#define NPC_Ksmall_SV		0.5	
#define	NPC_Ksmall_SV_q    (NPC_Ksmall_SV/5) 
#define SPM					1					
// 2阶数字低通滤波器参数
#if		NPC_LPF_FC==1
	#define	LPF_KIN0_q		0.00000982591682
	#define	LPF_KIN1_q		0.00001965183364
	#define	LPF_KIN2_q		LPF_KIN0_q
	#define	LPF_KOUT1_q		-1.99111429220165
	#define	LPF_KOUT2_q		0.99115359586894
#elif	NPC_LPF_FC==10
	#define	LPF_KIN0_q		0.00094469184384
	#define	LPF_KIN1_q		0.00188938368768
	#define	LPF_KIN2_q		LPF_KIN0_q
	#define	LPF_KOUT1_q		-1.91119706742607
	#define	LPF_KOUT2_q		0.91497583480143
#elif	NPC_LPF_FC==50
	#define	LPF_KIN0_q		0.02008336556421
	#define	LPF_KIN1_q		0.04016673112842
	#define	LPF_KIN2_q		LPF_KIN0_q
	#define	LPF_KOUT1_q		-1.56101807580072
	#define	LPF_KOUT2_q		0.64135153805756
#endif

// DSP系统
// 时钟频率
/*###########################################################################*/
#define	F_SYS				150//120//			// 系统时钟频率	MHz
#define	F_PWM				1000//2000//400//200//500//10000//	// 开关频率		Hz
#define	KM					20//50//10//100//32//		// 调制比
#define DTHETA				_IQ((float)1/KM)	// 步距角		不加float值总为0
#define	LED_DELAY			((int)F_PWM/4)		// 运行灯闪延时,设为1/2Hz

// 开关频率		F_PWM = F_SYS/[(2^TPS)*T1PR*2]
#if		F_SYS==150
 #if	F_PWM==10000	// 开关频率10kHz
	#define	T1_TPS			0			// EV T1CON 1分频
	#define T1PR_VAL		7500		// T1PR设置值
 #elif	F_PWM==2000		// 开关频率2kHz
 	#define	T1_TPS			0			// EV T1CON 1分频
	#define T1PR_VAL		37500
 #elif	F_PWM==1000		// 开关频率1kHz
 	#define	T1_TPS			1			// EV T1CON 2分频
	#define T1PR_VAL		37500
 #elif	F_PWM==500		// 开关频率500Hz
 	#define	T1_TPS			2			// EV T1CON 4分频
	#define T1PR_VAL		37500
 #elif	F_PWM==400		// 开关频率400Hz
 	#define	T1_TPS			2			// EV T1CON 4分频
	#define T1PR_VAL		46875
 #elif	F_PWM==200		// 开关频率200Hz
 	#define	T1_TPS			3			// EV T1CON 8分频
	#define T1PR_VAL		46875
 #endif
#elif	F_SYS==120
 #if	F_PWM==10000	// 开关频率10kHz
	#define	T1_TPS			0			// EV T1CON分频系数
	#define T1PR_VAL		6000		// T1PR设置值
 #elif	F_PWM==2000		// 开关频率2kHz
 	#define	T1_TPS			0			// EV T1CON分频系数
	#define T1PR_VAL		30000
 #elif	F_PWM==1000		// 开关频率1kHz
 	#define	T1_TPS			1			// EV T1CON分频系数
	#define T1PR_VAL		30000
 #elif	F_PWM==500		// 开关频率500Hz
 	#define	T1_TPS			2			// EV T1CON分频系数
	#define T1PR_VAL		30000
 #elif	F_PWM==400		// 开关频率400Hz
 	#define	T1_TPS			2			// EV T1CON分频系数
	#define T1PR_VAL		37500
 #elif	F_PWM==200		// 开关频率200Hz
 	#define	T1_TPS			3			// EV T1CON分频系数
	#define T1PR_VAL		37500
 #endif
#endif

// 死区时间		DBtime = T_HSPCLK*(DBT*(2^DBTPS))	(DBTPS<=5)
#define	DEADTIME		   	32//16//		//单位0.1us
#if		F_SYS==150
 #if	DEADTIME==32
	#define	VDBT			0x0f
	#define	VDBTPS			5
 #elif	DEADTIME==16
	#define	VDBT			0x0f
	#define	VDBTPS			4
 #endif
#elif	F_SYS==120
 #if	DEADTIME==32
	#define	VDBT			0x0c
	#define	VDBTPS			5
 #elif	DEADTIME==16
	#define	VDBT			0x0c
	#define	VDBTPS			4
 #endif
#endif

// 计算常量
/*###########################################################################*/
#define SQRT3				1.732050808	// sqrt(3)
#define SQRT_3				0.577350269	// 1/sqrt(3)
#define ONE_Q16				65535

#define CINI				5//10		// 计数器初值
#define CMAX				100			// 计数器最大值
#define CPDPINT				5			// PDPINT中断容许次数

#define	PI					3.1415926
#define	LOW_ACTIVE			1			// 1:低有效

// ADC相关常数
/*###########################################################################*/
// AD时间常数
#if		F_SYS==120
 #define CPU_CLOCK_SPEED	8.3333L		// for a 120MHz CPU clock speed
#elif	F_SYS==150
 #define CPU_CLOCK_SPEED	6.6667L		// for a 150MHz CPU clock speed
#endif
#define DELAY_US(A)			DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_CLOCK_SPEED) - 9.0L) / 5.0L)
// DSP28x_usDelay()	是DSP281x_usDelay.asm中定义好的汇编延时函数, 必须在RAM中运行才有确定的us延时
// DELAY_US(A)		是以us为单位的延时函数
// CPU_CLOCK_SPEED	是一个系统时钟周期内有多少ns

#define	WATCH_POINTS		200			// 观察临时变量数据区点数
#define AD_CH_NUM			6			// AD信号个数
// AD通道号
#define AD_CH_IA			0			// IA使用ADCINA0
#define AD_CH_IB			1
#define AD_CH_IDC			2
#define AD_CH_INP			3
#define AD_CH_UDC			4
#define AD_CH_UNP			5
#define AD_CH_REF25			6		   // REF2.5V使用ADCINA6
#define AD_CH_REF05			7		   // REF0.5V使用ADCINA7
//AD通道零偏		AD1 = AD-offset;
#define AD_OFF_IA			_IQ(-0.069)//0
#define AD_OFF_IB			_IQ(-0.056)//0
#define AD_OFF_IDC			0//_IQ(-0.08)
#define AD_OFF_INP			0//_IQ(-0.0)
#define AD_OFF_UDC		   _IQ(-0.0655)//	_IQ(-0.0643)//_IQ(-0.0650)//_IQ(-0.056)//0
#define AD_OFF_UNP			_IQ(-0.0170)//0
//AD通道系数K	( 采样板二次值V2 * K = 主电路一次值V1 )
// 电流传感器: 12A/24mA, Rs=200Ω, 即 V1/V2=(12/24m)/Rs=2.5, [-1.5,1.5]*2.5A-[0,3]V-[_IQ(-1.5),_IQ(1.5)]
#define AD_K_IA				_IQ(2.5)			// 交流以流向负载为正
#define AD_K_IB				AD_K_IA
#define AD_K_IDC			-AD_K_IA			// 直髟流向负载为正, 与硬件设计相反
#define AD_K_INP			-AD_K_IA
// 电压传感器: 1000/2500,R1=30kΩ,Rs=50,即 V1/V2=R1/(Rs*2.5)=240, [-1.5,1.5]*240-[0,3]V-[_IQ(-1.5),_IQ(1.5)]
#define AD_K_UDC			_IQ(235.3)//_IQ(240)//
#define AD_K_UNP			_IQ(242.6)//AD_K_UDC//

// AD校正值
#define AD_REF_HIGH			22159  //22072	   // (2.511/3*4095*2^4)^0x8000=()Q15signed	[^0x8000=-2^15]
#define AD_REF_LOW			-21805//-21827	   // (0.501/3*4095*2^4)^0x8000=()Q15signed

// 整流同步参数
#define DTHETA_DEFAULT_q	(1/400)		// 默认步距角度
#define STEP_DEFAULT		25000		// 默认2次CAP之涞淖钚〔绞?

// PI参数
// PI_U
#define PI_KP_U				500//50//512//256//150//			// 电压环PI比例系数		Q9
#define PI_KI_U		   		50//600//1500//10//0//				// 电压环PI积分系数		Q13
#define PI_KD_U				0									// 电压环PI微分系数		Q13
#define PI_MAX_U			1229//(30A)	 1638//					// 电压环PI限幅上限		Q11
#define PI_MIN_U			-PI_MAX_U							// 电压环PI限幅下限		Q11
#define PI_UI_MAX_U			PI_MAX_U//2047//1843//1024//		// 电压环PI积分项限幅上限	Q11
#define PI_UI_MIN_U			-PI_UI_MAX_U						// 电压环PI积分项限幅下限	Q11
// PI_I
#define PI_KP_I		   		1300//500//300//5000//				// 电流环PI比谍		Q9
#define PI_KI_I		   		200//50//30//100//400//				// 电流环PI积分系数		Q13
#define PI_KD_I				0									// 电流环PI微分系数		Q13
#define PI_MAX_I			2047//1843//1500//1770//			// 电流环PI限幅上限		Q11
#define PI_MIN_I			-PI_MAX_I							// 电流环PI限幅下限		Q11
#define PI_UI_MAX_I			1843//2047//1024//PI_MAX_I//		// 电流环PI积分项限幅上限	Q11
#define PI_UI_MIN_I			-PI_UI_MAX_I						// 电流环PI积分项限幅下限	Q11


// L3C系统
/*###########################################################################*/
// IO
#define IO_REG				(volatile unsigned int *)0x70E4   /* I/O port B data & dir reg */
#define IN_IDCOC			8			//PB.8
#define IN_START			9			//PB.9
#define IN_STOP				10			//PB.10
#define IN_RESET			11			//PB.11

#define OUT_IDCOC			12			//PB.12
#define OUT_START			13			//PB.13
#define OUT_STOP			14			//PB.14
#define OUT_RESET			15			//PB.15

#define IN_BIT_START		GpioDataRegs.GPBDAT.bit.GPIOB9
#define IN_BIT_STOP			GpioDataRegs.GPBDAT.bit.GPIOB10
#define IN_BIT_NpcOn		GpioDataRegs.GPBDAT.bit.GPIOB8
#define IN_BIT_NpcOff		GpioDataRegs.GPBDAT.bit.GPIOB11
#define IN_BIT_IDCOC		GpioDataRegs.GPBDAT.bit.GPIOB8
#define IN_BIT_RESET		GpioDataRegs.GPBDAT.bit.GPIOB11

#define LED_BIT_START		GpioDataRegs.GPBDAT.bit.GPIOB13
#define LED_BIT_STOP		GpioDataRegs.GPBDAT.bit.GPIOB14
#define LED_BIT_NpcOn		GpioDataRegs.GPBDAT.bit.GPIOB12
#define LED_BIT_NpcOff		GpioDataRegs.GPBDAT.bit.GPIOB15
#define LED_BIT_IDCOC		GpioDataRegs.GPBDAT.bit.GPIOB12
#define LED_BIT_RESET		GpioDataRegs.GPBDAT.bit.GPIOB15

#define LED_BIT_TWI_IDCOC	GpioDataRegs.GPBTOGGLE.bit.GPIOB12
#define LED_BIT_TWI_START	GpioDataRegs.GPBTOGGLE.bit.GPIOB13
#define LED_BIT_TWI_STOP	GpioDataRegs.GPBTOGGLE.bit.GPIOB14
#define LED_BIT_TWI_RESET	GpioDataRegs.GPBTOGGLE.bit.GPIOB15
#define LED_ON				0
#define LED_OFF				1


// KEY STATE
#define	KEY_ON			0						// 按下键
#define	KEY_OFF			1						// 未按键

// PWM ACTION STATE
#ifdef	LOW_ACTIVE
 #define FORCE_ACTIVE	0						// 设置强制有效
 #define FREE_UP_ACTIVE	1						// 设置上管有效
 #define FREE_LO_ACTIVE	2						// 设置下管有效
 #define FORCE_INACTIVE	3						// 设置强制无效
#else
 #define FORCE_ACTIVE	3						// 设置强制有效
 #define FREE_UP_ACTIVE	2						// 设置上管有效
 #define FREE_LO_ACTIVE	1						// 设置下管有效
 #define FORCE_INACTIVE	0						// 设置强制无效
#endif

// CAN STATE
#define	S_OK			0x0000					// 正常状态
#define SOK_HOLDON		0x0000					// 待机状态
#define SOK_CHARGING	0x3333					// 充电状态
#define	SOK_RUN			0xCCCC					// 运行状态

#define	S_ERROR			0xFFFF					// 故障状态
#define	SE_UACLV		0xC0C0					// 交流欠压
#define	SE_UACOV		0xA0A0					// 交流过压
#define	SE_IDCOC		0x9898					// 母线过流
#define	SE_UDCOV		0x8484					// 母线过压
#define	SE_PDPINT		0x8282					// IGBT故障
#define	SE_OT			0x8181					// IGBT过热
#define	S_ERROR_HALT	0xFFFF					// 故障状态后按停止 20091028

// 宏函数
/*###########################################################################*/
#define SETBIT(DMA,BITID) 		*DMA|=1<<BITID						// 将*DMA的第BITID位置1
#define CLRBIT(DMA,BITID) 		*DMA&=(0xffff-(1<<BITID))			// 将*DMA的第BITID位清0
#define	TESTBIT0(DMA,BITID)		((*DMA&(1<<BITID))==0)				// 测试*DMA的第BITID位为0?
#define	TESTBIT1(DMA,BITID)		((*DMA|(0xffff-(1<<BITID)))==0xffff)// 测试*DMA的贐ITID位为1?
//#define SETFLAG(FLAG,BITID) 	FLAG|=1<<BITID						// 将*DMA的第BITID位置1
//#define CLRFLAG(FLAG,BITID) 	FLAG&=(0xffff-(1<<BITID))			// 将*DMA的第BITID位清0
//#define	FLAGBIT0(FLAG,BITID)	((FLAG&(1<<BITID))==0)				// 测试FLAG的第BITID位0?
//#define	FLAGBIT1(FLAG,BITID)	((FLAG|(0xffff-(1<<BITID)))==0xffff)// 测试FLAG的第BITID位为1?
#define	NOP()					asm("	nop				")			// 空指令


// Include files
/*###########################################################################*/
#include "DSP281x_Device.h"

#include "dmc_ipark.h"
#include "dmc_pid_reg3.h"
#include "dmc_svgen_dq.h"

#if	CLOSE_CURRENT						// 0:电流开环(给定idref, iqref)	1:电流闭环模式
#include "dmc_clarke.h"
#include "dmc_iclarke.h"
#include "dmc_park.h"
#endif

#include "l3_adc.h"
#include "l3_ints.h"
#include "_lpf.h"
#include "TLsvgen_dq.h"
#include "l3_main.h"


#endif
