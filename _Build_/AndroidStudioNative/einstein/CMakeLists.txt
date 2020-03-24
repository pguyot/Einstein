# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

PROJECT(Einstein C CXX)

SET (LOCAL_PATH "../../..")

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3")

include_directories(
    ${LOCAL_PATH}/
	${LOCAL_PATH}/Emulator/
	${LOCAL_PATH}/Emulator/ROM/
	${LOCAL_PATH}/Emulator/Platform/
	${LOCAL_PATH}/Emulator/NativeCalls/
	${LOCAL_PATH}/Emulator/Host/
	${LOCAL_PATH}/Emulator/Log/
	${LOCAL_PATH}/Emulator/JIT/
	${LOCAL_PATH}/Emulator/JIT/Generic/
	${LOCAL_PATH}/Emulator/Network/
	${LOCAL_PATH}/Emulator/PCMCIA/
	${LOCAL_PATH}/Emulator/Serial/
	${LOCAL_PATH}/Emulator/Sound/
	${LOCAL_PATH}/Emulator/Screen/
)

SET (SourceFiles
		# Host support
		${LOCAL_PATH}/app/TAndroidNativeApp.cp
		# Einstein Emulator Base Code
		${LOCAL_PATH}/Emulator/TARMProcessor.cp
		${LOCAL_PATH}/Emulator/TDMAManager.cp
		${LOCAL_PATH}/Emulator/TEmulator.cp
		${LOCAL_PATH}/Emulator/TFlash.cp
		${LOCAL_PATH}/Emulator/TInterruptManager.cp
		${LOCAL_PATH}/Emulator/TMemory.cp
		${LOCAL_PATH}/Emulator/TMMU.cp
		${LOCAL_PATH}/Emulator/TNativePrimitives.cp
		# Manage Paths and File Access
		${LOCAL_PATH}/Emulator/Files/TFileManager.cp
		# GRab Owner Information form the Host and make it available to NewtonOS
		${LOCAL_PATH}/Emulator/Host/THostInfo.cp
		# Just In Time Compiler and Emulator of ARM CPU
		${LOCAL_PATH}/Emulator/JIT/TJITCache.cp
		${LOCAL_PATH}/Emulator/JIT/TJITPage.cp
		${LOCAL_PATH}/Emulator/JIT/TJITPerformance.cp
		# EMulating ARM with "C" Code
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_BlockDataTransfer.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_LogicalOp.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MoveOp.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MRS.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MSR.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_TestOp.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_Multiply.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_MultiplyAndAccumulate.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_Other.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_SingleDataSwap.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_SingleDataTransfer.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_Test.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGenericPage.cp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGenericROMPatch.cp
		# Logging what's happening during EMulation
		${LOCAL_PATH}/Emulator/Log/TBufferLog.cp
		${LOCAL_PATH}/Emulator/Log/TFileLog.cp
		${LOCAL_PATH}/Emulator/Log/TLog.cp
		${LOCAL_PATH}/Emulator/Log/TStdOutLog.cp
		# Base COde to allow NewtonOS to call EInstein EMulation Code
		${LOCAL_PATH}/Emulator/NativeCalls/TNativeCalls.cp
		${LOCAL_PATH}/Emulator/NativeCalls/TVirtualizedCalls.cp
		${LOCAL_PATH}/Emulator/NativeCalls/TVirtualizedCallsPatches.cp
		# Network EMulation
		${LOCAL_PATH}/Emulator/Network/TNetworkManager.cp
		${LOCAL_PATH}/Emulator/Network/TUsermodeNetwork.cp
		# PCMCIA Card Slot and Card Emulation
		${LOCAL_PATH}/Emulator/PCMCIA/TATACard.cp
		${LOCAL_PATH}/Emulator/PCMCIA/TLinearCard.cp
		${LOCAL_PATH}/Emulator/PCMCIA/TNE2000Card.cp
		${LOCAL_PATH}/Emulator/PCMCIA/TPCMCIACard.cp
		${LOCAL_PATH}/Emulator/PCMCIA/TPCMCIAController.cp
		# Information Interchange between NewtonOS and the Host
		${LOCAL_PATH}/Emulator/Platform/TPlatformManager.cp
		# Support for loading various ROM Files
		${LOCAL_PATH}/Emulator/ROM/TAIFFile.cp
		${LOCAL_PATH}/Emulator/ROM/TAIFROMImageWithREXes.cp
		${LOCAL_PATH}/Emulator/ROM/TFlatROMImage.cp
		${LOCAL_PATH}/Emulator/ROM/TFlatROMImageWithREX.cp
		${LOCAL_PATH}/Emulator/ROM/TROMImage.cp
		# Screen EMulation
		${LOCAL_PATH}/Emulator/Screen/TScreenManager.cp
		${LOCAL_PATH}/Emulator/Screen/TAndroidNativeScreenManager.cp
		# Serial Port Emulation
		${LOCAL_PATH}/Emulator/Serial/TSerialPortManager.cp
		# Sound Emulation
		${LOCAL_PATH}/Emulator/Sound/TBufferedSoundManager.cp
		${LOCAL_PATH}/Emulator/Sound/TAndroidNativeSoundManager.cp
		${LOCAL_PATH}/Emulator/Sound/TSoundManager.cp
		# Debug EInstein while running
		${LOCAL_PATH}/Monitor/TSymbolList.cp
		${LOCAL_PATH}/Monitor/UDisasm.cp
		# Cross Platform Support Code
		${LOCAL_PATH}/K/Misc/TCircleBuffer.cp
		${LOCAL_PATH}/K/Misc/TMappedFile.cp
		${LOCAL_PATH}/K/Streams/TFileStream.cp
		${LOCAL_PATH}/K/Streams/TStream.cp
		${LOCAL_PATH}/K/Threads/TCondVar.cp
		${LOCAL_PATH}/K/Threads/TMutex.cp
		${LOCAL_PATH}/K/Threads/TThread.cp
		${LOCAL_PATH}/K/Unicode/UUTF16Conv.cp
		${LOCAL_PATH}/K/Unicode/UUTF16CStr.cp
)

set_source_files_properties(${SourceFiles} PROPERTIES LANGUAGE CXX)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             einstein

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             ${SourceFiles}
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log
)

find_library(jnigraphics-lib jnigraphics)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       einstein
                       android
                       ${log-lib}
                       ${jnigraphics-lib}
                       m
)
