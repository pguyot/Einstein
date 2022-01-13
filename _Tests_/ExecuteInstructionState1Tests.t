// 00000000 rsb      r0, r2, r2, lsl #32            	| E0620202 - .b..
TEST(ExecuteInstructionState1, E0620202)
{
	UProcessorTests::ExecuteInstructionState1(0xE0620202, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x8796A5B4);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 eor      r0, ip, #1140850688            	| E22C0311 - .,..
TEST(ExecuteInstructionState1, E22C0311)
{
	UProcessorTests::ExecuteInstructionState1(0xE22C0311, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x75323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 eor      r0, r0, #65536                 	| E2200801 - ....
TEST(ExecuteInstructionState1, E2200801)
{
	UProcessorTests::ExecuteInstructionState1(0xE2200801, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01030304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 eor      r0, r0, #1140850688            	| E2200311 - ....
TEST(ExecuteInstructionState1, E2200311)
{
	UProcessorTests::ExecuteInstructionState1(0xE2200311, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x45020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 mla      r0, r4, r0, r5                 	| E0205094 - ..P.
TEST(ExecuteInstructionState1, E0205094)
{
	UProcessorTests::ExecuteInstructionState1(0xE0205094, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0xC9BF9F68);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 eor      ip, r0, #1140850688            	| E220C311 - ....
TEST(ExecuteInstructionState1, E220C311)
{
	UProcessorTests::ExecuteInstructionState1(0xE220C311, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x45020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 add      r5, r5, #3584                  	| E2855C0E - ..\.
TEST(ExecuteInstructionState1, E2855C0E)
{
	UProcessorTests::ExecuteInstructionState1(0xE2855C0E, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15162518);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 eor      r0, sp, #1140850688            	| E22D0311 - .-..
TEST(ExecuteInstructionState1, E22D0311)
{
	UProcessorTests::ExecuteInstructionState1(0xE22D0311, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x71363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 add      r1, r4, r5                     	| E0841005 - ....
TEST(ExecuteInstructionState1, E0841005)
{
	UProcessorTests::ExecuteInstructionState1(0xE0841005, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x26282A2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 and      r9, r4, r2, lsr #32            	| E0049A22 - ..."
TEST(ExecuteInstructionState1, E0049A22)
{
	UProcessorTests::ExecuteInstructionState1(0xE0049A22, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000010);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 mla      fp, r9, sl, r0                 	| E02B0A99 - .+..
TEST(ExecuteInstructionState1, E02B0A99)
{
	UProcessorTests::ExecuteInstructionState1(0xE02B0A99, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0xA0B475E4);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 add      r0, r0, r5                     	| E0800005 - ....
TEST(ExecuteInstructionState1, E0800005)
{
	UProcessorTests::ExecuteInstructionState1(0xE0800005, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x16181A1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 sub      fp, ip, #4                     	| E24CB004 - .L..
TEST(ExecuteInstructionState1, E24CB004)
{
	UProcessorTests::ExecuteInstructionState1(0xE24CB004, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x31323330);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 mov      r6, #254                       	| E3A060FE - ..`.
TEST(ExecuteInstructionState1, E3A060FE)
{
	UProcessorTests::ExecuteInstructionState1(0xE3A060FE, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x000000FE);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldr      r0, [r4, #16]!                 	| E5B40010 - ....
TEST(ExecuteInstructionState1, E5B40010)
{
	UProcessorTests::ExecuteInstructionState1(0xE5B40010, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 and      r9, r4, r0, lsr #32            	| E0049C20 - ....
TEST(ExecuteInstructionState1, E0049C20)
{
	UProcessorTests::ExecuteInstructionState1(0xE0049C20, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 mlas     fp, r9, sl, fp                 	| E03BBA99 - .;..
TEST(ExecuteInstructionState1, E03BBA99)
{
	UProcessorTests::ExecuteInstructionState1(0xE03BBA99, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0xCCE0A210);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x80000013);
	});
}
// 00000000 mla      fp, r9, sl, fp                 	| E02BBA99 - .+..
TEST(ExecuteInstructionState1, E02BBA99)
{
	UProcessorTests::ExecuteInstructionState1(0xE02BBA99, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0xCCE0A210);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 ldr      r0, 0x00000018                 	| E5BF0010 - ....
TEST(ExecuteInstructionState1, E5BF0010)
{
	UProcessorTests::ExecuteInstructionState1(0xE5BF0010, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 mul      fp, r9, sl                     	| E00B0A99 - ....
TEST(ExecuteInstructionState1, E00B0A99)
{
	UProcessorTests::ExecuteInstructionState1(0xE00B0A99, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x9FB272E0);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 sub      sp, sp, #4                     	| E24DD004 - .M..
TEST(ExecuteInstructionState1, E24DD004)
{
	UProcessorTests::ExecuteInstructionState1(0xE24DD004, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363734);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 muls     fp, r9, sl                     	| E01B0A99 - ....
TEST(ExecuteInstructionState1, E01B0A99)
{
	UProcessorTests::ExecuteInstructionState1(0xE01B0A99, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x9FB272E0);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x80000013);
	});
}
// 00000000 mlas     fp, r9, sl, sl                 	| E03BAA99 - .;..
TEST(ExecuteInstructionState1, E03BAA99)
{
	UProcessorTests::ExecuteInstructionState1(0xE03BAA99, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0xC8DC9E0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x80000013);
	});
}
// 00000000 orr      r0, r0, #224                   	| E38000E0 - ....
TEST(ExecuteInstructionState1, E38000E0)
{
	UProcessorTests::ExecuteInstructionState1(0xE38000E0, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x010203E4);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 and      r9, r4, r2, lsr #32            	| E0049C22 - ..."
TEST(ExecuteInstructionState1, E0049C22)
{
	UProcessorTests::ExecuteInstructionState1(0xE0049C22, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000000);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 orr      r0, r0, #239                   	| E38000EF - ....
TEST(ExecuteInstructionState1, E38000EF)
{
	UProcessorTests::ExecuteInstructionState1(0xE38000EF, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x010203EF);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 and      r9, r4, r3, lsr #32            	| E0049C23 - ...#
TEST(ExecuteInstructionState1, E0049C23)
{
	UProcessorTests::ExecuteInstructionState1(0xE0049C23, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x01020304);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x00000004);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
// 00000000 eors     r0, r0, #41216                 	| E2300CA1 - .0..
TEST(ExecuteInstructionState1, E2300CA1)
{
	UProcessorTests::ExecuteInstructionState1(0xE2300CA1, [](TARMProcessor& proc) {
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0), 0x0102A204);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1), 0x05060708);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2), 0x090A0B0C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3), 0x0D0E0F10);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4), 0x11121314);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5), 0x15161718);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6), 0x191A1B1C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7), 0x1D1E1F20);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8), 0x21222324);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9), 0x25262728);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x292A2B2C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x2D2E2F30);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0x31323334);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x35363738);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x393A3B3C);
		EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
		EXPECT_EQ(proc.GetCPSR(), 0x00000013);
	});
}
