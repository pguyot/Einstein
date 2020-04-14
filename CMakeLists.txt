#
# Use a single CMake setup to compile Einstein on all supported platforms.
# 
# Einstein has forked unintentioanlly into a well supported Xcode version,
# a much less supported MSWindows version, and a quite minimal Linux version.
# For the sake of maintainability, this branch of Einstein compiles either
# with FLTK as its cross-platform GUI, or with no GUI at all for 
# handheld devices.
#
# This CMakeLists.txt will be complete when compiling Einstein from scratch 
# can be explained in ten lines of text for every supported platform.
#

cmake_minimum_required(VERSION 3.1)

project(Einstein)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_OSX_ARCHITECTURES=x86_64)

set ( sources 
	Emulator/Files/TFileManager.cpp
	Emulator/Files/TFileManager.h
	Emulator/Host/THostInfo.cpp
	Emulator/Host/THostInfo.h
	Emulator/Host/UserInfoDefinitions.h
	Emulator/JIT/Generic/IncludeMask.h
	Emulator/JIT/Generic/IncludeMaskRm.h
    Emulator/JIT/Generic/IncludeModeRd.h
    Emulator/JIT/Generic/IncludeModeRn.h
    Emulator/JIT/Generic/IncludeModeRnRd.h
    Emulator/JIT/Generic/IncludeRd.h
    Emulator/JIT/Generic/IncludeRdRm.h
    Emulator/JIT/Generic/IncludeRdRnRsRm15.h
    Emulator/JIT/Generic/IncludeRdRsRm15.h
    Emulator/JIT/Generic/IncludeRm.h
    Emulator/JIT/Generic/IncludeRm15.h
    Emulator/JIT/Generic/IncludeRn.h
    Emulator/JIT/Generic/IncludeRn15.h
    Emulator/JIT/Generic/IncludeRnRd.h
    Emulator/JIT/Generic/IncludeRnRdRm.h
    Emulator/JIT/Generic/IncludeRnRsRm15.h
    Emulator/JIT/Generic/IncludeRsRm15.h
    Emulator/JIT/Generic/RepeatMacros.h
    Emulator/JIT/Generic/TJITGeneric.cpp
    Emulator/JIT/Generic/TJITGeneric.h
    Emulator/JIT/Generic/TJITGenericPage.cpp
    Emulator/JIT/Generic/TJITGenericPage.h
    Emulator/JIT/Generic/TJITGenericROMPatch.cpp
    Emulator/JIT/Generic/TJITGenericROMPatch.h
    Emulator/JIT/Generic/TJITGeneric_BlockDataTransfer.cpp
    Emulator/JIT/Generic/TJITGeneric_BlockDataTransfer.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer.cpp
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp.cpp
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp_template.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_LogicalOp.cpp
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_LogicalOp.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_LogicalOp_template.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MRS.cpp
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MRS.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MRS_template.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MSR.cpp
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MSR.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MSR_template.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MoveOp.cpp
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MoveOp.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_MoveOp_template.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_TestOp.cpp
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_TestOp.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_TestOp_template.h
    Emulator/JIT/Generic/TJITGeneric_DataProcessingPSRTransfer_common.h
    Emulator/JIT/Generic/TJITGeneric_LDM1_template.h
    Emulator/JIT/Generic/TJITGeneric_LDM2_template.h
    Emulator/JIT/Generic/TJITGeneric_LDM3_template.h
    Emulator/JIT/Generic/TJITGeneric_Macros.h
    Emulator/JIT/Generic/TJITGeneric_Multiply.cpp
    Emulator/JIT/Generic/TJITGeneric_Multiply.h
    Emulator/JIT/Generic/TJITGeneric_MultiplyAndAccumulate.cpp
    Emulator/JIT/Generic/TJITGeneric_MultiplyAndAccumulate.h
    Emulator/JIT/Generic/TJITGeneric_Multiply_template.h
    Emulator/JIT/Generic/TJITGeneric_Other.cpp
    Emulator/JIT/Generic/TJITGeneric_Other.h
    Emulator/JIT/Generic/TJITGeneric_STM1_template.h
    Emulator/JIT/Generic/TJITGeneric_STM2_template.h
    Emulator/JIT/Generic/TJITGeneric_SingleDataSwap.cpp
    Emulator/JIT/Generic/TJITGeneric_SingleDataSwap.h
    Emulator/JIT/Generic/TJITGeneric_SingleDataSwap_template.h
    Emulator/JIT/Generic/TJITGeneric_SingleDataTransfer.cpp
    Emulator/JIT/Generic/TJITGeneric_SingleDataTransfer.h
    Emulator/JIT/Generic/TJITGeneric_SingleDataTransfer_template.h
    Emulator/JIT/Generic/TJITGeneric_Test.cpp
    Emulator/JIT/Generic/TJITGeneric_Test.h
    Emulator/JIT/Generic/TJITGeneric_Test_template.h
    Emulator/JIT/JIT.h
    Emulator/JIT/TJIT.h
    Emulator/JIT/TJITCache.cpp
    Emulator/JIT/TJITCache.h
    Emulator/JIT/TJITPage.cpp
    Emulator/JIT/TJITPage.h
    Emulator/JIT/TJITPerformance.cpp
    Emulator/JIT/TJITPerformance.h
    Emulator/Log/TBufferLog.cpp
    Emulator/Log/TBufferLog.h
    Emulator/Log/TFileLog.cpp
    Emulator/Log/TFileLog.h
    Emulator/Log/TLog.cpp
    Emulator/Log/TLog.h
    Emulator/Log/TRAMLog.cpp
    Emulator/Log/TRAMLog.h
    Emulator/Log/TStdOutLog.cpp
    Emulator/Log/TStdOutLog.h
    Emulator/NativeCalls/TVirtualizedCalls.cpp
    Emulator/NativeCalls/TVirtualizedCalls.h
    Emulator/NativeCalls/TVirtualizedCallsPatches.cpp
    Emulator/NativeCalls/TVirtualizedCallsPatches.h
	Emulator/Serial/TTcpClientSerialPortManager.cpp
	Emulator/Serial/TTcpClientSerialPortManager.h
    Emulator/Network/TNetworkManager.cpp
    Emulator/Network/TNetworkManager.h
    Emulator/PCMCIA/TATACard.cpp
    Emulator/PCMCIA/TATACard.h
    Emulator/PCMCIA/TLinearCard.cpp
    Emulator/PCMCIA/TLinearCard.h
    Emulator/PCMCIA/TNE2000Card.cpp
    Emulator/PCMCIA/TNE2000Card.h
    Emulator/PCMCIA/TPCMCIACard.cpp
    Emulator/PCMCIA/TPCMCIACard.h
    Emulator/PCMCIA/TPCMCIAController.cpp
    Emulator/PCMCIA/TPCMCIAController.h
    Emulator/Platform/PlatformEvents.h
    Emulator/Platform/PlatformGestalt.h
    Emulator/Platform/TNewt.cpp
    Emulator/Platform/TNewt.h
    Emulator/Platform/TPlatformManager.cpp
    Emulator/Platform/TPlatformManager.h
    Emulator/ROM/PrecompiledPatches.cpp
    Emulator/ROM/PrecompiledPatches.h
    Emulator/ROM/TAIFFile.cpp
    Emulator/ROM/TAIFFile.h
    Emulator/ROM/TAIFROMImageWithREXes.cpp
    Emulator/ROM/TAIFROMImageWithREXes.h
    Emulator/ROM/TFlatROMImage.cpp
    Emulator/ROM/TFlatROMImage.h
    Emulator/ROM/TFlatROMImageWithREX.cpp
    Emulator/ROM/TFlatROMImageWithREX.h
    Emulator/ROM/TROMImage.cpp
    Emulator/ROM/TROMImage.h
#    Emulator/Scheduler/TScheduler.cpp
#    Emulator/Scheduler/TScheduler.h
#    Emulator/Scheduler/TTask.cpp
#    Emulator/Scheduler/TTask.h
#    Emulator/Screen/CocoaEmulatorApp.h
#    Emulator/Screen/CocoaScreenProxy.h
#    Emulator/Screen/TAndroidNativeScreenManager.cpp
#    Emulator/Screen/TAndroidNativeScreenManager.h
#    Emulator/Screen/TAndroidScreenManager.cpp
#    Emulator/Screen/TAndroidScreenManager.h
#    Emulator/Screen/TCocoaPowerButtonNSView.h
#    Emulator/Screen/TCocoaScreenGlue.h
#    Emulator/Screen/TCocoaScreenManager.h
#    Emulator/Screen/TCocoaScreenView.h
#    Emulator/Screen/TFBScreenManager.cpp
#    Emulator/Screen/TFBScreenManager.h
    Emulator/Screen/TFLScreenManager.cpp
    Emulator/Screen/TFLScreenManager.h
#    Emulator/Screen/TIOSScreenManager.h
    Emulator/Screen/TNullScreenManager.cpp
    Emulator/Screen/TNullScreenManager.h
#    Emulator/Screen/TSDLScreenManager.cpp
#    Emulator/Screen/TSDLScreenManager.h
    Emulator/Screen/TScreenManager.cpp
    Emulator/Screen/TScreenManager.h
#    Emulator/Screen/TX11ScreenManager.cpp
#    Emulator/Screen/TX11ScreenManager.h
    Emulator/Serial/TBasicSerialPortManager.cpp
    Emulator/Serial/TBasicSerialPortManager.h
    Emulator/Serial/TSerialPortManager.cpp
    Emulator/Serial/TSerialPortManager.h
    Emulator/Serial/TSerialPorts.cpp
    Emulator/Serial/TSerialPorts.h
#    Emulator/Sound/TAndroidNativeSoundManager.cpp
#    Emulator/Sound/TAndroidNativeSoundManager.h
#    Emulator/Sound/TAndroidSoundManager.cpp
#    Emulator/Sound/TAndroidSoundManager.h
    Emulator/Sound/TBufferedSoundManager.cpp
    Emulator/Sound/TBufferedSoundManager.h
    Emulator/Sound/TNullSoundManager.cpp
    Emulator/Sound/TNullSoundManager.h
#    Emulator/Sound/TPortAudioSoundManager.cpp
#    Emulator/Sound/TPortAudioSoundManager.h
#    Emulator/Sound/TPulseAudioSoundManager.cpp
#    Emulator/Sound/TPulseAudioSoundManager.h
    Emulator/Sound/TSoundManager.cpp
    Emulator/Sound/TSoundManager.h
    Emulator/TARMProcessor.cpp
    Emulator/TARMProcessor.h
    Emulator/TDMAManager.cpp
    Emulator/TDMAManager.h
    Emulator/TEmulator.cpp
    Emulator/TEmulator.h
    Emulator/TFlash.cpp
    Emulator/TFlash.h
    Emulator/THashMapCache.h
    Emulator/TInterruptManager.cpp
    Emulator/TInterruptManager.h
    Emulator/TMMU.cpp
    Emulator/TMMU.h
    Emulator/TMemory.cpp
    Emulator/TMemory.h
    Emulator/TMemoryConsts.h
    Emulator/TNativePrimitives.cpp
    Emulator/TNativePrimitives.h
#    Emulator/Win32/CompatibilityWin32.cpp
#    Emulator/Win32/CompatibilityWin32.h
    K/Defines/KDefinitions.cpp
    K/Defines/KDefinitions.h
    K/Defines/UByteSex.h
    K/Exceptions/Errors/TError.cpp
    K/Exceptions/Errors/TError.h
    K/Exceptions/Errors/TMemError.cpp
    K/Exceptions/Errors/TMemError.h
    K/Exceptions/IO/TEOFException.cpp
    K/Exceptions/IO/TEOFException.h
    K/Exceptions/IO/TIOException.cpp
    K/Exceptions/IO/TIOException.h
    K/Exceptions/TException.cpp
    K/Exceptions/TException.h
    K/Misc/RelocHack.h
    K/Misc/TCircleBuffer.cpp
    K/Misc/TCircleBuffer.h
    K/Misc/TDoubleLinkedList.h
    K/Misc/TMappedFile.cpp
    K/Misc/TMappedFile.h
#    K/Misc/TNameServerSingleton.cpp
#    K/Misc/TNameServerSingleton.h
#    K/Misc/UPStrings.cpp
#    K/Misc/UPStrings.h
    K/Streams/TFileStream.cpp
    K/Streams/TFileStream.h
    K/Streams/TRandomAccessStream.h
    K/Streams/TStream.cpp
    K/Streams/TStream.h
#    K/Tests/KDebug.h
#    K/Tests/TFunctionMonitor.h
#    K/Tests/TMonitorMethodClient.cpp
#    K/Tests/TMonitorMethodClient.h
#    K/Tests/TNewtonTraceMonitor.cpp
#    K/Tests/TNewtonTraceMonitor.h
#    K/Tests/TTraceMonitor.cpp
#    K/Tests/TTraceMonitor.h
    K/Threads/TCondVar.cpp
    K/Threads/TCondVar.h
#    K/Threads/TFiber.cpp
#    K/Threads/TFiber.h
    K/Threads/TMutex.cpp
    K/Threads/TMutex.h
    K/Threads/TThread.cpp
    K/Threads/TThread.h
    K/Unicode/UUTF16CStr.cpp
    K/Unicode/UUTF16CStr.h
    K/Unicode/UUTF16Conv.cpp
    K/Unicode/UUTF16Conv.h
    K/Unicode/UnicodeDefinitions.h
#    Monitor/TMacMonitor.h
#    Monitor/TMacMonitorView.h
    Monitor/TMonitor.cpp
    Monitor/TMonitor.h
    Monitor/TMonitorCore.cpp
    Monitor/TMonitorCore.h
    Monitor/TSymbolList.cpp
    Monitor/TSymbolList.h
    Monitor/UDisasm.cpp
    Monitor/UDisasm.h
#    app/AndroidGlue.cpp
#    app/AndroidGlue.h
#    app/Scripting/EinsteinDoNewtonScriptCommand.h
#    app/Scripting/EinsteinInstallPackageCommand.h
#    app/TAndroidApp.cpp
#    app/TAndroidApp.h
#    app/TAndroidNativeApp.cpp
#    app/TAndroidNativeApp.h
#    app/TCLIApp.cpp
#    app/TCLIApp.h
#    app/TCocoaAppController.h
#    app/TCocoaFileManager.h
#    app/TCocoaListenerWindow.h
#    app/TCocoaMonitorController.h
#    app/TCocoaPathHelper.h
#    app/TCocoaROMDumperController.h
#    app/TCocoaSetupController.h
#    app/TCocoaUserDefaults.h
    app/TFLApp.cpp
    app/TFLApp.h
#    app/TFLSettings.fl
    app/TFLSettings.cpp
    app/TFLSettings.h
#    app/TMaemoApp.cpp
#    app/TMaemoApp.h
    app/TPathHelper.cpp
    app/TPathHelper.h
    app/Version.h
#    app/einstein.cpp
#    app/einstein_maemo.cpp
#    app/iEinstein/Classes/iEinsteinAppDelegate.h
#    app/iEinstein/Classes/iEinsteinView.h
#    app/iEinstein/Classes/iEinsteinViewController.h
#    libffi-armlinux/include/ffi.h
#    libffi-armlinux/include/ffitarget.h
#    libffi-cygwin/include/ffi.h
#    libffi-cygwin/include/ffitarget.h
#    libffi-linux/include/ffi.h
#    libffi-linux/include/ffitarget.h
#    libffi-nokia/include/ffi.h
#    libffi-nokia/include/ffi_common.h
#    libffi-nokia/include/fficonfig.h
#    libffi-nokia/include/ffitarget.h
#    libffi-nokia2006/include/ffi.h
#    libffi-nokia2006/include/ffi_common.h
#    libffi-nokia2006/include/fficonfig.h
#    libffi-nokia2006/include/ffitarget.h
#    libffi-osx/include/ffi.h
#    libffi-osx/include/ffi_common.h
#    libffi-osx/include/fficonfig.h
#    libffi-osx/include/ffitarget.h
#    libffi-osx/src/powerpc/asm.h
#    libffi-osx/src/powerpc/ffitarget.h
#    libffi-osx/src/x86/ffitarget.h
#    libffi-win32/include/ffi.h
#    libffi-win32/include/ffitarget.h
#    libffi/acconfig.h
#    libffi/confdefs.h
#    libffi/include/ffi_common.h
#    libffi/include/ffi_mips.h
#    libffi/src/alpha/ffitarget.h
#    libffi/src/arm/ffitarget.h
#    libffi/src/fficonfig.h
#    libffi/src/ia64/ffitarget.h
#    libffi/src/ia64/ia64_flags.h
#    libffi/src/include/ffi.h
#    libffi/src/include/fficonfig.h
#    libffi/src/m68k/ffitarget.h
#    libffi/src/mips/ffitarget.h
#    libffi/src/pa/ffitarget.h
#    libffi/src/powerpc/asm.h
#    libffi/src/powerpc/ffitarget.h
#    libffi/src/s390/ffitarget.h
#    libffi/src/sh/ffitarget.h
#    libffi/src/sh64/ffitarget.h
#    libffi/src/sparc/ffitarget.h
#    libffi/src/x86/ffitarget.h
#    libffi/testsuite/libffi.call/ffitest.h
#    libffi/testsuite/libffi.special/ffitestcxx.h
#    portaudio/pa_asio/iasiothiscallresolver.cpp
#    portaudio/pa_asio/iasiothiscallresolver.h
#    portaudio/pa_asio/pa_asio.cpp
#    portaudio/pa_asio/pa_asio.h
#    portaudio/pa_beos/PlaybackNode.h
#    portaudio/pa_common/pa_allocation.h
#    portaudio/pa_common/pa_converters.h
#    portaudio/pa_common/pa_cpuload.h
#    portaudio/pa_common/pa_dither.h
#    portaudio/pa_common/pa_endianness.h
#    portaudio/pa_common/pa_hostapi.h
#    portaudio/pa_common/pa_process.h
#    portaudio/pa_common/pa_stream.h
#    portaudio/pa_common/pa_trace.h
#    portaudio/pa_common/pa_types.h
#    portaudio/pa_common/pa_util.h
#    portaudio/pa_common/portaudio.h
#    portaudio/pa_dll_switch/PaDllEntry.h
#    portaudio/pa_dll_switch/loadPA_DLL.cpp
#    portaudio/pa_dll_switch/portaudio.h
#    portaudio/pa_linux_alsa/pa_linux_alsa.h
#    portaudio/pa_mac_core/pa_mac_core.h
#    portaudio/pa_unix/pa_unix_util.h
#    portaudio/pa_win/pa_x86_plain_converters.h
#    portaudio/pa_win_ds/dsound_wrapper.h
#    portaudio/pa_win_wmme/pa_win_wmme.h
#    portaudio/pablio/pablio.h
#    portaudio/pablio/ringbuffer.h
)

if (APPLE)
	list (APPEND sources 
		Emulator/NativeCalls/TObjCBridgeCalls.mm
		Emulator/NativeCalls/TObjCBridgeCalls.h
		Emulator/Sound/TCoreAudioSoundManager.cpp
		Emulator/Sound/TCoreAudioSoundManager.h
	)
else()
	list (APPEND sources 
		Emulator/NativeCalls/NativeCallsDefines.h
		Emulator/NativeCalls/TNativeCalls.cpp
		Emulator/NativeCalls/TNativeCalls.h
	)
endif()

if (WIN32)
	list (APPEND sources
		Emulator/Sound/TWaveSoundManager.cpp
		Emulator/Sound/TWaveSoundManager.h
	)
else()
	list (APPEND sources
		Emulator/Network/TTapNetwork.cpp
		Emulator/Network/TTapNetwork.h
		Emulator/Network/TUsermodeNetwork.cpp
		Emulator/Network/TUsermodeNetwork.h
		Emulator/Serial/TBasiliskIISerialPortManager.cpp
		Emulator/Serial/TBasiliskIISerialPortManager.h
		Emulator/Serial/TPipesSerialPortManager.cpp
		Emulator/Serial/TPipesSerialPortManager.h
		Emulator/Serial/TPtySerialPortManager.cpp
		Emulator/Serial/TPtySerialPortManager.h
	)
endif()

if (APPLE)
	add_executable(Einstein MACOSX_BUNDLE ${sources} ${data})
else()
    configure_file(
	    Resources/MSWindows/Einstein.rc.in
	    Einstein.rc
	)
	add_executable(Einstein WIN32
        ${sources} ${data}
        ${CMAKE_CURRENT_BINARY_DIR}/Einstein.rc
        )
endif()

if (WIN32)
	set(FLTK_DIR "C:/Program Files/FLTK")
	add_compile_options(-bigobj)
endif()

find_package(FLTK REQUIRED NO_MODULE)

if (WIN32)
	#target_compile_options(Einstein PUBLIC -Wall)
	target_compile_definitions(Einstein PRIVATE TARGET_OS_WIN32=1 WIN32_LEAN_AND_MEAN=1)
endif()
if (APPLE)
	target_compile_options(Einstein PUBLIC -Wall)
	target_compile_definitions(Einstein PRIVATE NO_PORT_AUDIO NO_X11 TARGET_OS_OPENSTEP=1 NS_BLOCK_ASSERTIONS=1)
endif()
target_include_directories(Einstein PUBLIC ${CMAKE_SOURCE_DIR} ${FLTK_INCLUDE_DIRS})

if (WIN32)
target_link_libraries(Einstein 
	fltk
	winmm
)
endif()

if (APPLE)
target_link_libraries(Einstein 
	/usr/local/lib/libfltk.a
	pthread
	"-framework AddressBook"
	"-framework AudioUnit"
	"-framework AppKit"
	"-framework CoreAudio"
	"-framework Cocoa"
)
endif()

set_target_properties(Einstein PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Resources/FLInfo.plist.in")


#add_custom_command(
#    MAIN_DEPENDENCY "app/TFLSettings.fl"
#	OUTPUT "app/TFLSettings.cpp" "app/TFLSettings.h"
#	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/app
#	COMMAND fluid -c TFLSettings.fl
#)
#add_custom_target(
#  TFLSettings
#  DEPENDS "app/TFLSettings.cpp" "app/TFLSettings.h"
#)
#add_dependencies(Einstein TFLSettings)



#MACOSX_BUNDLE_INFO_STRING
#MACOSX_BUNDLE_ICON_FILE
#MACOSX_BUNDLE_GUI_IDENTIFIER
#MACOSX_BUNDLE_LONG_VERSION_STRING
#MACOSX_BUNDLE_BUNDLE_NAME
#MACOSX_BUNDLE_SHORT_VERSION_STRING
#MACOSX_BUNDLE_BUNDLE_VERSION
#MACOSX_BUNDLE_COPYRIGHT

# message ( ${CMAKE_CURRENT_BINARY_DIR} " --- " ${bundleName} )

