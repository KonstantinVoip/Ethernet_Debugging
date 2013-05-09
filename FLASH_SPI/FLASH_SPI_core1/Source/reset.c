extern void __reset(void);

#pragma section code_type ".reset"
 
#pragma force_active on
__declspec(section ".reset") 

asm void reset(void);

asm void reset(void)
{
	nofralloc
 	b	__reset
}
#pragma force_active reset
 
