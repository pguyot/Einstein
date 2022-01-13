// 00000000 beq      0x00000024 (will not branch)
TEST(ExecuteInstruction, 0A000007)
{
	UProcessorTests::ExecuteInstruction(0x0A000007, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// 00000000 bne      0x00000024 (will branch)
TEST(ExecuteInstruction, 1A000007)
{
	UProcessorTests::ExecuteInstruction(0x1A000007, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000028);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// mrs      r1, cpsr
// Note: CPSR is 00000013 (user mode) now.
TEST(ExecuteInstruction, E10F1000)
{
	UProcessorTests::ExecuteInstruction(0xE10F1000, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000013);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
		EXPECT_EQ(proc.GetSPSR(), 0x00000000);
	});
}

// 00000000 mrs      r1, spsr                       	| E14F1000 - .O..
// Note: SPSR is 0 now.
TEST(ExecuteInstruction, E14F1000)
{
	UProcessorTests::ExecuteInstruction(0xE14F1000, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
		EXPECT_EQ(proc.GetSPSR(), 0x00000000);
	});
}

// 00000000 msr      cpsr_ctl, #18                  	| E321F012 - .!..
TEST(ExecuteInstruction, E321F012)
{
	UProcessorTests::ExecuteInstruction(0xE321F012, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000012);
	});
}
// 00000000 ldr      r0, [r0]                       	| E5900000 - ....
TEST(ExecuteInstruction, E5900000)
{
	UProcessorTests::ExecuteInstruction(0xE5900000, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0xE5900000);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 msr      cpsr_ctl, #27                  	| E321F01B - .!..
TEST(ExecuteInstruction, E321F01B)
{
	UProcessorTests::ExecuteInstruction(0xE321F01B, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x0000001B);
	});
}
// 00000000 add      pc, r2, #20                    	| E282F014 - ....
TEST(ExecuteInstruction, E282F014)
{
	UProcessorTests::ExecuteInstruction(0xE282F014, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000018);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldr      r0, [r0, #1]                   	| E5900001 - ....
TEST(ExecuteInstruction, E5900001)
{
	UProcessorTests::ExecuteInstruction(0xE5900001, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x900001E5);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 b        0x00018688                     	| EA0061A0 - ..a.
TEST(ExecuteInstruction, EA0061A0)
{
	UProcessorTests::ExecuteInstruction(0xEA0061A0, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0001868C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 msr      cpsr_ctl, #19                  	| E321F013 - .!..
TEST(ExecuteInstruction, E321F013)
{
	UProcessorTests::ExecuteInstruction(0xE321F013, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// 00000000 ldrh     r7, [r1, #2]                   	| E1D170B2 - ..p.
TEST(ExecuteInstruction, E1D170B2)
{
	UProcessorTests::ExecuteInstruction(0xE1D170B2, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x000070B2);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldmia    sp, {sp}^                      	| E8DD2000 - ....
TEST(ExecuteInstruction, E8DD2000)
{
	UProcessorTests::ExecuteInstruction(0xE8DD2000, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 bics     r0, r0, #-16777216             	| E3D004FF - ....
TEST(ExecuteInstruction, E3D004FF)
{
	UProcessorTests::ExecuteInstruction(0xE3D004FF, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x60000013);
	});
}
// 00000000 mcr      15, 0, r0, cr1, cr1, {0}       	| EE010F11 - ....
TEST(ExecuteInstruction, EE010F11)
{
	UProcessorTests::ExecuteInstruction(0xEE010F11, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldrb     r0, [r0, #1]                   	| E5D00001 - ....
TEST(ExecuteInstruction, E5D00001)
{
	UProcessorTests::ExecuteInstruction(0xE5D00001, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x000000D0);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldrb     r0, [r0]                       	| E5D00000 - ....
TEST(ExecuteInstruction, E5D00000)
{
	UProcessorTests::ExecuteInstruction(0xE5D00000, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x000000E5);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 mov      r4, #156                       	| E3A0409C - ..@.
TEST(ExecuteInstruction, E3A0409C)
{
	UProcessorTests::ExecuteInstruction(0xE3A0409C, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x0000009C);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 b        0x00000000                     	| EAFFFFFE - ....
TEST(ExecuteInstruction, EAFFFFFE)
{
	UProcessorTests::ExecuteInstruction(0xEAFFFFFE, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000004);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldmia    r0, {fp}^                      	| E8D00800 - ....
TEST(ExecuteInstruction, E8D00800)
{
	UProcessorTests::ExecuteInstruction(0xE8D00800, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0xE8D00800);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldr      r0, [r0, #3]                   	| E5900003 - ....
TEST(ExecuteInstruction, E5900003)
{
	UProcessorTests::ExecuteInstruction(0xE5900003, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x03E59000);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 msr      cpsr_ctl, #17                  	| E321F011 - .!..
TEST(ExecuteInstruction, E321F011)
{
	UProcessorTests::ExecuteInstruction(0xE321F011, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000011);
	});
}
// 00000000 mvneqs   r4, r3, ror r5                 	| 01F94573 - ..Es
TEST(ExecuteInstruction, 01F94573)
{
	UProcessorTests::ExecuteInstruction(0x01F94573, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 msr      cpsr_ctl, #16                  	| E321F010 - .!..
TEST(ExecuteInstruction, E321F010)
{
	UProcessorTests::ExecuteInstruction(0xE321F010, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000010);
	});
}
// 00000000 ldr      r0, [r0, #2]                   	| E5900002 - ....
TEST(ExecuteInstruction, E5900002)
{
	UProcessorTests::ExecuteInstruction(0xE5900002, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x0002E590);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 msr      cpsr_ctl, #23                  	| E321F017 - .!..
TEST(ExecuteInstruction, E321F017)
{
	UProcessorTests::ExecuteInstruction(0xE321F017, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000017);
	});
}
// 00000000 ldrh     r7, [r1]                       	| E1D170B0 - ..p.
TEST(ExecuteInstruction, E1D170B0)
{
	UProcessorTests::ExecuteInstruction(0xE1D170B0, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x0000E1D1);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 adds     r2, r2, #0                     	| E2922000 - ....
TEST(ExecuteInstruction, E2922000)
{
	UProcessorTests::ExecuteInstruction(0xE2922000, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x40000013);
	});
}
// 00000000 ldrb     r0, [r0, #2]                   	| E5D00002 - ....
TEST(ExecuteInstruction, E5D00002)
{
	UProcessorTests::ExecuteInstruction(0xE5D00002, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldrb     r0, [r0, #3]                   	| E5D00003 - ....
TEST(ExecuteInstruction, E5D00003)
{
	UProcessorTests::ExecuteInstruction(0xE5D00003, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000003);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldrh     r7, 0x00000000                 	| E15F70B8 - ._p.
TEST(ExecuteInstruction, E15F70B8)
{
	UProcessorTests::ExecuteInstruction(0xE15F70B8, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x0000E15F);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldrh     r7, 0x00000002                 	| E15F70B6 - ._p.
TEST(ExecuteInstruction, E15F70B6)
{
	UProcessorTests::ExecuteInstruction(0xE15F70B6, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x000070B6);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
