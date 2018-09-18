/*
//###########################################################################
//
// FILE:	F2812_XintfBoot.cmd
//
// TITLE:	Linker Command File For F2812 Device with boot from XINTF Zone 7
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  1.00| 11 Sep 2003 | L.H  | First Release
//###########################################################################
*/

/* ======================================================
// For Code Composer Studio V2.2 and later
// ---------------------------------------
// In addition to this memory linker command file, 
// add the header linker command file directly to the project. 
// The header linker command file is required to link the
// peripheral structures to the proper locations within 
// the memory map.
//
// The header linker files are found in <base>\DSP281x_Headers\cmd
//   
// For BIOS applications add:      DSP281x_Headers_nonBIOS.cmd
// For nonBIOS applications add:   DSP281x_Headers_nonBIOS.cmd    
========================================================= */

/* ======================================================
// For Code Composer Studio prior to V2.2
// --------------------------------------
// 1) Use one of the following -l statements to include the 
// header linker command file in the project. The header linker
// file is required to link the peripheral structures to the proper 
// locations within the memory map                                    */

/* Uncomment this line to include file only for non-BIOS applications */
/* -l DSP281x_Headers_nonBIOS.cmd */

/* Uncomment this line to include file only for BIOS applications */
/* -l DSP281x_Headers_BIOS.cmd */

/* 2) In your project add the path to <base>\DSP281x_headers\cmd to the
   library search path under project->build options, linker tab, 
   library search path (-i).
/*========================================================= */

/* Define the memory block start/length for the F2812  
   PAGE 0 will be used to organize program sections
   PAGE 1 will be used to organize data sections

   Notes: 
         Memory blocks on F2812 are uniform (ie same
         physical memory) in both PAGE 0 and PAGE 1.  
         That is the same memory region should not be
         defined for both PAGE 0 and PAGE 1.
         Doing so will result in corruption of program 
         and/or data. 
*/

MEMORY
{
PAGE 0:    /* Program Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE1 for data allocation */

   ZONE0       : origin = 0x002000, length = 0x002000     /* XINTF zone 0 */
   ZONE1       : origin = 0x004000, length = 0x002000     /* XINTF zone 1 */
   RAML0       : origin = 0x008000, length = 0x001000     /* on-chip RAM block L0 */
   ZONE2       : origin = 0x080000, length = 0x080000     /* XINTF zone 2 */
   ZONE6       : origin = 0x100000, length = 0x040000     /* XINTF zone 6 allocated for code */
   OTP         : origin = 0x3D7800, length = 0x000800     /* on-chip OTP */
   FLASHJ      : origin = 0x3D8000, length = 0x002000     /* on-chip FLASH */
   FLASHI      : origin = 0x3DA000, length = 0x002000     /* on-chip FLASH */
   FLASHH      : origin = 0x3DC000, length = 0x004000     /* on-chip FLASH */
   FLASHG      : origin = 0x3E0000, length = 0x004000     /* on-chip FLASH */
   FLASHF      : origin = 0x3E4000, length = 0x004000     /* on-chip FLASH */
   FLASHE      : origin = 0x3E8000, length = 0x004000     /* on-chip FLASH */
   FLASHD      : origin = 0x3EC000, length = 0x004000     /* on-chip FLASH */
   FLASHC      : origin = 0x3F0000, length = 0x004000     /* on-chip FLASH */
   FLASHA      : origin = 0x3F6000, length = 0x001F80     /* on-chip FLASH */
   CSM_RSVD    : origin = 0x3F7F80, length = 0x000076     /* Part of FLASHA.  Program with all 0x0000 when CSM is in use. */
   BEGIN       : origin = 0x3F7FF6, length = 0x000002     /* Part of FLASHA.  Used for "boot to Flash" bootloader mode. */
   CSM_PWL     : origin = 0x3F7FF8, length = 0x000008     /* Part of FLASHA.  CSM password locations in FLASHA */

   RAMH0       : origin = 0x3F8000, length = 0x002000     /* on-chip RAM block H0 */
   
/* ZONE7       : origin = 0x3FC000, length = 0x003FC0 */   /* XINTF zone 7 available if MP/MCn=1 */ 
   ROM         : origin = 0x3FF000, length = 0x000FC0     /* Boot ROM available if MP/MCn=0 */
   RESET       : origin = 0x3FFFC0, length = 0x000002     /* part of boot ROM (MP/MCn=0) or XINTF zone 7 (MP/MCn=1) */
   VECTORS     : origin = 0x3FFFC2, length = 0x00003E     /* part of boot ROM (MP/MCn=0) or XINTF zone 7 (MP/MCn=1) */

PAGE 1 :   /* Data Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE0 for program allocation */
           /* Registers remain on PAGE1                                                  */

   RAMM0       : origin = 0x000000, length = 0x000400     /* on-chip RAM block M0 */
   RAMM1       : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */
   RAML1       : origin = 0x009000, length = 0x001000     /* on-chip RAM block L1 */
   ZONE6       : origin = 0x104000, length = 0x004000     /* XINTF zone 6 allocated for data */
   FLASHB      : origin = 0x3F4000, length = 0x002000     /* on-chip FLASH */

}

/* Allocate sections to memory blocks.
   Note:
         codestart user defined section in DSP28_CodeStartBranch.asm used to redirect code 
                   execution when booting to flash
         ramfuncs  user defined section to store functions that will be copied from Flash into RAM
*/ 
 
SECTIONS
{
 
   /* Allocate program areas: */
   .cinit              : > ZONE6      PAGE = 0
   .pinit              : > ZONE6,     PAGE = 0
   .text               : > ZONE6      PAGE = 0
   codestart           : > BEGIN      PAGE = 0
   reset_vec           : > RESET      PAGE = 0
   ramfuncs            : LOAD = ZONE6, 
                         RUN = RAMH0, 
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         PAGE = 0

   csmpasswds          : > CSM_PWL     PAGE = 0
   ramdata             : > RAMM1       PAGE = 1

   /* Allocate uninitalized data sections: */
   .stack              : > RAMM0       PAGE = 1
   .ebss               : > ZONE6       PAGE = 1
   .esysmem            : > ZONE6       PAGE = 1

   /* Initalized sections go in Flash */
   .econst             : > ZONE6,      PAGE = 1
   .switch             : > ZONE6,      PAGE = 1      

   /* Allocate IQ math areas: */
   IQmath              : > ZONE6       PAGE = 0                  /* Math Code */
   IQmathTables        : > ROM         PAGE = 0, TYPE = NOLOAD   /* Math Tables In ROM */

   /* .reset is a standard section used by the compiler.  It contains the */ 
   /* the address of the start of _c_int00 for C Code.   /*
   /* When booting from XINTF this is a pointer to the c_init routine */
   /* For this example we will go through codestart first, in order to*/
   /* disable the watchdog before the init.  Thus this is DSECT       */
   .reset              : > RESET,      PAGE = 0, TYPE = DSECT
   vectors             : > VECTORS     PAGE = 0, TYPE = DSECT

}
