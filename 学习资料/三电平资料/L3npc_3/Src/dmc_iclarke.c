/*=====================================================================================
 File name:        ICLARKE.C  (IQ version)                  
                    
 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  IClarke Transformation based on the original CLARKE.c by TI.
	ICLARK
		[         1,         0]
		[      -1/2, 1/2*sqrt3]

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
 03-17-2010 CK Revised based on the original CLARKE.c by TI.
-------------------------------------------------------------------------------------*/


#include "IQmathLib.h"         // Include header for IQmath library
// Don't forget to set a proper GLOBAL_Q in "IQmathLib.h" file
#include "dmc_type.h"
#include "dmc_iclarke.h"

void iclarke_calc(ICLARKE *v)
{	

   v->As = v->Alpha;

   v->Bs = (-v->Alpha + _IQmpy(v->Beta,_IQ(1.73205080756888)))>>1; // sqrt(3) = 1.73205080756888
 
}


