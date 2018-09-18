/******************************************************************************
// FILE:    l3_calc.c
// TITLE:   NPC 3-Level converter controller calculation programs
// DESCRIPTION:
//          Calculation Programs.
//
// 2009-11-01	V1.0
******************************************************************************/

#include "l3_global.h"

/*声明函数
*****************************************************************************/

/*全局一般变量
*****************************************************************************/
int16	iPdpint=0;					// 进PDPINT中断计数器



/*函数
*****************************************************************************/
/*funName:		Eva_T1UF_isr()
* Inputs: 		NONE
* Outputs: 		NONE
* Description: 	EVA T1下溢中断服务程序
******************************************************************************/
void Eva_T1UF_isr(void)
{
	EvaRegs.EVAIMRA.bit.T1UFINT = 1;
    EvaRegs.EVAIFRA.bit.T1UFINT = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}

