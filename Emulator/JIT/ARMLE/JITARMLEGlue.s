@ What to expect:
@ r4-r8, r10-r11, r13 (stack)
@ What to save:
@ r4-r11, r13, cpsr
@ zone:
@	jit_registers[0-14]
@	jit_cpsr
@	saved sp

@ Actual enter function.
@ EnterJITGlue(JITZone* inZone, KUInt32 inNativeAddr)
@ TODO: handle inNativeAddr param.
.section .text
.global EnterJITGlue   @ give the symbol add external linkage

EnterJITGlue:
@ save all registers to the stack, with the cpsr
		mrs		r1, cpsr
		stmfd	sp!, {r1,r4-r12,r14}
@ save the stack register into the zone.
		str		sp, [r0, #64]
@ load the jit cpsr
		ldr		r1, [r0, #60]
		msr		cpsr_f, r1
@ load the registers
		ldmib	r0, {r1-r14}
		ldr		r0, [r0]

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
		ldr		lr, [lr, #8]	@ exit_jit_zone
		stmia	lr!, {r0-r13}
		ldr		r0, save_lr2
		ldr		r1, [r0, #4]
		str		r1, [lr]!
@ save jit cpsr
		mrs		r1, cpsr
		str		r1, [lr]!
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
