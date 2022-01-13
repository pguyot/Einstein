// Make sure breakpoint ends test.
// e1200070		bkpt	#0x0
TEST(RunCode, 1)
{
	UProcessorTests::RunCode("E1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// e3a01001		mov		r1, #0x1
// e3310000		teq		r1, #0x0
// 03a02002		moveq	r2, #0x2
// e1200070		bkpt	#0x0
TEST(RunCode, 2)
{
	UProcessorTests::RunCode("E3A01001 E3310000 03A02002 E1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000001);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000014);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// e3a01001		mov		r1, #0x1
// e3310000		teq		r1, #0x0
// 03a02002		movne	r2, #0x2
// e1200070		bkpt	#0x0
TEST(RunCode, 3)
{
	UProcessorTests::RunCode("E3A01001 E3310000 13A02002 E1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000001);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000002);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000014);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// e3a01001		movs	r1, #0x1
// 03a02002		moveq	r2, #0x2
// e1200070		bkpt	#0x0
TEST(RunCode, 4)
{
	UProcessorTests::RunCode("E3B01001 03A02002 E1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000001);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000010);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// e3b01001		movs	r1, #0x1
// 03a02002		movne	r2, #0x2
// e1200070		bkpt	#0x0
TEST(RunCode, 5)
{
	UProcessorTests::RunCode("E3B01001 13A02002 E1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000001);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000002);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000010);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// E3A00301	mov		r0, #0x4000000
// E3A01001	mov		r1, #0x1
// E3A02002	mov		r2, #0x2
// E3A03003	mov		r3, #0x3
// E3A04004	mov		r4, #0x4
// E3A05005	mov		r5, #0x5
// E3A06006	mov		r6, #0x6
// E8A0007F	stm		r0!, {r0, r1, r2, r3, r4, r5, r6}
// E9101F80	ldmdb	r0, {r7, r8, r9, r10, r11, r12}
// E0400106	sub		r0, r0, r6, lsl #2
// E590D004	ldr		sp, [r0, #4]
// E790E004	ldr		lr, [r0, r4]
// E1200070	bkpt	#0x0
TEST(RunCode, 6)
{
	UProcessorTests::RunCode("E3A00301 E3A01001 E3A02002 E3A03003 E3A04004 E3A05005 E3A06006 E8A0007F E9101F80 E0400106 E590D004 E790E004 E1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x04000004);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000001);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000002);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000003);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000004);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000005);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000006);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000001);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000002);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000003);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000004);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000005);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000006);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000002);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000002);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000038);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// e361f017	msr		SPSR_c, #0x17
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e1200070	bkpt	#0x0
TEST(RunCode, 7)
{
	UProcessorTests::RunCode("e361f017 e1a0e00f e1b0f00e e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000014);
		EXPECT_EQ(proc.GetCPSR(), 0x00000017);
	});
}

// e361f010	msr		SPSR_c, #0x10
// e321f017	msr		CPSR_c, #0x17
// e361f011	msr		SPSR_c, #0x11
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e1200070	bkpt	#0x0
TEST(RunCode, 8)
{
	UProcessorTests::RunCode("e361f010 e321f017 e361f011 e1a0e00f e1b0f00e e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000001C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000011);
	});
}

// e361f010	msr		SPSR_c, #0x10
// e321f017	msr		CPSR_c, #0x17
// e3a00011	mov		r0, #0x11
// e2800206	add		r0, r0, #0x60000000
// e169f000	msr		SPSR_fc, r0
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e1200070	bkpt	#0x0
TEST(RunCode, 9)
{
	UProcessorTests::RunCode("e361f010 e321f017 e3a00011 e2800206 e169f000 e1a0e00f e1b0f00e e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x60000011);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000024);
		EXPECT_EQ(proc.GetCPSR(), 0x60000011);
	});
}

// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e129f000	msr		CPSR_fc, r0
// e1200070	bkpt	#0x0
TEST(RunCode, 10)
{
	UProcessorTests::RunCode("e3a00017 e2800206 e129f000 e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x60000017);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000014);
		EXPECT_EQ(proc.GetCPSR(), 0x60000017);
	});
}

// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e121f000	msr		CPSR_c, r0
// e1200070	bkpt	#0x0
TEST(RunCode, 11)
{
	UProcessorTests::RunCode("e3a00017 e2800206 e121f000 e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x60000017);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000014);
		EXPECT_EQ(proc.GetCPSR(), 0x00000017);
	});
}

// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e128f000	msr		CPSR_f, r0
// e1200070	bkpt	#0x0
TEST(RunCode, 12)
{
	UProcessorTests::RunCode("e3a00017 e2800206 e128f000 e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x60000017);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000014);
		EXPECT_EQ(proc.GetCPSR(), 0x60000013);
	});
}

// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e169f000	msr		SPSR_fc, r0
// e14f1000	mrs		r1, SPSR
// e10f2000	mrs		r2, CPSR
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e10f3000	mrs		r3, CPSR
// e1200070	bkpt	#0x0
TEST(RunCode, 13)
{
	UProcessorTests::RunCode("e3a00017 e2800206 e169f000 e14f1000 e10f2000 e1a0e00f e1b0f00e e10f3000 e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x60000017);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x60000017);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000013);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x60000017);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000028);
		EXPECT_EQ(proc.GetCPSR(), 0x60000017);
	});
}

// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e161f000	msr		SPSR_c, r0
// e14f1000	mrs		r1, SPSR
// e10f2000	mrs		r2, CPSR
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e10f3000	mrs		r3, CPSR
// e1200070	bkpt	#0x0
TEST(RunCode, 14)
{
	UProcessorTests::RunCode("e3a00017 e2800206 e161f000 e14f1000 e10f2000 e1a0e00f e1b0f00e e10f3000 e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x60000017);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000017);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000013);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000017);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000028);
		EXPECT_EQ(proc.GetCPSR(), 0x00000017);
	});
}

// e10f0000	mrs		r0, CPSR
// e169f000	msr		SPSR_fc, r0
// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e168f000	msr		SPSR_f, r0
// e14f1000	mrs		r1, SPSR
// e10f2000	mrs		r2, CPSR
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e10f3000	mrs		r3, CPSR
// e1200070	bkpt	#0x0
TEST(RunCode, 15)
{
	UProcessorTests::RunCode("e10f0000 e169f000 e3a00017 e2800206 e168f000 e14f1000 e10f2000 e1a0e00f e1b0f00e e10f3000 e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x60000017);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x60000013);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000013);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x60000013);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000024);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000030);
		EXPECT_EQ(proc.GetCPSR(), 0x60000013);
	});
}

// -- large block with a subroutine doing 64bits additions.
/*
 ldr     r0, =w1
 ldm     r0, {r0-r3}
 bl      sub
 mov     r2, r4
 mov     r3, r5
 mov     r10, r4
 bl      sub
 mov     r2, r4
 mov     r3, r5
 mov     r11, r4
 bl      sub
 mov     r2, r4
 mov     r3, r5
 mov     r12, r4
 bl      sub
 mov     r2, r4
 mov     r3, r5
 mov     r13, r4
 bl      sub
 sub     r2, r4
 mov     r3, r5
 bl      sub
 bkpt    0
 w1:
 .long   0x46474849
 w2:
 .long   0x41424344
 w3:
 .long   0x16171819
 w4:
 .long   0x11121314
 sub:
 adds    r4,r0,r2
 adcs    r5,r1,r3
 mov     r6, r6, LSL #4
 addeq   r6, #0x1
 addcs   r6, #0x2
 addmi   r6, #0x4
 addvs   r6, #0x8
 mov     r7, r7, ROR #4
 subne   r7, #0x1
 subcc   r7, #0x2
 subpl   r7, #0x4
 subvc   r7, #0x8
 orrhi   r8, #0x80000000
 orrge   r8, #0x40000000
 orrgt   r8, #0x20000000
 mov     r8, r8, LSR #3
 eorhi   r9, #0x80000000
 eorge   r9, #0x40000000
 eorgt   r9, #0x20000000
 mov     r9, r9, ASR #3
 mov     pc, lr
*/
TEST(RunCode, 16)
{
	UProcessorTests::RunCode("e59f00b8 e890000f eb000017 e1a02004 e1a03005 e1a0a004 eb000013 e1a02004 e1a03005 e1a0b004 eb00000f e1a02004 e1a03005 e1a0c004 eb00000b e1a02004 e1a03005 e1a0d004 eb000007 e0422004 e1a03005 eb000004 e1200070 46474849 41424344 16171819 11121314 e0904002 e0b15003 e1a06206 02866001 22866002 42866004 62866008 e1a07267 12477001 32477002 52477004 72477008 83888102 a3888101 c3888202 e1a081a8 82299102 a2299101 c2299202 e1a091c9 e1a0f00e 0000005c", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x46474849);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x41424344);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0xB9B8B7B7);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x575D6369);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x989FA6AE);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x000C420C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x024C1FFC);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x0DF0D800);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0xF270D800);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x5C5E6062);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0xA2A5A8AB);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0xE8ECF0F4);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x2F34393D);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000058);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000060);
		EXPECT_EQ(proc.GetCPSR(), 0x90000013);
	});
}

/*
mov r0, #0x04000000
sub r1, pc, #0x0304
sub r1, r1, #0x00020000
sub r1, r1, #0x01000000
str r1, [r0]
mov r2, #0x05
swp r3, r1, [r0]
swpb r4, r2, [r0]
ldr r5, [r0]
bkpt 0
*/
TEST(RunCode, 17)
{
	UProcessorTests::RunCode("e3a00301 e24f1fc1 e2411802 e2411401 e5801000 e3a02005 e1003091 e1404092 e5905000 e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x04000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0xFEFDFD08);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000005);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0xFEFDFD08);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x000000FE);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x05FDFD08);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000002C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

/*
 Test with page jumps using RAM (we copy code there).

 00000000	e3a00301	mov	r0, #0x4000000
 00000004	e28f1008	add	r1, pc, #0x8
 00000008	e8b103f0	ldm	r1!, {r4, r5, r6, r7, r8, r9}
 0000000c	e88003f0	stm	r0, {r4, r5, r6, r7, r8, r9}
 00000010	e280f004	add	pc, r0, #0x4
 04000000	e2833001	add	r3, r3, #0x1		# not executed
 04000004	e2833002	add	r3, r3, #0x2		# executed
 04000008	e2800601	add	r0, r0, #0x100000
 0400000c	e8b13c00	ldm	r1!, {r10, r11, r12, sp}
 04000010	e8803c00	stm	r0, {r10, r11, r12, sp}
 04000014	ea03fffa	b	0x4100004
 04100000	e2833004	add	r3, r3, #0x4		# not executed
 04100004	e2833008	add	r3, r3, #0x8		# executed
 04100008	e1200070	bkpt	#0x0
 */
TEST(RunCode, 18)
{
	UProcessorTests::RunCode("e3a00301 e28f1008 e8b103f0 e88003f0 e280f004 e2833001 e2833002 e2800601 e8b13c00 e8803c00 ea03fffa e2833004 e2833008 e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x04100000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x0000003C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0000000A);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0xE2833001);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0xE2833002);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0xE2800601);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0xE8B13C00);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0xE8803C00);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0xEA03FFFA);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0xE2833004);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0xE2833008);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0xE1200070);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x04100010);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

/*
 LDMDB with PC.

 add     r11, pc, #40        r11 = 48
 ldmdb   r11, {r4-r11, r13, pc}
 bkpt 0
 .long   0x00000010
 .long   0x0000000F
 .long   0x0000000E
 .long   0x0000000D
 .long   0x0000000C
 .long   0x0000000B
 .long   0x0000000A
 .long   0x00000009
 .long   0x00000008
 */
TEST(RunCode, 19)
{
	UProcessorTests::RunCode("e28fb028 e91baff0 e1200070 00000010 0000000F 0000000E 0000000D 0000000C 0000000B 0000000A 00000009 00000008", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0xE1200070);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000010);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x0000000F);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x0000000E);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x0000000D);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x0000000C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x0000000B);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x0000000A);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000009);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000010);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

/*
 mov    r1, #3
 mov    r0, #0x200
 add    r0, #0x57
 mov	r2, r0, lsr r1
 mov	r3, r0, lsr #3
 mov	r4, r0, asr r1
 mov	r5, r0, asr #3
 bkpt 0
*/
TEST(RunCode, 20)
{
	UProcessorTests::RunCode("e3a01003 e3a00c02 e2800057 e1a02130 e1a031a0 e1a04150 e1a051c0 e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000257);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000003);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x0000004A);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0000004A);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x0000004A);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x0000004A);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000024);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

/*
 * half-word store
   0:	e3a00301 	mov	r0, #67108864	; 0x4000000
   4:	e28f1b01 	add	r1, pc, #1024	; 0x400
   8:	e1c010b0 	strh	r1, [r0]
   c:	e5902000 	ldr	r2, [r0]
  10:   e1200070    bkpt 0
**/
TEST(RunCode, 21)
{
	UProcessorTests::RunCode("e3a00301 e28f1b01 e1c010b0 e1c010b0 e5902000 e1200070", [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x04000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x0000040C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x040C0000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000001C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
