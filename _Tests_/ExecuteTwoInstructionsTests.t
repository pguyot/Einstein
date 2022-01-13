// Make sure we don't crash.
// 00000000 add      pc, r2, #20                    	| E282F014 - ....
// 00000008 add      pc, r2, #20                    	| E282F014 - ....
TEST(ExecuteTwoInstructions, E282F014_E282F014)
{
	UProcessorTests::ExecuteTwoInstructions(0xE282F014, 0xE282F014, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// Test adds
// 00000000 mvn      r2, #0                         	| E3E02000 - ....
// 00000008 adds     r2, r2, #1                     	| E2922001 - ....
TEST(ExecuteTwoInstructions, E3E02000_E2922001)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3E02000, 0xE2922001, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000000C);
		EXPECT_EQ(proc.GetCPSR(), 0x60000013);
	});
}
// 00000000 mov      ip, #8                         	| E3A0C008 - ....
// 00000008 ldmdb    ip, {r8, r9}                   	| E91C0300 - ....
TEST(ExecuteTwoInstructions, E3A0C008_E91C0300)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A0C008, 0xE91C0300, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0xE3A0C008);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0xE91C0300);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000008);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000000C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldmia    r0, {fp}^                      	| E8D00800 - ....
// 00000008 msr      cpsr_ctl, #16                  	| E321F010 - .!..
TEST(ExecuteTwoInstructions, E8D00800_E321F010)
{
	UProcessorTests::ExecuteTwoInstructions(0xE8D00800, 0xE321F010, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000000C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000010);
	});
}
// 00000000 ldr      r1, [r0, #4]                   	| E5901004 - ....
// 00000008 ldr      r2, [r0, r3]                   	| E7902003 - ....
TEST(ExecuteTwoInstructions, E5901004_E7902003)
{
	UProcessorTests::ExecuteTwoInstructions(0xE5901004, 0xE7902003, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0xE7902003);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0xE5901004);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000000C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 mov      r0, #128                       	| E3A00080 - ....
// 00000008 ldrh     r7, [r0, -#124]                	| E15077BC - .Pw.
TEST(ExecuteTwoInstructions, E3A00080_E15077BC)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A00080, 0xE15077BC, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000080);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x0000E150);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000000C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// Test branches when they are not at address 0
// 00000000 mov      r1, #12                        	| E3A0100C - ....
// 00000008 b        0x00018690                     	| EA0061A0 - ..a.
TEST(ExecuteTwoInstructions, E3A0100C_EA0061A0)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A0100C, 0xEA0061A0, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x0000000C);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00018690);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 mov      r1, #12                        	| E3A0100C - ....
// 00000008 mov      r1, r1, lsl #32                	| E1A01081 - ....
TEST(ExecuteTwoInstructions, E3A0100C_E1A01081)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A0100C, 0xE1A01081, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000018);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000000C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 mov      r0, #128                       	| E3A00080 - ....
// 00000008 ldrh     r7, [r0, -#126]                	| E15077BE - .Pw.
TEST(ExecuteTwoInstructions, E3A00080_E15077BE)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A00080, 0xE15077BE, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000080);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x00000080);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000000C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// Make sure we don't crash.
// 00000000 mov      r0, #211                       	| E3A000D3 - ....
// 00000008 msr      cpsr, r0                       	| E129F000 - .)..
TEST(ExecuteTwoInstructions, E3A000D3_E129F000)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A000D3, 0xE129F000, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x000000D3);
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000000C);
		EXPECT_EQ(proc.GetCPSR(), 0x000000D3);
	});
}
// 00000000 mov      sp, #67108864                  	| E3A0D301 - ....
// 00000008 stmia    sp, {r8}^                      	| E8CD0100 - ....
TEST(ExecuteTwoInstructions, E3A0D301_E8CD0100)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A0D301, 0xE8CD0100, [](TARMProcessor& proc) {
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
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x04000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000000C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 mov      r0, #128                       	| E3A00080 - ....
// 00000008 ldrh     r7, [r0, -#128]                	| E15078B0 - .Px.
TEST(ExecuteTwoInstructions, E3A00080_E15078B0)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A00080, 0xE15078B0, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000080);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x0000E3A0);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000000C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}

// Test that ldr pc, [rx] puts the value + 4.
// 00000000 ldr      pc, [r0, #8]                       | E590F008 - ....
// 00000008 mov      r1, #128                       	| E3A01080 - ....
// 00000010 andeq    r0, r0, r0                     	| 00000000 - ....
TEST(ExecuteTwoInstructions, E590F008_E3A01080)
{
	UProcessorTests::ExecuteTwoInstructions(0xE590F008, 0xE3A01080, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000004);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
		EXPECT_EQ(proc.GetSPSR(), 0x00000000);
	});
}

// 00000000 mov      r1, #128                       	| E3A01080 - ....
// 00000008 ldr      pc, [r0]                           | E590F000 - ....
TEST(ExecuteTwoInstructions, E3A01080_E590F000)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A01080, 0xE590F000, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000080);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000010); // Prefetch Abort + 4
		EXPECT_EQ(proc.GetCPSR(), 0x00000097);
		EXPECT_EQ(proc.GetSPSR(), 0x00000013);
	});
}

// 00000000 mov      r1, #128                       	| E3A01080 - ....
// 00000008 ldr      r2, [r0]                           | E5902000 - ....
TEST(ExecuteTwoInstructions, E3A01080_E5902000)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A01080, 0xE5902000, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000080);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0xE3A01080);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x0000000C);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
		EXPECT_EQ(proc.GetSPSR(), 0x00000000);
	});
}

// 00000000 mov      r1, #128                       	| E3A01080 - ....
// 00000000 ldr      r2, [r0, -#4]                  	| E5102004 - ....
TEST(ExecuteTwoInstructions, E3A01080_E5102004)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A01080, 0xE5102004, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000080);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000014); // Data Abort + 4
		EXPECT_EQ(proc.GetCPSR(), 0x00000097);
		EXPECT_EQ(proc.GetSPSR(), 0x00000013);
	});
}

// 00000000 mov      r1, #128                       	| E3A01080 - ....
// 00000000 ldr      pc, [r0, -#4]                  	| E510F004 - ....
TEST(ExecuteTwoInstructions, E3A01080_E510F004)
{
	UProcessorTests::ExecuteTwoInstructions(0xE3A01080, 0xE510F004, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000080);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000014); // Data Abort + 4
		EXPECT_EQ(proc.GetCPSR(), 0x00000097);
		EXPECT_EQ(proc.GetSPSR(), 0x00000013);
	});
}

// 00000000 andeq    r0, r0, r0, ror r0             	| 00000070 - ...p
// 00000000 ldr      pc, [r0]                           | E590F000 - ....
TEST(ExecuteTwoInstructions, 00000070_E590F000)
{
	UProcessorTests::ExecuteTwoInstructions(0x00000070, 0xE590F000, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000074);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
		EXPECT_EQ(proc.GetSPSR(), 0x00000000);
	});
}

// TODO: test that swp instructions can be followed by another instruction.
