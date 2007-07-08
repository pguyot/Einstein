@ zone:
@	jit_registers[0-14]
@	jit_cpsr		+60
@	saved sp		+64
@   enter_glue_1	+68
@   enter_glue_2	+72
@   native_addr		+76

@ Actual enter function.
@ EnterJITGlue(JITZone* inZone)
.section .text
.global _Z12EnterJITGlueP7JITZone   @ give the symbol add external linkage

_Z12EnterJITGlueP7JITZone:
@ save all registers to the stack, with the cpsr
		mrs		r1, cpsr
		stmfd	sp!, {r1,r4-r12,r14}
@ save the stack register into the zone.
		str		sp, [r0, #64]
@ prepare the enter glue in the zone
		ldr		r1, EnterZoneGlue_1
		ldr		r2, EnterZoneGlue_2
		str		r1, [r0, #68]
		str		r2, [r0, #72]
@ load the jit cpsr
		ldr		r1, [r0, #60]
		msr		cpsr_f, r1
@ load the registers
		ldmib	r0, {r1-r14}
@ jump to the glue
		add		pc, r0, #68
EnterZoneGlue_1:
		ldr		r0, [r0]
EnterZoneGlue_2:
		ldr		pc, JumpAddr
JumpAddr:
		.word	0

.section .data
@ The following bits go into the native code:
.global ExitJITNativeSequence   @ give the symbol add external linkage
ExitJITNativeSequence:
		str		lr, save_lr
		mov		lr, pc
		ldr		pc, exit_jit_glue
return_value:
		.word	0
exit_jit_glue:
		.word	0
save_lr:
		.word	0

@ Actual exit function.
.global ExitJITGlue   @ give the symbol add external linkage
ExitJITGlue:
@ save lr next to pc
		str		lr, save_lr2
@ save all jit registers
		ldr		lr, exit_jit_zone
		stmia	lr!, {r0-r13}
		ldr		r0, save_lr2
		ldr		r1, [r0, #8]
		stmia	lr!, {r1}
@ save jit cpsr
		mrs		r1, cpsr
		stmia	lr!, {r1}
@ restore caller sp
		ldr		sp, [lr]
@ restore registers from the stack
		ldmfd	sp!, {r1,r4-r12,r14}
		msr		cpsr_f, r1
@ return the pre-defined return value.
		ldr		r0, [r0]
@ return
		mov		pc, lr
save_lr2:
		.word	0
exit_jit_zone:
		.word	0
.end
