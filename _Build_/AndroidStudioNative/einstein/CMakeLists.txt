# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

PROJECT(Einstein C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET (LOCAL_PATH "../../..")

# Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Ofast")

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
		${LOCAL_PATH}/app/TAndroidNativeApp.cpp
		${LOCAL_PATH}/app/json11.cpp
		# Einstein Emulator Base Code
		${LOCAL_PATH}/Emulator/TARMProcessor.cpp
		${LOCAL_PATH}/Emulator/TDMAManager.cpp
		${LOCAL_PATH}/Emulator/TEmulator.cpp
		${LOCAL_PATH}/Emulator/TFlash.cpp
		${LOCAL_PATH}/Emulator/TInterruptManager.cpp
		${LOCAL_PATH}/Emulator/TMemory.cpp
		${LOCAL_PATH}/Emulator/TMMU.cpp
		${LOCAL_PATH}/Emulator/TNativePrimitives.cpp
		# Manage Paths and File Access
		${LOCAL_PATH}/Emulator/Files/TFileManager.cpp
		# GRab Owner Information form the Host and make it available to NewtonOS
		${LOCAL_PATH}/Emulator/Host/THostInfo.cpp
		# Just In Time Compiler and Emulator of ARM CPU
		${LOCAL_PATH}/Emulator/JIT/TJITCache.cpp
		${LOCAL_PATH}/Emulator/JIT/TJITPage.cpp
		${LOCAL_PATH}/Emulator/JIT/TJITPerformance.cpp
		# EMulating ARM with "C" Code
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_BlockDataTransfer.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_LogicalOp.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MoveOp.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MRS.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MSR.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_TestOp.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_Multiply.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_MultiplyAndAccumulate.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_Other.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_SingleDataSwap.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_SingleDataTransfer.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_Test.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGenericPage.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGenericROMPatch.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_HalfwordAndSignedDataTransferImm.cpp
		${LOCAL_PATH}/Emulator/JIT/Generic/TJITGeneric_HalfwordAndSignedDataTransferReg.cpp
		# Logging what's happening during EMulation
		${LOCAL_PATH}/Emulator/Log/TBufferLog.cpp
		${LOCAL_PATH}/Emulator/Log/TFileLog.cpp
		${LOCAL_PATH}/Emulator/Log/TLog.cpp
		${LOCAL_PATH}/Emulator/Log/TStdOutLog.cpp
		# Base COde to allow NewtonOS to call EInstein EMulation Code
		${LOCAL_PATH}/Emulator/NativeCalls/TNativeCalls.cpp
		${LOCAL_PATH}/Emulator/NativeCalls/TVirtualizedCalls.cpp
		${LOCAL_PATH}/Emulator/NativeCalls/TVirtualizedCallsPatches.cpp
		# Network EMulation
		${LOCAL_PATH}/Emulator/Network/TNetworkManager.cpp
		${LOCAL_PATH}/Emulator/Network/TUsermodeNetwork.cpp
		# PCMCIA Card Slot and Card Emulation
		${LOCAL_PATH}/Emulator/PCMCIA/TATACard.cpp
		${LOCAL_PATH}/Emulator/PCMCIA/TLinearCard.cpp
		${LOCAL_PATH}/Emulator/PCMCIA/TNE2000Card.cpp
		${LOCAL_PATH}/Emulator/PCMCIA/TPCMCIACard.cpp
		${LOCAL_PATH}/Emulator/PCMCIA/TPCMCIAController.cpp
		# Information Interchange between NewtonOS and the Host
        ${LOCAL_PATH}/Emulator/Platform/TPlatformManager.cpp
        ${LOCAL_PATH}/Emulator/Platform/TNewt.cpp
		# Support for loading various ROM Files
		${LOCAL_PATH}/Emulator/ROM/TAIFFile.cpp
		${LOCAL_PATH}/Emulator/ROM/TAIFROMImageWithREXes.cpp
		${LOCAL_PATH}/Emulator/ROM/TFlatROMImageWithREX.cpp
		${LOCAL_PATH}/Emulator/ROM/TROMImage.cpp
		# Screen EMulation
		${LOCAL_PATH}/Emulator/Screen/TScreenManager.cpp
		${LOCAL_PATH}/Emulator/Screen/TAndroidNativeScreenManager.cpp
		# Serial Port Emulation
        ${LOCAL_PATH}/Emulator/Serial/TSerialPorts.cpp
        ${LOCAL_PATH}/Emulator/Serial/TSerialPortManager.cpp
		${LOCAL_PATH}/Emulator/Serial/TBasicSerialPortManager.cpp
		${LOCAL_PATH}/Emulator/Serial/TTcpClientSerialPortManager.cpp
		# Sound Emulation
		${LOCAL_PATH}/Emulator/Sound/TBufferedSoundManager.cpp
		${LOCAL_PATH}/Emulator/Sound/TAndroidNativeSoundManager.cpp
		${LOCAL_PATH}/Emulator/Sound/TSoundManager.cpp
		# Debug EInstein while running
		${LOCAL_PATH}/Monitor/TSymbolList.cpp
		${LOCAL_PATH}/Monitor/UDisasm.cpp
		# Cross Platform Support Code
		${LOCAL_PATH}/K/Misc/TCircleBuffer.cpp
        ${LOCAL_PATH}/K/Misc/TMappedFile.cpp
        ${LOCAL_PATH}/K/Misc/CRC32.cpp
		${LOCAL_PATH}/K/Streams/TFileStream.cpp
		${LOCAL_PATH}/K/Streams/TStream.cpp
		${LOCAL_PATH}/K/Threads/TCondVar.cpp
		${LOCAL_PATH}/K/Threads/TMutex.cpp
		${LOCAL_PATH}/K/Threads/TThread.cpp
		${LOCAL_PATH}/K/Unicode/UUTF16Conv.cpp
		${LOCAL_PATH}/K/Unicode/UUTF16CStr.cpp

        ${LOCAL_PATH}/app/TFLRexImage.cpp
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
