/* =================================================================================
File name:       TLSVGEN_DQ.H  (IQ version)                    
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Header file containing constants, data type definitions, and 
function prototypes for the TLSVGEN_DQ.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 05-15-2002		Release	Rev 1.0                                                   
------------------------------------------------------------------------------*/

#ifndef __L3_TLSVGEN_DQ_H__
#define __L3_TLSVGEN_DQ_H__


typedef struct 	{ _iq  Ualfa; 			/* Input: reference alfa-axis phase voltage   */
				  _iq  Ubeta;			/* Input: reference beta-axis phase voltage   */
				  _iq  Ta;				/* Output: reference phase-a switching function  */			
				  _iq  Tb;				/* Output: reference phase-b switching function   */
				  _iq  Tc;				/* Output: reference phase-c switching function   */
				  _iq  sec;
				  _iq  Vaa;
				  _iq  Vbb;
				  _iq  SR;
				   void   (*calc)();	    /* Pointer to calculation function */ 
				} TLSVGENDQ;
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				
typedef TLSVGENDQ *TLSVGENDQ_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the SVGENDQ object.
-----------------------------------------------------------------------------*/                     
#define TLSVGENDQ_DEFAULTS { 0,0,0,0,0,0,0,0,0, \
                       (void (*)(long))TLsvgendq_calc }

extern TLSVGENDQ tlsv;

/*------------------------------------------------------------------------------
Prototypes for the functions in SVGEN_DQ.C
------------------------------------------------------------------------------*/
void TLsvgendq_calc(TLSVGENDQ_handle);


#endif
