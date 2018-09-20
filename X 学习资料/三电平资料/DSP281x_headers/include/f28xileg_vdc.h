/* ==================================================================================
File name:       F28XILEG_VDC.H
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: This header file contains source for a the F28X Two leg current measurement 
and DC-bus measurement driver.

=====================================================================================
History:
-------------------------------------------------------------------------------------
 05-15-2002	Release	Rev 1.0                                      
----------------------------------------------------------------------------------*/

#ifndef __F28XILEG_VDC_H__
#define __F28XILEG_VDC_H__

#include "f28xbmsk.h"

/*-----------------------------------------------------------------------------
Define the structure of the ILEG2DCBUSMEAS Object
-----------------------------------------------------------------------------*/
typedef struct { int Imeas_a_gain;    /* Parameter: gain for Ia (Q13) */
                 int Imeas_a_offset;  /* Parameter: offset for Ia (Q15) */
                 int Imeas_a;         /* Output: measured Ia (Q15) */
                 int Imeas_b_gain;    /* Parameter: gain for Ib (Q13) */
                 int Imeas_b_offset;  /* Parameter: offset for Ib (Q15) */
                 int Imeas_b;         /* Output: measured Ib (Q15) */
                 int Vdc_meas_gain;   /* Parameter: gain for Vdc (Q13) */
                 int Vdc_meas_offset; /* Parameter: offset for Vdc (Q15) */
                 int Vdc_meas;        /* Output: measured Vdc (Q15) */
				 int Imeas_c;		  /* Output: computed Ic (Q15) */	
                 int Ch_sel;          /* Parameter: ADC channel selection */
                 void (*init)();      /* Pointer to the init function */
                 void (*read)();      /* Pointer to the read function */
               } ILEG2DCBUSMEAS;

/*-----------------------------------------------------------------------------
 Note 1 : It is necessary to call the init function to change the ADC 
            register settings, for the change in the channel setting for 
            Ch_sel setting changes to take effect.
            The read function will not detect or act upon this change.
-----------------------------------------------------------------------------*/
/* Default Initializer for the ILEG2DCBUSMEAS Object */

#define F28X_ILEG2_DCBUS_MEAS_DEFAULTS { 0x1FFF,0x0000,0x0000,             \
                                          0x1FFF,0x0000,0x0000,             \
                                          0x1FFF,0x0000,0x0000,             \
                                          0x0000,0x0710,                    \
                                          (void (*)(long))F28X_ileg2_dcbus_drv_init, \
                                          (void (*)(long))F28X_ileg2_dcbus_drv_read  \
                                         }

#define ILEG2DCBUSMEAS_DEFAULTS F28X_ILEG2_DCBUS_MEAS_DEFAULTS 

#define ADCTRL1_INIT_STATE  ADC_SUS_MODE0 + ADC_ACQ_PS_2 + \
                            ADC_CPS_1 + ADC_SEQ_CASC

#define ADCTRL2_INIT_STATE  ADC_EVA_SOC_SEQ1 

/*------------------------------------------------------------------------------
 Prototypes for the functions in F28XILEG_VDC.C
------------------------------------------------------------------------------*/
void F28X_ileg2_dcbus_drv_init(ILEG2DCBUSMEAS *);
void F28X_ileg2_dcbus_drv_read(ILEG2DCBUSMEAS *);

#endif /* __F28XILEG_VDC_H__ */
