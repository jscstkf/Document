/******************************************************************************
// FILE:    l3_ints.c
// TITLE:   NPC 3-Level converter controller interrupt programs
// DESCRIPTION:
//          Interrupt Programs.
//
// 2009-10-15	V1.0
******************************************************************************/

#include "l3_global.h"

/*声明函数
*****************************************************************************/
void	l3_spwm();
void	l3_svpwm();
void	l3_expwm();
void 	FindMid5(_iq *pAD);				// 取出5元素数组中间值存放在数组首
void 	ADUdc_deal();					// 对Udc,Unp作滤波处理

/*全局一般变量
*****************************************************************************/
int16	iEvaT1ufC=0;					// 进Evb_PDPINT_isr中断计数器

int16	iUdcC=0;						// Udc处理计数器
_iq		qADUdc[10],qADUnp[10];			// Udc处理缓存区
_iq		qUdc,qUnp;						// Udc处理缓存变量

/*全局结构体变量
*****************************************************************************/

// 绘图临时变量
#if	FLASH_OR_RAM==0
int16	iWatchC = 0;
int16	iInvC = 0;
#pragma DATA_SECTION(Watch16, "WatchTemp");
int16 	Watch16[4][WATCH_POINTS] = {0};
#pragma DATA_SECTION(Watch32, "WatchTemp");
int32	Watch32[4][WATCH_POINTS] = {0};
#endif

/*函数
*****************************************************************************/
/*funName:		Eva_PDPINT_isr()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	PDPINTA中断	INT1.1
******************************************************************************/
interrupt void Eva_PDPINT_isr(void)
{
	l3c.iPdpintC--;

// 容许多次PDPINT中断
//	if(l3c.iPdpintC>0){
//		l3c.iPdpintC--;
//		EvaRegs.COMCONA.bit.FCOMPOE = 1;		// 在PDPINT中断中重新打开PWM. 此句要放在清EVAIFRA之后才能有效.20091105
//	 #if	DUAL_EV_MODE						// 1:使能双EV输出三电平PWM模式
//		EvbRegs.COMCONB.bit.FCOMPOE = 1;
//	 #endif
//	}
//	else
		IDEL();

	LED_BIT_STOP  = LED_ON;					// 点亮STOP
	l3c.iErrTwinkC = 0;						// 启动ERR灯闪烁计数器

// 中断退出处理
	EvaRegs.EVAIFRA.all = BIT0;				// 使用all而不用bit, 以免read-modify-write过程清除了其他中断标志
	PieCtrlRegs.PIEACK.bit.ACK1 = 1;
}

#if	DUAL_EV_MODE							// 1:使能双EV输出三电平PWM模式
/*funName:		Evb_PDPINT_isr()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	PDPINTB中断	INT1.2
******************************************************************************/
interrupt void Evb_PDPINT_isr(void)
{

	l3c.iPdpintC--;

// 容许多次PDPINT中断
//	if(l3c.iPdpintC>0){
//		l3c.iPdpintC--;
//		EvaRegs.COMCONA.bit.FCOMPOE = 1;	// 在PDPINT中断中重新打开PWM. 此句要放在清EVAIFRA之后才能有效.20091105
//		EvbRegs.COMCONB.bit.FCOMPOE = 1;
//	}
//	else
		IDEL();

// 中断退出处理
	EvbRegs.EVBIFRA.all = BIT0;				// 使用all而不用bit, 以免read-modify-write过程清除了其他中断标志
	PieCtrlRegs.PIEACK.bit.ACK1 = 1;
}
#endif

/*funName:		Eva_T1UF_isr()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	T1UF中断	INT2.6
******************************************************************************/
interrupt void Eva_T1UF_isr(void)
{
	iEvaT1ufC++;
	if(iEvaT1ufC>=LED_DELAY){
		iEvaT1ufC = 1;
		LED_BIT_TWI_START = 1;					// 闪烁START灯
	}
	
//	EvaRegs.EVAIMRA.bit.T1UFINT = 1;

// 角度
	ipark.Angle += DTHETA;//_IQ(0.01);//
	if(ipark.Angle>=_IQ(1))
		ipark.Angle -= _IQ(1);

// 读取AD值
//***************************************************************
	ad.read(&ad);
// 对Udc,Unp作滤波处理

#if	ADUDC_DEAL
	ADUdc_deal();
#else
	l3c.qAD[AD_CH_UDC] = _IQmpy(ad.qOut[AD_CH_UDC], AD_K_UDC);
	l3c.qAD[AD_CH_UNP] = _IQmpy(ad.qOut[AD_CH_UNP], AD_K_UNP);
#endif

//	l3c.qUnpe = l3c.qAD[AD_CH_UNP] - UNPREF;
	l3c.qUnpe = UNPREF - l3c.qAD[AD_CH_UNP];

// 柔性平衡(Flexible Balance)控制: 1在Vnp反馈环节加LPF
//***************************************************************
#if	L3_NPC_FB
	lpfUnpe.qIn[0] = l3c.qUnpe;
	lpfUnpe.calc(&lpfUnpe);
	l3c.qUnpe = lpfUnpe.qOut[0];
#endif

// 电流环计算
//***************************************************************
	l3c.qAD[AD_CH_IA]  = _IQmpy(ad.qOut[AD_CH_IA ], AD_K_IA);
	l3c.qAD[AD_CH_IB]  = _IQmpy(ad.qOut[AD_CH_IB ], AD_K_IB);
	l3c.qAD[AD_CH_IDC] = _IQmpy(ad.qOut[AD_CH_IDC], AD_K_IDC);
	l3c.qAD[AD_CH_INP] = _IQmpy(ad.qOut[AD_CH_INP], AD_K_INP);
	
#if	CLOSE_CURRENT							// 0:电流开环(给定idref, iqref)	1:电流闭环模式
// CLARKE
	clarke.As = l3c.qAD[AD_CH_IA];
	clarke.Bs = l3c.qAD[AD_CH_IB];
	clarke.calc(&clarke);
	
// PARK
	park.Alpha	= clarke.Alpha;
	park.Beta	= clarke.Beta;
	park.calc(&park);

// PI
	pi_id.Ref = _IQ(OPEN_REF_ID_q);
	pi_id.Fdb = park.Ds;
	pi_id.calc(&pi_id);
	
	pi_iq.Ref =  _IQ(OPEN_REF_IQ_q);
	pi_iq.Fdb = park.Qs;
	pi_iq.calc(&pi_iq);
#else										// CLOSE_CURRENT==0, 电流开环模式
	pi_id.Out = _IQ(OPEN_REF_ID_q);
	pi_iq.Out = _IQ(OPEN_REF_IQ_q);
#endif

// IPARK
	ipark.Ds = pi_id.Out;
	ipark.Qs = pi_iq.Out;
	ipark.calc(&ipark);

// ICLARK	
	//	a = alfa
	//	b = (-alfa + beta*sqrt(3)) / 2
	l3c.qVa = ipark.Alpha;
	l3c.qVb = (-ipark.Alpha + _IQmpy(ipark.Beta, _IQ(SQRT3)))>>1;
	l3c.qVc = -l3c.qVa-l3c.qVb;

// L3C PWM算法
//***************************************************************
#if		(L3_MODE==L3_SV)				// 三电平SVPWM方法
	l3_svpwm();
#elif	(L3_MODE==L3_SP)				// 三电平SPWM方法
	l3_spwm();
#endif	

	l3_expwm();							// 三电平PWM扩展

//★调试记录参数
//***************************************************************
#if	FLASH_OR_RAM==0							// 1:FLASH运行			0:RAM运行
	if(iInvC<(SAMPLE_PWM-1))//24)//			//每SAMPLE_PWM次进行一次采样
		iInvC++;
	else{
		iInvC = 0;
		if(iWatchC<WATCH_POINTS){
			Watch16[0][iWatchC] = EvaRegs.CMPR1;//
			Watch16[1][iWatchC] = EvbRegs.CMPR4;//
			Watch16[2][iWatchC] = EvbRegs.CMPR5;//EvaRegs.CMPR2;//
			Watch16[3][iWatchC] = EvbRegs.CMPR6;//EvaRegs.CMPR3;//
			Watch32[0][iWatchC] = tlsv.SR;//l3c.qTa;//tlsv.Ualfa;//tlsv.Ualfa;// ipark.Angle;//l3c.qAD[AD_CH_UDC];//lpfUdc.qIn[0];//ad.qOut[AD_CH_IA ];//ipark.Ds;//
			Watch32[1][iWatchC] = l3c.qTb;//tlsv.Ubeta;//tlsv.Ubeta;//l3c.qTa;//lpfUdc.qOut[0];//ad.qOut[AD_CH_IB ];//ipark.Qs;//
			Watch32[2][iWatchC] = tlsv.Ta;//tlsv.sec;//l3c.qAD[AD_CH_UNP];//l3c.qTb;//l3c.qAD[AD_CH_IA];//ipark.Alpha;//ad.qOut[AD_CH_UDC];//
			Watch32[3][iWatchC] = tlsv.Tb;//tlsv.Vaa;//npc.qKs;//l3c.qTc;//l3c.qAD[AD_CH_INP];//l3c.qAD[AD_CH_IB];//ipark.Beta;//ad.qOut[AD_CH_UNP];//

			iWatchC++;
		}
	#if	REC_ALLTHETIME						//1:循环记录 	0:单次记录
		else
			iWatchC = 0;
	#endif
	}
#endif
	EvbRegs.T3CNT = EvaRegs.T1CNT;			// 同步T1和T3
		
// 中断退出处理
//***************************************************************
	EvaRegs.EVAIFRA.all	= BIT9;				// 使用all而不用bit, 以免read-modify-write过程清除了其他中断标志
	PieCtrlRegs.PIEACK.bit.ACK2	= 1;
}


/*funName:		l3_spwm()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	三电平SPWM方法
* 参考: 蔡凯, 程善美, 李自成. 单DSP三电平控制策略[J]. 电工技术学报. 2009, 24(07): 120-124.
******************************************************************************/
void l3_spwm()
{
#if	L3_NPC_FB
	_iq	qtemp;
#endif

	l3c.qTa = l3c.qVa;
	l3c.qTb = l3c.qVb;
	l3c.qTc = l3c.qVc;
	
#if	L3_SP_MODE!=L3_SP_NONPC
	if(l3c.flag.bit.NpcOn==KEY_ON)				// 由按键置位或清除该bit.NpcOn
	{
	// SP滞环变幅(Hysteresis Zoom)控制	
	//***************************************************************
	#if		L3_SP_MODE==L3_SP_HZ
		if(l3c.qUnpe>_IQ(NPC_DEAD_q))			// 中点需上移, kb1<1,kb2=1
		{
			npc.qKb1 = _IQ(NPC_KB_q);
			npc.qKb2 = _IQ(1);
		}
		#if	NPC_HYS_MODE==NPC_HYS_DOUBLE		// 双滞环
		else if (l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
		{
			npc.qKb1 = _IQ(NPC_KB_IN_q);
			npc.qKb2 = _IQ(1);
		}
		#endif
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_q))	// 中点需下移, kb2<1,kb1=1
		{
			npc.qKb1 = _IQ(1);
			npc.qKb2 = _IQ(NPC_KB_q);
		}
		#if	NPC_HYS_MODE==NPC_HYS_DOUBLE
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_IN_q))
		{
			npc.qKb1 = _IQ(1);
			npc.qKb2 = _IQ(NPC_KB_IN_q);
		}
	 	#endif
	// SP滞环偏移(Hysteresis Shift)控制
	//***************************************************************
	#elif	L3_SP_MODE==L3_SP_HS
		if(l3c.qUnpe>_IQ(NPC_DEAD_q))			// 中点需上移, Ks取>0
			npc.qKs = _IQ(NPC_KS_q);
		#if	NPC_HYS_MODE==NPC_HYS_DOUBLE
		else if (l3c.qUnpe>_IQ(NPC_DEAD_IN_q))	// 双滞环
			npc.qKs = _IQ(NPC_KS_IN_q);
		#endif
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_q))	// 中点需下移, Ks取>0
			npc.qKs = -_IQ(NPC_KS_q);
	 	#if	NPC_HYS_MODE==NPC_HYS_DOUBLE
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_IN_q))
			npc.qKs = -_IQ(NPC_KS_IN_q);
		#endif
	// SP载波幅移控制
	//***************************************************************
	#elif	L3_SP_MODE==L3_SP_SA
		if(l3c.qUnpe>_IQ(NPC_DEAD_q))			// 中点需上移, Kh取>0
			npc.qKh = _IQ(NPC_KH_q);
		#if	NPC_HYS_MODE==NPC_HYS_DOUBLE
		else if (l3c.qUnpe>_IQ(NPC_DEAD_IN_q))	// 双滞环
			npc.qKh = _IQ(NPC_KH_IN_q);
		#endif
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_q))	// 中点需下移, Kh取>0
			npc.qKh = -_IQ(NPC_KH_q);
	 	#if	NPC_HYS_MODE==NPC_HYS_DOUBLE
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_IN_q))
			npc.qKh = -_IQ(NPC_KH_IN_q);
		#endif

	#elif	L3_SP_MODE==L3_SP_SR
		if(l3c.qUnpe>_IQ(NPC_DEAD_q))			// 中点需上移, Kr取>0
			npc.qKr = _IQ(NPC_KR_q);
		#if	NPC_HYS_MODE==NPC_HYS_DOUBLE
		else if (l3c.qUnpe>_IQ(NPC_DEAD_IN_q))	// 双滞环
			npc.qKr = _IQ(NPC_KR_IN_q);
		#endif
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_q))	// 中点需下移, Kr取>0
			npc.qKr = -_IQ(NPC_KR_q);
	 	#if	NPC_HYS_MODE==NPC_HYS_DOUBLE
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_IN_q))
			npc.qKr = -_IQ(NPC_KR_IN_q);
		#endif
	#endif

	// 柔性平衡(Flexible Balance)控制: 2加直流电压前馈环节
	//***************************************************************
	#if	L3_NPC_FB
		qtemp = UNPREF + _IQdiv(l3c.qUnpe,_IQ(NPC_LPF_LBD_q));
		qtemp = _IQdiv(UNPREF, qtemp);
		npc.qKb1 = _IQmpy(npc.qKb1, qtemp);
		qtemp = UNPREF - _IQdiv(l3c.qUnpe,_IQ(NPC_LPF_LBD_q));
		qtemp = _IQdiv(UNPREF, qtemp);
		npc.qKb2 = _IQmpy(npc.qKb2, qtemp);
	#endif
	}					// if(l3c.flag.bit.NpcOn==1)
	else
	{
		npc.qKb1 = _IQ(1);
		npc.qKb2 = _IQ(1);
		npc.qKs  = _IQ(0);
		npc.qKh  = _IQ(0);
		npc.qKr  = _IQ(0);
	}

	// 偏移处理
	l3c.qTa += npc.qKs;
	l3c.qTb += npc.qKs;
	l3c.qTc += npc.qKs;
// 变幅处理
	if(l3c.qTa>=0)
		l3c.qTa = _IQmpy(l3c.qTa, npc.qKb2);
	else
		l3c.qTa = _IQmpy(l3c.qTa, npc.qKb1);

	if(l3c.qTb>=0)
		l3c.qTb = _IQmpy(l3c.qTb, npc.qKb2);
	else
		l3c.qTb = _IQmpy(l3c.qTb, npc.qKb1);

	if(l3c.qTc>=0)
		l3c.qTc = _IQmpy(l3c.qTc, npc.qKb2);
	else
		l3c.qTc = _IQmpy(l3c.qTc, npc.qKb1);

// 幅移处理
	if(l3c.qTa>=(-npc.qKh))
		l3c.qTa = _IQdiv((l3c.qTa+npc.qKh), (_IQ(1)+npc.qKh));
	else
		l3c.qTa = _IQdiv((l3c.qTa+npc.qKh), (_IQ(1)-npc.qKh));
	if(l3c.qTb>=(-npc.qKh))
		l3c.qTb = _IQdiv((l3c.qTb+npc.qKh), (_IQ(1)+npc.qKh));
	else
		l3c.qTb = _IQdiv((l3c.qTb+npc.qKh), (_IQ(1)-npc.qKh));
	if(l3c.qTc>=(-npc.qKh))
		l3c.qTc = _IQdiv((l3c.qTc+npc.qKh), (_IQ(1)+npc.qKh));
	else
		l3c.qTc = _IQdiv((l3c.qTc+npc.qKh), (_IQ(1)-npc.qKh));
	
	#if	L3_SP_MODE==L3_SP_SA		/*幅移算法比较时应该用变换后的abc 10.5.25 */
		l3c.qVa = l3c.qTa;
		l3c.qVb = l3c.qTb;
		l3c.qVc = l3c.qTc;
	#endif	
	
	#if	L3_SP_MODE==L3_SP_SR
	//if((_IQ(0.75)<=ipark.Angle)||(ipark.Angle<_IQ(0.833333)))  // A 0~pi/6  ipark.Angle 3*pi/2 ~ 10*pi/6

	 #if SPM == 2
		if((_IQ(0.75)<ipark.Angle)&&(ipark.Angle<_IQ(0.833333)))
		{
			if(l3c.qTa>(_IQ(1)+l3c.qTb))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTb += _IQmpy(npc.qKr,(l3c.qTa-_IQ(1)-l3c.qTb));
				else 
					l3c.qTb += _IQmpy(npc.qKr,(_IQ(1)+l3c.qTb));
			}
		}
		else if((_IQ(0.833333)<=ipark.Angle)&&(ipark.Angle<_IQ(0.916667)))
		{
			if(l3c.qTc>(_IQ(1)+l3c.qTb))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTb += _IQmpy(npc.qKr,(l3c.qTc-_IQ(1)-l3c.qTb));
				else 
					l3c.qTb += _IQmpy(npc.qKr,(_IQ(1)+l3c.qTb));
			}
		}
		else if((_IQ(0.916667)<=ipark.Angle)&&(ipark.Angle<_IQ(1)))
		{
			if(l3c.qTa>(_IQ(1)+l3c.qTc))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTa += _IQmpy(npc.qKr,(_IQ(1)-l3c.qTa));
				else 
					l3c.qTa += _IQmpy(npc.qKr,(l3c.qTa-_IQ(1)-l3c.qTc));
			}
		}
		else if((_IQ(0)<=ipark.Angle)&&(ipark.Angle<_IQ(0.0833333)))
		{
			if(l3c.qTa>(_IQ(1)+l3c.qTb))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTa += _IQmpy(npc.qKr,(_IQ(1)-l3c.qTa));
				else 
					l3c.qTa += _IQmpy(npc.qKr,(l3c.qTa-_IQ(1)-l3c.qTb));
			}
		}
		else if((_IQ(0.0833333)<=ipark.Angle)&&(ipark.Angle<_IQ(0.1666667)))
		{
			if(l3c.qTb>(_IQ(1)+l3c.qTc))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTc += _IQmpy(npc.qKr,(l3c.qTb-_IQ(1)-l3c.qTc));
				else 
					l3c.qTc += _IQmpy(npc.qKr,(_IQ(1)+l3c.qTc));
			}
		}
		else if((_IQ(0.1666667)<=ipark.Angle)&&(ipark.Angle<_IQ(0.25)))
		{
			if(l3c.qTa>(_IQ(1)+l3c.qTc))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTc += _IQmpy(npc.qKr,(l3c.qTa-_IQ(1)-l3c.qTc));
				else 
					l3c.qTc += _IQmpy(npc.qKr,(_IQ(1)+l3c.qTc));
			}
		}
		else if((_IQ(0.25)<=ipark.Angle)&&(ipark.Angle<_IQ(0.333333)))
		{
			if(l3c.qTb>(_IQ(1)+l3c.qTa))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTb += _IQmpy(npc.qKr,(_IQ(1)-l3c.qTb));
				else 
					l3c.qTb += _IQmpy(npc.qKr,(l3c.qTb-_IQ(1)-l3c.qTa));
			}
		}
		else if((_IQ(0.333333)<=ipark.Angle)&&(ipark.Angle<_IQ(0.4166667)))
		{
			if(l3c.qTb>(_IQ(1)+l3c.qTc))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTb += _IQmpy(npc.qKr,(_IQ(1)-l3c.qTb));
				else 
					l3c.qTb += _IQmpy(npc.qKr,(l3c.qTb-_IQ(1)-l3c.qTc));
			}
		}
		else if((_IQ(0.4166667)<=ipark.Angle)&&(ipark.Angle<_IQ(0.5)))
		{
			if(l3c.qTc>(_IQ(1)+l3c.qTa))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTa += _IQmpy(npc.qKr,(l3c.qTc-_IQ(1)-l3c.qTa));
				else 
					l3c.qTa += _IQmpy(npc.qKr,(_IQ(1)+l3c.qTa));
			}
		}
		else if((_IQ(0.5)<=ipark.Angle)&&(ipark.Angle<_IQ(0.5833333)))
		{
			if(l3c.qTb>(_IQ(1)+l3c.qTa))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTa += _IQmpy(npc.qKr,(l3c.qTb-_IQ(1)-l3c.qTa));
				else 
					l3c.qTa += _IQmpy(npc.qKr,(_IQ(1)+l3c.qTa));
			}
		}
		else if((_IQ(0.5833333)<=ipark.Angle)&&(ipark.Angle<_IQ(0.6666667)))
		{
			if(l3c.qTc>(_IQ(1)+l3c.qTb))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTc += _IQmpy(npc.qKr,(_IQ(1)-l3c.qTc));
				else 
					l3c.qTc += _IQmpy(npc.qKr,(l3c.qTc-_IQ(1)-l3c.qTb));
			}
		}
		else 
		{
			if(l3c.qTa>(_IQ(1)+l3c.qTa))
			{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTc += _IQmpy(npc.qKr,(_IQ(1)-l3c.qTc));
				else 
					l3c.qTc += _IQmpy(npc.qKr,(l3c.qTc-_IQ(1)-l3c.qTa));
			}
		}

	#else
	//	if((_IQ(0.75)<=ipark.Angle)||(ipark.Angle<_IQ(0.833333)))
		
		if((_IQ(0.75)<ipark.Angle)&&(ipark.Angle<_IQ(0.833333)))
		{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTb += _IQmpy(npc.qKr,(-l3c.qTb));
				else 
					l3c.qTb += _IQmpy(npc.qKr,(l3c.qTb+_IQ(1)-l3c.qTc));
		}
		
		else if((_IQ(0.833333)<=ipark.Angle)&&(ipark.Angle<_IQ(0.916667)))
		{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTb += _IQmpy(npc.qKr,(-l3c.qTb));
				else 
					l3c.qTb += _IQmpy(npc.qKr,(l3c.qTb+_IQ(1)-l3c.qTa));
		}
		else if((_IQ(0.916667)<=ipark.Angle)&&(ipark.Angle<_IQ(1)))
		{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTa += _IQmpy(npc.qKr,(l3c.qTb+_IQ(1)-l3c.qTa));
				else 
					l3c.qTa += _IQmpy(npc.qKr,l3c.qTa);
		}	
		else if((_IQ(0)<=ipark.Angle)&&(ipark.Angle<_IQ(0.0833333)))
		{		
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTa += _IQmpy(npc.qKr,(l3c.qTc+_IQ(1)-l3c.qTa));
				else 
					l3c.qTa += _IQmpy(npc.qKr,l3c.qTa);
		
		}
		else if((_IQ(0.0833333)<=ipark.Angle)&&(ipark.Angle<_IQ(0.1666667)))
		{
		
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTc += _IQmpy(npc.qKr,(-l3c.qTc));
				else 
					l3c.qTc += _IQmpy(npc.qKr,(l3c.qTc+_IQ(1)-l3c.qTa));
		
		}
		else if((_IQ(0.1666667)<=ipark.Angle)&&(ipark.Angle<_IQ(0.25)))
		{
		
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTc += _IQmpy(npc.qKr,(-l3c.qTc));
				else 
					l3c.qTc += _IQmpy(npc.qKr,(l3c.qTc+_IQ(1)-l3c.qTb));
			
		}
		else if((_IQ(0.25)<=ipark.Angle)&&(ipark.Angle<_IQ(0.333333)))
		{
			
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTb += _IQmpy(npc.qKr,(l3c.qTc+_IQ(1)-l3c.qTb));
				else 
					l3c.qTb += _IQmpy(npc.qKr,l3c.qTb);
			
		}
		else if((_IQ(0.333333)<=ipark.Angle)&&(ipark.Angle<_IQ(0.4166667)))
		{
		
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTb += _IQmpy(npc.qKr,(l3c.qTa+_IQ(1)-l3c.qTb));
				else 
					l3c.qTb += _IQmpy(npc.qKr,l3c.qTb);
			
		}
		else if((_IQ(0.4166667)<=ipark.Angle)&&(ipark.Angle<_IQ(0.5)))
		{
		
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTa += _IQmpy(npc.qKr,(-l3c.qTa));
				else 
					l3c.qTa += _IQmpy(npc.qKr,(l3c.qTa+_IQ(1)-l3c.qTb));
		
		}
		else if((_IQ(0.5)<=ipark.Angle)&&(ipark.Angle<_IQ(0.5833333)))
		{
			
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTa += _IQmpy(npc.qKr,(-l3c.qTa));
				else 
					l3c.qTa += _IQmpy(npc.qKr,(l3c.qTa+_IQ(1)-l3c.qTc));
			
		}
		else if((_IQ(0.5833333)<=ipark.Angle)&&(ipark.Angle<_IQ(0.6666667)))
		{
			
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTc += _IQmpy(npc.qKr,(l3c.qTa+_IQ(1)-l3c.qTc));
				else 
					l3c.qTc += _IQmpy(npc.qKr,l3c.qTc);
			
		}

		else
		{
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))
					l3c.qTc += _IQmpy(npc.qKr,(l3c.qTb+_IQ(1)-l3c.qTc));
				else 
					l3c.qTc += _IQmpy(npc.qKr,l3c.qTc);
		
		}	
	#endif	
					
	#endif
#endif	//L3_SP_MODE!=L3_SP_NONPC
		
	// 限幅
    if(l3c.qTa>_IQ(1))
		l3c.qTa = _IQ(1);
	else if(l3c.qTa<_IQ(-1))
		l3c.qTa = _IQ(-1);
    if(l3c.qTb>_IQ(1))
		l3c.qTb = _IQ(1);
	else if(l3c.qTb<_IQ(-1))
		l3c.qTb = _IQ(-1);
    if(l3c.qTc>_IQ(1))
		l3c.qTc = _IQ(1);
	else if(l3c.qTc<_IQ(-1))
		l3c.qTc = _IQ(-1);

}

/*funName:		l3_svpwm()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	三电平SVPWM方法
******************************************************************************/
void l3_svpwm()
{

//	tlsv.Ualfa = _IQmpy(ipark.Alpha,_IQ(0.866025404));	// SV用(2/sqrt(3))来标幺
//	tlsv.Ubeta = _IQmpy(ipark.Beta, _IQ(0.866025404));
//	tlsv.calc(&tlsv);

	tlsv.Ualfa = ipark.Alpha;	// SV用(2/sqrt(3))来标幺
	tlsv.Ubeta = ipark.Beta;
	
	tlsv.calc(&tlsv);
	
	l3c.qTa = tlsv.Ta;
	l3c.qTb = tlsv.Tb;
	l3c.qTc = tlsv.Tc;

}


/*funName:		l3_expwm()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	三电平PWM扩展
*	L3C PWM扩展 以A相为例
*	引脚[P1,P2,P7,P8]-->开关管[T1,T3,T2,T4]
*	ta>=0 : CMPR1 = Ta(+);	CMPR4 = T1PR;
*	ta<0  : CMPR1 = 0;		CMPR4 = Ta(-);
******************************************************************************/
void l3_expwm()
{	
	Uint16	uCMPR1,uCMPR2,uCMPR3,uCMPR4,uCMPR5,uCMPR6;

	// 用Vx来判区间，CMPR值取自Tx
	// SP中Tx=Vx, SV中有不同
#if L3_MODE==L3_SP
	if(l3c.qVa>=_IQ(0)){
		uCMPR1 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTa))>>16;
		uCMPR4 = T1PR_VAL;
	}
	else{
		uCMPR1 = 0;
		uCMPR4 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTa+_IQ(1)))>>16;
	}	
	if(l3c.qVb>=_IQ(0)){
		uCMPR2 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTb))>>16;
		uCMPR5 = T1PR_VAL;
	}
	else{
		uCMPR2 = 0;
		uCMPR5 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTb+_IQ(1)))>>16;
	}	
	if(l3c.qVc>=_IQ(0)){
		uCMPR3 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTc))>>16;
		uCMPR6 = T1PR_VAL;
	}
	else{
		uCMPR3 = 0;
		uCMPR6 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTc+_IQ(1)))>>16;
	}
#elif L3_MODE==L3_SV		// SV时不用+IQ(1)处理
	if(l3c.qVa<=_IQ(0)){
		uCMPR1 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTa))>>16;
		uCMPR4 = T1PR_VAL;
	}
	else{
		uCMPR1 = 0;
		uCMPR4 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTa))>>16;
	}	
	if(l3c.qVb<=_IQ(0)){
		uCMPR2 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTb))>>16;
		uCMPR5 = T1PR_VAL;
	}
	else{
		uCMPR2 = 0;
		uCMPR5 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTb))>>16;
	}	
	if(l3c.qVc<=_IQ(0)){
		uCMPR3 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTc))>>16;
		uCMPR6 = T1PR_VAL;
	}
	else{
		uCMPR3 = 0;
		uCMPR6 = ((long)T1PR_VAL * _IQtoIQ16(l3c.qTc))>>16;
	}
#endif

	// 注: TI中的CMPR与有效占空比成反相关关系, 故取CMPRx=(T1PR-Tx)
	EvaRegs.CMPR1 = T1PR_VAL-uCMPR1;
	EvaRegs.CMPR2 = T1PR_VAL-uCMPR2;
	EvaRegs.CMPR3 = T1PR_VAL-uCMPR3;
	EvbRegs.CMPR4 = T1PR_VAL-uCMPR4;
	EvbRegs.CMPR5 = T1PR_VAL-uCMPR5;
	EvbRegs.CMPR6 = T1PR_VAL-uCMPR6;
}

/*funName:		ADUdc_deal()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	对Udc,Unp作滤波处理
******************************************************************************/
void ADUdc_deal()
{
// ① 4个数去大去小取平均
	int16	i;
	_iq		qss;
	
	qADUdc[iUdcC] = ad.qOut[AD_CH_UDC];
	qADUnp[iUdcC] = ad.qOut[AD_CH_UNP];
	iUdcC++;
	if(iUdcC>3)
	{
		iUdcC = 0;
		for(i=0;i<3;i++){				//将4个数的最小数移到末尾
		// Udc	
			if(qADUdc[i]<qADUdc[i+1]){
				qss=qADUdc[i];
				qADUdc[i]=qADUdc[i+1];
				qADUdc[i+1]=qss;
			}
//		// Unp	
			if(qADUnp[i]<qADUnp[i+1]){
				qss=qADUnp[i];
				qADUnp[i]=qADUnp[i+1];
				qADUnp[i+1]=qss;
			}
		}
		for(i=2;i>0;i--){				//将3个数的最大数移到首位
		// Udc	
			if(qADUdc[i]>qADUdc[i-1]){
				qss=qADUdc[i];
				qADUdc[i]=qADUdc[i-1];
				qADUdc[i-1]=qss;
			}
//		// Unp	
			if(qADUnp[i]>qADUnp[i-1]){
				qss=qADUnp[i];
				qADUnp[i]=qADUnp[i-1];
				qADUnp[i-1]=qss;
			}
		}
		qUdc = (qADUdc[1]+qADUdc[2])>>1;//中间两个数取平均		
		qUnp = (qADUnp[1]+qADUnp[2])>>1;//中间两个数取平均		
	}

// 或② 低通滤波处理
//	lpfUdc.qIn[0] = ad.qOut[AD_CH_UDC];
//	lpfUdc.calc(&lpfUdc);
//	qUdc = lpfUdc.qOut[0];

//	lpfUnp.qIn[0] = ad.qOut[AD_CH_UNP];
//	lpfUnp.calc(&lpfUdc);
//	qUnp = lpfUnp.qOut[0];

// 或③ 不处理
//	qUdc = ad.qOut[AD_CH_UDC];
//	qUnp = ad.qOut[AD_CH_UNP];
	
	// 通道系数
//	l3c.qAD[AD_CH_UDC] = _IQmpy(qUdc, _IQ(1));			// 调试用，乘1看数字量
	l3c.qAD[AD_CH_UDC] = _IQmpy(qUdc, AD_K_UDC);
	l3c.qAD[AD_CH_UNP] = _IQmpy(qUnp, AD_K_UNP);
}

/*funName:		FindMid5()
* Inputs: 		5元素数组首地址
* Outputs:
* Description: 	取出5元素数组中间值存放在数组
******************************************************************************/
void FindMid5(_iq *pAD)
{
	int i;
	_iq qss;
	for(i=0;i<4;i++){				//将最小数移到末尾
		if(*(pAD+i)<*(pAD+i+1)){
			qss=*(pAD+i);
			*(pAD+i)=*(pAD+i+1);
			*(pAD+i+1)=qss;
		}
	}
	for(i=0;i<3;i++){				//将次最小数移到次末尾
		if(*(pAD+i)<*(pAD+i+1)){
			qss=*(pAD+i);
			*(pAD+i)=*(pAD+i+1);
			*(pAD+i+1)=qss;
		}
	}
	//取出余下中最小数为中间数,最后存放在数组首
	if(*(pAD)>*(pAD+1))
		*(pAD)=*(pAD+1);
	if(*(pAD)>*(pAD+2))
		*(pAD)=*(pAD+2);
}

//===========================================================================
// Function format demo
//===========================================================================
/*funName:		functionxx()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	函数的主要功能
******************************************************************************/
/*
void functionxx()
{
}
*/
