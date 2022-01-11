// 00000000 mov      r3, ip, lsl r3                 	| E1A0331C - ..3.
TEST(ExecuteInstructionState2, E1A0331C) {
    UProcessorTests::ExecuteInstructionState2(0xE1A0331C, [] (TARMProcessor& proc) {
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR0),  0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR1),  0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR2),  0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR3),  0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR4),  0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR5),  0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR6),  0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR7),  0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR8),  0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR9),  0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR10), 0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR11), 0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR12), 0xFFFFFFFF);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR13), 0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR14), 0x00000000);
        EXPECT_EQ(proc.GetRegister(TARMProcessor::kR15), 0x00000008);
        EXPECT_EQ(proc.GetCPSR(),                        0x00000013);
    });
}
