/*
 * (C) Copyright 2000, 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

//#include "common.h"
#include "ppc_defs.h"
//#include "ppc_asm.tmpl"

#define TICS_AT_67MHz  16666667
#define TICS_AT_133MHz 33333333
#define TICS_PER_1MHz  250000

/* file "dtime.s"  (For Metware High C/C++ Compiler/Assembler)
 *  Assembly language routine to convert 64-bit PowerPC TB facility to 
 *  Double-precision, floating-point number.  (Plus additional routines for
 *  testing.)   CJC 981216
 *	Register usage:
 *		r3 = FPU	(upper 32 bits of floating point value)
 *		r4 = FPL	(lower 32 bits of floating point value)
 *		r5 = TBU(time base upper - read from spr or loaded for test)
 *		r6 = TBL(time base lower - read from spr or loaded for test)
 *		r7 = leading zeroes in a register or shift count of +/-(zeroes - 11)
 *		r8 = accumulator for final EXPonent value of DPFP number
 *		r9 = shift count of 32 - n where n = +/-(zeroes -11)
 *		r10 = constant register of 11
 *		r11 = link register storage
 */
/* Following include file equates r7 to 7, etc. to make reading easier */

asm double dtime(void);
asm double conversion(void);
double ticks_per_sec( unsigned int freq_MHz );





double ticks_per_sec( unsigned int freq_MHz )
{
  switch (freq_MHz)
    {
    case 66:
    case 67:
      return (double) TICS_AT_67MHz;
    case 133:
      return (double) TICS_AT_133MHz;
    default:
      return (double) freq_MHz*TICS_PER_1MHz;
    }
}
  


asm double conversion(void)
{
	cntlzw	r7,r5		/*Find leading zeroes in TBU. Preserve in r7.*/
	addi	r9,r0,32	/*Will need a 32 in several places. Create one in r9.*/
	addi	r10,r0,11	/*Create a constant in r10 = 11. */
	subf.	r8,r7,r9	/*r8 will hold EXP.  Currently (32 - leading zeroes)*/
	beq+	tbu_is_zero	/*TBU never got incremented? (Zeroes=32?) (Most likely)*/
	subf.	r7,r10,r7	/*No. Is TB more than 2^^52?  (Zeroes<11?) r7 = (Z-11)*/
	add	    r8,r8,r9	/*Final exponent will be (64 - 1 - leading zeroes).*/
	bge+	tbu_lt_8yrs	/*If TB>2^^52, shift TBU bits right. (Not likely) */

tbu_gt_8yrs: 		    /*for Z<11: fpu = tbu>>n=(11-Z);*/
				        /*fpl = tbu<<n=(32-(11- Z))|tbl>>n=(11-Z);*/
	neg	    r7,r7		/*rlwnm shift count of (11-Z) = -(Z-11) = n = r7.*/
	subf	r9,r7,r9	/*rlwnm shift count of 32-n = 32 - (11-Z) = r9.*/
	rlwnm	r3,r5,r9,12,31	/*Shift TBU right n = (11 - Z). Mask off [0:11].*/
	rlwnm	r4,r5,r9,0,10	/*Shift remaining TBU bits left n = 32-(11-Z)*/
	rlwnm	r6,r6,r9,0,31	/*Shift TBL right n = (11 - Z)*/
	or	    r4,r6,r4	/*Or rest of TBU shifted left with TBL shifted right.*/
	b	    form_exponent	/*Go bias the exponent and or into FPU.*/
	
tbu_lt_8yrs:		/*for Z>=11: fpu=tbu<<n=(Z-11)|tbl>>n=(32-(Z-11));*/
				    /*fpl=tbl<<n=(Z- 11);*/
	subf	r9,r7,r9 	/*Form a shift count of 32 - (11-Z) = r9.*/
	rlwnm	r3,r5,r7,12,31	/*Shift TBU left n = (Z-11). Mask off [0:11].*/
	srw	r5,r6,r9 	/*Shift TBL bits right n = 32-(Z-11).*/
	or	r3,r3,r5 	/*Or TBU shifted left with TBL shifted right.*/
	rlwnm	r6,r6,r7,0,31 	/*Shift remainder of TBL left n = (Z-11).*/
	xor	r4,r6,r5 	/*XORing with the same value shifted right is like ANDing*/
	b	form_exponent 	/*fpl with a mask of all zeroes in bits [32-(Z-11):31].*/
			
tbu_is_zero:		    /*Z= 32*/
	cntlzw	r7,r6		/*Find leading zeroes in TBL.*/
	subf.	r8,r7,r9	/*EXP = (32 - leading zeroes).*/
	beq-	tbl_is_zero	/*Entire TBL count exactly zero?  (Not likely)*/
	subf.	r7,r10,r7	/*No. Is TB less than 2^^20?  (zeroes < 11?)*/
	bge-	tbl_lt_63ms	/*If not, will have to shift bits right. (Most likely)*/

tbl_gt_63ms:		/*for z<11: fpu = tbl>>n=(11-z); fpl = tbl<<n=(32-(11-z));*/
	neg	r7,r7		/*rlwnm shift count of (11-Z) = -(Z-11) = n = r7.*/
	subf	r9,r7,r9	/*rlwnm shift count of 32-n = 32 - (11-Z) = r9.*/
	rlwnm	r3,r6,r9,12,31	/*Shift TBL right n = (11 - z). Mask off [0:11].*/
	rlwnm	r4,r6,r9,0,10	/*Shift remaining TBL bits left n = 32 - (11 - Z).*/
	b	form_exponent

tbl_lt_63ms:		/*for z>=11: fpu = tbl<<(z-11); fpl = 0;*/
	rlwnm	r3,r6,r7,12,31	/*Shift TBL left n = (Z-11). Mask off bits 0-11.*/
	xor	r4,r4,r4	/*fpl = 0.*/
	b	form_exponent

tbl_is_zero:		/*for Z=32 && z=32: fpu = fpl = 0;*/
	xor	r3,r3,r3	/*Unlikely result that TB was zero. Prepare to*/
	xor	r4,r4,r4	/*return all zeroes for the floating point value.*/
	b compute_seconds 
	
form_exponent:
	addi	r8,r8,1022 	/*Add DP bias (1023) -1 to the exponent*/
	rlwinm r8,r8,20,1,12	/*Biased DP EXP will be (63-(leading zeroes in TB)+1023).*/
	or	r3,r3,r8

compute_seconds:
	//lis	r5, Local_storage@h
//	ori	r5, r5, Local_storage@l
//	stw	r3, 0(r5)
//	stw	r4, 4(r5)
//	lfd	f2, 0(r5)	/*Load back in as 64bit float */
//	fdiv	f1,f2,f1	/*Divide by bus clock ticks per second  */
//let the compiler do the division
//	blr			/*Return time in seconds as double in fp1  */
}





// Routine reads the TBU and TBL. Returns seconds as double.
//For CodeWarrior:*/


asm double dtime(void)
{
read_TB:
	mfspr	r5,269		//Get TBU.
	mfspr	r6,268		//Get TBL.
	mfspr	r7,269		//Get TBU again.
	subf.	r7,r5,r7	//Did it increment between reading TBU and TBL?
	bgt-	read_TB		//If so, read them again.  (Not likely)
	mflr	r11		//Save the return address.
	bl 	conversion	//Convert TBU and TBL into FPU and FPL 
	mtlr	r11
	blr
}









	













