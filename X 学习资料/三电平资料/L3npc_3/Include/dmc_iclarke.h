/* =================================================================================
File name:       ICLARKE.H  (IQ version)                  
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Header file containing constants, data type definitions, and 
function prototypes for the CLARKE.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20                                                   
 03-17-2010 CK Revised based on the original CLARKE.c by TI.
-------------------------------------------------------------------------------------*/

#ifndef __ICLARKE_H__
#define __ICLARKE_H__

typedef struct {  _iq  Alpha;		// Output: stationary d-axis stator variable 
				  _iq  Beta;		// Output: stationary q-axis stator variable
				  _iq  As;  		// Input: phase-a stator variable
				  _iq  Bs;			// Input: phase-b stator variable 
		 	 	  void  (*calc)();	// Pointer to calculation function
				 } ICLARKE;	            

typedef ICLARKE *ICLARKE_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the CLARKE object.
-----------------------------------------------------------------------------*/                     
#define ICLARKE_DEFAULTS{ 0, \
                          0, \
                          0, \
                          0, \
              			  (void (*)(Uint32))iclarke_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in ICLARKE.C
------------------------------------------------------------------------------*/
void iclarke_calc(ICLARKE_handle);

#endif // __ICLARKE_H__
