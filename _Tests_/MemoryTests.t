#include "Emulator/TMemory.h"
#include <K/Defines/UByteSex.h>
#if TARGET_OS_WIN32
#define kTempFlashPath "c:/EinsteinTests.flash"
#else
#define kTempFlashPath "/tmp/EinsteinTests.flash"
#endif

TEST(MemoryTests, ReadROMTest)
{
	KUInt8* romBuffer = (KUInt8*) malloc(TMemoryConsts::kLowROMEnd);
	TMemory theMem(nullptr, romBuffer, kTempFlashPath);
	int index;
	for (index = 0; index < 8; index++)
	{
		romBuffer[index] = (index * 16) + index;
	}

#if TARGET_RT_LITTLE_ENDIAN
	// Swap the ROM the way it is swapped
	KUInt32* theROM = (KUInt32*) romBuffer;
	theROM[0] = UByteSex::Swap(theROM[0]);
	theROM[1] = UByteSex::Swap(theROM[1]);
#endif

	Boolean fault = false;
	KUInt32 theWord = theMem.ReadP(0, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x00112233);
	theWord = theMem.ReadP(1, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x11223300);
	theWord = theMem.ReadP(2, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x22330011);
	theWord = theMem.ReadP(3, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x33001122);
	theWord = theMem.ReadP(4, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x44556677);

	KUInt8 theByte;
	fault = theMem.ReadBP(0, theByte);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theByte, 0x00);
	fault = theMem.ReadBP(1, theByte);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theByte, 0x11);
	fault = theMem.ReadBP(2, theByte);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theByte, 0x22);
	fault = theMem.ReadBP(3, theByte);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theByte, 0x33);
	fault = theMem.ReadBP(4, theByte);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theByte, 0x44);
	fault = theMem.ReadBP(5, theByte);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theByte, 0x55);
	fault = theMem.ReadBP(6, theByte);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theByte, 0x66);
	fault = theMem.ReadBP(7, theByte);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theByte, 0x77);

	(void) ::unlink(kTempFlashPath);
	::free(romBuffer);
}

TEST(MemoryTests, ReadWriteRAMTest)
{
	KUInt8* romBuffer = (KUInt8*) malloc(TMemoryConsts::kLowROMEnd);
	TMemory theMem(nullptr, (KUInt8*) romBuffer, kTempFlashPath);
	int index;
	Boolean fault;
	KUInt32 theWord;
	KUInt8 theByte;

	fault = theMem.WriteP(0x04000000, 0x00112233);
	EXPECT_EQ(fault, false);
	fault = theMem.WriteP(0x04000004, 0x44556677);
	EXPECT_EQ(fault, false);

	theWord = theMem.ReadP(0x04000000 + 0, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x00112233);
	theWord = theMem.ReadP(0x04000000 + 1, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x11223300);
	theWord = theMem.ReadP(0x04000000 + 2, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x22330011);
	theWord = theMem.ReadP(0x04000000 + 3, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x33001122);
	theWord = theMem.ReadP(0x04000000 + 4, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x44556677);

	for (index = 0; index < 8; index++)
	{
		fault = theMem.ReadBP(0x04000000 + index, theByte);
		EXPECT_EQ(fault, false);
		EXPECT_EQ(theByte, (KUInt8) index * 0x11);
	}

	for (index = 0; index < 8; index++)
	{
		fault = theMem.WriteBP(0x04000000 + index, ((index + 8) * 16) + index + 8);
		EXPECT_EQ(fault, false);
	}

	theWord = theMem.ReadP(0x04000000 + 0, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x8899AABB);
	theWord = theMem.ReadP(0x04000000 + 1, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0x99AABB88);
	theWord = theMem.ReadP(0x04000000 + 2, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0xAABB8899);
	theWord = theMem.ReadP(0x04000000 + 3, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0xBB8899AA);
	theWord = theMem.ReadP(0x04000000 + 4, fault);
	EXPECT_EQ(fault, false);
	EXPECT_EQ(theWord, 0xCCDDEEFF);

	(void) ::unlink(kTempFlashPath);
	::free(romBuffer);
}
