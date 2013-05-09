
.extern InterruptHandler	# void InterruptHandler(unsigned long exceptNumber)
.extern __reset

.globl gInterruptVectorTable, gInterruptVectorTableEnd	#, reset

.section	.intvec,4,1,6	


##############################################################################
#
#	isr_prologue
#
#	Saves the necessary registers for an interrupt service routine
#
##############################################################################

isr_prologue: 	.macro
	
				stwu     rsp,-80(rsp) 
				stw      r0,8(rsp) 
				mfctr    r0 
				stw      r0,12(rsp) 
				mfxer    r0 
				stw      r0,16(rsp) 
				mfcr     r0 
				stw      r0,20(rsp) 
				mflr     r0 
				stw      r0,24(rsp) 
				stw      r3,40(rsp) 
				stw      r4,44(rsp) 
				stw      r5,48(rsp) 
				stw      r6,52(rsp) 
				stw      r7,56(rsp) 
				stw      r8,60(rsp) 
				stw      r9,64(rsp) 
				stw      r10,68(rsp) 
				stw      r11,72(rsp) 
				stw      r12,76(rsp) 
				mfsrr0   r0 
				stw      r0,28(rsp) 
				mfsrr1   r0
				stw      r0,32(rsp)
	
				.endm


##############################################################################
#
#	isr_epilogue
#
#	Restores the necessary registers for an interrupt service routine
#
##############################################################################

isr_epilogue: 	.macro

				lwz      r0,32(rsp)
				mtsrr1   r0
				lwz      r0,28(rsp)
				mtsrr0   r0
				lwz      r3,40(rsp)
				lwz      r4,44(rsp)
				lwz      r5,48(rsp)
				lwz      r6,52(rsp)
				lwz      r7,56(rsp)
				lwz      r8,60(rsp)
				lwz      r9,64(rsp)
				lwz      r10,68(rsp)
				lwz      r11,72(rsp)
				lwz      r12,76(rsp)
				lwz      r0,24(rsp)
				mtlr     r0
				lwz      r0,20(rsp)
				mtcrf    0xff,r0
				lwz      r0,16(rsp)
				mtxer    r0
				lwz      r0,12(rsp)
				mtctr    r0
				lwz      r0,8(rsp)
				addi     rsp,rsp,80
				rfi

				.endm
				

##############################################################################
#
#	cisr_prologue
#
#	Saves the necessary registers for a critical interrupt service routine
#
##############################################################################

cisr_prologue: 	.macro
	
				stwu     rsp,-80(rsp) 
				stw      r0,8(rsp) 
				mfctr    r0 
				stw      r0,12(rsp) 
				mfxer    r0 
				stw      r0,16(rsp) 
				mfcr     r0 
				stw      r0,20(rsp) 
				mflr     r0 
				stw      r0,24(rsp) 
				stw      r3,40(rsp) 
				stw      r4,44(rsp) 
				stw      r5,48(rsp) 
				stw      r6,52(rsp) 
				stw      r7,56(rsp) 
				stw      r8,60(rsp) 
				stw      r9,64(rsp) 
				stw      r10,68(rsp) 
				stw      r11,72(rsp) 
				stw      r12,76(rsp) 
				mfspr    r0,58
				stw      r0,28(rsp) 
				mfspr    r0,59
				stw      r0,32(rsp)
				.endm


##############################################################################
#
#	cisr_epilogue
#
#	Restores the necessary registers for a critical interrupt service routine
#
##############################################################################

cisr_epilogue: 	.macro


				lwz      r0,32(rsp)
				mtspr    59,r0
				lwz      r0,28(rsp)
				mtspr    58,r0
				lwz      r3,40(rsp)
				lwz      r4,44(rsp)
				lwz      r5,48(rsp)
				lwz      r6,52(rsp)
				lwz      r7,56(rsp)
				lwz      r8,60(rsp)
				lwz      r9,64(rsp)
				lwz      r10,68(rsp)
				lwz      r11,72(rsp)
				lwz      r12,76(rsp)
				lwz      r0,24(rsp)
				mtlr     r0
				lwz      r0,20(rsp)
				mtcrf    0xff,r0
				lwz      r0,16(rsp)
				mtxer    r0
				lwz      r0,12(rsp)
				mtctr    r0
				lwz      r0,8(rsp)
				addi     rsp,rsp,80
				rfci
				.endm
				

gInterruptVectorTable:


##############################################################################
#
#	IVOR0 - 0x0100 critical interrupt
#
##############################################################################
		.org	0x100

		cisr_prologue
		
		li		r3,0x0100
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl

		cisr_epilogue
		

##############################################################################
#
#	IVOR1 - 0x0200 Machine Check
#
##############################################################################
		.org 	0x200

				stwu     rsp,-80(rsp) 
				stw      r0,8(rsp) 
				mfctr    r0 
				stw      r0,12(rsp) 
				mfxer    r0 
				stw      r0,16(rsp) 
				mfcr     r0 
				stw      r0,20(rsp) 
				mflr     r0 
				stw      r0,24(rsp) 
				stw      r3,40(rsp) 
				stw      r4,44(rsp) 
				stw      r5,48(rsp) 
				stw      r6,52(rsp) 
				stw      r7,56(rsp) 
				stw      r8,60(rsp) 
				stw      r9,64(rsp) 
				stw      r10,68(rsp) 
				stw      r11,72(rsp) 
				stw      r12,76(rsp) 
				mfspr	 r0,570
				stw      r0,28(rsp) 
				mfspr    r0,571
				stw      r0,32(rsp)
	

		
		li		r3,0x0200
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
				lwz      r0,32(rsp)
				mtspr    571,r0
				lwz      r0,28(rsp)
				mtspr    570,r0
				lwz      r3,40(rsp)
				lwz      r4,44(rsp)
				lwz      r5,48(rsp)
				lwz      r6,52(rsp)
				lwz      r7,56(rsp)
				lwz      r8,60(rsp)
				lwz      r9,64(rsp)
				lwz      r10,68(rsp)
				lwz      r11,72(rsp)
				lwz      r12,76(rsp)
				lwz      r0,24(rsp)
				mtlr     r0
				lwz      r0,20(rsp)
				mtcrf    0xff,r0
				lwz      r0,16(rsp)
				mtxer    r0
				lwz      r0,12(rsp)
				mtctr    r0
				lwz      r0,8(rsp)
				addi     rsp,rsp,80
				rfmci

		
##############################################################################
#
#	IVOR2 - 0x0300 Data Storage
#
##############################################################################
		.org	0x300		

		isr_prologue
		
		li		r3,0x0300
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue

##############################################################################
#
#	IVOR3 - 0x0400 Instruction Storage
#
##############################################################################
		.org	0x400

		isr_prologue
		
		li		r3,0x0400
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue

##############################################################################
#
#	IVOR4 - 0x0500 External Interrupt
#
##############################################################################
		.org	0x500

		isr_prologue
		
		li		r3,0x0500
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue

##############################################################################
#
#	IVOR5 - 0x0600 Alignment
#
##############################################################################
		.org	0x600

		isr_prologue
		
		li		r3,0x0600
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue

##############################################################################
#
#	IVOR6 - 0x0700 Program
#
##############################################################################
		.org	0x700

        nop
		isr_prologue
		
		li		r3,0x0700
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue

##############################################################################
#
#	IVOR10 - 0x0900 Decrementer
#
##############################################################################
		.org	0x900

		isr_prologue
		
		li		r3,0x0900
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue


##############################################################################
#
#	IVOR12 - 0x0B00 Watchdog timer interrupt
#
##############################################################################
		.org	0xB00

		cisr_prologue
		
		li		r3,0x0B00
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		cisr_epilogue
		
##############################################################################
#
#	IVOR8 - 0x0C00 System Call
#
##############################################################################
		.org	0xC00

		isr_prologue
		
		li		r3,0x0C00
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue


##############################################################################
#
#	IVOR11 - 0x0F00 Fixed interval timer
#
##############################################################################
		.org	0xF00

		isr_prologue
		
		li		r3,0x0F00
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue

##############################################################################
#
#	IVOR14 - 0x1000 	Instruction TLB Miss
#
##############################################################################
		.org	0x1000

		isr_prologue
		
		li		r3,0x1000
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue

##############################################################################
#
#	IVOR13 - 0x1100 	Data TLB Miss
#
##############################################################################
		.org	0x1100

		isr_prologue
		
		li		r3,0x1100
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue


##############################################################################
#
#	IVOR15 - 0x1500	Debug
#
##############################################################################
		.org	0x1500

        nop
		cisr_prologue
		
		li		r3,0x1500
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		cisr_epilogue
		
##############################################################################
#
#	IVOR32 - 0x1600	SPE APU unavailable
#
##############################################################################
		.org	0x1600

		isr_prologue
		
		li		r3,0x1600
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue
		

##############################################################################
#
#	IVOR33 - 0x1700	SPE -Floating point data exception
#
##############################################################################
		.org	0x1700

		isr_prologue
		
		li		r3,0x1700
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue
		
##############################################################################
#
#	IVOR34 - 0x1800	SPE - Floating point round exception
#
##############################################################################
		.org	0x1800

		isr_prologue
		
		li		r3,0x1800
		lis		r4,InterruptHandler@h
		ori		r4,r4,InterruptHandler@l
		mtlr	r4 
		blrl
		
		isr_epilogue

gInterruptVectorTableEnd:
